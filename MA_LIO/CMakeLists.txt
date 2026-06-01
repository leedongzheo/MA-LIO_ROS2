cmake_minimum_required(VERSION 3.8)
project(ma_lio)

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 17)
endif()
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Release) # Chuyển sang Release để thuật toán SLAM chạy mượt và nhanh hơn
add_definitions(-DROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/\")

# Tối ưu hóa xử lý đa luồng dựa trên cấu hình CPU
if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
  include(ProcessorCount)
  ProcessorCount(N)
  if(N GREATER 4)
    add_definitions(-DMP_EN -DMP_PROC_NUM=3)
  elseif(N GREATER 3)
    add_definitions(-DMP_EN -DMP_PROC_NUM=2)
  else()
    add_definitions(-DMP_PROC_NUM=1)
  endif()
else()
  add_definitions(-DMP_PROC_NUM=1)
endif()

# Tìm kiếm các gói phụ thuộc hệ thống
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(message_filters REQUIRED)
find_package(rosgraph_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED)
find_package(livox_ros_driver2 REQUIRED)
find_package(visualization_msgs REQUIRED)

# Định nghĩa các file giao diện Custom Message
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/Pose6D.msg"
  DEPENDENCIES geometry_msgs
)

# Khai báo mục tiêu thực thi thuật toán SLAM
add_executable(malio_mapping
  src/laserMapping.cpp
  src/BsplineSE3.cpp
  include/ikd-Tree/ikd_Tree.cpp
  src/preprocess.cpp
  src/parameters.cpp
)

# Khai báo các thư mục chứa file Header nội bộ cục bộ
target_include_directories(malio_mapping PRIVATE
  include
  ${EIGEN3_INCLUDE_DIR}
  ${PCL_INCLUDE_DIRS}
  "${CMAKE_CURRENT_BINARY_DIR}/rosidl_generator_cpp" # <--- THÊM ĐƯỜNG DẪN NÀY VÀO
)

# Khai báo các gói phụ thuộc của ROS2 (Đã bổ sung ${PROJECT_NAME} để nạp include từ build)
ament_target_dependencies(malio_mapping
  rclcpp 
  std_msgs 
  sensor_msgs 
  geometry_msgs 
  nav_msgs
  tf2 
  tf2_ros 
  pcl_conversions 
  message_filters
  livox_ros_driver2
  visualization_msgs
)

# Ép mục tiêu malio_mapping phải đợi bộ sinh tin nhắn rosidl chạy xong 100% để tránh Dependency Race
rosidl_get_typesupport_target(rosidl_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")
add_dependencies(malio_mapping ${rosidl_target})

# Gộp chung liên kết các thư viện bên ngoài và nội bộ vào mục PRIVATE chuẩn Modern CMake
target_link_libraries(malio_mapping 
  ${rosidl_target}
  ${PCL_LIBRARIES}
)

# Tiến hành cài đặt các mục tiêu thực thi và thư mục tài nguyên
install(TARGETS malio_mapping
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch config rviz_cfg
  DESTINATION share/${PROJECT_NAME}
)

ament_export_dependencies(rosidl_default_runtime)

ament_package()
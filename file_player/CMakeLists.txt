cmake_minimum_required(VERSION 3.8)
project(file_player)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# BẬT TÍNH NĂNG TỰ ĐỘNG BIÊN DỊCH QT CHUẨN MỚI
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # Chuyển sang ON để tự động xử lý file .ui
set(CMAKE_AUTORCC ON) # Chuyển sang ON để tự động xử lý file .qrc

# Ép CMake ưu tiên tìm module trong thư mục cục bộ trước
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(rosgraph_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(image_transport REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PCL REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(pcl_msgs REQUIRED)
find_package(irp_sen_msgs REQUIRED)
find_package(livox_ros_driver2 REQUIRED)
find_package(camera_info_manager REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets Gui OpenGL)
find_package(OpenGL REQUIRED)
# # Chèn vào dưới đoạn find_package(image_transport REQUIRED)
# message(STATUS "--- KIỂM TRA ĐƯỜNG DẪN INCLUDE: ${image_transport_INCLUDE_DIRS}")
# message(STATUS "--- KIỂM TRA THƯ VIỆN LIBRARIES: ${image_transport_LIBRARIES}")
# --- CHÈN CỤM LỆNH KIỂM TRA NÀY VÀO DƯỚI CÁC LỆNH FIND_PACKAGE ---
message(STATUS "=======================================================")
message(STATUS "[DEBUG] image_transport_FOUND: ${image_transport_FOUND}")
message(STATUS "[DEBUG] image_transport_INCLUDE_DIRS: ${image_transport_INCLUDE_DIRS}")
message(STATUS "[DEBUG] image_transport_LIBRARIES: ${image_transport_LIBRARIES}")
message(STATUS "[DEBUG] rclcpp_INCLUDE_DIRS: ${rclcpp_INCLUDE_DIRS}")
message(STATUS "=======================================================")
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Đưa trực tiếp file .ui và file .qrc vào danh sách mã nguồn
set(FILE_PLAYER_SRCS
  ${SRC_DIR}/main.cpp
  ${SRC_DIR}/mainwindow.cpp
  ${SRC_DIR}/ROSThread.cpp
  ${SRC_DIR}/mainwindow.ui
  ${SRC_DIR}/resources.qrc
)

set(FILE_PLAYER_HDRS
  ${SRC_DIR}/mainwindow.h
  ${SRC_DIR}/ROSThread.h
)

# KHÔNG DÙNG qt5_wrap_ui VÀ qt5_add_resources THỦ CÔNG NỮA
# Vì CMAKE_AUTOUIC và CMAKE_AUTORCC ở đầu file sẽ tự động làm việc này cực kỳ sạch sẽ.

add_executable(file_player
  ${FILE_PLAYER_SRCS}
  ${FILE_PLAYER_HDRS}
)

target_include_directories(file_player PRIVATE
  ${SRC_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${EIGEN3_INCLUDE_DIRS}
  ${PCL_INCLUDE_DIRS}
  # ${image_transport_INCLUDE_DIRS} # <--- bổ sung thêm biến đường dẫn image_transport
  # ${ament_cmake_core_INCLUDE_DIRS}
  # /opt/ros/jazzy/include/image_transport
  # /opt/ros/jazzy/include
  )

ament_target_dependencies(file_player
  rclcpp
  std_msgs
  geometry_msgs
  sensor_msgs
  nav_msgs
  rosgraph_msgs
  std_srvs
  image_transport
  cv_bridge
  pcl_conversions
  pcl_msgs
  irp_sen_msgs
  livox_ros_driver2
  camera_info_manager
)

target_link_libraries(file_player
  Qt5::Widgets
  Qt5::Gui
  Qt5::OpenGL
  ${OpenCV_LIBRARIES}
  ${PCL_LIBRARIES}
  image_transport::image_transport        # <--- DÙNG TARGET CHUẨN HIỆN ĐẠI NÀY
  OpenGL::GL
)

install(TARGETS file_player
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
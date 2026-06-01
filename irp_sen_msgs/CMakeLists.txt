cmake_minimum_required(VERSION 3.8)
project(irp_sen_msgs)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

set(msg_files
  "msg/Inspva.msg"
  "msg/LaserScanArray.msg"
  "msg/Altimeter.msg"
  "msg/Fog3axis.msg"
  "msg/Imu.msg"
  "msg/Fog.msg"
  "msg/Encoder.msg"
  "msg/Vrs.msg"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  DEPENDENCIES std_msgs geometry_msgs sensor_msgs
)

ament_export_dependencies(rosidl_default_runtime)
ament_package()

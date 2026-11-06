cmake_minimum_required(VERSION 3.8)
project(gamepad_io)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(SDL2 REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/RumbleCommand.msg"
)
ament_export_dependencies(rosidl_default_runtime)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

add_executable(gamepad_print src/gamepad_print.cpp)
ament_target_dependencies(gamepad_print rclcpp sensor_msgs)
target_link_libraries(gamepad_print SDL2::SDL2)
install(TARGETS gamepad_print DESTINATION lib/${PROJECT_NAME})
install(
  DIRECTORY launch config  # 同时安装启动文件夹和配置文件夹
  DESTINATION share/${PROJECT_NAME}  # 放入当前功能包的共享目录
)

add_executable(gamepad_rumble src/gamepad_rumble.cpp)
ament_target_dependencies(gamepad_rumble rclcpp sensor_msgs)
rosidl_get_typesupport_target(cpp_typesupport_target
  ${PROJECT_NAME} "rosidl_typesupport_cpp")
target_link_libraries(gamepad_rumble "${cpp_typesupport_target}")
install(TARGETS gamepad_rumble DESTINATION lib/${PROJECT_NAME})

ament_package()

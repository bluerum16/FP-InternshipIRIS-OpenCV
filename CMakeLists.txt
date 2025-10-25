cmake_minimum_required(VERSION 3.10)

project(object-detection)
find_package(OpenCV REQUIRED)
add_executable(object-detection src/camera.cpp)
target_include_directories(object-detection PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(object-detection PRIVATE ${OpenCV_LIBS})
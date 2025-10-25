cmake_minimum_required(VERSION 3.10)

project(object_detection)
find_package(OpenCV REQUIRED)
add_executable(object_detection src/main.cpp)
target_include_directories(object_detection PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(object_detection PRIVATE ${OpenCV_LIBS})
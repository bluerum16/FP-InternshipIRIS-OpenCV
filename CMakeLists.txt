cmake_minimum_required(VERSION 3.10)

project(lane_detection)
find_package(OpenCV REQUIRED)
add_executable(lane_detection
src/main.cpp
src/websocket.cpp
src/udp.cpp
src/vision.cpp
)
include_directories(${OpenCV_INCLUDE_DIRS} include)
target_include_directories(lane_detection PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(lane_detection PRIVATE ${OpenCV_LIBS})
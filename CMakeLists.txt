cmake_minimum_required(VERSION 2.8)
project(hmi-display)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
add_subdirectory(hmi-display)
add_subdirectory(video)
add_subdirectory(rtp-payloader)
message(STATUS "PROJECT_NAME = ${PROJECT_NAME}")
file(COPY images/test2.png images/map.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})

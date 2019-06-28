
cmake_minimum_required(VERSION 2.8.12)

project("generated_code")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find requirements
find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)


message(STATUS "Configuring location...")
add_library(location_lib location.cxx)
target_link_libraries(location_lib fastcdr fastrtps)

add_executable(location locationPubSubTypes.cxx locationPublisher.cxx locationSubscriber.cxx locationPubSubMain.cxx)
target_link_libraries(location fastcdr fastrtps
        location_lib )




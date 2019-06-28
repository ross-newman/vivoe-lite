project(dds)
add_library(dds location/locationPublisher.cxx location/location.cxx location/locationPubSubTypes.cxx location/locationSubscriber.cxx)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find requirements
#find_package(fastcdr REQUIRED)
#find_package(fastrtps REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
message(STATUS "PROJECT_NAME = dds")
target_link_libraries(dds fastcdr fastrtps)


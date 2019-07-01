project(dds)
add_library(dds location/locationPublisher.cxx location/location.cxx location/locationPubSubTypes.cxx location/locationSubscriber.cxx)
add_dependencies(dds Fast_RTPS)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_BINARY_DIR}/external/Fast-RTPS/src/include)
include_directories(${CMAKE_BINARY_DIR}/external/Fast-RTPS/src/thirdparty/fastcdr/include)
include_directories(${CMAKE_BINARY_DIR}/external/Fast-RTPS/build/include)
include_directories(${CMAKE_BINARY_DIR}/external/Fast-RTPS/build/thirdparty/fastcdr/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
message(STATUS "PROJECT_NAME = dds")
target_link_libraries(dds fastcdr fastrtps)


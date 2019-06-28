project(hmi-video)
include_directories(../rtp-payloader)
message(STATUS "PROJECT_NAME = video")
add_library(hmi-video gvaVideo.cpp gvaVideoRtpYuv.cpp)

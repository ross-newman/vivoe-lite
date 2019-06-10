project(hmi-video)
include_directories(../rtp-payloader)
add_library(hmi-video gvaVideo.cpp gvaVideoRtpYuv.cpp)

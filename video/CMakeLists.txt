project(hmi-video)
include_directories(../rtp-payloader)
message(STATUS "PROJECT_NAME = video")
add_library(hmi-video gva_video.cc gva_video_rtp_yuv.cc)

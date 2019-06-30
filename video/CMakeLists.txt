project(hmi-video)
include_directories(${CMAKE_BINARY_DIR}/external/rtp-payloader/src)
message(STATUS "PROJECT_NAME = video")
add_library(hmi-video gva_video.cc gva_video_rtp_yuv.cc)

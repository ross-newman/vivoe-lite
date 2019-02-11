#!/bin/bash
xrandr -s 640x480
export GST_DEBUG=0
gst-launch-0.10 videotestsrc horizontal-speed=1 pattern=0 is-live=true \
! video/x-raw-yuv, width=640, height=480, framerate=25/1 \
! queue \
! textoverlay text="Lockheed MArtin CAM1" font-desc="Sans" \
! ffmpegcolorspace \
! ximagesink sync=false


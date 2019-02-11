#!/bin/bash
export GST_DEBUG=0

WIDTH=640
HEIGHT=480
FRAMERATE=8
SENSOR=$1
OVERLAY=$2
DISPLAY_X=$3
PATTERN=$4
FRAMERATE=$5

source getipaddr.sh $SENSOR

echo "Launching \"$OVERLAY\" on $IPADDR with camera=$SENSOR, pattern=$PATTERN, disp=$DISPLAY_X, framerate=$FRAMERATE"

if [ $DISPLAY_X -eq 1 ]; then
  #xrandr -s $HEIGHTxWIDTH
  gst-launch-0.10 videotestsrc horizontal-speed=0 pattern=$PATTERN \
    is-live=true \
  ! video/x-raw-yuv, format=\(fourcc\)UYVY, width=$WIDTH, height=$HEIGHT, \
    framerate=$FRAMERATE/1 \
  ! textoverlay text="$OVERLAY" ypad=45 font-desc="Sans 36" shaded-background=true \
  ! tee name=t \
  ! ffmpegcolorspace \
  ! ximagesink t. \
  ! queue \
  ! rtpvrawpay mtu=660 \
  ! multiudpsink clients=$IPADDR:5004 sync=true &> /dev/null &
else
  gst-launch-0.10 videotestsrc horizontal-speed=1 pattern=$PATTERN \
    is-live=true \
  ! video/x-raw-yuv, format=\(fourcc\)UYVY, width=$WIDTH, height=$HEIGHT, \
    framerate=$FRAMERATE/1 \
  ! textoverlay text="$OVERLAY" ypad=45 font-desc="Sans 36" shaded-background=true \
  ! queue \
  ! rtpvrawpay mtu=660 \
  ! multiudpsink clients=$IPADDR:5004 &> /dev/null &
fi


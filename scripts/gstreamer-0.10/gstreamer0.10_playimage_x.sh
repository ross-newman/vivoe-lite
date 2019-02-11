#!/bin/bash

if [ $# -le 4 ]; then
  echo "Syntax error, require the following arguments ($#):"
  echo "  $0 <camera_no> <png_filename> <text> <framerate> <display> <timerlocation>"
  echo "  framerate, 1=off,1=on" 
  echo "  timerlocation, 1=right, 2=left, 3=centre (optional)" 
  exit 1
fi

export IPADDR=unknownip
source ./getipaddr.sh $1
CAMERA=$1
FILE=$2
TEXT="$3"
FRAMERATE=$4
DISPLAY_X=$5
LOCATION=$6

case "$LOCATION" in
  1)
    ALIGN=" halignment=right ypad=50"
    ;;
  2)
    ALIGN=" halignment=left ypad=50"
    ;;
  3)
    ALIGN=" halignment=center ypad=50"
    ;;
  4)
    ALIGN=" halignment=right valignment=bottom  ypad=50"
    ;;
  *)
    LOCATION=1
    ALIGN=" halignment=right ypad=50"
    ;;
esac

echo "Launching \"$FILE\" on $IPADDR with camera=$CAMERA, text=\"$TEXT\", framerate=$FRAMERATE, location=$LOCATION"

if [ $DISPLAY_X -eq 0 ]; then
  gst-launch-0.10 filesrc location=$FILE  \
  ! pngdec \
  ! ffmpegcolorspace \
  ! video/x-raw-yuv, format=\(fourcc\)UYVY  \
  ! imagefreeze \
  ! video/x-raw-yuv, framerate=$FRAMERATE/1 \
  ! timeoverlay $ALIGN \
  ! textoverlay text="$TEXT" shaded-background=true ypad=50 font-desc="Sans 24" \
  ! rtpvrawpay mtu=660 \
  ! multiudpsink clients=$IPADDR:5004 sync=true &> /dev/null &
else
  gst-launch-0.10 filesrc location=$FILE  \
  ! pngdec \
  ! ffmpegcolorspace \
  ! video/x-raw-yuv, format=\(fourcc\)UYVY  \
  ! imagefreeze \
  ! video/x-raw-yuv, framerate=$FRAMERATE/1 \
  ! timeoverlay $ALIGN \
  ! textoverlay text="$TEXT" ypad=50 font-desc="Sans" shaded-background=true \
  ! tee name=t \
  ! ffmpegcolorspace \
  ! ximagesink sync=true t. \
  ! queue\
  ! rtpvrawpay mtu=660 \
  ! multiudpsink clients=$IPADDR:5004 sync=true &> /dev/null &
fi 

exit


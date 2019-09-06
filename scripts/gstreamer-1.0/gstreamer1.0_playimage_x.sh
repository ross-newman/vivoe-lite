#!/bin/bash

if [ $# -le 4 ]; then
  echo "Syntax error, require the following arguments ($#):"
  echo "  $0 <camera_no> <png_filename> <text> <framerate> <display> <timerlocation>"
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
#GST_FLAGS=-v
PORT=5004

# For live video only
if [ $DISPLAY_X -eq 2 ]; then
  if [ -e "/dev/video8" ]; then
    # Webcam on RD-140
    DEVICE=/dev/video8
  else
    # Default when only a single camera present
    DEVICE=/dev/video0
  fi
  FILE=$DEVICE
fi

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
sleep 1
if [ $DISPLAY_X -eq 0 ]; then
  gst-launch-1.0 $GST_FLAGS filesrc location=$FILE  \
  ! pngdec \
  ! videoconvert \
  ! video/x-raw, format=UYVY  \
  ! imagefreeze \
  ! video/x-raw, framerate=$FRAMERATE/1 \
  ! timeoverlay $ALIGN \
  ! textoverlay text="$TEXT" shaded-background=true ypad=50 font-desc="Sans 24" \
  ! rtpvrawpay mtu=660 \
  ! multiudpsink clients=$IPADDR:$PORT sync=true &
fi
if [ $DISPLAY_X -eq 1 ]; then
  gst-launch-1.0 $GST_FLAGS filesrc location=$FILE  \
  ! pngdec \
  ! videoconvert \
  ! video/x-raw, format=UYVY \
  ! imagefreeze \
  ! video/x-raw, framerate=$FRAMERATE/1 \
  ! timeoverlay $ALIGN \
  ! textoverlay text="${IPADDR}:${PORT}@${FRAMERATE}Htz" ypad=50 font-desc="Sans" shaded-background=true \
  ! queue\
  ! tee name=t \
  ! timeoverlay $ALIGN \
  ! videoconvert \
  ! queue\
  ! ximagesink sync=true t. \
  ! queue\
  ! rtpvrawpay mtu=660 \
  ! multiudpsink clients=$IPADDR:$PORT sync=true &
fi 
if [ $DISPLAY_X -eq 2 ]; then
  gst-launch-1.0 $GST_FLAGS v4l2src device=${DEVICE} \
  ! video/x-raw, width=640, height=480 \
  ! videoconvert \
  ! video/x-raw, format=UYVY \
  ! timeoverlay $ALIGN \
  ! textoverlay text="${IPADDR}:${PORT}@${FRAMERATE}Htz" ypad=50 font-desc="Sans" shaded-background=true \
  ! queue\
  ! rtpvrawpay mtu=660 \
  ! multiudpsink clients=$IPADDR:$PORT sync=true &
fi 
if [ $DISPLAY_X -eq 3 ]; then
  gst-launch-1.0  $GST_FLAGS v4l2src device=${DEVICE} \
  ! video/x-raw, width=640, height=480 \
  ! videoconvert \
  ! ximagesink sync=true 
fi
if [ $DISPLAY_X -eq 4 ]; then
  gst-launch-1.0 $GST_FLAGS v4l2src device=${DEVICE} \
  ! video/x-raw, width=640, height=480 \
  ! videoconvert \
  ! video/x-raw, format=UYVY \
  ! timeoverlay $ALIGN \
  ! textoverlay text="${IPADDR}:${PORT}@${FRAMERATE}Htz" ypad=50 font-desc="Sans" shaded-background=true \
  ! queue\
  ! tee name=t \
  ! timeoverlay $ALIGN \
  ! videoconvert \
  ! queue\
  ! ximagesink sync=true t. \
  ! queue\
  ! rtpvrawpay mtu=660 \
  ! multiudpsink clients=$IPADDR:$PORT sync=true &
fi 

exit
gst-launch-1.0 -v $GST_FLAGS v4l2src device=/dev/video8 ! video/x-raw, width=640, height=480 ! videoconvert ! ximagesink sync=true 

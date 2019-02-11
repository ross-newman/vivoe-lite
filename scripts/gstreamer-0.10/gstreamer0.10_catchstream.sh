#!/bin/bash
export GST_DEBUG=0
gst-launch-0.10 -v udpsrc multicast-group=239.192.1.101 \
multicast-iface=eth0 port=5004 \
caps="application/x-rtp, media=video, clock-rate=(int)90000, \
encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(int)8, payload=(int)96, \
width=(string)1024, height=(string)768" \
! rtpvrawdepay \
! ffmpegcolorspace \
! videoscale \
! video/x-raw-rgb, width=440, height=320 \
! ximagesink sync=false
exit 0


#!/bin/bash

pkill gst-launch-0.10

echo "Starting streams..."

## Simulated streams from images
#                              CAMERA FILE                    TEXT           FPS DISPLAY LOCATION
./gstreamer0.10_playimage_x.sh 1      IMG2_FRONT_CENTRE.png  "Front"         2   0
./gstreamer0.10_playimage_x.sh 2      IMG2_FRONT_CENTRE.png  "Front"         2   0
./gstreamer0.10_playimage_x.sh 3      IMG2_FRONT_CENTRE.png  "Front"         2   0
./gstreamer0.10_playimage_x.sh 4      IMG1_FRONT_LEFT.png    "Front"         2   0

./gstreamer0.10_playimage_x.sh 6      IMG1_FRONT_LEFT.png    "Front Left"    4   0
./gstreamer0.10_playimage_x.sh 7      IMG8_LEFT.png          "Left"          4   0
./gstreamer0.10_playimage_x.sh 8      IMG7_REAR_LEFT.png     "Rear Left"     4   0
./gstreamer0.10_playimage_x.sh 9      IMG3_FRONT_RIGHT.png   "Front Right"   4   0
./gstreamer0.10_playimage_x.sh 10     IMG4_RIGHT.png         "Right"         4   0
./gstreamer0.10_playimage_x.sh 11     IMG5_REAR_RIGHT.png    "Rear Right"    4   0
#./gstreamer0.10_playimage_x.sh 12     IMG2_FRONT_CENTRE.png  "BMS"           2   0
./gstreamer0.10_playimage_x.sh 13     IMG2_FRONT_CENTRE.png  "Primary Sight" 4   0      3

## Synthetic video streams
#                             CAMERA OVERLAY             DISPLAY PATTERN FPS
#./gstreamer0.10_playstream.sh 13     "PrimEng"           0       13       2 
./gstreamer0.10_playstream.sh 12     "BMSVideo1"         0       0       2 

echo "Done..."

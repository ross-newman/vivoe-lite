#!/bin/bash

# Kill stale processes
pkill gst-launch-0.10

#                             CAMERA OVERLAY             DISPLAY PATTERN FPS
## Sights
#./gstreamer0.10_playstream.sh 1      "Primary Sight Day" 0       1       8 
#./gstreamer0.10_playstream.sh 2      "Primary Sight TI"  0       19      8 
#./gstreamer0.10_playstream.sh 3      "Panning Sight Day" 0       13      8 
./gstreamer0.10_playstream.sh 4      "Panning Sight TI"  0       18      8 
#./gstreamer0.10_playstream.sh 5      "Javlin Sight"      0       13      8 
## Cameras
./gstreamer0.10_playstream.sh 6      "Front Left"        0       6       2 
./gstreamer0.10_playstream.sh 7      "Left"              0       6       2 
./gstreamer0.10_playstream.sh 8      "Rear Left"         0       6       2 
./gstreamer0.10_playstream.sh 9      "Front Right"       0       6       2 
./gstreamer0.10_playstream.sh 10     "Right"             0       6       2 
./gstreamer0.10_playstream.sh 11     "Rear Right"        0       6       2 
./gstreamer0.10_playstream.sh 12     "BMSVideo1"         0       6       2 

## Engagement Feed
#./gstreamer0.10_playstream.sh 13     "Engagement Feed"   0       6       2 
#./gstreamer0.10_playstream.sh 14     "Surveilance Feed"  0       6       2 

## WCSP FCC Manager
#./gstreamer0.10_playstream.sh 15     "FCC Alignment"     0       6       2 


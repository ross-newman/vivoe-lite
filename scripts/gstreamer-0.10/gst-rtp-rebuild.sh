#!/bin/bash

## If no GObject-2.0
# sudo apt-get install libgtk2.0-dev gstreamer-0.10 libgstreamer0.10-dev libgstreamer-plugins-base0.10-0 libgstreamer-plugins-base0.10-dev

#rm -rf gstreamer-0.10.36 > /dev/null
#rm -rf gst-plugins-good-0.10.31 > /dev/null

#wget -nc https://gstreamer.freedesktop.org/src/gstreamer/gstreamer-0.10.36.tar.gz  --no-check-certificate
#tar xf ./gstreamer-0.10.36.tar.gz
#cd gstreamer-0.10.36


wget https://gstreamer.freedesktop.org/src/gst-plugins-good/gst-plugins-good-0.10.31.tar.gz --no-check-certificate
tar xf gst-plugins-good-0.10.31.tar.gz
cd gst-plugins-good-0.10.31
./configure
cd ./gst/rtp
# Patch the line number offset to start at 1
sed -i 's/line = field;/line = field+1;/g' ./gstrtpvrawpay.c
make


#!/bin/bash

sudo apt-get install build-essential libjpeg9-dev imagemagick libv4l-dev git cmake uvcdynctrl
cd video_streamer
make
sudo make install
cd ..


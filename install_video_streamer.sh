#!/bin/bash

sudo apt-get install build-essential -y 
sudo apt-get install libjpeg9-dev -y 
sudo apt-get install imagemagick -y
sudo apt-get install libv4l-dev -y 
sudo apt-get install git -y 
sudo apt-get install cmake -y 
sudo apt-get install uvcdynctrl -y

cd video_streamer
make
sudo make install
cd ..


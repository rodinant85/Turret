#!/bin/bash

sudo apt-get install iptables

git clone --branch quadro_node https://github.com/rodinant85/Turret

cd Turret

echo "install node_js..."
sh ./install_node_js.sh

echo "install video_streamer..."
sh ./install_video_streamer.sh

echo "install python library and main program..."
sudo sh ./install_python_lib.sh
sudo sh ./install_main_and_websocket.sh

echo "install services..."
sudo cp quadro_* /etc/systemd/system
sudo systemctl daemon-reload

echo "delete install files..."
rm -rf quadro*
rm -rf install*
rm -rf drive_main
rm -rf websocket_server

echo "enable main service..."
sudo systemctl enable quadro_main.service

cd

echo "reboot..."
sudo reboot




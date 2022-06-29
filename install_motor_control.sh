#!/bin/bash

git clone --branch motor_control_plate https://github.com/rodinant85/Turret
cd Turret

echo "install python library and main program..."

sudo sh ./install_python_lib.sh
sudo sh ./install_main_and_websocket.sh

echo "install services..."
sudo cp machine_* /etc/systemd/system
sudo systemctl daemon-reload

sudo systemctl enable machine_gun_motor_control_main.service

cd ..
rm -rf Turret/

echo "reboot..."
sudo reboot




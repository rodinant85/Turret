#!/bin/bash

git clone --branch machine_gun https://github.com/rodinant85/Turret

cd Turret
cd rear_sight
mkdir build;cd build
cmake ..
make -j4
cp rear_sight ../rear_sight
cd ..
rm -rf build
cd ..
cp -r rear_sight ../rear_sight

echo "install python library and main program..."

sudo sh ./install_python_lib.sh
sudo sh ./install_main.sh

echo "install services..."
sudo cp machine_* /etc/systemd/system
sudo systemctl daemon-reload

sudo systemctl enable machine_gun_main.service

cd ..
rm -rf Turret/

echo "reboot..."
sudo reboot




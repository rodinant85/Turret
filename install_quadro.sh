#!/bin/bash

git clone --branch quadro https://github.com/rodinant85/Turret

cd rear_sight
mkdir build;cd build
cmake ..
make -j4
cp rear_sight ../rear_sight
cd ..
rm -rf build
cp -r rear_sight ../rear_sight

./install_python_lib.sh
./install_main_and_websocket.sh


sudo cp quadro_* /etc/systemd/system
sudo systemctl daemon-reload

sudo systemctl enable quadro_main.service

rm -rf *

sudo reboot




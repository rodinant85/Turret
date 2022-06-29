#!/bin/bash

cd machine_gun_main
sudo pyinstaller -F main.py
sudo mkdir /opt/machine_gun_main
sudo chmod 755 /opt/machine_gun_main
sudo cp dist/main /opt/machine_gun_main/
sudo chmod 755 /opt/machine_gun_main/main


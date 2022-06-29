#!/bin/bash

cd motor_control_main
sudo pyinstaller -F main.py
sudo mkdir /opt/motor_control_main
sudo chmod 755 /opt/motor_control_main
sudo cp dist/main /opt/motor_control_main/
sudo chmod 755 /opt/motor_control_main/main


cd ../websocket_server
sudo pyinstaller -F ws_server.py
sudo cp dist/ws_server /opt/
sudo chmod 755 /opt/ws_server



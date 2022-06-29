#!/bin/bash

cd drive_main
sudo pyinstaller -F main.py
sudo mkdir /opt/quadro_main
sudo chmod 755 /opt/quadro_main
sudo cp dist/main /opt/quadro_main/
sudo chmod 755 /opt/quadro_main/main


cd ../websocket_server
sudo pyinstaller -F ws_server.py
sudo cp dist/ws_server /opt/
sudo chmod 755 /opt/ws_server



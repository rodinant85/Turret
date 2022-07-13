#!/bin/bash

tar -xvf node-v16.16.0-linux-armv7l.tar.xz
cd node-v16.16.0-linux-armv7l/
sudo cp -R * /usr/local/

cd ..
rm -rf node-v16.16.0*
cd rear_sight
npm init -y
npm install express
cd ..

wget https://nodejs.org/dist/v16.16.0/node-v16.16.0-linux-armv7l.tar.xz \
tar -xvf node-v16.16.0-linux-armv7l.tar.xz 


cd node-v16.16.0-linux-armv7l/ \
sudo cp -R * /usr/local/ \
node -v \
npm -v 


cd \
mkdir rear_sight_server \
cd rear_sight_server 


npm init -y \
npm install express \
node app.js

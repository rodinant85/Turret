#!/bin/bash

# here is a filepath to executable file of the iron turtle controller
# possibly you can change this path on which your wish and reboot your Raspberry
# (your Iron Turtle)
RUNNABLE_FILE_FOLDER_PATH="/home/pi/rear_sight/"
RUNNABLE_FILE_NAME="./rear_sight"

# here is a filepath to the log file now it doesn't print any logs, but you can
# set a file for this
LOG_PATH="/dev/null"

sleep 15

export OPENSSL_CONF=""

cd $RUNNABLE_FILE_FOLDER_PATH
$RUNNABLE_FILE_NAME  #&> $LOG_PATH
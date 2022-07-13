'''
Open a new terminal and type sudo nano /etc/rc.local
This will open the Nano editor, with the rc.local file loaded.
At the end of the file (before exit 0 in the code) add the following:
sudo python3 /home/pi/rpi_poweroff.py &
'''
import RPi.GPIO as GPIO
from time import sleep
import os

GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)  # Turn off warnings output
GPIO.setup(40, GPIO.IN, pull_up_down=GPIO.PUD_UP)  # Set pin #40 (GPIO21) to input

print(GPIO.input(40))


def shutdown(channel):
    sleep(3)
    if GPIO.input(40) == 0:
        print("Poweroff ...")
        os.system("sudo poweroff")
    else:
        print('Reboot...')
        os.system("sudo reboot")


GPIO.add_event_detect(40, GPIO.FALLING, callback=shutdown, bouncetime=2000)

while True:
    sleep(1)

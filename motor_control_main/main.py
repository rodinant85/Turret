import threading
import sys
import time
from app import ws_client
from app.logger import exc_log, info_log
from app import socket_pulse
from app.diagnostic import *


info_log = info_log('INFO_MAIN')
exc_log = exc_log('EXC_MAIN')


def set_hostname(hostname: str):
    try:
        os.system(f"hostnamectl set-hostname {hostname}")
        time.sleep(1)
        os.system("systemctl restart avahi-daemon.service")
        time.sleep(5)
    except Exception as e:
        print(e)


def stop_program():
    info_log.info('Stop server. Exit...')
    sys.stdout.flush()
    os._exit(-1)


def start_websocket():
    info_log.info('Starting machine_gun_websocket.service')
    os.system("systemctl start machine_gun_websocket.service")
    time.sleep(10)


def system_monitoring():
    while True:
        if get_temperature() > 70:
            exc_log.error('CPU temp > 70.0')
            ws_client.send_error(f'[{time.strftime("%d.%m.%y, %H:%M:%S")}] '
                                 f'Drive plate CPU Temp is {get_temperature()}!')
        if not serialport(name=f'/dev/{port}'):
            exc_log.error('Serial ERROR')
            ws_client.send_error(f'[{time.strftime("%d.%m.%y, %H:%M:%S")}]'
                                 f' Drive plate Serial ERROR')
        if not service_alive('machine_gun_websocket.service'):
            exc_log.error('Websocket service is not alive')
        #if not ws_client.ready():
        #    exc_log.error('Websocket is not response')
        time.sleep(10)


usb_ports = usb_ports()

if len(usb_ports) != 0:
    port = usb_ports[0]
else:
    port = 'serial0'

info_log.info(f'Serial port: {port}')
if serialport(name=f'/dev/{port}'):
    info_log.info('Serial is OK')
else:
    exc_log.error('Serial does not open')

ip = get_ip()
if not get_ip():
    exc_log.error('NETWORK IS NONE')
else:
    info_log.info(f'Drive plate ip: {ip}')


start_websocket()

if service_alive('machine_gun_websocket.service'):
    info_log.info('Websocket service is alive')
else:
    exc_log.error('Websocket service is not alive')

if ws_client.ready():
    info_log.info('Websocket is ready')
else:
    exc_log.error('Websocket not start')

if get_temperature() < 70.0:
    info_log.info('Temp is normal')
else:
    exc_log.error('CPU temp > 70.0')
    # stop_program()

pulse = threading.Thread(target=socket_pulse.start, )
pulse.start()

monitoring = threading.Thread(target=system_monitoring, )
monitoring.start()

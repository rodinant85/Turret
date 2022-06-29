from logger import exc_log, info_log
import drive_plate_listener
import ws_client
import time
from diagnostic import *

info_log = info_log('INFO')
exc_log = exc_log('EXCEPTION')


def set_hostname(hostname: str):
    try:
        os.system(f"hostnamectl set-hostname {hostname}")
        time.sleep(1)
        os.system("systemctl restart avahi-daemon.service")
        time.sleep(5)
    except Exception as e:
        print(e)


def stop_program():
    info_log.info('Stop main program')
    sys.stdout.flush()
    os._exit(-1)


def stop_rear_sight():
    info_log.info('Stop machine_gun_rear_sight.service')
    sys.stdout.flush()
    os.system("systemctl stop machine_gun_rear_sight.service")


def start_rear_sight():
    info_log.info('Starting machine_gun_rear_sight.service')
    os.system("systemctl start machine_gun_rear_sight.service")
    time.sleep(20)


MY_URL = get_ip()
info_log.info(f'MY_URL: {MY_URL}')
is_server_response = False


if is_camera_ready(0):
    info_log.info('Camera is OK')
else:
    exc_log.error('No camera')
    stop_program()


if get_temperature() < 70.0:
    info_log.info('Temp is normal')
else:
    exc_log.error('CPU temp > 70.0')

ws2_server_url = drive_plate_listener.get_ws_ip()
if not ws2_server_url:
    exc_log.error('WS2 server not response')
else:
    info_log.info(f'WS2 server URL: {ws2_server_url}')
    is_server_response = True

start_rear_sight()

if service_alive('machine_gun_rear_sight.service'):
    info_log.info('Service rear_sight is working')

if not web_ready(f'http://{MY_URL}:56778'):
    exc_log.error('Web not response for 60 sec. Reboot.')
    stop_program()
else:
    info_log.info('Web is working')


while True:
    if not is_server_response:
        ws2_server_url = drive_plate_listener.get_ws_ip()
        if ws2_server_url:
            info_log.info(f'ws2 ok {ws2_server_url}')
            ws_client.send_error('Get WS2 ip address. Reboot your web page', ip=ws2_server_url)
            is_server_response = True
    if service_alive('machine_gun_rear_sight.service'):
        if get_temperature() > 70:
            exc_log.error('CPU temp > 70.0')
            ws_client.send_error(f'[{time.strftime("%d.%m.%y, %H:%M:%S")}] '
                                 f'Drive plate CPU Temp is {get_temperature()}!')
            time.sleep(1)
            stop_rear_sight()
        if not is_web_alive(f'http://{MY_URL}:56778'):
            exc_log.error('Web is not response')
            stop_rear_sight()
    if get_temperature() < 50 and not service_alive('machine_gun_rear_sight.service'):
        info_log.info('Temp is normal')
        start_rear_sight()
    time.sleep(10)

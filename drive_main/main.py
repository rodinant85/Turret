from app.logger import exc_log, info_log
from app import ws_client
from app.diagnostic import *

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


def stop_service(service: str):
    info_log.info(f'Stop {service}.service')
    sys.stdout.flush()
    os.system(f'systemctl stop {service}.service')


def start_service(service: str):
    info_log.info(f'Starting {service}.service')
    os.system(f'systemctl start {service}.service')
    time.sleep(10)


def usb_ports():
    response = os.popen(f'ls /dev | grep  ttyUSB').read()
    usb_list = response.split('\n')[:-1]
    return usb_list


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


if is_camera_ready(0):
    info_log.info('Camera is OK')
else:
    exc_log.error('No camera')
    #stop_program()


if get_temperature() < 70.0:
    info_log.info('Temp is normal')
else:
    exc_log.error('CPU temp > 70.0')


MY_IP = get_ip()
if not MY_IP:
    exc_log.error('NETWORK IS NONE')
else:
    info_log.info(f'MY IP: {MY_IP}')

CONFIG_PATH = "/home/pi/rear_sight/src/server_files/components/scripts/config_ws2.js"
os.makedirs(os.path.dirname(CONFIG_PATH), exist_ok=True)
with open(CONFIG_PATH, 'w') as js_file:
    js_file.write(f'let WS2_URI = "ws://{MY_IP}:56779"')

start_service('quadro_websocket_server')

if service_alive('quadro_websocket_server.service'):
    info_log.info('Websocket service is alive')
else:
    exc_log.error('Websocket service is not alive')

if ws_client.server_ready():
    info_log.info('Websocket is ready')
else:
    exc_log.error('Websocket not start')

start_service('quadro_rear_sight')

if service_alive('quadro_rear_sight.service'):
    info_log.info('Service rear_sight is working')


if not web_ready(f'http://{MY_IP}:56778'):
    exc_log.error('Web not response for 60 sec. Reboot.')
    stop_program()
else:
    info_log.info('Web is working')


while True:
    if get_temperature() > 70:
        exc_log.error('CPU temp > 70.0')
        ws_client.send_error(f'[{time.strftime("%d.%m.%y, %H:%M:%S")}] '
                             f'CPU Temp is {get_temperature()}!')
    if not serialport(name=f'/dev/{port}'):
        exc_log.error('Serial ERROR')
        ws_client.send_error(f'[{time.strftime("%d.%m.%y, %H:%M:%S")}]'
                             f' Drive plate Serial ERROR')
    if not service_alive('quadro_websocket_server.service'):
        exc_log.error('Websocket service is not alive')
    if not service_alive('quadro_rear_sight.service'):
        exc_log.error('Rear_sight service is not alive')
    if not ws_client.server_ready():
        exc_log.error('Websocket is not response')
    if not is_web_alive(f'http://{MY_IP}:56778'):
        exc_log.error('Web is not response')
    time.sleep(10)

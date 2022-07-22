import os
import subprocess
import serial
import time
from smbus import SMBus
import spidev
import re
import socket
import requests


def is_camera_ready(num=0):
    camera_response = os.popen(f'v4l2-ctl --device=/dev/video{str(num)} -I').read()
    if f'ok' in camera_response:
        return 1
    else:
        return 0


def get_temperature():
    temp_response = os.popen(f'vcgencmd measure_temp').read()
    temp = float(re.search(r'-?\d\.?\d*', temp_response).group())
    return temp


def get_ip():
    for i in range(0, 60):
        eth_response = os.popen(f'ip addr show eth0').read()
        if 'inet 192.168.' in eth_response:
            return eth_response.split("inet ")[1].split("/")[0]
        else:
            time.sleep(1)
            continue
    wlan_response = os.popen(f'ip addr show wlan0').read()
    if 'inet ' in wlan_response:
        return wlan_response.split("inet ")[1].split("/")[0]
    else:
        return None


def tcp_socket(ip: str, port: int):
    try:
        sock = socket.socket()
        sock.connect((ip, port))
        sock.close()
        return 1
    except:
        return 0


def is_web_alive(address: str):
    try:
        res = requests.get(address)
        if res:
            return True
    except Exception:
        return False


def web_ready(address: str, time_for_check=60):
    timer_start = time.time()
    while True:
        try:
            if (time.time() - timer_start) > time_for_check:
                return False
            res = requests.get(address)
            if res:
                return True
        except Exception:
            time.sleep(1)
            continue


def service_alive(service_name: str):
    try:
        res = subprocess.check_output('systemctl is-active ' + service_name, shell=True)
        if 'inactive' in str(res):
            return 0
        else:
            return 1
    except subprocess.CalledProcessError:
        return 0


def serialport(name='/dev/serial0'):
    try:
        ser = serial.Serial(
            port=name,
            baudrate=115200,
            xonxoff=False,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=0.5)

        if ser.isOpen():
            return 1
        else:
            return 0
    except Exception as e:
        print('SERIAL:', e)
        return 0


def lte_connection(name='usb0'):
    response = os.system(f'ping -I {name} -c 1 8.8.8.8')
    return response


def wifi_connection(name='wlan0'):
    response = os.system(f'ping -I {name} -c 1 8.8.8.8')
    return response


def eth_connection(name='eth0'):
    response = os.system(f'ping -I {name} -c 1 8.8.8.8')
    return response


def i2c_ready(addr: int, value: int):
    try:
        bus = SMBus(1)
        data = bus.read_byte_data(addr, 0x00)
        # print(data, value, sep=" ")
        if hex(data) == hex(value):
            return 1
        else:
            return 0
    except Exception as e:
        print(e)
        return 0


def spi_ready(n_spi=0, chip=0):
    try:
        spi = spidev.SpiDev()
        spi.open(n_spi, chip)
        spi.max_speed_hz = 4000000
        spi.close()
        return 1
    except Exception as e:
        print(e)
        return 0


if __name__ == '__main__':
    if spi_ready():
        print('SPI is OK')
    if serialport():
        print('Serial is OK')
    if is_camera_ready():
        print('Camera is OK')
    if tcp_socket(get_ip(), 56778):
        print('Websocket is OK')
    if is_web_alive(f'http://{get_ip()}:56778'):
        print('Webserver is OK')

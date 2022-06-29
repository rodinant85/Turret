import os
import socket
import time
from diagnostic import get_ip


CONFIG_PATH = "/home/pi/rear_sight/src/server_files/components/scripts/config_ws2.js"

# create dirs and subdirs if not exist
os.makedirs(os.path.dirname(CONFIG_PATH), exist_ok=True)


with open(CONFIG_PATH, 'w') as js_file:
    js_file.write(f'let WS2_URI = "ws://{get_ip()}:56779"')


def save_config(ip):
    try:
        with open(CONFIG_PATH, 'w') as js_file:
            js_file.write(f'let WS2_URI = "ws://{ip}:56779"')
    except FileNotFoundError as e:
        print("No such file. Creating config file...")
        os.mknod(CONFIG_PATH)
        with open(CONFIG_PATH, 'w') as js_file:
            js_file.write(f'let WS2_URI = "ws://{ip}:56779"')
        save_config(ip)


listener_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
listener_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
listener_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
listener_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
listener_socket.bind(("", 29802))
listener_socket.settimeout(5)


def get_ws_ip():
    timer_start = time.time()
    while True:
        try:
            if (time.time() - timer_start) > 15:
                timer_start = time.time()
                return None
            data, addr = listener_socket.recvfrom(1024)
            if data.decode() == "I am a server":
                print(f'WS2_SERV_ADDR: {addr[0]}')
                #print(f'WS2_SERV_ADDR: {get_ip()}')
                save_config(addr[0])
                #save_config(get_ip())
                print('Config saved')
                return addr[0]
        except Exception as e:
            print(e)
            time.sleep(1)
            continue


if __name__ == '__main__':
    while True:
        data, addr = listener_socket.recvfrom(1024)
        if data.decode() == "I am a server":
            print(f'WS2_SERV_ADDR: {addr[0]}')
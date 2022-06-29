import socket
import time

serv_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
serv_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
serv_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
serv_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
serv_socket.bind(("", 29802))
serv_socket.settimeout(0.1)


def start():
    while True:
        serv_socket.sendto(b'I am a server', ("255.255.255.255", 29802))
        time.sleep(2)

if __name__ == '__main__':
    start()
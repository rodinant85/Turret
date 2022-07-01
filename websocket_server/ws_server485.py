import asyncio
import logging
import time
import serial.rs485
import os
import websockets
from websockets import WebSocketServerProtocol
from threading import Timer
import RPi.GPIO as GPIO

txen = 22
GPIO.setmode(GPIO.BCM)
GPIO.setup(txen, GPIO.OUT)


logging.basicConfig(level=logging.INFO)

def usb_ports():
    response = os.popen(f'ls /dev | grep  ttyUSB').read()
    usb_list = response.split('\n')[:-1]
    return usb_list

usb_ports = usb_ports()

if len(usb_ports) != 0:
    port = usb_ports[0]
else:
    port = 'serial0'

logging.info(f'Serial port: /dev/{port}')

ser = serial.rs485.RS485(
        port='/dev/serial0',
        baudrate=115200,
        xonxoff=False,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=0.1
    )

ser.rs485_mode = serial.rs485.RS485Settings()


def to_bytes(message: str):
    msg_header = 0xff
    msg_type = 0x00
    zooming_step_coefficient = 0.005
    speed_direction = None

    # {"comm":["MV_HLD:DX:-0.089:DY:0.115"]}
    try:
        string_x, string_y = message.strip('{"comm":["MV_HLD:DX:').strip('"]}').split(":DY:")
        float_x = float(string_x)
        float_y = float(string_y)
    except ValueError as ve:
        print(ve)
        return None

    speed_on_x = int(round(float_x / zooming_step_coefficient))
    speed_on_y = int(round(float_y / zooming_step_coefficient))

    # OY inverted
    speed_on_y = -speed_on_y

    dir_x = 0 if speed_on_x > 0 else 1
    dir_y = 0 if speed_on_y > 0 else 1

    speed_on_x = 255 if abs(speed_on_x) > 255 else abs(speed_on_x)
    speed_on_y = 255 if abs(speed_on_y) > 255 else abs(speed_on_y)

    if dir_x == 0 and dir_y == 0:
        speed_direction = 0x00
    if dir_x == 1 and dir_y == 0:
        speed_direction = 0x01
    if dir_x == 1 and dir_y == 1:
        speed_direction = 0x02
    if dir_x == 0 and dir_y == 1:
        speed_direction = 0x03

    _msg = [msg_header, msg_type, speed_on_x, speed_on_y, speed_direction, 0, 0, 0, 0, 0, 0]
    crc8 = sum(_msg[1:]) % 256
    _msg.append(crc8)
    return _msg


def serial_to_web(serial_data: bytes):
    if len(serial_data) == 12 and sum(serial_data[1:-1]) % 256 == serial_data[11]:
        stp_x = int.from_bytes(serial_data[2:6], byteorder='little', signed=True)
        stp_y = int.from_bytes(serial_data[6:10], byteorder='little', signed=True)
        sw = serial_data[10]
        sw_x_max = 1 if ((sw & 0b00000001) != 0) else 0
        sw_x_min = 1 if ((sw & 0b00000010) != 0) else 0
        sw_y_max = 1 if ((sw & 0b00000100) != 0) else 0
        sw_y_min = 1 if ((sw & 0b00001000) != 0) else 0
        msg = '{"resp":{"STP_X":' + str(stp_x) + ',"STP_Y":' + str(stp_y) + ',"SW_X_MAX":' + str(sw_x_max) + \
              ',"SW_X_MIN":' + str(sw_x_min) + ',"SW_Y_MAX":' + str(sw_y_max) + ',"SW_Y_MIN":' + \
              str(sw_y_min) + '}}'
        return msg
    else:
        ser.reset_input_buffer()
        return None


def stop_motors():
    logging.info('Stop command send')
    motor_stop_command = b'\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
    GPIO.output(txen, GPIO.HIGH)
    for i in range(1, 20):
        ser.write(motor_stop_command)
        time.sleep(0.05)
    GPIO.output(txen, GPIO.LOW)


class Server:
    clients = set()

    def __init__(self, time_to_stop_motors: int = 1):
        self.time_to_stop_motors = time_to_stop_motors
        self.t = Timer(self.time_to_stop_motors, stop_motors)
        #self.t.start()

    async def register(self, ws: WebSocketServerProtocol) -> None:
        self.clients.add(ws)
        logging.info(f'{ws.remote_address} connects')

    async def unregister(self, ws: WebSocketServerProtocol) -> None:
        self.clients.remove(ws)
        logging.info(f'{ws.remote_address} disconnect')

    async def send_to_clients(self, message: str) -> None:
        if self.clients:
            await asyncio.wait([client.send(message) for client in self.clients])

    async def send_to_last_client(self, message: str, ws: WebSocketServerProtocol) -> None:
        if self.clients:
            await asyncio.wait([client.send(message) for client in self.clients])

    async def ws_handler(self, ws: WebSocketServerProtocol, uri: str) -> None:
        await self.register(ws)
        try:
            await self.distribute(ws)
        finally:
            await self.unregister(ws)

    async def distribute(self, ws: WebSocketServerProtocol) -> None:
        async for message in ws:
            web_msg_header = '{"comm":["MV_HLD:'
            if web_msg_header in message:
                self.t.cancel()
                self.t = Timer(self.time_to_stop_motors, stop_motors)
                self.t.start()
                b = to_bytes(message)
                bts = bytes(b)
                try:
                    GPIO.output(txen, GPIO.HIGH)
                    ser.write(bts)
                    GPIO.output(txen, GPIO.LOW)
                except Exception as e:
                    print(e)
            if 'error' in message:
                await self.send_to_clients(message)

            serial_data = ser.read(12)
            if serial_to_web(serial_data) is not None:
                await self.send_to_clients(serial_to_web(serial_data))


async def main():
    server = Server()
    start_server = websockets.serve(server.ws_handler, '', 56779, ping_interval=None)
    async with start_server:
        await asyncio.Future()


asyncio.run(main())


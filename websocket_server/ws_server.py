import asyncio
import logging
import time
import aioserial
import os
import websockets
import system_info
from bitarray import bitarray
from websockets import WebSocketServerProtocol
from threading import Timer


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

ser = aioserial.AioSerial(
    # port='/dev/serial0',
    port='/dev/' + port,
    baudrate=115200,
    xonxoff=False,
    parity=aioserial.PARITY_NONE,
    stopbits=aioserial.STOPBITS_ONE,
    bytesize=aioserial.EIGHTBITS,
    timeout=0.1)


def parse_speed_msg_to_bytes(message: str):
    msg_header = 0xff
    msg_type = 0x00
    zooming_step_coefficient = 0.005

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

    dir_x = 0 if speed_on_x > 0 else 1
    dir_y = 0 if speed_on_y > 0 else 1

    speed_on_x = 255 if abs(speed_on_x) > 255 else abs(speed_on_x)
    speed_on_y = 255 if abs(speed_on_y) > 255 else abs(speed_on_y)

    speed_direction = int.from_bytes(bitarray([0, 0, 0, 0, 0, 0, dir_y, dir_x]), "little")

    _msg = [msg_header, msg_type, speed_direction, speed_on_x, speed_on_y, 0, 0, 0, 0, 0, 0]
    crc8 = sum(_msg[1:]) % 256
    _msg.append(crc8)
    return _msg


def parse_config_msg_to_bytes(message: str):
    msg_header = 0xff
    msg_type = 0x01

    # message example
    # '{"config":{"OX_MAX_FREQ":5000;"OY_MAX_FREQ":6000}}'
    max_freq_x, max_freq_y = message.strip('{"config":{"OX_MAX_FREQ":').strip('}}').split(';"OY_MAX_FREQ":')

    max_freq_x_hi = int(max_freq_x) // 256
    max_freq_x_lo = int(max_freq_x) % 256
    max_freq_y_hi = int(max_freq_y) // 256
    max_freq_y_lo = int(max_freq_y) % 256

    _msg = [msg_header, msg_type, max_freq_x_lo, max_freq_x_hi, max_freq_y_lo, max_freq_y_hi, 0, 0, 0, 0, 0]
    crc8 = sum(_msg[1:]) % 256
    _msg.append(crc8)
    return _msg


def parse_byte_to_str(serial_data: bytes):
    status_msg_type = 0x02
    settings_msg_type = 0x01
    if len(serial_data) == 12 and sum(serial_data[1:-1]) % 256 == serial_data[11]:
        if serial_data[1] == status_msg_type or serial_data[1] == 0x00: # delete serial_data[1] == 0x00 for prod
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

        if serial_data[1] == settings_msg_type:
            max_freq_x = int.from_bytes(serial_data[2:4], byteorder='little', signed=False)
            max_freq_y = int.from_bytes(serial_data[4:6], byteorder='little', signed=False)
            msg = '{"confirm":{"OX_MAX_FREQ":' + str(max_freq_x) + ',"OY_MAX_FREQ":' + str(max_freq_y) + '}}'
            return msg

    else:
        ser.reset_input_buffer()
        return None


async def send_system_info(server):
    while True:
        await server.send_to_clients(system_info.get_all())
        await asyncio.sleep(30)


async def read_serial(server):
    while True:
        serial_data = await ser.read_async(size=12)
        web_message = parse_byte_to_str(serial_data)
        if web_message is not None:
            await server.send_to_clients(web_message)


def stop_motors():
    print('Stop command send')
    motor_stop_command = b'\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
    for i in range(1, 20):
        ser.write(motor_stop_command)
        time.sleep(0.05)


class Server:
    clients = set()

    def __init__(self, time_to_stop_motors: int = 1):
        self.time_to_stop_motors = time_to_stop_motors
        self.t = Timer(self.time_to_stop_motors, stop_motors)
        # self.t.start()

    async def register(self, ws: WebSocketServerProtocol) -> None:
        self.clients.add(ws)
        logging.info(f'{ws.remote_address} connects')
        await self.send_to_clients(system_info.get_all())

    async def unregister(self, ws: WebSocketServerProtocol) -> None:
        self.clients.remove(ws)
        logging.info(f'{ws.remote_address} disconnect')

    async def send_to_clients(self, message: str) -> None:
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
            command_msg_header = '{"comm":["MV_HLD:'
            config_msg_header = '{"config":'
            error_msg_header = '{"error":'
            if message.startswith(command_msg_header):
                self.t.cancel()
                self.t = Timer(self.time_to_stop_motors, stop_motors)
                self.t.start()
                b = parse_speed_msg_to_bytes(message)
                bts = bytes(b)
                try:
                    await ser.write_async(bts)
                    # await asyncio.sleep(0.05)
                except Exception as e:
                    print(e)

            if message.startswith(config_msg_header):
                b = parse_config_msg_to_bytes(message)
                bts = bytes(b)
                try:
                    await ser.write_async(bts)
                    # await asyncio.sleep(0.05)
                except Exception as e:
                    print(e)

            if message.startswith(error_msg_header):
                await self.send_to_clients(message)


async def main():
    server = Server()
    start_server = websockets.serve(server.ws_handler, '', 56779, ping_interval=None)
    task_serial = asyncio.create_task(read_serial(server))
    task_info = asyncio.create_task(send_system_info(server))
    async with start_server:
        await asyncio.Future()
    await task_serial
    await task_info


asyncio.run(main())


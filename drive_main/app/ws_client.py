import time
from websocket import create_connection


def server_ready(address='ws://127.0.0.1:56779', time_for_check=15):
    timer_start = time.time()
    while True:
        try:
            if (time.time() - timer_start) > time_for_check:
                return False
            ws = create_connection(address)
            if ws:
                ws.send('OK')
                time.sleep(0.5)
                ws.close()
                return True
        except Exception:
            time.sleep(1)
            continue


def send_error(text_error: str):
    ws = create_connection(f'ws://127.0.0.1:56779')
    ws.send(f'{{"error":"{text_error}"}}')
    time.sleep(0.5)
    ws.close()


def send(message, ip='127.0.0.1'):
    try:
        ws = create_connection(f'ws://{ip}:56779')
        if ws:
            ws.send(message)
            time.sleep(0.5)
            ws.close()
    except Exception:
        print('Message not send')
        pass


if __name__ == '__main__':
    if server_ready():
        print('WS ready')
    else:
        print('WS not ready')
    send_error('ERROR')

import time
from websocket import create_connection


def ready(ip='127.0.0.1', time_for_check=60):
    timer_start = time.time()
    while True:
        try:
            if (time.time() - timer_start) > time_for_check:
                return False
            ws = create_connection(f'ws://{ip}:56779')
            if ws:
                time.sleep(0.5)
                ws.close()
                return True
        except Exception:
            time.sleep(1)
            continue


def send_error(text_error: str, ip='127.0.0.1'):
    try:
        ws = create_connection(f'ws://{ip}:56779')
        if ws:
            ws.send(f'{{"error":"{text_error}"}}')
            time.sleep(0.5)
            ws.close()
    except Exception:
        print('Message not send')
        pass

        
def send(message, ip='192.168.88.54'):
    try:
        ws = create_connection(f'ws://{ip}:56779')
        if ws:
            ws.send(message)
            print('send')
            time.sleep(0.5)
            ws.close()
    except Exception:
        print('Message not send')
        pass


if __name__ == '__main__':
    if ready(ip='0.0.0.0'):
        print('WS ready')
    else:
        print('WS not ready')
    send_error('ERROR', ip='0.0.0.0')

import datetime
import json
import psutil


def get_ip_address(wifi=False):
    # wifi or eth
    ip = psutil.net_if_addrs()["eth0"][0].address
    if wifi:
        ip = psutil.net_if_addrs()["wlan0"][0].address
        return f"wl {ip}"
    return f"et {ip}"


def get_uptime():
    uptime = datetime.datetime.now() - datetime.datetime.fromtimestamp(round(psutil.boot_time()))
    uptimehours = uptime.seconds // 3600
    uptimeminutes = (uptime.seconds % 3600) // 60
    print(uptimeminutes)
    return f'{uptimehours} hours : {uptimeminutes} min'


def get_last_reboot():
    firsttimestamp = datetime.datetime.fromtimestamp(psutil.boot_time()).strftime("%d.%m.%y-%H:%M:%S")
    return f"{firsttimestamp}"


def get_cpu_temperature():
    temp = psutil.sensors_temperatures()["cpu_thermal"][0].current
    return f"{temp} C"


def get_ram_usage():
    total = psutil.virtual_memory().total // (2 ** 20)
    used = psutil.virtual_memory().used // (2 ** 20)
    percent = psutil.virtual_memory().percent
    return f"{total}M, {percent}%"


def get_cpu_usage():
    current_usage = psutil.cpu_percent(interval=None)
    max_usage = round(psutil.cpu_freq().current / 1000, 2)  # GHz
    return f"{max_usage}H, {current_usage}%"


def get_disk_usage():
    total = psutil.disk_usage('/').total // (2 ** 30)
    used = psutil.disk_usage('/').used // (2 ** 30)
    print(used)
    percent = psutil.disk_usage('/').used / psutil.disk_usage('/').total * 100
    return f"{total}G, {round(percent, 2)}%"


def get_all():
    message = {"system_info": {"CPU Temp (camera plate)": get_cpu_temperature(),
                           "SD (camera plate)": get_disk_usage(),
                           "RAM (camera plate)": get_ram_usage(),
                           "CPU (camera plate)": get_cpu_usage(),
                           "Uptime (camera plate)": get_uptime(),
                           "Last reboot (camera plate)": get_last_reboot(),
                           "IP (camera plate)": get_ip_address()}}
    json_msg = json.dumps(message)
    return json_msg


if __name__ == '__main__':
    print(get_all())

import time
import json
import socket
import random
from datetime import datetime

if __name__ == '__main__':
    data = {
        "name": "dcdc",
        "timestamp": None,
        "online_status": 1,
        "data": [2.14] * 64,
        "EPO 故障标志": 1,
    }
    data2 = {
        "name": "dcdc2",
        "timestamp": None,
        "online_status": 0,
        "data": [1] * 64,
        "高压侧硬件过压标志": 1,
    }
    data3 = {
        "name": "pcs",
        "timestamp": None,
        "online_status": 1,
        "data": [123] * 64,
        "IGBT OCP标志": 0,
    }
    data4 = {
        "name": "pcs2",
        "timestamp": None,
        "online_status": 1,
        "data": [456] * 64,
        "IGBT OCP标志": 1,
    }

    jsonServer = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    jsonServer.bind(('localhost', 8081))
    jsonServer.listen(5)

    conn, addr = jsonServer.accept()

    while True:
        try:
            print("connected from", addr[0], ":", addr[1])
            data['data'] = [random.randint(0, 100) for _ in range(65)]
            data2['data'] = [round(random.uniform(0, 100), 2) for _ in range(65)]
            data['timestamp'] = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            data2['timestamp'] = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            data3['timestamp'] = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            data4['timestamp'] = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            # data['高压侧硬件过压标志'] = random.randint(0, 1)
            json_str = json.dumps(data).encode('utf-8')
            length = len(json_str).to_bytes(4, byteorder='big')
            conn.sendall(length + json_str)
            print('json_str:', json_str)
            time.sleep(0.1)
            json_str1 = json.dumps(data2).encode('utf-8')
            length = len(json_str1).to_bytes(4, byteorder='big')
            conn.sendall(length + json_str1)
            print('json_str2:', json_str1)
            time.sleep(0.1)
            json_str3 = json.dumps(data3).encode('utf-8')
            length = len(json_str3).to_bytes(4, byteorder='big')
            conn.sendall(length + json_str3)
            print('json_str3:', json_str3)
            time.sleep(0.1)
            json_str4 = json.dumps(data4).encode('utf-8')
            length = len(json_str4).to_bytes(4, byteorder='big')
            conn.sendall(length + json_str4)
            print('json_str4:', json_str4)
            time.sleep(0.1)
        except Exception as e:
            print("断开连接......等待客户端连接......", str(e))
            conn, addr = jsonServer.accept()
            time.sleep(2)
            continue

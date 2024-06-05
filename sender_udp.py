import socket
import time
import random


UDP_IP = "localhost"
UDP_PORT = 35393

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

while True:
    data = []
    for row in range(8):
        data.append([])
        for col in range(18 - row):
            power = random.randint(0, 499)
            data[row].append(str(power))
        data[row] = ','.join(data[row])
    data = ':'.join(data)
    sock.sendto(data.encode(), (UDP_IP, UDP_PORT))
    print('сообщение отправлено', data, sep='\n')
    time.sleep(1)



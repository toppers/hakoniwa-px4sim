import socket
import threading
from pymavlink import mavutil


qgc_ipaddr = '192.168.11.56'
px4_ipaddr = '127.0.0.1'
px4_portno = 18570
my_ipaddr = '192.168.11.33'
my_portno = 54001

# 起動時のログ出力
print(f"FROM QGC({qgc_ipaddr}) TO PX4({px4_ipaddr}:{px4_portno})")

# 受信するソケットを作成
recv_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
recv_socket.bind((my_ipaddr, my_portno))

# 転送先のアドレスとポートを設定
forward_address = (px4_ipaddr, px4_portno)

while True:
    # データを受信
    data, addr = recv_socket.recvfrom(1024)

    # データを転送
    if data:
        send_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        send_socket.sendto(data, forward_address)
        send_socket.close()
        print(f"Forwarded data to {forward_address}")

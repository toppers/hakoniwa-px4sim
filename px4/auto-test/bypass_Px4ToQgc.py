import socket
import threading
from pymavlink import mavutil


qgc_ipaddr = '192.168.11.56'
qgc_portno = 14550
px4_ipaddr = '127.0.0.1'
my_ipaddr = '127.0.0.1'
ether_ipaddr = '192.168.11.33'

# 起動時のログ出力
print(f"PX4({px4_ipaddr}) TO QGC({qgc_ipaddr}:{qgc_portno})")

# 受信するソケットを作成
recv_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
recv_socket.bind((my_ipaddr, qgc_portno))

# 転送先のアドレスとポートを設定
forward_address = (qgc_ipaddr, qgc_portno)

while True:
    # データを受信
    data, addr = recv_socket.recvfrom(1024)

    # データを転送
    if data:
        send_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        send_socket.sendto(data, forward_address)
        send_socket.close()
        print(f"Forwarded data to {forward_address}")

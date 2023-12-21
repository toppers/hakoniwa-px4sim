from pymavlink import mavutil
import threading

# PX4とQGroundControlのIPアドレスとポート番号を設定
px4_ipaddr = '127.0.0.1'  # PX4のIPアドレス
my_ipaddr = '192.168.11.33'   # EthernetのIPアドレス
qgc_ipaddr = '192.168.11.56'  # QGroundControlのIPアドレス

mavlink_connection_qgc_in = mavutil.mavlink_connection(f'udpin:{my_ipaddr}:54001')
mavlink_connection_qgc = mavutil.mavlink_connection(f'udpout:{qgc_ipaddr}:14550')
mavlink_connection_px4 = mavutil.mavlink_connection(f'udpout:127.0.0.1:18570')

bypass_thread_activate_flag = False

def bypass_Px4ToQgc():
    while True:
        # PX4からのメッセージを受信する
        msg = mavlink_connection_px4.recv_match(blocking=True)
        if msg is not None:
            message_type = msg.get_type()

            # 受信したメッセージをQGCにバイパスする
            mavlink_connection_qgc.write(msg.get_msgbuf())

def bypass_QgcToPx4():
    while True:
        # QGCからのメッセージを受信する
        msg = mavlink_connection_qgc.recv_match(blocking=True)
        if msg is not None:
            message_type = msg.get_type()
            if message_type != "HEARTBEAT":
                print(f"Received Reply Message with type: {message_type}")
            # 受信したメッセージを PX4にバイパスする
            mavlink_connection_px4.write(msg.get_msgbuf())

while True:
    # QGCからのメッセージを受信する
    msg = mavlink_connection_qgc_in.recv_match(blocking=True)
    if msg is not None:

        if bypass_thread_activate_flag == False:
            #受信したQGCのメッセージをPX4にバイパスすする
            message_type = msg.get_type()
            if message_type != "HEARTBEAT":
                print(f"Received Reply Message with type: {message_type}")
            mavlink_connection_px4.write(msg.get_msgbuf())

            #ここでスレッドを起動して、PX4からのメッセージをQGCへバイパスするスレッドを起動する。
            thread = threading.Thread(target=bypass_Px4ToQgc)
            thread.start()
            thread = threading.Thread(target=bypass_QgcToPx4)
            thread.start()
            bypass_thread_activate_flag = True

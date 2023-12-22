from pymavlink import mavutil
import threading
import json
import argparse
import datetime
import time
import utils.mavlink_msg as mav_msg

parser = argparse.ArgumentParser(description='Qgc command recording Tool')
parser.add_argument('config_path', help='Path to the JSON configuration file')
args = parser.parse_args()

# Read the JSON file
with open(args.config_path, 'r') as json_file:
    config_data = json.load(json_file)

px4_ipaddr = config_data['px4_ipaddr']
px4_portno = config_data['px4_portno']
my_ipaddr = config_data['my_ipaddr']
my_portno = config_data['my_portno']
qgc_ipaddr = config_data['qgc_ipaddr']
qgc_portno = config_data['qgc_portno']
print(f'me:{my_ipaddr}:{my_portno}')
print(f'px4:{px4_ipaddr}:{px4_portno}')
print(f'qgc:{qgc_ipaddr}:{qgc_portno}')

# Establish MAVLink connections
mavlink_connection_qgc_in = mavutil.mavlink_connection(f'udpin:{my_ipaddr}:{my_portno}')
mavlink_connection_qgc = mavutil.mavlink_connection(f'udpout:{qgc_ipaddr}:{qgc_portno}')
mavlink_connection_px4 = mavutil.mavlink_connection(f'udpout:{px4_ipaddr}:{px4_portno}')

bypass_thread_activate_flag = False

def get_current_time_in_usec():
    # 現在の日時を取得
    now = datetime.datetime.now()
    # Unixエポックからの経過秒数を計算
    epoch_time = (now - datetime.datetime(1970, 1, 1)).total_seconds()
    # マイクロ秒に変換
    return int(epoch_time * 1000000)


boot_time = get_current_time_in_usec()

def get_rtime_from_boot():
    return get_current_time_in_usec() - boot_time

def bypass_Px4ToQgc():
    while True:
        # PX4からのメッセージを受信する
        msg = mavlink_connection_px4.recv_match(blocking=True)
        if msg is not None:
            message_type = msg.get_type()
            if message_type == "COMMAND_ACK":
                print(f"PX4: {get_rtime_from_boot()}:{message_type}")

            # 受信したメッセージをQGCにバイパスする
            mavlink_connection_qgc.write(msg.get_msgbuf())

def bypass_QgcToPx4():
    while True:
        # QGCからのメッセージを受信する
        msg = mavlink_connection_qgc.recv_match(blocking=True)
        if msg is not None:
            message_type = msg.get_type()
            print(f"QGC: {get_rtime_from_boot()}:{message_type}")
            if message_type == "HEARTBEAT":
                mav_msg.dump_heartbeat(msg)
            if message_type == "COMMAND_LONG":
                mav_msg.dump_command_long(msg)
            if message_type == "COMMAND_INT":
                mav_msg.dump_command_int(msg)
            if message_type == "PING":
                mav_msg.dump_ping(msg)
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

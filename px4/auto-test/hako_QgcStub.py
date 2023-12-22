from pymavlink import mavutil
import threading
import json
import argparse
import datetime
import utils.mavlink_msg as mav_msg
from operations.takeoff import TakeoffOperation
import signal
import time

running = True

parser = argparse.ArgumentParser(description='Qgc Stub Tool')
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
mavlink_connection_px4 = mavutil.mavlink_connection(f'udpout:{px4_ipaddr}:{px4_portno}')

operation = TakeoffOperation(mavlink_connection_px4, 131.3212432861328)


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

def QgcStub():
    global running
    global command_long_ack_recv

    time.sleep(1)
   # 関数が開始した時刻を記録
    start_time = time.time()
    hb_sent_time = 0
    ping_sent_time = 0

    operation.start(start_time)

    while running:
        time.sleep(1)
        current_time = time.time()
        if (current_time - hb_sent_time) > 5:
            hb_msg = mav_msg.create_heartbeat_message()
            #mav_msg.dump_heartbeat(hb_msg)
            mavlink_connection_px4.write(hb_msg.get_msgbuf())
            hb_sent_time = time.time()

        if (current_time - ping_sent_time) > 13:
            ping_msg = mav_msg.create_ping_message()
            #mav_msg.dump_ping(ping_msg)
            mavlink_connection_px4.write(ping_msg.get_msgbuf())
            ping_sent_time = time.time()

        if not operation.is_op_done():
            operation.do_operation(current_time)

def Px4Receiver():
    global running
    global command_long_ack_recv

    while running:
        current_time = time.time()

        # PX4からのメッセージを受信する
        msg = mavlink_connection_px4.recv_match(blocking=True)
        if msg is not None:
            if not operation.is_op_done():
                operation.event_msg(msg)
            message_type = msg.get_type()
            if message_type == "COMMAND_ACK":
                print(f"PX4: {get_rtime_from_boot()}:{message_type}")


thread = threading.Thread(target=QgcStub)
thread.start()
Px4Receiver()

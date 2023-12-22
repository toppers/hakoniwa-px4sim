import datetime
from pymavlink import mavutil
from pymavlink.dialects.v20 import common as mavlink2

def dump_header(msg):
    # メッセージのヘッダ情報を出力
    print("Header:")
    print(f"  Message Type: {msg.get_type()}")
    print(f"  System ID: {msg.get_srcSystem()}")
    print(f"  Component ID: {msg.get_srcComponent()}")

def dump_ping(msg):
    # PING メッセージの内容を出力
    print("PING dump:")
    dump_header(msg)
    print(f"  Seq: {msg.seq}")
    print(f"  Time (usec): {msg.time_usec}")
    print(f"  Target System: {msg.target_system}")
    print(f"  Target Component: {msg.target_component}")


def dump_command_long(msg):
    # COMMAND_LONG メッセージの内容を出力
    print("COMMAND_LONG dump:")
    dump_header(msg)
    print(f"  Command: {msg.command}")
    print(f"  Param1: {msg.param1}")
    print(f"  Param2: {msg.param2}")
    print(f"  Param3: {msg.param3}")
    print(f"  Param4: {msg.param4}")
    print(f"  Param5: {msg.param5}")
    print(f"  Param6: {msg.param6}")
    print(f"  Param7: {msg.param7}")
    print(f"  Target System: {msg.target_system}")
    print(f"  Target Component: {msg.target_component}")
    print(f"  Confirmation: {msg.confirmation}")

def dump_command_int(msg):
    # COMMAND_INT メッセージの内容を出力
    print("COMMAND_INT dump:")
    dump_header(msg)
    print(f"  Command: {msg.command}")
    print(f"  Param1: {msg.param1}")
    print(f"  Param2: {msg.param2}")
    print(f"  Param3: {msg.param3}")
    print(f"  Param4: {msg.param4}")
    print(f"  X: {msg.x}")  # 緯度 or X座標
    print(f"  Y: {msg.y}")  # 経度 or Y座標
    print(f"  Z: {msg.z}")  # 高度 or Z座標
    print(f"  Target System: {msg.target_system}")
    print(f"  Target Component: {msg.target_component}")
    print(f"  Frame: {msg.frame}")
    print(f"  Current: {msg.current}")
    print(f"  Autocontinue: {msg.autocontinue}")

def dump_heartbeat(msg):
    # HEARTBEAT メッセージの内容を出力
    print("HEARTBEAT dump:")
    dump_header(msg)
    print(f"  Type: {msg.type}")
    print(f"  Autopilot: {msg.autopilot}")
    print(f"  Base Mode: {msg.base_mode}")
    print(f"  Custom Mode: {msg.custom_mode}")
    print(f"  System Status: {msg.system_status}")
    print(f"  Mavlink Version: {msg.mavlink_version}")

def create_header(mav):
    mav.srcSystem = 255  # システムIDを0xFFに設定
    mav.srcComponent = 190  # コンポーネントIDを0xBEに設定

def create_heartbeat_message():
    # HEARTBEATメッセージのパラメータを設定
    type = mavutil.mavlink.MAV_TYPE_GCS
    autopilot = mavutil.mavlink.MAV_AUTOPILOT_INVALID
    base_mode = 192
    custom_mode = 0  # カスタムモード（オートパイロット依存）
    system_status = mavutil.mavlink.MAV_STATE_ACTIVE
    mavlink_version = 3

    # HEARTBEATメッセージを作成
    mav = mavlink2.MAVLink(None)
    heartbeat = mavlink2.MAVLink_heartbeat_message(
        type, autopilot, base_mode, custom_mode, system_status, mavlink_version
    )
    create_header(mav)
    msgbuf = heartbeat.pack(mav)
    return heartbeat

# グローバル変数でシーケンス番号を管理
ping_seq_num = 0

def create_ping_message():
    global ping_seq_num
    target_system = 1
    target_component = 1    

    # 現在のUnixタイムスタンプ（マイクロ秒）を取得
    now = datetime.datetime.now()
    epoch = datetime.datetime(1970, 1, 1)
    time_usec = int((now - epoch).total_seconds() * 1_000_000)

    # PINGメッセージを作成
    mav = mavlink2.MAVLink(None)
    ping = mavlink2.MAVLink_ping_message(
        time_usec, ping_seq_num, target_system, target_component
    )
    create_header(mav)
    msgbuf = ping.pack(mav)

    # シーケンス番号をインクリメント
    ping_seq_num += 1

    return ping

def create_command_long(target_system, target_component, command, param1, param2, param3, param4, param5, param6, param7):
    # MAVLinkインスタンスの作成
    mav = mavlink2.MAVLink(None)

    # COMMAND_LONGメッセージの作成
    command_long = mavlink2.MAVLink_command_long_message(
        target_system, target_component, command, 0, param1, param2, param3, param4, param5, param6, param7
    )

    # メッセージにヘッダ情報を追加
    create_header(mav)
    msgbuf = command_long.pack(mav)
    return command_long

def create_command_long_takeoff(alt: float):
    target_system = 1
    target_component = 1
    command = mavutil.mavlink.MAV_CMD_NAV_TAKEOFF
    param1 = -1.0  # 通常は無視される（-1はデフォルト値を意味する）
    param2 = 0.0
    param3 = 0.0
    param4 = float('nan')  # nan（Not a Number）を使用
    param5 = float('nan')
    param6 = float('nan')
    param7 = alt # 目標高度（メートル）

    return create_command_long(target_system, target_component, command, param1, param2, param3, param4, param5, param6, param7)


def create_command_long_arm(arm=True):
    target_system = 1
    target_component = 1

    command = mavutil.mavlink.MAV_CMD_COMPONENT_ARM_DISARM
    param1 = 1.0 if arm else 0.0  # 1.0: アーム, 0.0: ディスアーム
    param2 = 0.0
    param3 = 0.0
    param4 = 0.0
    param5 = 0.0
    param6 = 0.0
    param7 = 0.0

    return create_command_long(target_system, target_component, command, param1, param2, param3, param4, param5, param6, param7)


#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import hakosim
import hakopy
import hako_pdu
import pdu_info
import os
import time

config_path = ''

def my_on_initialize(context):
    global config_path
    robot_name = 'DroneTransporter'
    hako_binary_path = os.getenv('HAKO_BINARY_PATH', '/usr/local/lib/hakoniwa/hako_binary/offset')
    pdu_manager = hako_pdu.HakoPduManager(hako_binary_path, config_path)
    pdu = pdu_manager.get_pdu(robot_name, pdu_info.HAKO_AVATOR_CHANNLE_ID_COLLISION)
    pdu_data = pdu.get()
    pdu_data['collision'] = False
    pdu_data['contact_num'] = 0
    pdu.write()

    pdu = pdu_manager.get_pdu(robot_name, pdu_info.HAKO_AVATOR_CHANNEL_ID_DISTURB)
    pdu_data = pdu.get()
    pdu.write()
    pdu = pdu_manager.get_pdu(robot_name, pdu_info.HAKO_AVATOR_CHANNEL_ID_CAMERA_DATA)
    pdu_data = pdu.get()
    pdu.write()
    pdu = pdu_manager.get_pdu(robot_name, pdu_info.HAKO_AVATOR_CHANNEL_ID_CAMERA_INFO)
    pdu_data = pdu.get()
    pdu.write()
    pdu = pdu_manager.get_pdu(robot_name, pdu_info.HAKO_AVATOR_CHANNEL_ID_LIDAR_DATA)
    pdu_data = pdu.get()
    pdu.write()
    pdu = pdu_manager.get_pdu(robot_name, pdu_info.HAKO_AVATOR_CHANNEL_ID_LIDAR_POS)
    pdu_data = pdu.get()
    pdu.write()
    pdu = pdu_manager.get_pdu(robot_name, pdu_info.HAKO_AVATOR_CHANNEL_ID_STAT_MAG)
    pdu_data = pdu.get()
    pdu.write()
    return 0

def my_on_reset(context):
    return 0


def button_event(client, index):
    data = client.getGameJoystickData()
    data['button'] = list(data['button'])

    hakopy.usleep(500000)
    data['button'][index] = True
    client.putGameJoystickData(data)
    hakopy.usleep(500000)
    data['button'][index] = False
    client.putGameJoystickData(data)

def takeoff(client, height):
    print("START TAKEOFF: ", height)
    pose = client.simGetVehiclePose()
    while (pose.position.z_val) < height:
        pose = client.simGetVehiclePose()
        data = client.getGameJoystickData()
        data['axis'] = list(data['axis']) 
        data['axis'][1] = -0.5
        #data['axis'][0] = 1.0 #heading
        #data['axis'][2] = 1.0 #vy
        #data['axis'][3] = 1.0 #vx
        client.putGameJoystickData(data)
        hakopy.usleep(30000)

    data = client.getGameJoystickData()
    data['axis'] = list(data['axis']) 
    data['axis'][1] = 0
    client.putGameJoystickData(data)
    print("DONE")

def reply_and_wait_res(command):
    ret = command.write()
    if ret == False:
        print('"ERROR: hako_asset_pdu_write')
        return False
    while True:
        pdu = command.read()
        if pdu == None:
            print('ERROR: hako_asset_pdu_read')
            return 0
        if pdu['header']['result'] == 1:
            pdu['header']['result'] = 0
            command.write()
            print('DONE')
            break
        #print("result: ",  pdu['header']['result'])
        hakopy.usleep(30000)
    return True

def takeoff_wait(client, height):
    print("INFO: takeoff")
    command, pdu_cmd = client.get_packet(pdu_info.HAKO_AVATOR_CHANNEL_ID_CMD_TAKEOFF, client.get_vehicle_name(client.default_drone_name))
    pdu_cmd['height'] = height
    pdu_cmd['speed'] = 5
    pdu_cmd['yaw_deg'] = client._get_yaw_degree(client.default_drone_name)
    return reply_and_wait_res(command)

def almost_equal_deg(target_deg, real_deg, diff_deg):
    if abs(target_deg - real_deg) <= diff_deg:
        return True
    else:
        return False

def stop_control(client):
    while True:
        data = client.getGameJoystickData()
        data['axis'] = list(data['axis'])
        data['axis'][0] = 0.0 #heading
        data['axis'][1] = 0.0 #up/down
        data['axis'][2] = 0.0
        data['axis'][3] = 0.0
        client.putGameJoystickData(data)
        hakopy.usleep(30000)

def do_control(client, v1 = 0, v2 = 0, type = 'angular'):
    global target_values
    print(f"START CONTROL: v1({v1}) v2({v2})")
    while True:
        data = client.getGameJoystickData()
        data['axis'] = list(data['axis'])
        data['axis'][0] = 0.0 #heading
        data['axis'][1] = 0.0 #up/down
        #data['axis'][2] = 0.0 #roll
        #print(f"v1: {v1}, v2: {v2}")
        #print("vx: ",  target_values.get_ctrl_value('Vx'))
        #print("vy: ",  target_values.get_ctrl_value('Vy'))

        data['axis'][2] = 0.0
        data['axis'][3] = 0.0
        if v1 > 0:
            if type == 'angular':
                data['axis'][2] = target_values.get_ctrl_value('Rx')
            elif type == 'speed':
                data['axis'][3] = -target_values.get_ctrl_value('Vx')
        elif v1 < 0:
            if type == 'angular':
                data['axis'][2] = -target_values.get_ctrl_value('Rx')
            elif type == 'speed':
                data['axis'][3] = target_values.get_ctrl_value('Vx')

        if v2 > 0:
            if type == 'angular':
                data['axis'][3] = -target_values.get_ctrl_value('Ry')
            elif type == 'speed':
                data['axis'][2] = target_values.get_ctrl_value('Vy')
        elif v2 < 0:
            if type == 'angular':
                data['axis'][3] = target_values.get_ctrl_value('Ry')
            elif type == 'speed':
                data['axis'][2] = -target_values.get_ctrl_value('Vy')
        #print("axis2: ", data['axis'][2])
        #print("axis3: ", data['axis'][3])
        client.putGameJoystickData(data)
        hakopy.usleep(30000)

        stop_time = target_values.stop_time_usec
        if (stop_time > 0) and (hakopy.simulation_time() >= stop_time):
            break 

pdu_manager = None
client = None

class TargetValues:
    def __init__(self):
        self.values = {}
        self.max_values = {}
        self.stop_time_usec = -1

    def set_stop_time(self, value: int):
        self.stop_time_usec = value
    
    def set_target(self, key, value, max_value=None):
        self.values[key] = float(value)
        if max_value is not None:
            self.max_values[key] = max_value[key]
        print(f"Target {key}: {self.values[key]}")

    def get_target_value(self, key):
        if key in self.values and key in self.max_values:
            return self.values[key]
        else:
            print(f"Invalid key or missing max value: {key}")
            return None

    def get_ctrl_value(self, key):
        if key in self.values and key in self.max_values:
            return self.values[key] / self.max_values[key]
        else:
            print(f"Invalid key or missing max value: {key}")
            return None

    def has_key(self, key):
        if key in self.values:
            return True
        else:
            return False


target_values = TargetValues()

def pos_control(client, X = 0, Y = 0, speed = 5):
    global target_values
    print(f"START CONTROL: X({X}) Y({Y}) S({speed})")
    #call api
    pose = client.simGetVehiclePose()
    command, pdu_cmd = client.get_packet(pdu_info.HAKO_AVATOR_CHANNEL_ID_CMD_MOVE, client.get_vehicle_name(client.default_drone_name))
    pdu_cmd['x'] = X
    pdu_cmd['y'] = Y
    pdu_cmd['z'] = pose.position.z_val
    pdu_cmd['speed'] = speed
    pdu_cmd['yaw_deg'] = 0
    reply_and_wait_res(command)
    print("reply done")
    while True:
        hakopy.usleep(30000)

        stop_time = target_values.stop_time_usec
        if (stop_time > 0) and (hakopy.simulation_time() >= stop_time):
            break 

def my_on_manual_timing_control(context):
    global pdu_manager
    global client
    global target_values
    print("INFO: on_manual_timing_control enter")

    # takeoff
    if (target_values.has_key('X')):
        height = 3
        if (target_values.has_key('Z')):
            evaluation_start_time = hakopy.simulation_time() * 1e-06
            height = -target_values.values['Z']
        takeoff_wait(client, height)
        if (target_values.has_key('Z')):
            hakopy.usleep(100000000)
    else:
        # start
        button_event(client, 0)
        takeoff(client, 3)

    if (target_values.has_key('Z')) == False:
        evaluation_start_time = hakopy.simulation_time() * 1e-06
    print("EVALUATION_START_TIME: ", evaluation_start_time)
    with open('/tmp/v.txt', 'w') as f:
        f.write(str(evaluation_start_time))
    
    if (target_values.has_key('Rx')):
        do_control(client, target_values.values['Rx'], -target_values.values['Ry'], 'angular')
    elif (target_values.has_key('Vx')):
        do_control(client, target_values.values['Vx'], target_values.values['Vy'], 'speed')
    elif (target_values.has_key('Z')):
        pass
    elif (target_values.has_key('X')):
        pos_control(client, target_values.values['X'], target_values.values['Y'], target_values.values['S'])

    if (target_values.has_key('X')) == False:
        print("INFO: start stop control")
        evaluation_start_time = hakopy.simulation_time() * 1e-06
        print("EVALUATION_START_TIME: ", evaluation_start_time)
        with open('/tmp/v.txt', 'w') as f:
            f.write(str(evaluation_start_time))
        stop_control(client)

    #for _ in range(0,3):
    #    # sleep 1sec
    #    hakopy.usleep(1000000)
    print("INFO: on_manual_timing_control exit")
    return 0

my_callback = {
    'on_initialize': my_on_initialize,
    'on_simulation_step': None,
    'on_manual_timing_control': my_on_manual_timing_control,
    'on_reset': my_on_reset
}

def main():
    global client
    global config_path
    global target_values

    if len(sys.argv) != 5 and len(sys.argv) != 6:
        print(f"Usage: {sys.argv[0]} <config_path> <stop_time> <key:value> <key:value> [S:TargetSpeed]")
        return 1

    asset_name = 'DronePlantModel'
    config_path = sys.argv[1]
    delta_time_usec = 3000

    stop_time = int(sys.argv[2])
    target_values.set_stop_time(stop_time)

    max_value = {}
    if (sys.argv[3].split(':')[0] == 'Rx') or (sys.argv[3].split(':')[0] == 'Ry'):
        max_value['Rx'] = 20
        max_value['Ry'] = 20
        target_values.set_target(sys.argv[3].split(':')[0], sys.argv[3].split(':')[1], max_value)
        target_values.set_target(sys.argv[4].split(':')[0], sys.argv[4].split(':')[1], max_value)
    elif (sys.argv[3].split(':')[0] == 'Vx') or (sys.argv[3].split(':')[0] == 'Vy'):
        max_value['Vx'] = 10
        max_value['Vy'] = 10
        target_values.set_target(sys.argv[3].split(':')[0], sys.argv[3].split(':')[1], max_value)
        target_values.set_target(sys.argv[4].split(':')[0], sys.argv[4].split(':')[1], max_value)
    elif (sys.argv[3].split(':')[0] == 'Z'):
        target_values.set_target(sys.argv[3].split(':')[0], sys.argv[3].split(':')[1])
        target_values.set_target(sys.argv[4].split(':')[0], sys.argv[4].split(':')[1])
        target_values.set_target(sys.argv[5].split(':')[0], sys.argv[5].split(':')[1])
    elif (sys.argv[3].split(':')[0] == 'X') or (sys.argv[3].split(':')[0] == 'Y'):
        target_values.set_target(sys.argv[3].split(':')[0], sys.argv[3].split(':')[1])
        target_values.set_target(sys.argv[4].split(':')[0], sys.argv[4].split(':')[1])
        if len(sys.argv) == 6:
            target_values.set_target(sys.argv[5].split(':')[0], sys.argv[5].split(':')[1])
        else:
            target_values.set_target('S', 5)

    # connect to the HakoSim simulator
    client = hakosim.MultirotorClient(config_path)
    client.default_drone_name = "DroneTransporter"
    client.pdu_manager = hako_pdu.HakoPduManager('/usr/local/lib/hakoniwa/hako_binary/offset', config_path)
    client.enableApiControl(True)
    client.armDisarm(True)

    ret = hakopy.asset_register(asset_name, config_path, my_callback, delta_time_usec, hakopy.HAKO_ASSET_MODEL_PLANT)
    if ret == False:
        print(f"ERROR: hako_asset_register() returns {ret}.")
        return 1

    ret = hakopy.start()
    print(f"INFO: hako_asset_start() returns {ret}")

    return 0

if __name__ == "__main__":
    sys.exit(main())

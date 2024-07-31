#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import hakosim
import hakopy
import hako_pdu
import pdu_info
import os
import math

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

def almost_equal_deg(target_deg, real_deg, diff_deg):
    if abs(target_deg - real_deg) <= diff_deg:
        return True
    else:
        return False

def angular_control(client, phi = 0, theta = 0, psi = 0):
    print("START ANGULAR CONTROL: ", phi)
    diff_deg = 0.1
    pose = client.simGetVehiclePose()
    roll, pitch, yaw = hakosim.hakosim_types.Quaternionr.quaternion_to_euler(pose.orientation)
    while (almost_equal_deg(phi, math.degrees(roll), diff_deg) == False) or (almost_equal_deg(theta, math.degrees(pitch), diff_deg) == False):
        pose = client.simGetVehiclePose()
        roll, pitch, yaw = hakosim.hakosim_types.Quaternionr.quaternion_to_euler(pose.orientation)
        data = client.getGameJoystickData()
        data['axis'] = list(data['axis'])
        data['axis'][0] = 0.0 #heading
        data['axis'][1] = 0.0 #up/down
        data['axis'][2] = 0.0 #roll
        if almost_equal_deg(phi, math.degrees(roll), diff_deg) == False:
            if phi > 0:
                data['axis'][2] = 1.0
            else:
                data['axis'][2] = -1.0
            print(f"NONE phi: {phi}, roll: {math.degrees(roll)}")
        else:
            print(f"DONE phi: {phi}, roll: {math.degrees(roll)}")

        data['axis'][3] = 0.0 #pitch
        if almost_equal_deg(theta, math.degrees(pitch), diff_deg) == False:
            if theta > 0:
                data['axis'][3] = -1.0
            else:
                data['axis'][3] = 1.0

            print(f"NONE theta: {theta}, pitch: {math.degrees(pitch)}")
        else:
            print(f"DONE theta: {theta}, pitch: {math.degrees(pitch)}")


        client.putGameJoystickData(data)
        hakopy.usleep(30000)

    data = client.getGameJoystickData()
    data['axis'] = list(data['axis']) 
    data['axis'][1] = 0
    client.putGameJoystickData(data)
    print("DONE")

pdu_manager = None
client = None
def my_on_manual_timing_control(context):
    global pdu_manager
    global client
    print("INFO: on_manual_timing_control enter")

    # start
    button_event(client, 0)
    # takeoff
    takeoff(client, 3)

    angular_control(client, 20, -20, 0)
    #angular_control(client, 20, 0, 0)
    #angular_control(client, 0, -20, 0)

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

    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <config_path>")
        return 1

    asset_name = 'DronePlantModel'
    config_path = sys.argv[1]
    delta_time_usec = 3000

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

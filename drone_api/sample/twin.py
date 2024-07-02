#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import hakosim
import time
import math
import numpy
import pprint

import hako_pdu
import pdu_info

def get_robot_pos(client, name, channel_id):
    pdu = client.pdu_manager.get_pdu(name, channel_id)
    pdu_data = pdu.read()
    pdu_data['linear']['y'] = -pdu_data['linear']['y']
    return pdu_data

def transport(client, baggage_pos, transfer_pos):
    h = 1.0
    client.moveToPosition(baggage_pos['x'], baggage_pos['y'], h, 0, 90)
    client.moveToPosition(baggage_pos['x'], baggage_pos['y'], h, 5)
    client.moveToPosition(baggage_pos['x'], baggage_pos['y'], h, 5, 0)
    client.moveToPosition(baggage_pos['x'], baggage_pos['y'], 0.1, 0.01, 0)
    client.grab_baggage(True)
    client.moveToPosition(baggage_pos['x'], baggage_pos['y'], h, 0.01)
    client.moveToPosition(transfer_pos['x'], transfer_pos['y'], h, 0.1)
    client.moveToPosition(transfer_pos['x'], transfer_pos['y'], transfer_pos['z'], 0.01)
    client.grab_baggage(False)
    client.moveToPosition(transfer_pos['x'], transfer_pos['y'], h, 0.01)

def debug_pos(client):
    pose = client.simGetVehiclePose()
    print(f"POS  : {pose.position.x_val} {pose.position.y_val} {pose.position.z_val}")
    roll, pitch, yaw = hakosim.hakosim_types.Quaternionr.quaternion_to_euler(pose.orientation)
    print(f"ANGLE: {math.degrees(roll)} {math.degrees(pitch)} {math.degrees(yaw)}")


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <config_path>")
        return 1

    # connect to the HakoSim simulator
    client = hakosim.MultirotorClient(sys.argv[1])
    client.confirmConnection()
    client.enableApiControl(True)
    client.armDisarm(True)


    client.takeoff(1)
    robo_pos = get_robot_pos(client, "TB3RoboAvatar", 0)
    print(f'robo_pos({robo_pos['linear']['x']}, {robo_pos['linear']['y']}, {robo_pos['linear']['z']})')
    baggage_pos = { "x": 0.0, "y": -4 }
    transfer_pos = { "x": robo_pos['linear']['x'], "y": robo_pos['linear']['y'], "z": 0.1 }
    transport(client, baggage_pos, transfer_pos)
    debug_pos(client)
    robo_pos = get_robot_pos(client, "TB3RoboAvatar", 0)
    print(f'robo_pos({robo_pos['linear']['x']}, {robo_pos['linear']['y']}, {robo_pos['linear']['z']})')

    client.moveToPosition(baggage_pos['x'], baggage_pos['y'], 1.0, 0, 90)

    while True:
        time.sleep(5)
        robo_pos = get_robot_pos(client, "TB3RoboAvatar", 0)
        print(f'robo_pos({robo_pos['linear']['x']}, {robo_pos['linear']['y']}, {robo_pos['linear']['z']})')
        if robo_pos['linear']['x'] >= 2.0:
            break

    time.sleep(1)
    robo_pos = get_robot_pos(client, "TB3RoboAvatar", 0)
    print(f'robo_pos({robo_pos['linear']['x']}, {robo_pos['linear']['y']}, {robo_pos['linear']['z']})')
    baggage_pos = { "x": robo_pos['linear']['x'], "y": robo_pos['linear']['y'] }
    transfer_pos = { "x": 0.0, "y": 0, "z": 0.1 }
    transport(client, baggage_pos, transfer_pos)
    debug_pos(client)
    robo_pos = get_robot_pos(client, "TB3RoboAvatar", 0)
    print(f'robo_pos({robo_pos['linear']['x']}, {robo_pos['linear']['y']}, {robo_pos['linear']['z']})')

    client.land()
    debug_pos(client)

    return 0

if __name__ == "__main__":
    sys.exit(main())

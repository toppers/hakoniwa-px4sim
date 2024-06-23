#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import hakosim
import time
import math
import numpy
import pprint

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
    baggage_pos = { "x": 0.0, "y": -4 }
    transfer_pos = { "x": 0.17, "y": 0, "z": 0.1 }
    transport(client, baggage_pos, transfer_pos)
    debug_pos(client)

    client.moveToPosition(baggage_pos['x'], baggage_pos['y'], 1.0, 0, 90)

    time.sleep(10)

    baggage_pos = { "x": 1.01, "y": -0.031 }
    transfer_pos = { "x": 0.0, "y": 0, "z": 0.1 }
    transport(client, baggage_pos, transfer_pos)
    debug_pos(client)

    client.land()
    debug_pos(client)

    return 0

if __name__ == "__main__":
    sys.exit(main())

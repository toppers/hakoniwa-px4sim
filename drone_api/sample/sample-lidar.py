#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import hakosim
import time
import math
import numpy
import pprint

def transport(client, baggage_pos, transfer_pos):
    client.moveToPosition(baggage_pos['x'], baggage_pos['y'], 3, 5)
    time.sleep(3)
    client.moveToPosition(baggage_pos['x'], baggage_pos['y'], 0.7, 0.01)
    time.sleep(3)
    client.grab_baggage(True)
    time.sleep(3)
    client.moveToPosition(transfer_pos['x'], transfer_pos['y'], 3, 5)
    time.sleep(3)
    client.moveToPosition(transfer_pos['x'], transfer_pos['y'], transfer_pos['z'], 0.01)
    time.sleep(3)
    client.grab_baggage(False)
    time.sleep(3)
    client.moveToPosition(transfer_pos['x'], transfer_pos['y'], 3, 0.01)

def debug_pos(client):
    pose = client.simGetVehiclePose()
    print(f"POS  : {pose.position.x_val} {pose.position.y_val} {pose.position.z_val}")
    roll, pitch, yaw = hakosim.hakosim_types.Quaternionr.quaternion_to_euler(pose.orientation)
    print(f"ANGLE: {math.degrees(roll)} {math.degrees(pitch)} {math.degrees(yaw)}")

def parse_lidarData(data):

    # reshape array of floats to array of [X,Y,Z]
    points = numpy.array(data.point_cloud, dtype=numpy.dtype('f4'))
    points = numpy.reshape(points, (int(points.shape[0]/3), 3))
    
    return points

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <config_path>")
        return 1

    # connect to the HakoSim simulator
    client = hakosim.MultirotorClient(sys.argv[1])
    client.confirmConnection()
    client.enableApiControl(True)
    client.armDisarm(True)
    client.takeoff(3)

    lidarData = client.getLidarData()
    if (len(lidarData.point_cloud) < 3):
        print("\tNo points received from Lidar data")
    else:
        print(f"len: {len(lidarData.point_cloud)}")
        points = parse_lidarData(lidarData)
        print("\tReading: time_stamp: %d number_of_points: %d" % (lidarData.time_stamp, len(points)))
        print("\t\tlidar position: %s" % (pprint.pformat(lidarData.pose.position)))
        print("\t\tlidar orientation: %s" % (pprint.pformat(lidarData.pose.orientation)))
    
        condition = numpy.logical_and(points <= 3, points > 0)
        filtered_points = points[numpy.any(condition, axis=1)]
        print(filtered_points)
    
    baggage_pos = { "x": 0, "y": 3 }
    transfer_pos = { "x": 0, "y": -1, "z": 0.7 }
    transport(client, baggage_pos, transfer_pos)
    debug_pos(client)

    baggage_pos = { "x": 0, "y": 4 }
    transfer_pos = { "x": 0, "y": -1, "z": 1.2 }
    transport(client, baggage_pos, transfer_pos)
    debug_pos(client)

    client.moveToPosition(-2, -1, 3, 5)
    debug_pos(client)
    time.sleep(3)
    client.moveToPosition(-2, -1, 0.7, 5)
    debug_pos(client)
    time.sleep(3)

    png_image = client.simGetImage("0", hakosim.ImageType.Scene)
    if png_image:
        with open("scene.png", "wb") as f:
            f.write(png_image)
    client.land()
    debug_pos(client)

    return 0

if __name__ == "__main__":
    sys.exit(main())

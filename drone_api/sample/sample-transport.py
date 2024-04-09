#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import hakosim
import time

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
    #client.moveToPosition(transfer_pos['x'], transfer_pos['y'], 3, 0.01)


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
    baggage_pos = { "x": 0, "y": 3 }
    transfer_pos = { "x": 0, "y": -3, "z": 3 }
    transport(client, baggage_pos, transfer_pos)
    #baggage_pos = { "x": 0, "y": 4 }
    #transfer_pos = { "x": 0, "y": -3, "z": 3 }
    #transport(client, baggage_pos, transfer_pos)

    client.moveToPosition(-5, -10, 3, 5)
    time.sleep(3)
    client.moveToPosition(-5, -10, 0.7, 5)
    time.sleep(3)

    png_image = client.simGetImage("0", hakosim.ImageType.Scene)
    if png_image:
        with open("scene.png", "wb") as f:
            f.write(png_image)
    #client.land()

    return 0

if __name__ == "__main__":
    sys.exit(main())

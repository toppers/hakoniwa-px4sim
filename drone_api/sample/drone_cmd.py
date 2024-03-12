#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import hakosim

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
    client.moveToPosition(0, 3, 5, 5)
    client.land()

    return 0

if __name__ == "__main__":
    sys.exit(main())

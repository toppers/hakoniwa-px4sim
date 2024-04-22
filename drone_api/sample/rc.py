#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import hakosim
import pygame
import time
import math

def joystick_control(client: hakosim.MultirotorClient, joysitck):
    try:
        while True:
            data = client.getGameJoystickData()
            for event in pygame.event.get():
                if event.type == pygame.JOYAXISMOTION:
                    data['axis'] = list(data['axis']) 
                    data['axis'][event.axis] = event.value
                elif event.type == pygame.JOYBUTTONDOWN:
                    data['button'] = list(data['button'])
                    data['button'][event.button] = True
                elif event.type == pygame.JOYBUTTONUP:
                    data['button'] = list(data['button'])
                    data['button'][event.button] = False
            client.putGameJoystickData(data)
    except KeyboardInterrupt:
        # Ctrl+Cが押されたときにジョイスティックをクリーンアップ
        pygame.joystick.quit()
        pygame.quit()

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <config_path>")
        return 1
    pygame.init()
    pygame.joystick.init()
    joystick = pygame.joystick.Joystick(0)
    joystick.init()

    # connect to the HakoSim simulator
    client = hakosim.MultirotorClient(sys.argv[1])
    client.confirmConnection()
    client.enableApiControl(True)
    client.armDisarm(True)
    joystick_control(client, joystick)
    return 0

if __name__ == "__main__":
    sys.exit(main())

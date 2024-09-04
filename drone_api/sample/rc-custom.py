#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import hakosim
import pygame
import time
import os
from rc_utils.rc_utils import RcConfig, StickMonitor

# デフォルトのJSONファイルパス
DEFAULT_CONFIG_PATH = "rc_config/ps4-control.json"

def saveCameraImage(client):
    png_image = client.simGetImage("0", hakosim.ImageType.Scene)
    if png_image:
        with open("scene.png", "wb") as f:
            f.write(png_image)

def joystick_control(client: hakosim.MultirotorClient, joystick, stick_monitor: StickMonitor):
    try:
        while True:
            data = client.getGameJoystickData()
            for event in pygame.event.get():
                if event.type == pygame.JOYAXISMOTION:
                    if event.axis < 6:
                        op_index = stick_monitor.rc_config.get_op_index(event.axis)
                        stick_value = stick_monitor.stick_value(event.axis, event.value)
                        if (abs(stick_value) > 0.1):
                            print(f"stick event: stick_index={event.axis} op_index={op_index} event.value={event.value} stick_value={stick_value}")
                        data['axis'] = list(data['axis'])
                        data['axis'][op_index] = stick_value
                    else:
                        print(f'ERROR: not supported axis index: {event.axis}')
                elif event.type == pygame.JOYBUTTONDOWN or event.type == pygame.JOYBUTTONUP:
                    if event.button < 16:
                        data['button'] = list(data['button'])
                        event_op_index = stick_monitor.rc_config.get_event_op_index(event.button)
                        if event_op_index is not None:
                            event_triggered = stick_monitor.switch_event(event.button, (event.type == pygame.JOYBUTTONDOWN))
                            print(f"button event: switch_index={event.button} event_op_index={event_op_index} down: {(event.type == pygame.JOYBUTTONDOWN)} event_triggered={event_triggered}")
                            data['button'][event_op_index] = event_triggered
                            if event_triggered:
                                if event_op_index == stick_monitor.rc_config.SWITCH_CAMERA_SHOT:
                                    time.sleep(0.5)
                                    saveCameraImage(client)
                    else:
                        print(f'ERROR: not supported button index: {event.button}')
            client.putGameJoystickData(data)
    except KeyboardInterrupt:
        pygame.joystick.quit()
        pygame.quit()

def main():
    if len(sys.argv) != 2 and len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <config_path> [rc_config_path]")
        return 1
    
    config_path = sys.argv[1]
    rc_config_path = os.getenv("RC_CONFIG_PATH", DEFAULT_CONFIG_PATH)
    if (len(sys.argv) == 3):
        rc_config_path = sys.argv[2]
    
    if not os.path.exists(config_path):
        print(f"ERROR: Config file not found at '{config_path}'")
        return 1
    if not os.path.exists(rc_config_path):
        print(f"ERROR: Config file not found at '{rc_config_path}'")
        return 1

    # RcConfigとStickMonitorの初期化
    rc_config = RcConfig(rc_config_path)
    print("Controller: ", rc_config_path)
    print("Mode: ", rc_config.config['mode'])
    stick_monitor = StickMonitor(rc_config)

    pygame.init()
    pygame.joystick.init()

    joystick_count = pygame.joystick.get_count()
    print(f"Number of joysticks: {joystick_count}")
    try:
        joystick = pygame.joystick.Joystick(0)
        joystick.init()
        print(f'ジョイスティックの名前: {joystick.get_name()}')
        print(f'ボタン数 : {joystick.get_numbuttons()}')
    except pygame.error:
        print('ジョイスティックが接続されていません')
        pygame.joystick.quit()
        pygame.quit()
        return 1

    client = hakosim.MultirotorClient(config_path)
    client.default_drone_name = "DroneTransporter"
    client.confirmConnection()
    client.enableApiControl(True)
    client.armDisarm(True)
    joystick_control(client, joystick, stick_monitor)
    return 0

if __name__ == "__main__":
    sys.exit(main())

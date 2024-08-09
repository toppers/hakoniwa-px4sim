#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import hakosim
import pygame
import time
import math

def average_axis(history, new_value, history_length=5):
    history.append(new_value)
    if len(history) > history_length:
        history.pop(0)
    return sum(history) / len(history)
a_value = 0.9  # 端の方を敏感にするために大きくする
b_value = 0.1  # 中央付近のカーブを調整するために小さくする

def cubic_stick_response(x):
    """
    ドローンのスティック操作を3次関数で計算する関数

    Parameters:
    x (float): スティックの入力値（0から1の範囲）

    Returns:
    float: スティック操作の出力値（0から1の範囲）
    """
    global a_value
    global b_value
    a = a_value
    b = b_value
    c = 1 - (a + b)
    y = a * x**3 + b * x**2 + c * x
    return y


def discretize_value(value):
    return value
#    return round(value / 0.5) * 0.5
#    return round(value / 0.25) * 0.25
#    return round(value / 0.1) * 0.1

def saveCameraImage(client):
    png_image = client.simGetImage("0", hakosim.ImageType.Scene)
    if png_image:
        with open("scene.png", "wb") as f:
            f.write(png_image)

def joystick_control(client: hakosim.MultirotorClient, joysitck):
    axis_history = {0: [], 1: [], 2: [], 3: []} 
    magnet_on = False
    magnet_button_index = 1
    camera_button_index = 2
    try:
        while True:
            data = client.getGameJoystickData()
            for event in pygame.event.get():
                if event.type == pygame.JOYAXISMOTION:
                    if (event.axis < 4):
                        #print('GET AXIS VALUE:',event.axis)
                        avg_value = average_axis(axis_history[event.axis], event.value)
                        #print('AVG AXIS VALUE:',avg_value)
                        data['axis'] = list(data['axis']) 
                        data['axis'][event.axis] = cubic_stick_response(discretize_value(avg_value))
                        #print('CUBIC AXIS VALUE:',data['axis'][event.axis])
                    else:
                        print(f'ERROR: not supported axis index: {event.axis}')
                elif event.type == pygame.JOYBUTTONDOWN:
                    if (event.button < 16):
                        print("button down: index=", event.button)
                        data['button'] = list(data['button'])
                        if (event.button != magnet_button_index):
                            data['button'][event.button] = True
                        else:
                            if magnet_on:
                                magnet_on = False
                            else:
                                magnet_on = True
                            print(f'magnet_on: {magnet_on}')
                            data['button'][event.button] = magnet_on
                        if event.button == camera_button_index:
                            time.sleep(0.5)
                            saveCameraImage(client)
                    else:
                        print(f'ERROR: not supported button index: {event.button}')
                elif event.type == pygame.JOYBUTTONUP:
                    if (event.button < 16):
                        if event.button != magnet_button_index:
                            print("button up: index=", event.button)
                            data['button'] = list(data['button'])
                            data['button'][event.button] = False
                    else:
                        print(f'ERROR: not supported button index: {event.button}')
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
    # 接続されているジョイスティックの数を取得
    joystick_count = pygame.joystick.get_count()
    print(f"Number of joysticks: {joystick_count}")
    try:
        # ジョイスティックのインスタンス生成
        joystick = pygame.joystick.Joystick(0)
        joystick.init()
        print('ジョイスティックの名前:', joystick.get_name())
        print('ボタン数 :', joystick.get_numbuttons())
    except pygame.error:
        print('ジョイスティックが接続されていません')
        pygame.joystick.quit()
        pygame.quit()
        sys.exit()
    # connect to the HakoSim simulator
    client = hakosim.MultirotorClient(sys.argv[1])
    client.default_drone_name = "DroneTransporter"
    client.confirmConnection()
    client.enableApiControl(True)
    client.armDisarm(True)
    joystick_control(client, joystick)
    return 0

if __name__ == "__main__":
    sys.exit(main())

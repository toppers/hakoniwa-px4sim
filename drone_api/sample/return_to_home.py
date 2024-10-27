#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import hakosim
import hako_pdu
import math
import os
import hakopy
import time

class DroneController:
    HEADING_AXIS = 0
    UP_DOWN_AXIS = 1
    ROLL_AXIS = 2
    PITCH_AXIS = 3
    YAW_TOLERANCE = 0.01
    POSITION_TOLERANCE = 0.2
    CLOSE_YAW_RANGE = 10
    CLOSE_RANGE = 30

    def __init__(self, client, default_drone_name="DroneTransporter", height=3.0, power=0.1, yaw_power=0.9):
        self.client = client
        self.client.default_drone_name = default_drone_name
        self.height = height
        self.power = power
        self.yaw_power = yaw_power

    def _print_progress(self, message):
        sys.stdout.write(f"\r{message}")
        sys.stdout.flush()


    def _get_pose(self):
        return self.client.simGetVehiclePose()

    def _update_joystick(self, axis, value):
        data = self.client.getGameJoystickData()
        data['axis'] = list(data['axis'])
        data['axis'][axis] = value
        self.client.putGameJoystickData(data)

    def debug_pos(self):
        pose = self._get_pose()
        print(f"POS  : {pose.position.x_val} {pose.position.y_val} {pose.position.z_val}")
        roll, pitch, yaw = hakosim.hakosim_types.Quaternionr.quaternion_to_euler(pose.orientation)
        print(f"ANGLE: {math.degrees(roll)} {math.degrees(pitch)} {math.degrees(yaw)}")

    def adjust_heading(self):
        while True:
            pose = self._get_pose()
            _, _, yaw = hakosim.hakosim_types.Quaternionr.quaternion_to_euler(pose.orientation)
            yaw_deg = math.degrees(yaw)
            if abs(yaw_deg) <= self.YAW_TOLERANCE:
                break
            power = 0.05 if abs(yaw_deg) < self.CLOSE_YAW_RANGE else self.power
            self._update_joystick(self.HEADING_AXIS, power if yaw_deg > 0 else -power)
            self._print_progress(f"Adjusting Heading... yaw_val: {yaw_deg:.5f} power: {power}")

        self._update_joystick(self.HEADING_AXIS, 0.0)
        self._print_progress("Heading adjustment complete\n")

    def move_z(self):
        while self._get_pose().position.z_val > self.height - 0.1:
            z_val = self._get_pose().position.z_val
            self._update_joystick(self.UP_DOWN_AXIS, self.power)
            self._print_progress(f"Moving to target height... z_val: {z_val:.2f}, power: {self.power}")

        self._update_joystick(self.UP_DOWN_AXIS, 0.0)
        self._print_progress("Reached target height\n")

    def move_x(self, target_x=0.0):
        while abs(self._get_pose().position.x_val - target_x) > self.POSITION_TOLERANCE:
            x_val = self._get_pose().position.x_val
            power = 0.05 if abs(x_val - target_x) < self.CLOSE_RANGE else self.power
            self._update_joystick(self.PITCH_AXIS, power if x_val > target_x else -power)
            self._print_progress(f"Moving to target X... x_val: {x_val:.2f}, power: {power}")

        self._update_joystick(self.PITCH_AXIS, 0.0)
        self._print_progress("X position reached\n")

    def move_y(self, target_y=0.0):
        while abs(self._get_pose().position.y_val - target_y) > self.POSITION_TOLERANCE:
            y_val = self._get_pose().position.y_val
            power = 0.05 if abs(y_val - target_y) < self.CLOSE_RANGE else self.power
            self._update_joystick(self.ROLL_AXIS, power if y_val > target_y else -power)
            self._print_progress(f"Moving to target Y... y_val: {y_val:.2f}, power: {power}")

        self._update_joystick(self.ROLL_AXIS, 0.0)
        self._print_progress("Y position reached\n")

    def return_to_home(self):
        self.adjust_heading()
        self.move_x()
        self.move_y()
        self.move_z()

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <config_path>")
        return 1

    client = hakosim.MultirotorClient(sys.argv[1])
    hako_binary_path = os.getenv('HAKO_BINARY_PATH', '/usr/local/lib/hakoniwa/hako_binary/offset')
    client.pdu_manager = hako_pdu.HakoPduManager(hako_binary_path, sys.argv[1])

    client.confirmConnection()
    client.enableApiControl(True)
    client.armDisarm(True)

    controller = DroneController(client, height=2.0, power=0.5, yaw_power=0.8)

    controller.return_to_home()        

    return 0

if __name__ == "__main__":
    sys.exit(main())

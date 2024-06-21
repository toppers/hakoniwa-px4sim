import hakopy
import hako_pdu
import pdu_info
import math
import json
import os
import time
import hakosim_types
import hakosim_lidar

class ImageType:
    Scene = "png"

class HakoDrone:
    def __init__(self, name):
        self.name = name
        self.enableApiControl = False
        self.arm = False
        self.camera_cmd_request_id = 1
        self.camera_move_cmd_request_id = 1

class MultirotorClient:
    def __init__(self, config_path):
        self.pdu_manager = None
        self.config_path = config_path
        self.pdudef = self._load_json(config_path)
        self.vehicles = {}
        default_drone_set = False
        for entry in self.pdudef['robots']:
            entry_name = entry['name']
            if len(entry['shm_pdu_readers']) > 0 or len(entry['shm_pdu_writers']) > 0 or len(entry['rpc_pdu_readers']) > 0 or len(entry['rpc_pdu_writers']) > 0:
                self.vehicles[entry_name] = HakoDrone(entry_name)
                if not default_drone_set:
                    self.default_drone_name = entry_name
                    default_drone_set = True

    def _load_json(self, path):
        try:
            with open(path, 'r') as file:
                return json.load(file)
        except FileNotFoundError:
            print(f"ERROR: File not found '{path}'")
        except json.JSONDecodeError:
            print(f"ERROR: Invalid Json fromat '{path}'")
        except PermissionError:
            print(f"ERROR: Permission denied '{path}'")
        except Exception as e:
            print(f"ERROR: {e}")
        return None

    def confirmConnection(self):
        hako_binary_path = os.getenv('HAKO_BINARY_PATH', '/usr/local/lib/hakoniwa/hako_binary/offset')
        self.pdu_manager = hako_pdu.HakoPduManager(hako_binary_path, self.config_path)
        ret = hakopy.init_for_external()
        if ret == False:
            print(f"ERROR: init_for_external() returns {ret}.")
            return False
        return True

    def enableApiControl(self, v, vehicle_name=None):
        if vehicle_name is None:
            vehicle_name = self.default_drone_name
        if vehicle_name in self.vehicles:
            self.vehicles[vehicle_name].enableApiControl = v
        else:
            print(f"Vehicle '{vehicle_name}' not found.")

    def armDisarm(self, v, vehicle_name=None):
        if vehicle_name is None:
            vehicle_name = self.default_drone_name
        if vehicle_name in self.vehicles:
            self.vehicles[vehicle_name].arm = v
        else:
            print(f"Vehicle '{vehicle_name}' not found.")

    def simGetVehiclePose(self, vehicle_name=None):
        name = self.get_vehicle_name(vehicle_name)
        pdu = self.pdu_manager.get_pdu(name, pdu_info.HAKO_AVATOR_CHANNLE_ID_POS)
        pdu_data = pdu.read()
        pos = hakosim_types.Vector3r(pdu_data['linear']['x'], pdu_data['linear']['y'], pdu_data['linear']['z'])
        orientation = hakosim_types.Quaternionr.euler_to_quaternion(pdu_data['angular']['x'], pdu_data['angular']['y'], pdu_data['angular']['z'])
        #print(f"roll:{pdu_data['angular']['x']}")
        #print(f"pitch: {pdu_data['angular']['y']}")
        #print(f"yaw: {pdu_data['angular']['z']}")
        return hakosim_types.Pose(pos, orientation)

    def get_packet(self, channel, vehicle_name):
        command = self.pdu_manager.get_pdu(vehicle_name, channel)
        cmd = command.get()
        cmd['header']['request'] = 1
        cmd['header']['result'] = 0
        cmd['header']['result_code'] = 0
        return command, cmd

    def reply_and_wait_res(self, command):
        ret = command.write()
        if ret == False:
            print('"ERROR: hako_asset_pdu_write')
            return False
        while True:
            pdu = command.read()
            if pdu == None:
                print('ERROR: hako_asset_pdu_read')
                return 0
            if pdu['header']['result'] == 1:
                pdu['header']['result'] = 0
                command.write()
                print('DONE')
                break
            #print("result: ",  pdu['header']['result'])
            time.sleep(1)
        return True

    def get_vehicle_name(self, vehicle_name):
        if vehicle_name is None:
            return self.default_drone_name
        if vehicle_name in self.vehicles:
            return vehicle_name
        else:
            print(f"Vehicle '{vehicle_name}' not found.")
            return None

    def takeoff(self, height, vehicle_name=None):
        if self.get_vehicle_name(vehicle_name) != None:
            print("INFO: takeoff")
            command, pdu_cmd = self.get_packet(pdu_info.HAKO_AVATOR_CHANNEL_ID_CMD_TAKEOFF, self.get_vehicle_name(vehicle_name))
            pdu_cmd['height'] = height
            pdu_cmd['speed'] = 5
            pdu_cmd['yaw_deg'] = self._get_yaw_degree(vehicle_name)
            return self.reply_and_wait_res(command)
        else:
            return False
        
    def moveToPosition(self, x, y, z, speed, yaw_deg=None, vehicle_name=None):
        if self.get_vehicle_name(vehicle_name) != None:
            print("INFO: moveToPosition")
            command, pdu_cmd = self.get_packet(pdu_info.HAKO_AVATOR_CHANNEL_ID_CMD_MOVE, self.get_vehicle_name(vehicle_name))
            pdu_cmd['x'] = x
            pdu_cmd['y'] = y
            pdu_cmd['z'] = z
            pdu_cmd['speed'] = speed
            if yaw_deg == None:
                yaw_deg = self._get_yaw_degree(vehicle_name)
            pdu_cmd['yaw_deg'] = yaw_deg
            #print(f'yaw_deg: {yaw_deg}')
            return self.reply_and_wait_res(command)
        else:
            return False

    def land(self, vehicle_name=None):
        if self.get_vehicle_name(vehicle_name) != None:
            print("INFO: Landing")
            command, pdu_cmd = self.get_packet(pdu_info.HAKO_AVATOR_CHANNEL_ID_CMD_LAND, self.get_vehicle_name(vehicle_name))
            pdu_cmd['height'] = 0
            pdu_cmd['speed'] = 5
            pdu_cmd['yaw_deg'] = self._get_yaw_degree(vehicle_name)
            return self.reply_and_wait_res(command)
        else:
            return False

    def wait_grab(self, grab, vehicle_name):
        while True:
            command = self.pdu_manager.get_pdu(self.get_vehicle_name(vehicle_name), pdu_info.HAKO_AVATOR_CHANNEL_ID_STAT_MAG)
            pdu_cmd = command.read()
            #print("magnet_on: ", pdu_cmd['magnet_on'])
            if grab:
                if pdu_cmd['magnet_on'] == 1 and pdu_cmd['contact_on'] == 1:
                    break
            else:
                if pdu_cmd['magnet_on'] == 0 and pdu_cmd['contact_on'] == 0:
                    break

    def grab_baggage(self, grab, vehicle_name=None):
        if self.get_vehicle_name(vehicle_name) != None:
            print("INFO: grab baggage: ", grab)
            command, pdu_cmd = self.get_packet(pdu_info.HAKO_AVATOR_CHANNEL_ID_CMD_MAG, self.get_vehicle_name(vehicle_name))
            pdu_cmd['magnet_on'] = grab
            command.write()
            self.wait_grab(grab, vehicle_name)
            pdu_cmd['header']['request'] = 0
            pdu_cmd['header']['result'] = 0
            command.write()
            return True
        else:
            return False

    def _get_camera_data(self, vehicle):
        while True:
            command = self.pdu_manager.get_pdu(vehicle.name, pdu_info.HAKO_AVATOR_CHANNEL_ID_CAMERA_DATA)
            pdu_data = command.read()
            if pdu_data['request_id'] == vehicle.camera_cmd_request_id:
                return pdu_data['image']['data']
            #time.sleep(0.5)

    def _get_camera_info(self, vehicle):
        while True:
            command = self.pdu_manager.get_pdu(vehicle.name, pdu_info.HAKO_AVATOR_CHANNEL_ID_CAMERA_INFO)
            pdu_data = command.read()
            if pdu_data['request_id'] == vehicle.camera_move_cmd_request_id:
                #print("request_id", pdu_data['request_id'])
                #print("angle", pdu_data['angle'])
                return pdu_data['angle']
            time.sleep(0.5)

    def _get_yaw_degree(self, vehicle_name=None):
        pose = self.simGetVehiclePose(vehicle_name)
        _, _, yaw = hakosim_types.Quaternionr.quaternion_to_euler(pose.orientation)
        return math.degrees(yaw)

    def simGetImage(self, id, image_type, vehicle_name=None):
        vehicle_name = self.get_vehicle_name(vehicle_name)
        if vehicle_name != None:
            vehicle = self.vehicles[vehicle_name]
            #print("INFO: get image ")
            command, pdu_cmd = self.get_packet(pdu_info.HAKO_AVATOR_CHANNEL_ID_CMD_CAMERA, vehicle_name)
            pdu_cmd['request_id'] = vehicle.camera_cmd_request_id
            pdu_cmd['encode_type'] = 0
            command.write()
            img = self._get_camera_data(vehicle)
            pdu_cmd['header']['request'] = 0
            pdu_cmd['header']['result'] = 0
            command.write()
            vehicle.camera_cmd_request_id = vehicle.camera_cmd_request_id + 1
            return bytes(img)
        else:
            return None
    def simSetCameraOrientation(self, id, degree, vehicle_name=None):
        vehicle_name = self.get_vehicle_name(vehicle_name)
        if vehicle_name != None:
            vehicle = self.vehicles[vehicle_name]
            command, pdu_cmd = self.get_packet(pdu_info.HAKO_AVATOR_CHANNEL_ID_CMD_CAMERA_MOVE, vehicle_name)
            pdu_cmd['request_id'] = vehicle.camera_move_cmd_request_id
            pdu_cmd['angle']['x'] = 0
            pdu_cmd['angle']['y'] = degree
            pdu_cmd['angle']['z'] = 0
            command.write()
            info = self._get_camera_info(vehicle)
            pdu_cmd['header']['request'] = 0
            pdu_cmd['header']['result'] = 0
            command.write()
            vehicle.camera_move_cmd_request_id = vehicle.camera_move_cmd_request_id + 1
            return info
        else:
            return None
    def simGetCameraImage(self, id, image_type, vehicle_name=None):
        if vehicle_name != None:
            vehicle = self.vehicles[vehicle_name]
            img = self._get_camera_data(vehicle)
            return bytes(img)
        else:
            return None


    def getLidarData(self, vehicle_name=None):
        vehicle_name = self.get_vehicle_name(vehicle_name)
        if vehicle_name != None:
            vehicle = self.vehicles[vehicle_name]
            lidar_pdu = self.pdu_manager.get_pdu(vehicle.name, pdu_info.HAKO_AVATOR_CHANNEL_ID_LIDAR_DATA)
            lidar_pdu_data = lidar_pdu.read()
            lidar_pos_pdu = self.pdu_manager.get_pdu(vehicle.name, pdu_info.HAKO_AVATOR_CHANNEL_ID_LIDAR_POS)
            lidar_pos_pdu_data = lidar_pos_pdu.read()
            time_stamp = lidar_pdu_data['header']['stamp']['sec']
            point_cloud_bytes = lidar_pdu_data['data__raw']
            height = lidar_pdu_data['height']
            row_step = lidar_pdu_data['row_step']
            total_data_bytes = height * row_step
            point_cloud = hakosim_lidar.LidarData.extract_xyz_from_point_cloud(point_cloud_bytes, total_data_bytes)
            position = hakosim_types.Vector3r(lidar_pos_pdu_data['linear']['x'], lidar_pos_pdu_data['linear']['y'], lidar_pos_pdu_data['linear']['z'])
            orientation = hakosim_types.Quaternionr.euler_to_quaternion(lidar_pos_pdu_data['angular']['x'], lidar_pos_pdu_data['angular']['y'], lidar_pos_pdu_data['angular']['z'])
            pose = hakosim_types.Pose(position, orientation)
            return hakosim_lidar.LidarData(point_cloud, time_stamp, pose)
        else:
            return None

    def getGameJoystickData(self, vehicle_name=None):
        vehicle_name = self.get_vehicle_name(vehicle_name)
        if vehicle_name != None:
            vehicle = self.vehicles[vehicle_name]
            game_pdu = self.pdu_manager.get_pdu(vehicle.name, pdu_info.HAKO_AVATOR_CHANNEL_ID_GAME_CTRL)
            game_pdu_data = game_pdu.read()
            return game_pdu_data
        else:
            return None

    def putGameJoystickData(self, data, vehicle_name=None):
        vehicle_name = self.get_vehicle_name(vehicle_name)
        if vehicle_name != None:
            vehicle = self.vehicles[vehicle_name]
            game_pdu = self.pdu_manager.get_pdu(vehicle.name, pdu_info.HAKO_AVATOR_CHANNEL_ID_GAME_CTRL)
            game_pdu.obj = data
            game_pdu.write()
            return True
        else:
            return False

import hakopy
import hako_pdu
import pdu_info
import json

class HakoDrone:
    def __init__(self, name):
        self.name = name
        self.enableApiControl = False
        self.arm = False

class MultirotorClient:
    def __init__(self, config_path):
        self.pdu_manager = None
        self.config_path = config_path
        self.pdudef = self._load_json(config_path)
        self.vehicles = {}
        default_drone_set = False
        for entry in self.pdudef['robots']:
            entry_name = entry['name']
            if len(entry['shm_pdu_readers']) > 0 or len(entry['shm_pdu_writers']) > 0:
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
        self.pdu_manager = hako_pdu.HakoPduManager('/usr/local/lib/hakoniwa/hako_binary/offset', self.config_path)
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
            return self.reply_and_wait_res(command)
        else:
            return False
        
    def moveToPosition(self, x, y, z, speed, vehicle_name=None):
        if self.get_vehicle_name(vehicle_name) != None:
            print("INFO: moveToPosition")
            command, pdu_cmd = self.get_packet(pdu_info.HAKO_AVATOR_CHANNEL_ID_CMD_MOVE, self.get_vehicle_name(vehicle_name))
            pdu_cmd['x'] = x
            pdu_cmd['y'] = y
            pdu_cmd['z'] = z
            pdu_cmd['speed'] = speed
            return self.reply_and_wait_res(command)
        else:
            return False

    def land(self, vehicle_name=None):
        if self.get_vehicle_name(vehicle_name) != None:
            print("INFO: moveToPosition")
            command, pdu_cmd = self.get_packet(pdu_info.HAKO_AVATOR_CHANNEL_ID_CMD_LAND, self.get_vehicle_name(vehicle_name))
            pdu_cmd['height'] = 0
            pdu_cmd['speed'] = 5
            return self.reply_and_wait_res(command)
        else:
            return False


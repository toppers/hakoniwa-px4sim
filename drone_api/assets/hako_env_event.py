import hakopy
import sys
import time
import os
import hako_pdu
import hakosim
import pdu_info
from lib.hako_area_accessor_impl import HakoAreaAccessorImpl
from lib.hako_area_pro_accessor_impl import HakoAreaPropAccessorImpl
from lib.hako_aabb_object_space import HakoAABBObjectSpace

# Declare the global variable for delta_time_usec
delta_time_usec = 0
config_path = ''
area_config_dir=''
clinet = None

def my_on_initialize(context):
    robot_name = 'DroneTransporter'
    hako_binary_path = os.getenv('HAKO_BINARY_PATH', '/usr/local/lib/hakoniwa/hako_binary/offset')
    pdu_manager = hako_pdu.HakoPduManager(hako_binary_path, config_path)
    pdu = pdu_manager.get_pdu(robot_name, pdu_info.HAKO_AVATOR_CHANNEL_ID_DISTURB)
    pdu_data = pdu.get()
    pdu.write()
    return 0

def my_on_reset(context):
    print("INFO: RESET EVENT OCCURRED")
    return 0

def on_manual_timing_control(context):
    global delta_time_usec
    area_accessor = HakoAreaAccessorImpl(os.path.join(area_config_dir, 'area.json'))
    prop_accessor = HakoAreaPropAccessorImpl(os.path.join(area_config_dir, 'area_property.json'))
    drone_size = (0.4, 0.4, 0.1)
    while True:
        pose = client.simGetVehiclePose()
        drone_position = (pose.position.x_val, pose.position.y_val, pose.position.z_val)
        object_space = HakoAABBObjectSpace(drone_position, drone_size)
        area_id = area_accessor.get_area_id(object_space)
        pdu = client.pdu_manager.get_pdu(client.default_drone_name, pdu_info.HAKO_AVATOR_CHANNEL_ID_DISTURB)
        pdu_data = pdu.get()
        pdu_data['d_wind']['value']['x'] = 0.0
        pdu_data['d_wind']['value']['y'] = 0.0
        pdu_data['d_wind']['value']['z'] = 0.0
        if area_id is not None:
            property_info = prop_accessor.get_property(area_id)
            wind = property_info.get_wind_velocity()
            if wind is not None:
                print(f"{hakopy.simulation_time()} wind ==> {property_info.get_wind_velocity()}")
                pdu_data['d_wind']['value']['x'] = wind[0]
                pdu_data['d_wind']['value']['y'] = wind[1]
                pdu_data['d_wind']['value']['z'] = wind[2]
        else:
            print(f"{hakopy.simulation_time()}: No wind")

        pdu.write()
        ret = hakopy.usleep(delta_time_usec)
        if ret == False:
            break

    return 0

my_callback = {
    'on_initialize': my_on_initialize,
    'on_simulation_step': None,
    'on_manual_timing_control': on_manual_timing_control,
    'on_reset': my_on_reset
}

def main():
    global delta_time_usec
    global config_path
    global area_config_dir
    global client
    
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <config_path> <delta_time_msec> <area_config_dir>")
        return 1

    asset_name = 'HakoEnv'
    config_path = sys.argv[1]
    delta_time_usec = int(sys.argv[2]) * 1000
    area_config_dir = sys.argv[3]
    client = hakosim.MultirotorClient(config_path)    
    client.default_drone_name = "DroneTransporter"
    hako_binary_path = os.getenv('HAKO_BINARY_PATH', '/usr/local/lib/hakoniwa/hako_binary/offset')
    client.pdu_manager = hako_pdu.HakoPduManager(hako_binary_path, config_path)
    client.enableApiControl(True)
    client.armDisarm(True)

    ret = hakopy.asset_register(asset_name, config_path, my_callback, delta_time_usec, hakopy.HAKO_ASSET_MODEL_PLANT)
    if ret == False:
        print(f"ERROR: hako_asset_register() returns {ret}.")
        return 1
    
    ret = hakopy.start()
    print(f"INFO: DONE {ret}")

    return 0

if __name__ == "__main__":
    sys.exit(main())

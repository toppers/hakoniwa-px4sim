import json
from operations.takeoff import TakeoffOperation
from operations.start_landing import StartLandingOperation

class ScenarioManager:
    def __init__(self, drone_config_data):
        self.index = 0
        self.current = None
        self.connection = None
        self.drone_config_data = drone_config_data

    def load_scenario(self, filename):
        with open(filename, 'r') as file:
            data = json.load(file)
        self.scenario_data = data

    def _scale_lat_lon(self, v):
        return v * 1e07
    
    def _scale_lat_lon_meter(self, v):
        return (float(v)/111000.0) * 1e07

    def _get_op(self, op):
        if op['operation'] == 'takeoff':
            altitude = op['alt'] + self.drone_config_data['simulation']['location']['altitude']
            duration_sec = op['duration_sec']
            print(f"INFO: takeoff operation alt: {altitude}")
            self.current = TakeoffOperation(self.connection, altitude, duration_sec)
        elif op['operation'] == 'start_landing':
            lat = int(self._scale_lat_lon(self.drone_config_data['simulation']['location']['latitude'])) + int(self._scale_lat_lon_meter(op['lat']))
            lon = int(self._scale_lat_lon(self.drone_config_data['simulation']['location']['longitude'])) + int(self._scale_lat_lon_meter(op['lon']))
            alt = op['alt'] + self.drone_config_data['simulation']['location']['altitude']
            duration_sec = op['duration_sec']
            print(f"INFO: start_landing operation lat: {lat} lon: {lon} alt: {alt}")
            self.current = StartLandingOperation(self.connection, lat, lon, alt, duration_sec)
        else:
            print(f"ERROR: not supported operation {op['operation']}")
            self.current = None
            return None

    def load_operation(self, connection):
        self.connection = connection
        op = self.scenario_data['scenario'][self.index]
        self._get_op(op)
        return self.current
    
    def operation_is_done(self):
        if self.current is None:
            return True
        return self.current.is_op_done()

    def event_msg(self, msg):
        if self.current is not None:
            self.current.event_msg(msg)

    def get_current(self):
        return self.current

    def next_operation(self):
        self.index += 1
        if self.index >= len(self.scenario_data['scenario']):
            self.current = None
            return None
        op = self.scenario_data['scenario'][self.index]
        self._get_op(op)
        return self.current

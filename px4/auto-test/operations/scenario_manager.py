import json
from operations.takeoff import TakeoffOperation
from operations.start_landing import StartLandingOperation

class ScenarioManager:
    def __init__(self):
        self.index = 0
        self.current = None
        self.connection = None

    def load_scenario(self, filename):
        with open(filename, 'r') as file:
            data = json.load(file)
        self.scenario_data = data

    def _get_op(self, op):
        if op['operation'] == 'takeoff':
            altitude = op['alt']
            duration_sec = op['duration_sec']
            print(f"INFO: takeoff operation alt: {altitude}")
            self.current = TakeoffOperation(self.connection, altitude, duration_sec)
        elif op['operation'] == 'start_landing':
            lat = op['lat']
            lon = op['lon']
            alt = op['alt']
            duration_sec = op['duration_sec']
            print(f"INFO: start_landing operation lat: {lat} lon: ${lon} alt: {alt}")
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

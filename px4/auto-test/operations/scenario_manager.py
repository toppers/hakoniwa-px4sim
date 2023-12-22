import json
from operations.takeoff import TakeoffOperation

class ScenarioManager:
    def __init__(self):
        self.index = 0
        self.current = None

    def load_scenario(self, filename):
        with open(filename, 'r') as file:
            data = json.load(file)
        self.scenario_data = data

    def load_operation(self, connection):
        op = self.scenario_data['scenario'][self.index]
        if op['operation'] == 'takeoff':
            altitude = op['alt']
            print(f"INFO: takeoff operation alt: {altitude}")
            self.current = TakeoffOperation(connection, altitude)
        else:
            print(f"ERROR: not supported operation {op['operation']}")
            return None
        return self.current
    
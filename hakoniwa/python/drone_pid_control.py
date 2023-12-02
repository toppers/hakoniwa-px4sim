from hako_runner import HakoRunner
from hako_asset_pdu import HakoAssetPdu
from hako_apl_ops import HakoAplOps

class DronePidControl(HakoAplOps):
    def __init__(self):
        pass

    def initialize(self, pdu: HakoAssetPdu):
        self.pdu = pdu
        self.read_channel = 1
        self.write_channel = 2

    def step(self):
        #GET PDU
        read_data = self.pdu.get_read_pdu_json(self.read_channel)
        #print("read_data: ", read_data['linear']['z'])

        cmd_vel = self.pdu.get_write_pdu_json(self.write_channel)        
        #print(str(cmd_vel))
        #WRITE PDU
        self.pdu.update_write_buffer(self.write_channel, cmd_vel)

    def reset(self):
        #TODO
        pass

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print("Usage: <config.json>")
        sys.exit(1)
    
    filepath = sys.argv[1]
    print("filepath=", filepath)

    runner = HakoRunner(filepath)
    apl = DronePidControl()
    runner.initialize(apl)

    runner.run()
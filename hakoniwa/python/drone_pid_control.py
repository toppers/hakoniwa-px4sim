from hako_runner import HakoRunner
from hako_asset_pdu import HakoAssetPdu
from hako_apl_ops import HakoAplOps
from simple_pid import PID
import csv

class DronePidControl(HakoAplOps):
    def __init__(self, pid_ctrl):
        super().__init__()
        self.time = 0.0
        self.pid = pid_ctrl
        self.csv_file = open('height_data.csv', 'w', newline='')
        self.csv_writer = csv.writer(self.csv_file)
        self.csv_writer.writerow(['Time', 'Height'])
        self.last_sync_time = 0

    def initialize(self, pdu: HakoAssetPdu):
        self.pdu = pdu
        self.read_channel = 1
        self.write_channel = 2

    # 推力を計算する関数
    def calculate_thrust(self, height: float):
        value = self.pid(height)
        limited_value = 9.81 + max(-2, min(2, value))
        return limited_value

    def step(self):
        #GET PDU
        read_data = self.pdu.get_read_pdu_json(self.read_channel)
        height = -read_data['linear']['z']

        cmd_vel = self.pdu.get_write_pdu_json(self.write_channel)
        cmd_vel['linear']['z'] = self.calculate_thrust(height)

        #WRITE PDU
        self.pdu.update_write_buffer(self.write_channel, cmd_vel)

        self.csv_writer.writerow([self.time, height])
        self.time = self.time + 0.001
        if (self.time - self.last_sync_time) > 1.0:
            self.last_sync_time = self.time
            self.csv_file.flush()
            print(f"T: {self.time:.3f}, U: {cmd_vel['linear']['z']:.3f}, H: {height:.3f}")

    def reset(self):
        self.csv_file.close()
        pass

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print("Usage: <config.json>")
        sys.exit(1)
    
    filepath = sys.argv[1]
    print("filepath=", filepath)

    runner = HakoRunner(filepath)

    # PIDコントローラのパラメータ設定
    Kp = 1.0  # 比例ゲイン
    Ki = 0.01  # 積分ゲイン
    Kd = 0.01  # 微分ゲイン
    s = 10.0  # 目標高度（メートル）

    # PIDコントローラの作成
    pid_controller = PID(Kp, Ki, Kd, setpoint = s)
    apl = DronePidControl(pid_controller)
    runner.initialize(apl)

    runner.run()
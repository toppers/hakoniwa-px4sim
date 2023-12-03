from hako_runner import HakoRunner
from hako_asset_pdu import HakoAssetPdu
from hako_apl_ops import HakoAplOps
from simple_pid import PID
import csv

class CsvData:
    def __init__(self, file_Name, header):
        self.csv_file = open(file_Name, 'w', newline='')
        self.csv_writer = csv.writer(self.csv_file)
        self.csv_writer.writerow(header)
    
    def write(self, value):
        self.csv_writer.writerow(value)

    def flush(self):
        self.csv_file.flush()
    
    def close(self):
        self.csv_file.close()

class DronePidControl(HakoAplOps):
    def __init__(self, pid_height, pid_phi, pid_theta, pid_psi):
        super().__init__()
        self.time = 0.0
        self.pid_height = pid_height
        self.csv_height = CsvData('python/results/height_data.csv', ['Time', 'Height'])
        self.csv_north = CsvData('python/results/north_data.csv', ['Time', 'North'])
        self.csv_east = CsvData('python/results/east_data.csv', ['Time', 'East'])

        self.pid_phi = pid_phi
        self.csv_phi = CsvData('python/results/phi_data.csv', ['Time', 'Phi'])

        self.pid_theta = pid_theta
        self.csv_theta = CsvData('python/results/theta_data.csv', ['Time', 'Theta'])

        self.pid_psi = pid_psi
        self.csv_psi = CsvData('python/results/psi_data.csv', ['Time', 'Psi'])

        self.last_sync_time = 0

    def initialize(self, pdu: HakoAssetPdu):
        self.pdu = pdu
        self.read_channel = 1
        self.write_channel = 2

    # 推力を計算する関数
    def calculate_thrust(self, height: float):
        if self.pid_height != None:
            value = self.pid_height(height)
            limited_value = 9.81 + max(-2, min(2, value))
            return limited_value
        return 0

    def calculate_phi(self, radian: float):
        if self.pid_phi != None:
            value = self.pid_phi(radian)
            limited_value = max(-2, min(2, value))
            return limited_value
        return 0

    def calculate_theta(self, radian: float):
        if self.pid_theta is not None:
            value = self.pid_theta(radian)
            limited_value = max(-2, min(2, value))
            return limited_value
        return 0

    def calculate_psi(self, radian: float):
        if self.pid_psi is not None:
            value = self.pid_psi(radian)
            limited_value = max(-2, min(2, value))
            return limited_value
        return 0

    def flush(self):
        self.csv_height.flush()
        self.csv_phi.flush()
        self.csv_theta.flush()
        self.csv_psi.flush()
        self.csv_north.flush()
        self.csv_east.flush()


    def step(self):
        #GET PDU
        read_data = self.pdu.get_read_pdu_json(self.read_channel)
        height = -read_data['linear']['z']
        x = read_data['linear']['x']
        y = read_data['linear']['y']

        phi   = read_data['angular']['x']
        theta = read_data['angular']['y']
        psi   = read_data['angular']['z']

        cmd_vel = self.pdu.get_write_pdu_json(self.write_channel)
        cmd_vel['linear']['z'] = self.calculate_thrust(height)
        cmd_vel['angular']['x'] = self.calculate_phi(phi)
        cmd_vel['angular']['y'] = self.calculate_theta(theta)
        cmd_vel['angular']['z'] = self.calculate_psi(psi)
        #WRITE PDU
        self.pdu.update_write_buffer(self.write_channel, cmd_vel)

        self.csv_height.write([self.time, height])
        self.csv_north.write([self.time, x])
        self.csv_east.write([self.time, y])
        self.csv_phi.write([self.time, phi])
        self.csv_theta.write([self.time, theta])
        self.csv_psi.write([self.time, psi])

        self.time = self.time + 0.001
        if (self.time - self.last_sync_time) > 1.0:
            self.last_sync_time = self.time
            self.flush()
            print(f"T: {self.time:.3f}, U: {cmd_vel['linear']['z']:.3f}, H: {height:.3f}")
            print(f"T: {self.time:.3f}, U: {cmd_vel['linear']['z']:.3f}, N: {x:.3f}")
            print(f"T: {self.time:.3f}, U: {cmd_vel['linear']['z']:.3f}, E: {y:.3f}")
            print(f"T: {self.time:.3f}, U: {cmd_vel['linear']['z']:.3f}, PHI  : {phi:.3f}")
            print(f"T: {self.time:.3f}, U: {cmd_vel['linear']['z']:.3f}, THETA: {theta:.3f}")
            print(f"T: {self.time:.3f}, U: {cmd_vel['linear']['z']:.3f}, PSI  : {psi:.3f}")

    def reset(self):
        self.csv_height.close()
        self.csv_north.close()
        self.csv_east.close()
        self.csv_phi.close()
        self.csv_theta.close()
        self.csv_psi.close()
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
    pid_height = PID(Kp, Ki, Kd, setpoint = s)

    # PIDコントローラのパラメータ設定（ロール角用）
    Kp_phi = 0.5  # 比例ゲイン
    Ki_phi = 0.01  # 積分ゲイン
    Kd_phi = 0.01  # 微分ゲイン
    setpoint_phi = 0.2  # 目標ロール角（ラジアン）

    # ロール角用のPIDコントローラの作成
    pid_phi = PID(Kp_phi, Ki_phi, Kd_phi, setpoint=setpoint_phi)

    # thetaとpsiのPID制御器のパラメータ設定
    Kp_theta = 0.5  # 比例ゲイン
    Ki_theta = 0.01  # 積分ゲイン
    Kd_theta = 0.01  # 微分ゲイン
    setpoint_theta = 0.2  # 目標ピッチ角（ラジアン）

    Kp_psi = 0.5  # 比例ゲイン
    Ki_psi = 0.01  # 積分ゲイン
    Kd_psi = 0.01  # 微分ゲイン
    setpoint_psi = 0.2  # 目標ヨー角（ラジアン）

    # thetaとpsiのPIDコントローラの作成
    pid_theta = PID(Kp_theta, Ki_theta, Kd_theta, setpoint=setpoint_theta)
    pid_psi = PID(Kp_psi, Ki_psi, Kd_psi, setpoint=setpoint_psi)

    apl = DronePidControl(pid_height, pid_phi, pid_theta, pid_psi)
    runner.initialize(apl)

    runner.run()
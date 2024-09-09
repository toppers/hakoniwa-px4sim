import argparse
import json
import numpy as np
from parser import Parser
from scipy import signal
import csv
from analyze import calculate_margins

def optimize_pid_for_transfer_function(file_path, Kp, Ki, Kd):
    parser = Parser(file_path)
    parser.update_constant('Kp', Kp)
    parser.update_constant('Ki', Ki)
    parser.update_constant('Kd', Kd)
    num, den = parser.get_transfer_function_coefficients()

    system = signal.TransferFunction(num, den)
    w = np.logspace(-2, 4, num=1000)  # 10^-2 から 10^4 の範囲で解析
    w, mag, phase = signal.bode(system, w=w)
    
    # ゲイン余裕と位相余裕を計算
    gain_margin, phase_margin, gain_cross_freq, phase_cross_freq = calculate_margins(w, mag, phase)
    
    return gain_margin, phase_margin, gain_cross_freq, phase_cross_freq

def main(file_path, Ki, Kd, output_csv):
    # CSVファイルの準備
    with open(output_csv, mode='w', newline='') as file:
        writer = csv.writer(file)
        # メタ行を出力
        writer.writerow(['Kp', 'Ki', 'Kd', 'Gain Margin (dB)', 'Phase Margin (deg)', 'Gain Crossover Frequency (rad/s)', 'Phase Crossover Frequency (rad/s)'])

        # Kp をループで変化させる (10^-4 から 10^3 まで、0.5倍ずつ)
        Kp_values = [10**i for i in np.arange(-4, 3.5, 0.5)]  # 10^-4 から 0.5倍ずつ 10^3 まで
        for Kp in Kp_values:
            gain_margin, phase_margin, gain_cross_freq, phase_cross_freq = optimize_pid_for_transfer_function(file_path, Kp, Ki, Kd)
            # 結果をCSVに出力
            writer.writerow([Kp, Ki, Kd, gain_margin, phase_margin, gain_cross_freq, phase_cross_freq])
            print(f"Kp: {Kp}, Ki: {Ki}, Kd: {Kd}, Gm: {gain_margin}, Pm: {phase_margin}, Wc: {gain_cross_freq}, Wpi: {phase_cross_freq}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Optimize PID parameters for the given transfer function JSON file and output results to CSV")
    
    # Transfer function JSON fileのパスを引数として追加
    parser.add_argument('file_path', type=str, help="Path to the transfer function JSON file")
    
    # Ki, Kd を引数として追加
    parser.add_argument('--Ki', type=float, required=True, help="Integral gain (Ki) for the PID controller")
    parser.add_argument('--Kd', type=float, required=True, help="Derivative gain (Kd) for the PID controller")
    
    # CSV出力ファイルのパス
    parser.add_argument('--output_csv', type=str, default='pid_results.csv', help="Path to output CSV file")
    
    args = parser.parse_args()
    
    # メイン処理を実行
    main(args.file_path, args.Ki, args.Kd, args.output_csv)

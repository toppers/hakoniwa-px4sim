import argparse
import json
import numpy as np
from parser import Parser
import control as ctrl
import csv
import pandas as pd
import matplotlib.pyplot as plt

def plot(csv_path):
    # CSVデータを読み込む
    df = pd.read_csv(csv_path)

    # グラフを描くために、Kiごとにデータを分割する
    unique_ki_values = df['Ki'].unique()

    # グラフの初期設定
    plt.figure(figsize=(8,6))

    # Kiごとに異なるプロットを作成
    for ki in unique_ki_values:
        ki_data = df[df['Ki'] == ki]
        plt.plot(ki_data['Kp'], ki_data['PM'], label=f'Ki = {ki}', marker='o')

    # グラフの装飾
    plt.xlabel('Kp')
    plt.xscale('log')  # 横軸を対数スケールに設定
    plt.ylabel('PM (Phase Margin)')
    plt.title('Kp vs PM for Different Ki Values')
    plt.legend()
    plt.grid(True)

    # グラフを表示
    plt.show()

def optimize_pid_for_transfer_function(file_path, Kp, Ki, Kd):
    parser = Parser(file_path)
    parser.update_constant('Kp', Kp)
    parser.update_constant('Ki', Ki)
    parser.update_constant('Kd', Kd)
    num, den = parser.get_transfer_function_coefficients()

    system = ctrl.TransferFunction(num, den)
    gain_margin, phase_margin, gain_cross_freq, phase_cross_freq = ctrl.margin(system)
    
    return gain_margin, phase_margin, gain_cross_freq, phase_cross_freq

def main(file_path, Kd, output_csv):
    # CSVファイルの準備
    with open(output_csv, mode='w', newline='') as file:
        writer = csv.writer(file)
        # メタ行を出力
        writer.writerow(['Ki', 'Kp', 'Kd', 'PM', 'Wc'])

        # Kp, Ki をループで変化させる
        Ki_values = [0] + [10**i for i in np.arange(-4, 1, 0.5)]
        Kp_values = [10**i for i in np.arange(-4, 1, 0.1)]
        for Ki in Ki_values:
            for Kp in Kp_values:
                gain_margin, phase_margin, gain_cross_freq, phase_cross_freq = optimize_pid_for_transfer_function(file_path, Kp, Ki, Kd)
                # 結果をCSVに出力
                writer.writerow([Ki, Kp, Kd, phase_margin, gain_cross_freq])
                print(f"Kp: {Kp}, Ki: {Ki}, Kd: {Kd}, Gm: {gain_margin}, Pm: {phase_margin}, Wc: {gain_cross_freq}, Wpi: {phase_cross_freq}")

    plot(output_csv)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Optimize PID parameters for the given transfer function JSON file and output results to CSV")
    
    # Transfer function JSON fileのパスを引数として追加
    parser.add_argument('file_path', type=str, help="Path to the transfer function JSON file")
    
    # Kd を引数として追加
    parser.add_argument('--Kd', type=float, required=True, help="Derivative gain (Kd) for the PID controller")
    
    # CSV出力ファイルのパス
    parser.add_argument('--output_csv', type=str, default='pid_results.csv', help="Path to output CSV file")
    
    args = parser.parse_args()
    
    # メイン処理を実行
    main(args.file_path, args.Kd, args.output_csv)

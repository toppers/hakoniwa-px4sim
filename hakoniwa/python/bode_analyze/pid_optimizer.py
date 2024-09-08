import argparse
import json
import numpy as np
from parser import Parser
from scipy.optimize import minimize
from scipy import signal
import matplotlib.pyplot as plt


# 伝達関数のシミュレーション
def simulate_pid(Kp, Ki, Kd, transfer_function_data, target_response):
    parser = Parser(transfer_function_data)
    num, den = parser.get_transfer_function_coefficients()
    
    # 伝達関数を作成
    system = signal.TransferFunction(num, den)
    
    # 時間範囲を指定してステップ応答を計算
    t = np.linspace(0, 10, len(target_response))  # target_responseと同じ長さに
    t, y = signal.step(system, T=t)
    return t, y

# コスト関数の修正（誤差の積分やオーバーシュートをペナルティとして加える）
def cost_function(params, transfer_function_data, target_response):
    Kp, Ki, Kd = params
    t, y = simulate_pid(Kp, Ki, Kd, transfer_function_data, target_response)
    
    # 応答の誤差（目標ステップ応答とシミュレーション結果の差）
    error = np.sum((y - target_response) ** 2)
    
    # オーバーシュートにペナルティを加える
    overshoot_penalty = np.sum(np.maximum(0, y - target_response))
    
    return error + 10 * overshoot_penalty  # オーバーシュートを重視したペナルティ

# 最適化を実行する関数
def optimize_pid(transfer_function_data, target_response):
    # 初期PIDパラメータ
    initial_params = [1, 0.1, 0.01]  # 初期のKp, Ki, Kd
    
    # 最適化実行
    result = minimize(cost_function, initial_params, args=(transfer_function_data, target_response))
    
    # 最適なパラメータを取得
    optimal_Kp, optimal_Ki, optimal_Kd = result.x
    return optimal_Kp, optimal_Ki, optimal_Kd

# メイン関数
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Optimize PID parameters for the given transfer function JSON file")
    parser.add_argument('file_path', type=str, help="Path to the transfer function JSON file")
    args = parser.parse_args()

    # JSONファイルのパス
    json_file_path = args.file_path

    # 目標ステップ応答を設定
    target_response = np.ones(200)  # 目標応答（単純なステップ応答を1とする）

    # 最適化実行
    optimal_Kp, optimal_Ki, optimal_Kd = optimize_pid(json_file_path, target_response)
    print(f"Optimal Kp: {optimal_Kp}, Ki: {optimal_Ki}, Kd: {optimal_Kd}")

    # 最適化されたPIDパラメータでステップ応答をシミュレーション
    t, optimized_response = simulate_pid(optimal_Kp, optimal_Ki, optimal_Kd, json_file_path, target_response)

    # 結果をプロット
    plt.plot(t, optimized_response, label="Optimized Response")
    plt.plot(t, target_response[:len(t)], 'r--', label="Target Response")
    plt.xlabel('Time')
    plt.ylabel('Response')
    plt.legend()
    plt.grid(True)
    plt.show()

import argparse
import matplotlib.pyplot as plt
from scipy import signal
from parser import Parser
import numpy as np

# ゲイン余裕と位相余裕の計算関数
def calculate_margins(w, mag, phase):
    gain_cross_freq = None
    phase_at_gain_cross = None
    phase_cross_freq = None
    gain_at_phase_cross = None

    # 位相 -180度を探す
    for i in range(len(phase) - 1):
        if phase[i] > -180 and phase[i + 1] <= -180:
            phase_cross_freq = w[i] + (w[i + 1] - w[i]) * (-180 - phase[i]) / (phase[i + 1] - phase[i])
            gain_at_phase_cross = mag[i]
    
    # ゲイン 0 dBを探す
    for i in range(len(mag) - 1):
        if mag[i] > 0 and mag[i + 1] <= 0:
            gain_cross_freq = w[i] + (w[i + 1] - w[i]) * (0 - mag[i]) / (mag[i + 1] - mag[i])
            phase_at_gain_cross = phase[i]
    
    gain_margin = None
    if gain_at_phase_cross is not None:
        gain_margin = -gain_at_phase_cross
    
    phase_margin = None
    if phase_at_gain_cross is not None:
        phase_margin = phase_at_gain_cross + 180

    return gain_margin, phase_margin, gain_cross_freq, phase_cross_freq

# ボード線図を表示する関数
def plot_bode_and_margins(transfer_function_data):
    parser = Parser(transfer_function_data)
    num, den = parser.get_transfer_function_coefficients()

    system = signal.TransferFunction(num, den)
    w, mag, phase = signal.bode(system)

    # ゲイン余裕と位相余裕を計算
    gain_margin, phase_margin, gain_cross_freq, phase_cross_freq = calculate_margins(w, mag, phase)

    # ゲイン余裕と位相余裕の結果を表示
    print(f"ゲイン余裕: {gain_margin} dB")
    print(f"位相余裕: {phase_margin} degrees")
    print(f"ゲイン余裕発生周波数: {gain_cross_freq} rad/s")
    print(f"位相余裕発生周波数: {phase_cross_freq} rad/s")

    # ボード線図の描画
    plt.figure()

    # ゲイン線図
    plt.subplot(2, 1, 1)
    plt.semilogx(w, mag)
    plt.title('Bode Plot')
    plt.ylabel('Magnitude (dB)')
    plt.grid(which='both', linestyle='--', linewidth=0.7)  # 縦軸・横軸に波線を表示
    
    # ゲイン交差周波数に縦線を追加
    if gain_cross_freq:
        plt.axvline(x=gain_cross_freq, color='r', linestyle='--', label='Gain Cross Freq')
        plt.axhline(y=0, color='k', linestyle='--')  # ゲイン0dBのライン

    # 位相線図
    plt.subplot(2, 1, 2)
    plt.semilogx(w, phase)
    plt.ylabel('Phase (degrees)')
    plt.xlabel('Frequency (rad/s)')
    plt.grid(which='both', linestyle='--', linewidth=0.7)  # 縦軸・横軸に波線を表示

    # 位相交差周波数に縦線と位相余裕に横線を追加
    if phase_cross_freq:
        plt.axvline(x=phase_cross_freq, color='r', linestyle='--', label='Phase Cross Freq')
        plt.axhline(y=-180, color='k', linestyle='--')  # 位相-180度のライン

    plt.show()

# ステップ応答をプロットする関数
def plot_step_response(transfer_function_data):
    parser = Parser(transfer_function_data)
    num, den = parser.get_transfer_function_coefficients()

    # 伝達関数を生成
    system = signal.TransferFunction(num, den)

    # ステップ応答を計算
    t, step_response = signal.step(system)

    # ステップ応答をプロット
    plt.figure()
    plt.plot(t, step_response)
    plt.title('Step Response')
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.grid()
    plt.show()

# インパルス応答をプロットする関数
def plot_impulse_response(transfer_function_data):
    parser = Parser(transfer_function_data)
    num, den = parser.get_transfer_function_coefficients()

    # 伝達関数を生成
    system = signal.TransferFunction(num, den)

    # インパルス応答を計算
    t, impulse_response = signal.impulse(system)

    # インパルス応答をプロット
    plt.figure()
    plt.plot(t, impulse_response)
    plt.title('Impulse Response')
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.grid()
    plt.show()

# メイン処理
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Bode, Step, and Impulse Response Plotter from transfer function JSON file")
    parser.add_argument('file_path', type=str, help="Path to the transfer function JSON file")
    parser.add_argument('--response', type=str, choices=['bode', 'step', 'impulse'], default='bode', help="Type of response to plot (bode, step, impulse)")
    args = parser.parse_args()

    transfer_function_data = args.file_path

    # ボード線図、ステップ応答、インパルス応答のいずれかをプロット
    if args.response == 'bode':
        plot_bode_and_margins(transfer_function_data)
    elif args.response == 'step':
        plot_step_response(transfer_function_data)
    elif args.response == 'impulse':
        plot_impulse_response(transfer_function_data)

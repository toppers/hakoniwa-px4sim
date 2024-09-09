import argparse
import matplotlib.pyplot as plt
from parser import Parser
import numpy as np
import control as ctrl

# 極をプロットする関数
def plot_poles(transfer_function_data):
    parser = Parser(transfer_function_data)
    num, den = parser.get_transfer_function_coefficients()

    system = ctrl.TransferFunction(num, den)
    poles = system.poles()
    print(f"システムの極: {poles}")

    # 極をプロット
    plt.figure()
    plt.scatter(poles.real, poles.imag, color='red', marker='x')
    plt.axhline(0, color='black', linewidth=0.5)
    plt.axvline(0, color='black', linewidth=0.5)
    plt.grid(True)
    plt.title('Pole-Zero Plot')
    plt.xlabel('Real')
    plt.ylabel('Imaginary')
    plt.show()


# ボード線図を表示する関数
def plot_bode_and_margins(transfer_function_data):
    parser = Parser(transfer_function_data)
    num, den = parser.get_transfer_function_coefficients()
    system = ctrl.TransferFunction(num, den)
    
    # Bodeプロットと位相余裕・ゲイン余裕の計算
    mag, phase, omega = ctrl.bode(system, dB=True, Hz=False, omega_limits=(1e-2, 1e4), plot=False)
    gm, pm, wg, wp = ctrl.margin(system)
    
    # ゲイン余裕と位相余裕を表示
    print(f"ゲイン余裕 (Gain Margin): {20 * np.log10(gm) if gm else '∞'} dB")
    print(f"位相余裕 (Phase Margin): {pm} degrees")
    print(f"ゲイン余裕発生周波数: {wg} rad/s")
    print(f"位相余裕発生周波数: {wp} rad/s")
    
    # Bodeプロットを描画
    plt.figure()
    
    # ゲイン線図
    plt.subplot(2, 1, 1)
    plt.semilogx(omega, 20 * np.log10(mag))
    plt.title('Bode Plot')
    plt.ylabel('Magnitude (dB)')
    plt.grid(True, which="both", linestyle='--')
    
    # ゲイン交差周波数に縦線を追加
    if wg:
        plt.axvline(wg, color='r', linestyle='--', label=f'Gain Cross @ {wg:.2f} rad/s')
    plt.axhline(0, color='k', linestyle='--')

    # 位相線図
    plt.subplot(2, 1, 2)
    plt.semilogx(omega, phase)
    plt.ylabel('Phase (degrees)')
    plt.xlabel('Frequency (rad/s)')
    plt.grid(True, which="both", linestyle='--')

    # 位相交差周波数に縦線を追加
    if wp:
        plt.axvline(wp, color='r', linestyle='--', label=f'Phase Cross @ {wp:.2f} rad/s')
    plt.axhline(-180, color='k', linestyle='--')

    plt.show()

# ステップ応答をプロットする関数
def plot_step_response(transfer_function_data):
    parser = Parser(transfer_function_data)
    num, den = parser.get_transfer_function_coefficients()

    system = ctrl.TransferFunction(num, den)
    
    # ステップ応答の計算とプロット
    t, y = ctrl.step_response(system)
    plt.figure()
    plt.plot(t, y)
    plt.title('Step Response')
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.grid(True)
    plt.show()

# インパルス応答をプロットする関数
def plot_impulse_response(transfer_function_data):
    parser = Parser(transfer_function_data)
    num, den = parser.get_transfer_function_coefficients()

    system = ctrl.TransferFunction(num, den)
    
    # インパルス応答の計算とプロット
    t, y = ctrl.impulse_response(system)
    plt.figure()
    plt.plot(t, y)
    plt.title('Impulse Response')
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.grid(True)
    plt.show()

# メイン処理
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Bode, Step, Impulse, and Pole-Zero Plotter from transfer function JSON file")
    parser.add_argument('file_path', type=str, help="Path to the transfer function JSON file")
    parser.add_argument('--response', type=str, choices=['bode', 'step', 'impulse', 'poles'], default='bode', help="Type of response to plot (bode, step, impulse, poles)")
    args = parser.parse_args()

    transfer_function_data = args.file_path

    # ボード線図、ステップ応答、インパルス応答、極のプロットのいずれかをプロット
    if args.response == 'bode':
        plot_bode_and_margins(transfer_function_data)
    elif args.response == 'step':
        plot_step_response(transfer_function_data)
    elif args.response == 'impulse':
        plot_impulse_response(transfer_function_data)
    elif args.response == 'poles':
        plot_poles(transfer_function_data)

import argparse
import matplotlib.pyplot as plt
import control as ctrl
import numpy as np

import json
import re
from sympy import symbols, Poly, simplify

class TransParser:
    def __init__(self, json_file):
        self.json_file = json_file
        self.data = self.load_transfer_function()
        self.constants = self.data['constants']

    def update_constant(self, const_name, new_value):
        if const_name in self.constants:
            self.constants[const_name] = new_value
        else:
            print(f"{const_name} is not found in constants")

    # JSONファイルの読み込み
    def load_transfer_function(self):
        with open(self.json_file, 'r') as file:
            data = json.load(file)
        return data

    # 伝達関数の各項を定数で置き換えて評価する
    def evaluate_terms(self, terms):
        evaluated_terms = []
        for term in terms:
            for const, value in self.constants.items():
                # 正規表現で変数名の前後が他の文字と一致しない場合のみ置き換え
                pattern = r'\b' + re.escape(const) + r'\b'
                term = re.sub(pattern, str(value), term)
            evaluated_terms.append(eval(term))  # 式を計算
        return evaluated_terms

    # C(s) の取得
    def get_controller(self):
        controller_data = self.data['controller']
        numerator = self.evaluate_terms(controller_data['num'])
        denominator = self.evaluate_terms(controller_data['den'])
        return Poly(numerator, s), Poly(denominator, s)

    # P(s) (プラント) の取得
    def get_plants(self):
        plants_data = self.data['plants']
        overall_num = Poly([1], s)  # 初期値 1
        overall_den = Poly([1], s)  # 初期値 1
        for plant in plants_data:
            num = self.evaluate_terms(plant['num'])
            den = self.evaluate_terms(plant['den'])
            plant_num_poly = Poly(num, s)
            plant_den_poly = Poly(den, s)
            # 複数プラントがある場合は掛け合わせる
            overall_num *= plant_num_poly
            overall_den *= plant_den_poly
        return overall_num, overall_den

    # L(s) = C(s) * P(s) の計算
    def calculate_l(self):
        # コントローラ C(s) とプラント P(s) を取得
        C_num, C_den = self.get_controller()
        P_num, P_den = self.get_plants()

        # L(s) の計算 (分子同士と分母同士の掛け算)
        L_num = C_num * P_num
        L_den = C_den * P_den
        return L_num, L_den

    # W(s) = L(s) / (1 + L(s)) の計算
    # W(s) = L_num / (L_den + L_num) の計算
    def calculate_w(self):
        # L(s) の分子と分母を取得
        L_num, L_den = self.calculate_l()

        # W(s) = L_num / (L_den + L_num) を計算
        W_num = L_num  # W(s) の分子は L(s) の分子
        W_den = L_den + L_num  # W(s) の分母は L(s) の分母 + L(s) の分子

        return W_num,  W_den
    
    # Ed(s) = P(s) / (1 + L(s)) の計算
    def calculate_ed(self):
        # プラント P(s) を取得
        P_num, P_den = self.get_plants()
        
        # L(s) の分子と分母を取得
        L_num, L_den = self.calculate_l()

        # Ed(s) = P_num / (P_den * (1 + L(s)))
        Ed_num = P_num  # Ed(s) の分子はプラント P(s) の分子
        Ed_den = P_den * (L_den + L_num)  # 分母は P(s) の分母と 1 + L(s) の掛け算
        
        return Ed_num, Ed_den

    # 多項式の係数をリスト形式で取得するメソッド
    def get_coefficients(self, poly):
        # sympyの `all_coeffs` メソッドを使用して係数をリストとして取得し、floatに変換
        return [float(coeff) for coeff in poly.all_coeffs()]



# ボード線図を表示する関数
def plot_bode_and_margins(num, den):
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
    phase = np.rad2deg(phase) 
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

# 極をプロットする関数
def plot_poles(num, den):
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


# ステップ応答をプロットする関数
def plot_step_response(num, den):
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
def plot_impulse_response(num, den):
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
    parser.add_argument('func_type', type=str, choices=['ws', 'ls', 'eds'], default='ls', help="Type of transfer function type")
    parser.add_argument('--mode', type=str, choices=['bode', 'step', 'impulse', 'poles' ], default='bode', help="Type of response to plot (bode, step, impulse, poles)")
    args = parser.parse_args()

    transfer_function_data = args.file_path
    tfd = TransParser(transfer_function_data)

    # s をシンボルとして定義
    s = symbols('s')
    num = []
    den = []
    if args.func_type == 'ws':
        # W(s) の計算
        W_num, W_den = tfd.calculate_w()
        num = tfd.get_coefficients(W_num)
        den = tfd.get_coefficients(W_den)
    elif args.func_type == 'ls':
        # L(s) の計算
        L_num, L_den = tfd.calculate_l()
        num = tfd.get_coefficients(L_num)
        den = tfd.get_coefficients(L_den)
    else:
        Ed_num, Ed_den = tfd.calculate_ed()
        num = tfd.get_coefficients(Ed_num)
        den = tfd.get_coefficients(Ed_den)

    print('num: ', num)
    print('den: ', den)

    # ボード線図、ステップ応答、インパルス応答、極のプロットのいずれかをプロット
    if args.mode == 'bode':
        plot_bode_and_margins(num, den)
    elif args.mode == 'step':
        plot_step_response(num, den)
    elif args.mode == 'impulse':
        plot_impulse_response(num, den)
    elif args.mode == 'poles':
        plot_poles(num, den)


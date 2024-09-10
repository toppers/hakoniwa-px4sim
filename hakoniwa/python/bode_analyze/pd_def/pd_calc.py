import argparse
import json
import math
import re

class PDEvaluator:
    def __init__(self, json_file):
        self.json_file = json_file
        self.data = self.load()
        self.plant_u = self.data['plant']['u']
        self.plant_v = self.data['plant']['v']
        self.constants = self.data['constants']
        self.constants.update(self.data['args'])  # 'args' の値を 'constants' に追加
        self.PM = self.data['args']['PM']
        self.Ki = self.data['args']['Ki']
        self.Wc = self.data['args']['Wc']
        self.Ts = self.data['constants']['Ts']
        self.Ix = self.data['constants']['Ix']

    def load(self):
        """JSONファイルを読み込む"""
        with open(self.json_file, 'r') as file:
            data = json.load(file)
        return data

    def substitute_variables(self, expression, constants):
        """正規表現で変数を定数に置換する"""
        # 定数を正規表現で置換
        for key, value in constants.items():
            expression = re.sub(r'\b' + re.escape(key) + r'\b', str(value), expression)
        return expression

    def calc(self):
        """数式を計算する前の状態で出力し、その後に計算"""
        # 変数の置換
        u_expr_substituted = self.substitute_variables(self.plant_u, self.constants)
        v_expr_substituted = self.substitute_variables(self.plant_v, self.constants)

        # 置換後の数式を表示（計算前の状態）
        print(f"u with substituted values (before eval): {u_expr_substituted}")
        print(f"v with substituted values (before eval): {v_expr_substituted}")

        # evalで計算
        u_result = eval(u_expr_substituted)
        v_result = eval(v_expr_substituted)

        # 計算結果の出力
        print(f"u result: {u_result}")
        print(f"v result: {v_result}")


        p_s = 1/ ((1j)*self.Wc*self.Ix*(self.Ts*(1j*self.Wc) + 1))
        print("real: ", p_s.real)
        print("im: ", p_s.imag)


        self.u = u_result
        self.v = v_result

    def calc_phi_m(self):
        """PMからφmを計算"""
        pm = math.radians(self.PM)
        return pm - math.pi
    
    def getKp(self):
        """Kpを計算"""
        phi_m = self.calc_phi_m()
        print("phi_m", phi_m)
        numerator = self.u * math.cos(phi_m) + self.v * math.sin(phi_m)
        denominator = self.u**2 + self.v**2
        kp = numerator / denominator
        return kp

    def getKd(self):
        """Kdを計算"""
        phi_m = self.calc_phi_m()
        numerator = self.u * math.sin(phi_m) - self.v * math.cos(phi_m)
        denominator = self.Wc * (self.u**2 + self.v**2)
        kd = ((self.Ki / self.Wc**2) - (numerator / denominator))
        return kd

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Calculate PD parameters for the given Argument JSON file")    
    parser.add_argument('file_path', type=str, help="Path to the Argument JSON file")
    args = parser.parse_args()

    evaluator = PDEvaluator(args.file_path)
    evaluator.calc()

    Kp = evaluator.getKp()
    Kd = evaluator.getKd()
    print("Kp: ", Kp)
    print("Kd: ", Kd)

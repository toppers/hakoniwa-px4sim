import json
import re

class Parser:
    def __init__(self, json_file):
        self.json_file = json_file
        self.data = self.load_transfer_function()
        self.numerator = self.data['numerator']
        self.denominator = self.data['denominator']
        self.constants = self.data['constants']

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


    # 分子と分母を評価して数値に変換
    def get_transfer_function_coefficients(self):
        evaluated_numerator = self.evaluate_terms(self.numerator)
        evaluated_denominator = self.evaluate_terms(self.denominator)

        print("evaluated_numerator: ", evaluated_numerator)
        print("evaluated_denominator: ", evaluated_denominator)

        return evaluated_numerator, evaluated_denominator

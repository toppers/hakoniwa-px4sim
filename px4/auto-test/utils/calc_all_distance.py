import os
import pandas as pd
import re
import argparse
import math
from calc_actual_distance import calculate_total_distance_quick
from calc_expected_distance import calculate_total_distance

# コマンドライン引数の定義
parser = argparse.ArgumentParser(description="test eval directory.")
parser.add_argument("dir", help="Path to the eval directory")
parser.add_argument("--scenario", help="Test Scenario")
parser.add_argument("--first", help="Column name for the first parameter", default='FirstParam')
parser.add_argument("--second", help="Column name for the second parameter", default='SecondParam')
args = parser.parse_args()

def normal_pdf(x, mean, std):
    """正規分布の確率密度関数"""
    return (1 / (std * math.sqrt(2 * math.pi))) * math.exp(-0.5 * ((x - mean) / std) ** 2)


def extract_data(file_path):
    return calculate_total_distance_quick(file_path)

def extract_params(file_name):
    """ ファイル名からパラメータの値を抽出 """
    file_name = file_name.replace(".csv", "")
    match = re.search(r'p([0-9._]+)-p([0-9._]+)', file_name)
    if match:
        return match.groups()
    return None, None

expected_distance = calculate_total_distance(args.scenario)
print(f'expected_distance: {expected_distance}')

# ディレクトリ内のすべてのファイルを走査
directory = args.dir
data = []

for file_name in os.listdir(directory):
    file_path = os.path.join(directory, file_name)
    
    if os.path.isfile(file_path):
        param1, param2 = extract_params(file_name)
        actual_distance = extract_data(file_path)
        peak_pdf = normal_pdf(expected_distance, expected_distance, 50)
        actual_pdf = normal_pdf(actual_distance, expected_distance, 50)
        #print(f'peak_pdf: {peak_pdf} actual_pdf:{actual_pdf}')
        score = (actual_pdf/peak_pdf) * 100.0
        print(f'param1={param1} param2={param2} d={actual_distance} score={score}')
        if param1 is not None and param2 is not None and score is not None:
            data.append([param1.replace("_", "."), param2.replace("_", "."), score])

# DataFrame を作成
df = pd.DataFrame(data, columns=[args.first, args.second, 'Score'])

df[args.first] = pd.to_numeric(df[args.first], errors='coerce')
df[args.second] = pd.to_numeric(df[args.second], errors='coerce')

# DataFrame をカラム名でソート
df.sort_values(by=[args.first, args.second], inplace=True)

# CSV に変換
df.to_csv('distance_result.csv', index=False)


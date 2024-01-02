import json
import math
import argparse

def calculate_3d_distance(x1, y1, z1, x2, y2, z2):
    """ 二点間の3次元ユークリッド距離を計算する（単位：メートル） """
    return math.sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2 + (z1 - z2) ** 2)

# コマンドライン引数の解析
parser = argparse.ArgumentParser(description="Calculate the total travel distance from a JSON scenario.")
parser.add_argument("json_file", help="Path to the JSON file containing the scenario")
args = parser.parse_args()

# JSONファイルの読み込み
with open(args.json_file, 'r') as file:
    data = json.load(file)

# シナリオデータの取得
scenario = data['scenario']

# 移動距離の計算
total_distance = 0
for i in range(len(scenario)):
    if i == 0:
        distance = scenario[0].get('alt')
    else:
        op1 = scenario[i-1]
        op2 = scenario[i]
        distance = calculate_3d_distance(op1.get('lat', 0), op1.get('lon', 0), op1.get('alt', 0),
                                        op2.get('lat', 0), op2.get('lon', 0), op2.get('alt', 0))
    #print(f'distance[{i}]: {distance}')
    total_distance += distance

print(f"{total_distance}")

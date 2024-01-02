import pandas as pd
import numpy as np
import argparse

parser = argparse.ArgumentParser('calculate distance')
parser.add_argument('csv_file', help='moving data')
args = parser.parse_args()

# CSVファイルの読み込み
csv_file = args.csv_file
df = pd.read_csv(csv_file)

# ユークリッド距離の計算
def calculate_distance(row1, row2):
    return np.sqrt((row1['X'] - row2['X'])**2 + (row1['Y'] - row2['Y'])**2  + (row1['Z'] - row2['Z'])**2)

# 連続する座標間の距離を計算
distances = [calculate_distance(df.iloc[i], df.iloc[i+1]) for i in range(len(df)-2)]

# 総移動距離の計算
total_distance = sum(distances)

print(f"{total_distance}")

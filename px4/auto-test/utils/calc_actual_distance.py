import pandas as pd
import numpy as np
import argparse


def calculate_total_distance_quick(csv_file):
    # CSVファイルの読み込み
    df = pd.read_csv(csv_file, skipfooter=1, engine='python')

    # X, Y, Z の差分を計算
    diff = df[['X', 'Y', 'Z']].diff().iloc[1:]

    # 各差分の2乗和の平方根を計算（ユークリッド距離）
    distances = np.sqrt((diff ** 2).sum(axis=1))

    # 総移動距離の計算
    return distances.sum()

def calculate_total_distance(csv_file):
    # CSVファイルの読み込み
    df = pd.read_csv(csv_file)

    # ユークリッド距離の計算
    def calculate_distance(row1, row2):
        return np.sqrt((row1['X'] - row2['X'])**2 + (row1['Y'] - row2['Y'])**2  + (row1['Z'] - row2['Z'])**2)

    # 連続する座標間の距離を計算
    distances = [calculate_distance(df.iloc[i], df.iloc[i+1]) for i in range(len(df)-2)]

    # 総移動距離の計算
    return sum(distances)

if __name__ == "__main__":
    parser = argparse.ArgumentParser('calculate distance')
    parser.add_argument('csv_file', help='CSV file containing moving data')
    args = parser.parse_args()

    total_distance = calculate_total_distance(args.csv_file)
    print(f"Total distance: {total_distance}")

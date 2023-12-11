import matplotlib.pyplot as plt
import pandas as pd

# CSVファイルの読み込み
# 4つのCSVファイルを読み込むためのファイルパスのリスト
file_paths = ['./cmake-build/log_rotar_0.csv', './cmake-build/log_rotar_1.csv', './cmake-build/log_rotar_2.csv', './cmake-build/log_rotar_3.csv']

# データフレームのリストを作成
dfs = []

# 各CSVファイルを読み込んで、データフレームのリストに追加
for file_path in file_paths:
    df = pd.read_csv(file_path)
    dfs.append(df)

# 各データフレームについてプロット
for i, df in enumerate(dfs):
    plt.plot(df['TIME'], df['RPM'], label=f'Rotor {i}')

# グラフの設定
plt.xlabel('Time (s)')
plt.ylabel('RPM')
plt.title('RPM over Time for Each Rotor')
plt.legend()

# グラフの表示
plt.show()

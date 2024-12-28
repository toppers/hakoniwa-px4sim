import pandas as pd
import matplotlib.pyplot as plt
import sys

# コマンドライン引数からファイルパスと列名を取得
if len(sys.argv) != 3:
    print("Usage: python script.py <path_to_csv_file> <column_name>")
    sys.exit(1)

file_path = sys.argv[1]
column_name = sys.argv[2]

# CSVファイルの読み込み
df = pd.read_csv(file_path)

# グラフの描画
plt.figure(figsize=(10, 6))  # グラフのサイズ指定
plt.plot(df['Time'].to_numpy(), df[column_name].to_numpy())  # 時間と指定された列をNumPy配列に変換してプロット
plt.title(f'{column_name} over Time')  # グラフのタイトル
plt.xlabel('Time')  # x軸のラベル
plt.ylabel(column_name)  # y軸のラベル
plt.grid(True)  # グリッドの表示
plt.show()  # グラフの表示

import matplotlib.pyplot as plt
import pandas as pd

# CSVファイルの読み込み
# この例では、ファイル名を 'your_csv_file.csv' と仮定します
file_path = './cmake-build/log_thrust.csv'

# データフレームを読み込む
df = pd.read_csv(file_path)

# 各列についてプロット
plt.plot(df['TIME'], df['Thrust'], label='Thrust')
plt.plot(df['TIME'], df['Tx'], label='Tx')
plt.plot(df['TIME'], df['Ty'], label='Ty')
plt.plot(df['TIME'], df['Tz'], label='Tz')

# グラフの設定
plt.xlabel('Time (s)')
plt.ylabel('Values')
plt.title('Thrust and Torques over Time')
plt.legend()

# グラフの表示
plt.show()

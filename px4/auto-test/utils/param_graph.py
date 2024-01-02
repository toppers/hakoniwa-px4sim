import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
import numpy as np
from scipy.interpolate import griddata
import argparse

# コマンドライン引数の解析
parser = argparse.ArgumentParser(description="Create a 3D scatter plot from a CSV file.")
parser.add_argument("csv_file", help="Path to the CSV file")
parser.add_argument("first", help="Column name for the X-axis")
parser.add_argument("second", help="Column name for the Y-axis")
args = parser.parse_args()

# CSV ファイルを読み込む
df = pd.read_csv(args.csv_file)

# 3D散布図を作成
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# 指定された列名で値を取得
x = df[args.first].astype(float)
y = df[args.second].astype(float)
z = df['Score'].astype(float)  # 'Score'列は固定と仮定

# グリッドデータの生成
xi = np.linspace(x.min(), x.max(), 100)
yi = np.linspace(y.min(), y.max(), 100)
xi, yi = np.meshgrid(xi, yi)

# グリッド上でのz値の補間
zi = griddata((x, y), z, (xi, yi), method='linear')

# サーフェスプロットの描画
ax.plot_surface(xi, yi, zi, rstride=1, cstride=1, cmap=cm.coolwarm, linewidth=0, antialiased=False)

# 等高線の描画
ax.contour(xi, yi, zi, zdir='z', offset=ax.get_zlim()[0], cmap=cm.coolwarm)

# ラベルの追加
ax.set_xlabel(args.first)
ax.set_ylabel(args.second)
ax.set_zlabel('Score')

# カラーバーの表示
m = cm.ScalarMappable(cmap=cm.coolwarm)
m.set_array(z)
fig.colorbar(m)

# 上からの視点を設定
ax.view_init(elev=90, azim=-90)

plt.show()

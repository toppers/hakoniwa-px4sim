"""
hako_TimelineAnalyzer.py

概要:
このツールは、CSVフォーマットの時系列データを読み込み、指定された列のデータをグラフ化します。
複数のCSVファイルをマージして一つのグラフに表示する機能も提供します。

機能:
- 複数のCSVファイルの時系列データをマージしてグラフ化
- 指定された時間範囲内のデータのみをプロット
- 時間差分の計算オプション
- TIME列のバリデーションと共通部分の抽出
- 同名カラムの扱い（ファイル名プレフィックス付き）

使用方法:
python hako_TimelineAnalyzer.py <file_paths> --columns <columns> [--start_time <start>] [--duration <duration>] [--diff]

引数:
- file_paths: CSVファイルのパス（複数指定可能）
- --columns: グラフに表示する列名。ファイルが複数ある場合、'ファイル名.カラム名' で特定のファイルのカラムを指定できます。
- --start_time: グラフ表示の開始時間（デフォルトは0）
- --duration: 表示する期間（デフォルトは60秒）
- --diff: 時間差分の計算を行う場合に指定

補足:
- ファイルが複数ある場合、カラム名には自動的にファイル名がプレフィックスとして付けられます。この挙動はファイルが1つのみの場合には適用されません。
- TIME列のバリデーションでは、全てのファイルに共通する時間点のみが使用されます。一部のファイルにのみ存在する時間点は無視されます。

例:
python hako_TimelineAnalyzer.py data1.csv data2.csv --columns data1.Temp data2.Pressure --start_time 10 --duration 30 --diff

"""

import pandas as pd
import matplotlib.pyplot as plt
import argparse
import sys
import os

time_colname='timestamp'

def validate_and_merge_time_columns(df_list):
    # 全てのデータフレームのTIME列を比較して、共通の値を見つける
    common_times = set(df_list[0][time_colname])
    for df in df_list[1:]:
        common_times.intersection_update(set(df[time_colname]))

    if len(common_times) < len(df_list[0][time_colname]):
        print(f"Warning: Only {len(common_times)} common time points found. Others will be skipped.")

    # 共通のTIME値に基づいて各データフレームをフィルタリング
    filtered_df_list = []
    for df in df_list:
        filtered_df = df[df[time_colname].isin(common_times)]
        filtered_df_list.append(filtered_df)

    return filtered_df_list


# argparseを使用してコマンドライン引数を処理
parser = argparse.ArgumentParser(description="Plot columns from a CSV file over time.")
parser.add_argument("file_paths", nargs='+', help="Path(s) to the CSV file(s)")
parser.add_argument("--columns", nargs='+', help="Column name(s) to plot", required=True)
parser.add_argument("--start_time", type=int, default=0, help="Start time for plotting (in seconds)")
parser.add_argument("--duration", type=int, default=sys.maxsize, help="Duration to plot from start time (in seconds)")
parser.add_argument("--diff", action='store_true', help="Calculate time differences")
args = parser.parse_args()

# 複数のCSVファイルを読み込み
df_list = []
is_multiple_files = len(args.file_paths) > 1  # ファイルが複数あるかどうかのチェック

for file_path in args.file_paths:
    df = pd.read_csv(file_path)

    if is_multiple_files:
        # ファイル名（拡張子なし）を取得
        file_name = os.path.splitext(os.path.basename(file_path))[0]

        # TIME以外のカラム名にファイル名をプレフィックスとして追加
        df.rename(columns=lambda x: f"{file_name}.{x}" if x != time_colname else x, inplace=True)

    df_list.append(df)

# TIME列のバリデーション
try:
    df_list = validate_and_merge_time_columns(df_list)
except ValueError as e:
    print(e)
    sys.exit(1)

# データフレームのマージ
df = pd.concat(df_list, axis=1)
df = df.loc[:,~df.columns.duplicated()]  # 重複する列を除去

# TIME列の差分計算が指定されている場合は処理を行う
if args.diff:
    df[time_colname] = (df[time_colname] - df[time_colname].iloc[0]) / 1e6  # μsから秒に変換

# フィルタリング
end_time = args.start_time + args.duration
filtered_df = df[(df[time_colname] >= args.start_time) & (df[time_colname] <= end_time)]

# グラフの描画
plt.figure(figsize=(10, 6))  # グラフのサイズ指定

# 複数の列のデータをプロット
for column_name in args.columns:
    plt.plot(filtered_df[time_colname].to_numpy(), filtered_df[column_name].to_numpy(), label=column_name)

plt.title('Data over Time')  # グラフのタイトル
plt.xlabel('Time (seconds)' if args.diff else 'Time')  # x軸のラベル
plt.ylabel('Values')  # y軸のラベル
plt.grid(True)  # グリッドの表示
plt.legend()  # 凡例の表示
plt.show()  # グラフの表示

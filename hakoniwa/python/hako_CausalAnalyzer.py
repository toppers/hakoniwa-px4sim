"""
hako_CausalAnalyzer.py

概要:
hako_CausalAnalyzerは、複数のCSVファイルから時系列データを読み込み、指定された変数間の因果関係を分析し、可視化するツールです。独立変数と従属変数の関係をscatterまたはlineグラフで表示します。

機能:
- 複数のCSVファイルからのデータ読み込み。各ファイルにはTIME列が含まれることが前提です。
- scatterプロットでは、独立変数の時間に対して最も近い時点の従属変数データをプロットします。
- lineプロットでは、時系列データをスケール調整して表示します。
- 指定された時間範囲内のデータのみを分析およびプロット。

使用方法:
python hako_CausalAnalyzer.py <file_paths> --independent <independent_var> --dependent <dependent_var> [--graph_type <type>] [--start_time <start>] [--duration <duration>]

引数:
- file_paths: 分析するデータセットのファイルパス（複数指定可能）。
- --independent: 因果関係の元となる独立変数の名前。
- --dependent: 因果関係の結果として現れる従属変数の名前。
- --graph_type: 生成するグラフの種類（scatterまたはline）。
- --start_time: 分析を開始する時間（デフォルトは0）。
- --duration: 分析する期間（デフォルトは60秒）。
- --diff: 時間差分の計算を行う場合に指定

例:
python hako_CausalAnalyzer.py data1.csv data2.csv --independent Temperature --dependent Sales --graph_type scatter --start_time 10 --duration 30

補足:
- 各データファイルはCSV形式であり、TIME列が含まれていることが前提です。
- 独立変数と従属変数は、データファイル内のカラム名と一致している必要があります。
- scatterプロットでは、時間的に最も近い点を独立変数と従属変数のペアとして使用します。
- lineプロットでは、時系列データをスケール調整し、全てのファイルのデータを同じ時間スケールで表示します。
"""

import pandas as pd
import matplotlib.pyplot as plt
import argparse
import sys
import os

timecolumn = 'timestamp'

def read_and_preprocess(file_paths, start_time, duration, diff):
    df_list = []

    for file_path in file_paths:
        df = pd.read_csv(file_path)

        # --diff オプションが指定されている場合、時間差分を計算し、単位を秒に変換
        if diff:
            df[timecolumn] = (df[timecolumn] - df[timecolumn].iloc[0]) / 1e6

        # 時間範囲でフィルタリング
        end_time = start_time + duration
        filtered_df = df[(df[timecolumn] >= start_time) & (df[timecolumn] <= end_time)]

        df_list.append(filtered_df)

    return df_list

def prepare_scatter_data(df_list, independent_var, dependent_var):
    # 独立変数と従属変数のデータフレームを特定
    if len(df_list) > 1:
        independent_df = df_list[0] if independent_var in df_list[0] else df_list[1]
        dependent_df = df_list[1] if independent_var in df_list[0] else df_list[0]
    else:
        independent_df = df_list[0]
        dependent_df = df_list[0]

    scatter_data = []
    last_index = 0  # 従属変数の検索開始位置

    # 独立変数のデータをループ
    for _, row in independent_df.iterrows():
        closest_time = row[timecolumn]
        closest_value = None

        # 従属変数のデータを検索
        for idx in range(last_index, len(dependent_df)):
            dependent_time = dependent_df.iloc[idx][timecolumn]

            if dependent_time >= closest_time:
                closest_value = dependent_df.iloc[idx][dependent_var]
                last_index = idx
                break

        if closest_value is not None:
            scatter_data.append({
                timecolumn: closest_time,
                independent_var: row[independent_var],
                dependent_var: closest_value
            })
        else:
            # 最も近い従属変数の値が見つからない場合、検索を終了
            break

    return pd.DataFrame(scatter_data)

def normalize_series(data):
    min_val = data.min()
    max_val = data.max()
    return (data - min_val) / (max_val - min_val)

def prepare_line_data(df_list, independent_var, dependent_var):
    # ファイルが1つの場合、マージは不要で、データはそのまま使用される
    if len(df_list) == 1:
        # すでに単一のデータフレームとしてデータが存在するため、そのまま返す
        line_data = df_list[0]
    else:
        # ファイルが複数の場合、指定された独立変数と従属変数のデータフレームをマージ
        # 独立変数と従属変数が異なるデータフレームに存在することを想定
        independent_df = df_list[0] if independent_var in df_list[0] else df_list[1]
        dependent_df = df_list[1] if independent_var in df_list[0] else df_list[0]

        # データフレームを時間でソートし、最も近い時間のデータ同士でマージ
        line_data = pd.merge_asof(independent_df.sort_values(timecolumn), dependent_df.sort_values(timecolumn), on=timecolumn)

    # 必要なカラムのみを選択
    line_data = line_data[[independent_var, dependent_var, timecolumn]].dropna()

    return line_data



def analyze_causal_relationship(df, independent_var, dependent_var, graph_type):
    if graph_type == 'scatter':
        return prepare_scatter_data(df, independent_var, dependent_var)
    elif graph_type == 'line':
        return prepare_line_data(df, independent_var, dependent_var)


import matplotlib.pyplot as plt

def plot_data(df, independent_var, dependent_var, graph_type):
    fig, ax1 = plt.subplots(figsize=(10, 6))

    if graph_type == 'scatter':
        # Scatterプロット
        plt.scatter(df[independent_var], df[dependent_var], color='purple')
        plt.xlabel(independent_var)  # x軸には独立変数の名前
        plt.ylabel(dependent_var)    # y軸には従属変数の名前
        plt.title(f'{independent_var} vs {dependent_var} - {graph_type.title()} Plot')
        plt.grid(True)
        plt.legend()
        plt.grid(True)
    elif graph_type == 'line':
        # 独立変数のプロット
        ax1.plot(df[timecolumn], df[independent_var], color='blue', label=independent_var)
        ax1.set_xlabel(timecolumn)
        ax1.set_ylabel(independent_var, color='blue')
        ax1.tick_params(axis='y', labelcolor='blue')

        # 従属変数のプロットのための第二のy軸を作成
        ax2 = ax1.twinx()
        ax2.plot(df[timecolumn], df[dependent_var], color='orange', label=dependent_var)
        ax2.set_ylabel(dependent_var, color='orange')
        ax2.tick_params(axis='y', labelcolor='orange')

        # タイトルとグリッド
        plt.title(f'{independent_var} and {dependent_var} over Time')
        ax1.grid(True)

        # 凡例
        lines, labels = ax1.get_legend_handles_labels()
        lines2, labels2 = ax2.get_legend_handles_labels()
        ax2.legend(lines + lines2, labels + labels2, loc='upper left')

    plt.show()


# コマンドライン引数の処理
parser = argparse.ArgumentParser(description="Analyze and plot causal relationships from time series data.")
parser.add_argument("file_paths", nargs='+', help="Path(s) to the CSV file(s)")
parser.add_argument("--independent", required=True, help="Independent variable name")
parser.add_argument("--dependent", required=True, help="Dependent variable name")
parser.add_argument("--graph_type", choices=['scatter', 'line'], default='scatter', help="Type of graph to plot")
parser.add_argument("--start_time", type=int, default=0, help="Start time for analysis")
parser.add_argument("--duration", type=int, default=60, help="Duration for analysis")
parser.add_argument("--diff", action='store_true', help="Calculate time differences")
args = parser.parse_args()

# データの読み込みと前処理
df = read_and_preprocess(args.file_paths, args.start_time, args.duration, args.diff)

# 因果関係の分析
analyzed_data = analyze_causal_relationship(df, args.independent, args.dependent, args.graph_type)

# データのプロット
plot_data(analyzed_data, args.independent, args.dependent, args.graph_type)

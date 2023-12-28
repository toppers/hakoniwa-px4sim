import argparse
import json
import pandas as pd
import numpy as np
import os
import math

def normal_pdf(x, mean, std):
    """正規分布の確率密度関数"""
    return (1 / (std * math.sqrt(2 * math.pi))) * math.exp(-0.5 * ((x - mean) / std) ** 2)

def evaluate(actual_mean, actual_stdev, expected_mean, expected_stdev):
    print(f'actual_mean: {actual_mean}')
    print(f'actual_stdev: {actual_stdev}')
    # 平均値での正規分布のピーク値
    peak_pdf = normal_pdf(expected_mean, expected_mean, expected_stdev)

    # 平均値の評価
    mean_score = (normal_pdf(actual_mean, expected_mean, expected_stdev) / peak_pdf) * 100

    # 平均 + 標準偏差の評価
    upper_score = (normal_pdf(actual_mean + actual_stdev, expected_mean, expected_stdev) / peak_pdf) * 100

    # 平均 - 標準偏差の評価
    lower_score = (normal_pdf(actual_mean - actual_stdev, expected_mean, expected_stdev) / peak_pdf) * 100

    # スコアの平均を計算
    total_score = (mean_score + upper_score + lower_score) / 3
    print(f'mean_score: {mean_score}')
    print(f'upper_score: {upper_score}')
    print(f'lower_score: {lower_score}')
    print(f'total_score: {total_score}')
    return total_score

def load_data(file_path, config):
    data = pd.read_csv(file_path)

    # 最後の行はデータが中途半端な可能性があるため
    data = data.drop(data.index[-1])

    #print("file_path: ", file_path)
    time_col = config['time_colname']
    data_col = config['data_colname']

    data[time_col] = (data[time_col] - data[time_col].iloc[0])
    #print(data[1:10])
    # タイムスタンプを秒単位に変換 (マイクロ秒から秒へ)
    if config["time_unit"] == "usec":
        data[time_col] = data[time_col] / 1e6
        #print(data[1:10])

    offset = config['offset_sec']
    duration = config['duration_sec']
    last_time = data.tail(1)[time_col].iloc[0]
    #print(data.tail())
    #print(f'offset: {offset}')
    #print(f'duration: {duration}')
    #print(f'last_time: {last_time}')

    # 相対時間を計算
    if config['data_range_from_end']:
        relative_start_time = last_time - offset - duration
        relative_end_time   = last_time - offset
    else:
        relative_start_time = 0 + offset
        relative_end_time   = 0 + offset + duration
    print(f'relative_start_time: {relative_start_time}')
    print(f'relative_end_time: {relative_end_time}')

    # 指定された時間範囲のデータを抽出
    data = data[(data[time_col] >= relative_start_time) & (data[time_col] <= relative_end_time)]
    return data[data_col]

def main():
    parser = argparse.ArgumentParser(description='Evaluate the mean and standard deviation of data.')
    parser.add_argument('input_json', help='Path to the JSON configuration file')
    parser.add_argument('data_location_path', help='Path to the data location')
    args = parser.parse_args()

    with open(args.input_json, 'r') as json_file:
        config = json.load(json_file)

    filepath = os.path.join(args.data_location_path, config['file'])
    data = load_data(filepath, config)
    actual_mean = np.mean(data)
    actual_stdev = np.std(data)

    score = evaluate(actual_mean, actual_stdev, config['expected_mean_value'], config['expected_stdev_value'])
    print(f'Score: {score}')

if __name__ == '__main__':
    main()

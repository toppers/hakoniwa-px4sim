import argparse
import json
import pandas as pd
import numpy as np
import os

def evaluate(actual_mean, actual_stdev, expected_mean, expected_stdev):
    print(f'actual_mean: {actual_mean}')
    print(f'actual_stdev: {actual_stdev}')
    mean_diff_score = max(0, 100 - (abs(actual_mean - expected_mean) / expected_mean * 100))
    stdev_diff_score = max(0, 100 - (actual_stdev / expected_stdev * 100))
    print(f'mean_diff_score: {mean_diff_score}')
    print(f'stdev_diff_score: {stdev_diff_score}')
    total_score = (mean_diff_score + stdev_diff_score) / 2
    return total_score

def load_data(file_path, config):
    data = pd.read_csv(file_path)
    time_col = config['time_colname']
    data_col = config['data_colname']


    data[time_col] = (data[time_col] - data[time_col].iloc[0])
    # タイムスタンプを秒単位に変換 (マイクロ秒から秒へ)
    if config["time_unit"] == "usec":
        data[time_col] = data[time_col] / 1e6
        #print(data[1:10])

    offset = config['offset_sec']
    duration = config['duration_sec']
    last_time = data.tail(1)[time_col].iloc[0]


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

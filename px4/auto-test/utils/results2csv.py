import os
import pandas as pd
import re
import argparse

# コマンドライン引数の定義
parser = argparse.ArgumentParser(description="test eval directory.")
parser.add_argument("dir", help="Path to the eval directory")
parser.add_argument("--first", help="Column name for the first parameter", default='FirstParam')
parser.add_argument("--second", help="Column name for the second parameter", default='SecondParam')
args = parser.parse_args()

def extract_data(file_path):
    """ ファイルから Score の値を抽出 """
    with open(file_path, 'r') as file:
        for line in file:
            if line.startswith('Score:'):
                return float(line.split(':')[1].strip())
    return None

def extract_params(file_name):
    """ ファイル名からパラメータの値を抽出 """
    file_name = file_name.replace(".txt", "")
    match = re.search(r'p([0-9._]+)-p([0-9._]+)', file_name)
    if match:
        return match.groups()
    return None, None

# ディレクトリ内のすべてのファイルを走査
directory = args.dir
data = []

for file_name in os.listdir(directory):
    file_path = os.path.join(directory, file_name)
    
    if os.path.isfile(file_path):
        param1, param2 = extract_params(file_name)
        score = extract_data(file_path)
        
        if param1 is not None and param2 is not None and score is not None:
            data.append([param1.replace("_", "."), param2.replace("_", "."), score])

# DataFrame を作成
df = pd.DataFrame(data, columns=[args.first, args.second, 'Score'])

# DataFrame をカラム名でソート
#df.sort_values(by=[args.first, args.second], inplace=True)

# 同じパラメータの組み合わせで平均化
df_avg = df.groupby([args.first, args.second]).mean().reset_index()

# カラムを数値型に変換
df_avg[args.first] = pd.to_numeric(df_avg[args.first], errors='coerce')
df_avg[args.second] = pd.to_numeric(df_avg[args.second], errors='coerce')

# DataFrame をカラム名でソート
df_avg.sort_values(by=[args.first, args.second], inplace=True)


# CSV に変換
df_avg.to_csv('averaged_result.csv', index=False)

print('平均化されたスコアのCSVファイルに変換されました: averaged_result.csv')
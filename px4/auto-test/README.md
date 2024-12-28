# できること

箱庭ドローンシミュレータでは、以下を自動化できます。

* 箱庭シミュレータ内部のコンポーネント（センサや物理モデルなど）のテスト
* ドローン制御プログラムのテスト
* 機体パラメータのチューニング・テスト

# 自動テストシステムのアーキテクチャ

<img width="929" alt="スクリーンショット 2023-12-29 10 59 02" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/6115dfa5-8cf6-47ff-b6d6-843aca488422">

自動テストシステムは、複数のコンポーネントから構成され、ドローンの動作テスト、機体パラメータのチューニング、ドローン制御プログラムの検証を可能にします。以下の説明は、システムの各コンポーネントとその機能を明確にします。

- **Drone Parameter Generator**: 
  機体のパラメータをテスト開始時に生成し、テストの各イテレーションで異なる機体特性をシミュレートできるようにします。

- **Test Scenario**: 
  定義されたテストシナリオを含むドキュメントです。ここには、テストの操作手順が記述されています。

- **Test Scenario Executor**: 
  テストシナリオに従って、`Hakoniwa` と `PX4` 内部の機体制御プログラムを用いてテストを実行します。

- **Hakoniwa**: 
  箱庭シミュレーション環境で、テストシナリオに基づきドローンの動作をシミュレートするコンポーネントです。箱庭シミュレーション内部の各種コンポーネント（センサや物理モデルなど）をテストできます。

- **PX4**: 
  ドローンのフライトコントロールソフトウェアであり、ドローンの飛行を制御します。PX4内部の各種コンポーネント（機体制御プログラム）をテストできます。

- **Logs / LogOutputDirectory**: 
  テスト実行中に生成される詳細なログファイル（センサモデルや物理モデルの内部ログデータ）で、テスト評価するために利用します。

- **Test Result Evaluator**: 
  テストの実行結果であるログ情報解析し、成功または失敗の結果を `Test Result` に記録します。

これらのコンポーネントは連携して動作し、柔軟かつ精密なテストを実現します。結果として、開発プロセスが大幅に加速されます。

## Drone Parameter Generator

[機体のパラメータ](https://github.com/toppers/hakoniwa-px4sim/blob/main/hakoniwa/README.md#機体のパラメータ説明)は、[drone_config.py](https://github.com/toppers/hakoniwa-px4sim/blob/main/hakoniwa/python/drone_config.py)で設定できます。

このスクリプトを使用して、指定されたJSONファイル内の値を更新できます。

### 使い方

スクリプトはコマンドラインから次のように実行されます：

```
python drone_config.py <json_file> <path> <new_value>
```

### 引数

- `json_file`: 更新するJSONファイルへのパスを指定します。
- `path`: 更新する要素へのドット区切りパス。例えば、`components.droneDynamics.mass_kg`のように指定します。
- `new_value`: 要素に設定する新しい値。値の型は自動的に元の値の型に合わせて変換されます。

### 値の型変換

プログラムは、新しい値を次のルールに従って適切な型に変換します：

- 整数（`int`）への変換が必要な場合、新しい値は整数に変換されます。
- 浮動小数点数（`float`）への変換が必要な場合、新しい値は浮動小数点数に変換されます。
- 文字列（`str`）への変換が必要な場合、新しい値は文字列に変換されます。
- 真偽値（`bool`）への変換が必要な場合、`"true"`または`"false"`の文字列が真偽値に変換されます。
- JSON配列やオブジェクトなど、その他のデータ型を更新する場合、適切なJSON形式の文字列を使用します（例: `'[0.015, 0.015, 0.03]'`）。

### エラーハンドリング

- もし指定したパスが存在しない場合、プログラムはエラーメッセージを出力します。
- もし指定したキーが存在しない場合、プログラムはエラーメッセージを出力します。
- 型変換に失敗した場合、プログラムはエラーメッセージを出力します。

### 実行例

JSONファイル内の`components.droneDynamics.mass_kg`の値を`1.5`に更新するには、次のコマンドを実行します：

```
python drone_config.py path_to_file.json components.droneDynamics.mass_kg 1.5
```

### 注意

- このスクリプトは、コマンドライン引数として有効なJSONファイルのパスと、更新する要素の有効なパス、そして有効な新しい値が提供されることを前提としています。
- JSONファイルはプログラムによって上書きされます。元のファイルのバックアップを取っておくことを推奨します。


## Test Scenario

テストシナリオは、ドローンの飛行テストを自動化するためにJSON形式で定義されます。各シナリオは、特定の操作とその操作に必要なパラメータを含むオブジェクトのリストです。

### シナリオの形式

テストシナリオは以下の形式で定義します：

```json
{
    "scenario": [
        {
            "operation": "<操作名>",
            "param1": <値1>,
            "param2": <値2>,
            ...
        },
        ...
    ]
}
```

### オペレーション項目

テストシナリオでは、以下のオペレーション項目を設定できます：

* takeoff: ドローンに離陸命令を出します。
* start_landing: ドローンに特定の位置に向かって移動します。

現時点でサポートされているオペレーションは takeoff と start_landing のみです。

#### takeoff

離陸を指示します。

* alt: 離陸後の目標高度。単位はメートル(m)。
* duration_sec: 離陸操作を続ける時間。単位は秒(sec)。

#### start_landing

移動を指示します。

* lat: 初期位置の緯度からの相対位置。単位はメートル(m)。
* lon: 初期位置の経度からの相対位置。単位はメートル(m)。
* alt: 初期位置の高度からの相対位置。単位はメートル(m)。
* duration_sec: 移動操作を続ける時間。単位は秒(sec)。

### 設定例

* [takeoff](https://github.com/toppers/hakoniwa-px4sim/blob/main/px4/auto-test/test_scenario/takeoff.json)
  * 離陸指示を出し、5m上昇してホバリングを続けます。20秒経過するとこのオペレーションは終了します。
* [takeoff_and_move.json](https://github.com/toppers/hakoniwa-px4sim/blob/main/px4/auto-test/test_scenario/takeoff_and_move.json)
  * 離陸指示を出し、10m上昇してホバリングを続けます。その後、東→北→西→南の順番で10m移動し続けます。各オペレーションは20秒間持続します。 

## Test Scenario Executor

このプログラムは、MAVLinkを介してPX4フライトコントローラと通信し、JSON形式で定義されたテストシナリオに基づいてドローンの飛行テストを自動的に実行します。

### プログラムの概要

- テストシナリオ実行プログラムは、3つのJSONファイルを引数として受け取ります：
  - コンフィグファイル：MAVLink接続設定が含まれます。
  - ドローンコンフィグファイル：ドローンの物理特性やセンサー設定が含まれます。
  - テストシナリオファイル：実行する操作のシーケンスが定義されています。

- このプログラムは、シグナルハンドラを使用して適切なシャットダウンを保証します。

### 使用方法

コマンドラインから以下のようにプログラムを実行します：

```
python hako_QgcStub.py <config_path> <drone_config_path> <test_scenario_path>
```

- `config_path`: MAVLink接続設定を含むJSONファイルへのパス。
- `drone_config_path`: ドローンの設定を含むJSONファイルへのパス。
- `test_scenario_path`: テストシナリオを含むJSONファイルへのパス。

### 主要な機能

- **QgcStub**: QGroundControlのスタブ機能を提供し、定期的にハートビートとピングメッセージをPX4に送信します。
- **Px4Receiver**: PX4からのメッセージをリッスンし、適切なイベントをシナリオマネージャに通知します。

### シナリオマネージャ

- シナリオマネージャは、テストシナリオをロードし、指定された操作を順番に実行します。
- 各操作が完了すると、次の操作が自動的に開始されます。

### マルチスレッド実行

- プログラムはマルチスレッドで実行され、一方のスレッドがPX4からのメッセージをリッスンし、もう一方のスレッドがシナリオを実行します。

### エラーハンドリング

- もしサポートされていない操作がロードされた場合、プログラムはエラーメッセージを出力し、終了します。
- プログラムはシグナル（例えば、Ctrl+C）によって中断された場合、適切にシャットダウンするように設計されています。

### 注意

- このプログラムは、有効なJSONファイルのパスと正しいフォーマットのテストシナリオが提供されることを前提としています。
- テストシナリオの実行中にプログラムを中断する場合は、シグナルを送信してください。

## Logs / LogOutputDirectory

箱庭シミュレーションは、実行中の内部コンポーネントのロギングデータをCSVファイルとして `LogOutputDirectory` に出力します。`logOutputDirectory` は、[機体のパラメータ](https://github.com/toppers/hakoniwa-px4sim/tree/main/hakoniwa#機体のパラメータ説明)で指定された場所に設定されます。

### ログの内容

出力されるログには以下のデータが含まれます。各CSVファイルの第一カラムには `timestamp` があり、この値はマイクロ秒単位(`usec`)で、PX4の時間と同じです。

#### PX4との通信データ

- **hil_sensor (log_hil_sensor.csv)**: HILセンサーデータ。
- **hil_gps (log_comm_hil_gps.csv)**: HIL GPSデータ。
- **hil_actuator (log_comm_hil_actuator_controls.csv)**: HILアクチュエータコントロールデータ。

#### センサ/アクチュエータのログ

- **drone_dynamics (drone_dynamics.csv)**
  - `X`, `Y`, `Z`: ドローンの位置（初期位置からの相対位置、単位：メートル）。
  - `Rx`, `Ry`, `Rz`: ドローンの姿勢角（単位：ラジアン）。
- **rotor (log_rotor_<id>.csv)**
  - `RPM`: ローターの回転数。
- **thruster (log_thrust.csv)**
  - `Thrust`: 推力。
  - `Tx`, `Ty`, `Tz`: トルク。
- **acc (log_acc.csv)**
  - `X`, `Y`, `Z`: 加速度（単位：m/s^2）。
- **gyro (log_gyro.csv)**
  - `X`, `Y`, `Z`: 角速度（単位：rad/s）。
- **mag (log_mag.csv)**
  - `X`, `Y`, `Z`: 磁気センサの値（単位：nT）。
- **baro (log_baro.csv)**
  - `abs_p`, `diff_p`, `p_alt`: 気圧センサデータ（MAVLinkと同じ情報）。
- **gps (log_gps.csv)**
  - `lat`, `lon`, `alt`, `vel`, `vn`, `ve`, `vd`, `cog`: GPSデータ（MAVLinkと同じ情報）。


## Test Result Evaluator

テスト結果の評価は、出力されたログデータをベースに行います。`TestResultEvaluator` スクリプトとしてはさまざまなものが考えられますが、一例として、データの平均値と標準偏差を計算し、それらを予め定義された期待値と比較してスコアを計算する [eval_data_mean_stdev.py](https://github.com/toppers/hakoniwa-px4sim/blob/main/px4/auto-test/evaluation/eval_data_mean_stdev.py) を用意しています。

テスト内容に応じてこれと同等なスクリプトを作成することで、さまざまなテスト評価をすることができます。

### eval_data_mean_stdev.py の機能

- ログデータから特定の時間範囲のデータを抽出し、平均値と標準偏差を計算します。
- これらの計算された値を、事前に定義された期待値と比較し、どの程度一致しているかを評価します。

### 使用方法

以下のようにコマンドラインからスクリプトを実行します：

```
python eval_data_mean_stdev.py <input_json> <data_location_path>
```

- `input_json`: 設定を含むJSONファイルへのパス。
- `data_location_path`: データが保存されているディレクトリへのパス。

`input_json`の設定例：

```json
{
    "file": "drone_dynamics.csv",
    "time_colname": "timestamp",
    "time_unit": "usec",
    "data_colname": "Z",
    "data_range_from_end": true,
    "offset_sec": 0,
    "duration_sec": 5, 
    "expected_mean_value": -5,
    "expected_stdev_value": 0.1
}
```

### 注意

- このスクリプトは一例であり、異なるタイプのデータや評価基準に応じてカスタマイズすることが可能です。
- データの解析と評価は、統計的な手法に基づいています。したがって、データの性質や評価の目的に応じて適切な手法を選択する必要があります。


# テスト自動実行方法

このセクションでは、システムの各コンポーネントを組み合わせてテストを自動化する方法を説明します。以下のスクリプトは、シミュレーションの設定、テストの実行、そして結果の評価を一連のステップとして実行します。

## テストスクリプトの概要

テストを自動化するためのBashスクリプト([auto-test.bash](https://github.com/toppers/hakoniwa-px4sim/blob/main/px4/auto-test/auto-test.bash))は以下の手順でテストを自動実行します：

1. **テスト条件の準備**：
   - ベースとなるドローンの機体パラメータ設定をコピーし、必要に応じて特定のパラメータを変更します。

2. **テストの実行**：
   - [hako-start.bash](https://github.com/toppers/hakoniwa-px4sim/blob/main/hako-start.bash) スクリプトを使用して、指定された設定とテストシナリオでシミュレーションを実行します。

3. **テスト結果の評価**：
   - `eval_data_mean_stdev.py` スクリプトを使用して、テスト結果を評価し、結果をテキストファイルに出力します。

## 使用方法

スクリプトはコマンドラインから以下のように実行します：

```
bash auto-test.bash <test-resource-path> <test_scenario>
```

- `<test-resource-path>`：テストリソースのパス（設定ファイルやログ出力ディレクトリが含まれます）。
- `<test_scenario>`：実行するテストシナリオへのパス。

```
test_resource
├── configs
│   ├── config.json
│   └── drone_config_base.json
├── eval
├── logs
└── work 
```

## テストの例

以下は、空気抵抗係数の異なる複数のテストを実行するサンプルコードです。
このコードは、異なる空気抵抗係数を設定し、それぞれのシナリオでテストを実行し、結果を評価します。

```bash

for p in 0 1 2 3 4
do
    echo "INFO: START TEST PARAM=${p}"
    do_test components.droneDynamics.airFrictionCoefficient 0.${p} result_0${p}
    sleep 5
    echo "INFO: END TEST"
done
```

## 注意

このスクリプトは一例であり、異なるテストパラメータやシナリオに応じてカスタマイズすることが可能です。
スクリプトを実行する前に、必要なすべての依存関係が正しく設定されていることを確認してください。

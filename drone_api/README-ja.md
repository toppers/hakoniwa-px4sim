
[English](README.md) ｜ 日本語

# これは何？

箱庭のプラントモデルを Python プログラムで制御するためのAPIを提供しています。

箱庭API(`hakosim`)は、基本的に、[AirSim](https://microsoft.github.io/AirSim/)のAPIのシグネチャを踏襲しています。

箱庭の Python API 概要は、[こちら](https://github.com/toppers/hakoniwa-px4sim/tree/main/drone_api/libs)を参照ください。

# サンプルプログラム

Python API の利用方法を理解するための[サンプルプログラム](https://github.com/toppers/hakoniwa-px4sim/blob/main/drone_api/sample/sample.py)を用意しています。

サンプルプログラムでは、以下の機能を実行しています。

* ドローンのテイクオフ
* ドローンの移動
* ドローンの位置情報のデバッグ表示
* 荷物の搬送
* ドローン前方のカメラ画像の取得とファイル保存
* 3DLiDARデータの取得とデバッグ表示
* ドローンの着陸

# インストール方法

以下のコマンドを実行してください。

## Windows の場合

```
cd drone_api
mkdir install_dir
<path/to>/install.bat 
```

## Windows 以外の場合

```bash
cd drone_control
bash build.bash
cd ..
```

```
bash drone_api/install.bash
```

また、環境変数PYTHONPATHに以下を追加してください。

```
export PYTHONPATH=/usr/local/lib/hakoniwa/py:${PYTHONPATH}
```

# Unityの設定

Scenes/ApiDemo を利用してください。

シーン選択後、`Generate` してください。

# シミュレーション実行方法

シミュレーション実行方法は以下のとおりです。

```
cd hakoniwa
```

## Windows の場合

```
./drone-app.bat ../../hakoniwa-unity-drone-model ./config/win/api_sample
```

## Windows 以外の場合

```
bash drone-app.bash ../../hakoniwa-unity-drone-model ./config/api_sample
```

# Pythonプログラムの実行方法

シミュレーション実行完了後であれば、任意のタイミングで、Pythonプログラムを実行して、箱庭APIを呼び出すことができます。

```bash
cd drone_api/sample
```

## Windows の場合

```
python sample.py ../../../hakoniwa-unity-drone-model/custom.json
```

## Windows 以外の場合

```
python3.12 sample.py ../../../hakoniwa-unity-drone-model/custom.json
```

# 箱庭ドローンのコンフィグファイル

Unity上で実現された箱庭ドローンにはさまざまなセンサ/アクチュエータがあります。これらの機能はパラメータを持っており、hakoniwa-unity-drone-model/plugin/plugin-srcs 直下にパラメータ定義ファイル(drone_config.json)を配置することで、パラメータ反映させることができます。


設定例：
```json
{
    "drones": {
        "DroneTransporter": {
            "audio_rotor_path": "file:///Users/tmori/Downloads/spo_ge_doron_tobi_r01.mp3",
            "LiDARs": {
                "LiDAR": {
                    "Enabled": false,
                    "NumberOfChannels": 16,
                    "RotationsPerSecond": 10,
                    "PointsPerSecond": 10000,
                    "MaxDistance": 10,
                    "X": 0.45, "Y": 0, "Z": -0.3,
                    "Roll": 0, "Pitch": 0, "Yaw" : 0,
                    "VerticalFOVUpper": 5,
                    "VerticalFOVLower": -5,
                    "HorizontalFOVStart": -5,
                    "HorizontalFOVEnd": 5,
                    "DrawDebugPoints": true
                }
            }
        }
    }
}
```

* audio_rotor_path: ドローンのローター音源のファイルパス。ファイルが存在する場合、ローター音を流すことができます。
* LiDARs: ドローンに搭載されたLiDARの設定
* Enabled: LiDARの有効化または無効化。trueは有効、falseは無効。
* NumberOfChannels: LiDARのチャンネル数
* RotationsPerSecond: LiDARの回転速度(回転数/秒)
* PointsPerSecond: LiDARの秒間の総ポイント数
* MaxDistance: LiDARで計測可能な最大距離（単位：メートル）
* X, Y, Z: LiDARの配置位置（ドローンの位置座標からの相対）（単位：メートル）
* Roll, Pitch, Yaw: LiDARの配置姿勢（ロール、ピッチ、ヨーの角度）
* VerticalFOVUpper: LiDARの上方向のFOV（視野角）
* VerticalFOVLower: LiDARの下方向のFOV（視野角）
* HorizontalFOVStart: LiDARの水平方向のFOVの開始角度
* HorizontalFOVEnd: LiDARの水平方向のFOVの終了角度
* DrawDebugPoints: LiDARのデバッグポイント描画の有無（Unityエディタを利用している場合のみ）

なお、箱庭のLiDARの基本スペックは以下を上限としています。

```
周波数：5Hz
水平：-90° 〜 90°
垂直：-30° 〜 30°
分解能：1°
```

上記で計算されるPDUデータサイズを上限サイズとしていますのでパラメータ定義は以下の制約があります。

* NumberOfChannels: 61
* HorizontalPointsPerRotation = 181
  * PointsPerRotation = PointsPerSecond / RotationsPerSecond
  * HorizontalPointsPerRotation = PointsPerRotation / NumberOfChannels

水平方向と垂直方向の分解能は以下の計算式で求めています。

* 水平方向：HorizontalRanges / HorizontalPointsPerRotation
  * HorizontalRanges = HorizontalFOVEnd - HorizontalFOVStart
* 垂直方向：VerticalRanges / param.NumberOfChannels
  * VerticalRanges = VerticalFOVUpper - VerticalFOVLower

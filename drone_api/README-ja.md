
[English](README.md) ｜ 日本語

# これは何？

箱庭のプラントモデルを Python プログラムで制御するためのAPIを提供しています。

箱庭API(`hakosim`)は、基本的に、AirSimのAPIのシグネチャを踏襲しています。


# プログラム例

初期化処理を実行し、以下の操作を実行しています。

* ３m離陸します。
  * client.takeoff(3)
* (0, 3, 3)の位置へ、5m/secで移動します。
  * client.moveToPosition(0, 3, 3, 5)
* 着陸します。
  * client.land()

```
# connect to the HakoSim simulator
client = hakosim.MultirotorClient(<custom.json>)
client.confirmConnection()
client.enableApiControl(True)
client.armDisarm(True)

client.takeoff(3)
client.moveToPosition(0, 3, 3, 5)
client.land()
```

# インストール方法

以下のコマンドを実行してください。

```
bash drone_api/install.bash
```

# Unityの設定

Scenes/ApiDemo を利用してください。

シーン選択後、`Generate` してください。

# シミュレーション実行方法

シミュレーション実行方法は以下のとおりです。

```
cd hakoniwa
```

```
bash drone-app.bash ../../hakoniwa-unity-drone-model ./config/api_sample
```

# Pythonプログラムの実行方法

シミュレーション実行完了後であれば、任意のタイミングで、Pythonプログラムを実行して、箱庭APIを呼び出すことができます。

```bash
cd drone_api/sample
```

```
python3.12 drone_cmd.py ../../../hakoniwa-unity-drone-model/custom.json
```
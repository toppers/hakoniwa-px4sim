[English](README.md) ｜ 日本語

# 箱庭ドローン制御

## これは何？

ここでは、箱庭の[ドローン物理モデル](https://github.com/toppers/hakoniwa-px4sim/blob/main/drone_physics/README-ja.md)をPID制御する演習プログラムを作成します。


## 課題

ドローンを 10m 浮上させてホバリングしてください。

評価ポイントは以下のとおりです。

* 50点：100 秒後にホバリング状態になっていること
* 30点: ホバリングするまでの時間が 10秒以内
* 20点: オーバーシュートが 1m以内

## 演習内容
### 前提とする情報

* 質量：0.1 [ $kg$ ]
* 空気摩擦定数：0.0001 [ $m/s\cdot kg$ ]
* シミュレーション時間ステップ：3msec

### プログラミング言語

C/C++言語で作成します。

### ファイル構成

`hako_module_drone_controller_impl.cpp` を実装します。

```
drone_control
├── CMakeLists.txt
├── README.md
├── cmake-build
├── include
│   └── hako_module_drone_controller.h
└── workspace
    ├── DroneAvator
    │   ├── CMakeLists.txt
    │   └── hako_module_drone_controller_impl.cpp
    ├── hako_module_drone_controller.c
    └── hako_module_drone_controller_impl.h
```

### プログラム構成

以下の２関数を実装してください。

* hako_module_drone_controller_impl_init
  * プログラム起動時に１回だけ呼び出されます。制御に必要な初期化処理を実装します。
  * 成功した場合は 0 を返してください。失敗した場合は 0 以外の値を返します。
* hako_module_drone_controller_impl_run
  * 制御タイミング毎に呼び出されます。ホバリング制御に必要な処理を実装します。
  * 入力引数 in に、ドローンの状態（位置と姿勢角）が与えられます。
  * 戻り値には、ドローンの制御指示値(推力とトルク)を返します。

**実装対象関数:**
`TODO` 部分を書き換えてください。
```
int hako_module_drone_controller_impl_init(void* context)
{
    //TODO
}

mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    //TODO
}
```

### 入出力データ情報

#### mi_drone_control_in_t

現在の機体の位置と姿勢情報です。

**データ型:**
```
typedef struct {
    double pos_x;
    double pos_y;
    double pos_z;
    double euler_x;
    double euler_y;
    double euler_z;
} mi_drone_control_in_t;
```
* 座標系：NED
* pos_x, pos_y, pos_z
  * 単位：メートル
  * pos_x: North
  * pos_y: East
  * pos_z: Down
  * 初期値：全て 0 
* euler_x, euler_y, euler_z
  * 単位：ラジアン
  * euler_x: x軸方向の角度（ロール）
  * euler_y: y軸方向の角度（ピッチ）
  * euler_z: z軸方向の角度（ヨー）
  * 初期値：全て 0 

#### mi_drone_control_out_t

機体への制御出力情報です。

**データ型:**
```
typedef struct {
    double thrust;
    double torque_x;
    double torque_y;
    double torque_z;
} mi_drone_control_out_t;
```
* thrust
  * 単位：Newton (N)
  * 機体の垂直方向の推力
* torque_x, torque_y, torque_z
  * 単位：Newton-meter (Nm)
  * torque_x: ロール方向のトルク
  * torque_y: ピッチ方向のトルク
  * torque_z: ヨー方向のトルク


# 環境セットアップ


作成したプログラムは、[drone_control/workspace](https://github.com/toppers/hakoniwa-px4sim/tree/main/drone_control/workspace) 配下に固有のディレクトリ名を設定して配置してください。

配置例：
```
Gemini
├── CMakeLists.txt
└── hako_module_drone_controller_impl.cpp
```

`CMakeLists.txt` の `project` をディレクトリ名と一致していることを確認してください。

例：ディレクトリ名が`Gemini`なので、project名は`Gemini`
```
# ローダブルモジュールのビルド設定
project(Gemini)
```

# ビルド方法

## Windows の場合

[hakoniwa](https://github.com/toppers/hakoniwa-px4sim/tree/main/hakoniwa)のビルドで自動的にビルドされます。

## Windows 以外の場合(WSL2/Mac)

`cmake-build` に移動して、以下のコマンドを実行してください。

```
cmake ..
make
```

成功すると、ローダブルモジュールが作成されます。

例：lib<ディレクトリ名>.so が作成されます
```
cmake-build/workspace/Gemini/libGemini.so 
```

# シミュレーション実行するための準備

作成したローダブルモジュールをシミュレーション上で実行するには以下の対応が必要となります。

1. hakoniwa-px4sim の [hakoniwa](https://github.com/toppers/hakoniwa-px4sim/blob/main/hakoniwa/README-ja.md) のインストール
2. [hakoniwa-unity-drone-model](https://github.com/toppers/hakoniwa-unity-drone-model) のインストール
3. [箱庭側の設定](#箱庭側の設定)
4. [Unity側の設定](#Unity側の設定)

## 箱庭側の設定

### １個の機体をシミュレーションする場合

デフォルトで用意している config ファイル(hakoniwa/config/drone_config_0.json)を使います。


### 複数機体を同時にシミュレーションする場合

箱庭ドローンシミュレータの機体パラメータを実行させたいローダーブルモジュール毎に用意する必要があります。

例：
```
hakoniwa/config/sample_control
├── drone_config_0.json
├── drone_config_1.json
└── drone_config_2.json
```

機体パラメータファイルの書式は以下としてください。

* `drone_config_<index>.json`
* `<index>` は、0からの連番としてください。

各機体のパラメータとしては、以下の項目のみを設定してください。

* name
  * ローダブルモジュールのディレクトリ名を指定してください。例：Gemini
* components.droneDynamics.position_meter
  * 機体の初期位置を設定してください。他の機体と重複しない場所を指定ください。
* controller.moduleDirectory
  * `hakoniwa`ディレクトリからの相対パスで、ローダブルモジュールが配置されているディレクトリ名を指定してください。
  * 例：../drone_control/cmake-build/workspace/Gemini

## Unity側の設定

### １個の機体をシミュレーションする場合

Scenes/Hakoniwaを利用してください。

シーン選択後、`Generate` してください。

### 複数機体を同時にシミュレーションする場合

Scenes/MultiDronesを利用してください。

機体をローダブルモジュール数分だけ配置してください。

機体の名前は、ローダブルモジュールのディレクトリ名と一致させてください。

例：

![スクリーンショット 2024-02-19 14 45 27](https://github.com/toppers/hakoniwa-px4sim/assets/164193/f556e5f7-327c-47dd-80a6-c41a0382f619)


配置完了後、`Generate` してください。

# シミュレーション実行方法

`hakoniwa`ディレクトリ直下で、以下のコマンドを実行してください。

Windowsの場合：
```
drone-control.bat <hakoniwa-unity-drone-model location> <config directory>
```

Windows以外の場合：
```
bash drone-control.bash <hakoniwa-unity-drone-model location> <config directory>
```

`<config directory>` には、機体パラメータファイルを配置したディレクトリパスを指定してください。


## 例（Windowsの場合）
例：１個の機体をシミュレーションする場合
```
./drone-control.bat ../../hakoniwa-unity-drone-model config/win
```

例：複数機体をシミュレーションする場合
```
./drone-control.bat ../../hakoniwa-unity-drone-model config/win/sample_control
```

## 例（Windows以外の場合）
例：１個の機体をシミュレーションする場合
```
bash drone-control.bash ../../hakoniwa-unity-drone-model config
```

例：複数機体をシミュレーションする場合
```
bash drone-control.bash ../../hakoniwa-unity-drone-model config/sample_control
```

成功すると、以下のログが出力されますので、その後に、Unity側のシミュレーションを開始してください。

```
HAKO_CAPTURE_SAVE_FILEPATH : ./capture.bin
HAKO_BYPASS_IPADDR : 127.0.0.1
HAKO_CUSTOM_JSON_PATH : ../../../release/hakoniwa-unity-drone-model/custom.json
DRONE_CONFIG_PATH : config/sample_control
HAKO_BYPASS_PORTNO : 54001
INFO: LOADED drone config file: config/sample_control/drone_config_0.json
INFO: LOADED drone config file: config/sample_control/drone_config_1.json
INFO: LOADED drone config file: config/sample_control/drone_config_2.json
INFO: hako_master_init() success
INFO: setup start
DroneDynamicType: BodyFrame
INFO: logpath: ./drone_log0/drone_dynamics.csv
Rotor vendor: None
Thruster vendor: None
param_A: 1.53281e-08
param_B: 1.3e-10
param_Jr: 1e-10
DroneDynamicType: BodyFrame
INFO: logpath: ./drone_log1/drone_dynamics.csv
Rotor vendor: None
Thruster vendor: None
param_A: 1.53281e-08
param_B: 1.3e-10
param_Jr: 1e-10
DroneDynamicType: BodyFrame
INFO: logpath: ./drone_log2/drone_dynamics.csv
Rotor vendor: None
Thruster vendor: None
param_A: 1.53281e-08
param_B: 1.3e-10
param_Jr: 1e-10
INFO: loading drone & controller: 0
SUCCESS: Loaded module name: ChatGPT3_5
INFO: loading drone & controller: 1
SUCCESS: Loaded module name: ChatGPT4
INFO: loading drone & controller: 2
SUCCESS: Loaded module name: Gemini
Robot: Gemini, PduWriter: Gemini_drone_motor
channel_id: 0 pdu_size: 88
INFO: Gemini create_lchannel: logical_id=0 real_id=0 size=88
Robot: Gemini, PduWriter: Gemini_drone_pos
channel_id: 1 pdu_size: 48
INFO: Gemini create_lchannel: logical_id=1 real_id=1 size=48
Robot: ChatGPT4, PduWriter: ChatGPT4_drone_motor
channel_id: 0 pdu_size: 88
INFO: ChatGPT4 create_lchannel: logical_id=0 real_id=2 size=88
Robot: ChatGPT4, PduWriter: ChatGPT4_drone_pos
channel_id: 1 pdu_size: 48
INFO: ChatGPT4 create_lchannel: logical_id=1 real_id=3 size=48
Robot: ChatGPT3_5, PduWriter: ChatGPT3_5_drone_motor
channel_id: 0 pdu_size: 88
INFO: ChatGPT3_5 create_lchannel: logical_id=0 real_id=4 size=88
Robot: ChatGPT3_5, PduWriter: ChatGPT3_5_drone_pos
channel_id: 1 pdu_size: 48
INFO: ChatGPT3_5 create_lchannel: logical_id=1 real_id=5 size=48
WAIT START
```

# P制御とPD制御の制御戦略

ドローンの垂直運動の物理方程式から、P制御とPD制御戦略の例を示します。

## プラント側の物理方程式

z軸はNED座標系。

**対象の運動方程式：**

$\ddot{z} = -\frac{u(t)}{m} + g - \frac{d}{m} \dot{z}$

**ラプラス変換:**

$s^{2}Z(s) = -\frac{U(s)}{m} + \frac{g}{s} - \frac{d}{m} s Z(s)$

## 制御側の式

Rは正の値で指定する。

### P制御

**制御式:**

$u(t) = K_p ( z(t) + R  ) + m g$

**ラプラス変換:**

$U(s) = K_p ( Z(s) + \frac{R}{s} ) + \frac{m g}{s}$

#### プラント側の U(s) に P 制御の式をラプラス関数で代入

$s^{2}Z(s) = -\frac{ K_p ( Z(s) + \frac{R}{s}) + \frac{mg}{s}  }{m} + \frac{g}{s} - \frac{d}{m} s Z(s)$

左辺に微分項、右辺を積分項にしてみる。

$s^{2}Z(s) + \frac{d}{m} s Z(s) + \frac{K_p}{m} Z(s) = ( -\frac{K_p R}{m}) \frac{1}{s}$

ここからZ(s)を求めるとこうなる。

$Z(s) = \frac{( -\frac{K_p R}{m})}{s(s^{2} + \frac{d}{m} s + \frac{K_p}{m})}$

ここで、

$\omega_n^{2} = \frac{K_p}{m}$

$\zeta = \frac{d}{2\sqrt{mK_p}}$

とおくと、こうなる。

$Z(s) = - R \frac{1}{s} \frac{\omega_n^{2}}{s^{2} + 2 \zeta \omega_n s + \omega_n^{2}}$

### PD制御

**制御式:**

$u(t) = K_p ( z(t) + R  ) + m g + K_d ( \dot{z(t) + R} ) $

Rは定数なので、こうなる。

$u(t) = K_p ( z(t) + R  ) + m g + K_d  \dot{z(t)}  $

**ラプラス変換:**

$U(s) = K_p ( Z(s) + \frac{R}{s} ) + \frac{m g}{s} + K_d s Z(s) $

#### プラント側のU(s) にPD制御の式をラプラス関数で代入

$s^{2}Z(s) = -\frac{ K_p ( Z(s) + \frac{R}{s}) + \frac{mg}{s} + K_d s Z(s) }{m} + \frac{g}{s} - \frac{d}{m} s Z(s)$

左辺に微分項、右辺を積分項にしてみる。

$s^{2}Z(s) + \frac{d}{m} s Z(s) +  K_d s Z(s) + \frac{K_p}{m} Z(s) = ( -\frac{K_p R}{m}) \frac{1}{s}$

ここからZ(s)を求めるとこうなる。

$Z(s) = \frac{( -\frac{K_p R}{m})}{s(s^{2} + (\frac{d}{m} + K_d ) s + \frac{K_p}{m})}$

ここで、

$\omega_n^{2} = \frac{K_p}{m}$

$\zeta = \frac{d + m K_d}{2\sqrt{mK_p}}$

とおくと、こうなる。

$Z(s) = - R \frac{1}{s} \frac{\omega_n^{2}}{s^{2} + 2 \zeta \omega_n s + \omega_n^{2}}$

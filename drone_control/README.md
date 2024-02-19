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

* 質量：0.1kg
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

以下の２関数を実施してください。

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

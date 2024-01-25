# PID 制御

箱庭内のドローンは、PX4の制御プログラムを使わずに PID 制御できます。

制御プログラムは、以下のコードです。

https://github.com/toppers/hakoniwa-px4sim/blob/main/hakoniwa/src/assets/drone/controller/drone_pid_control.cpp

本コードでは、機体の高さとφの角度をPID制御しています。

## インストール手順

まず、箱庭コマンドをインストールするため、以下のインストールを実施してください。

[箱庭コマンドおよびライブラリのインストール手順](https://github.com/toppers/hakoniwa-px4sim/tree/main/hakoniwa#%E7%AE%B1%E5%BA%AD%E3%82%B3%E3%83%9E%E3%83%B3%E3%83%89%E3%81%8A%E3%82%88%E3%81%B3%E3%83%A9%E3%82%A4%E3%83%96%E3%83%A9%E3%83%AA%E3%81%AE%E3%82%A4%E3%83%B3%E3%82%B9%E3%83%88%E3%83%BC%E3%83%AB%E6%89%8B%E9%A0%86)


## 実行手順

**ディレクトリ移動:**

```
cd hakoniwa
```

**プログラム実行:**

以下のスクリプトを実行すると、３秒間のプログラムを実行し、シミュレーション時間で50秒間の結果がグラフ表示されます。

```
bash do_pid_test.bash 3 50
```

<img width="936" alt="スクリーンショット 2024-01-25 10 40 54" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/aa0192b9-8ef4-4816-98f8-f4a04e31c9d8">


本スクリプトの仕様は以下のとおりです。

```
Usage: do_pid_test.bash <run-time-sec> <duration>
```

* <run-time-sec>: シミュレーション実行時間（実時間：単位は秒です）
* <duration>：グラフで表示するシミュレーション時間（単位は秒です）

## PID制御用のパラメータ

PID制御パラメータは、機体のパラメータ(hakoniwa/config/drone_config_pid.json)で変更できます。

- **controller**: 制御プログラム用のコンテナ
  - **pid**: PID制御用のパラメータ定義コンテナ
    - **position**: 位置制御用のパラメータ定義コンテナ
      - **x**: 位置xのパラメータ定義コンテナ
      - **y**: 位置yのパラメータ定義コンテナ
      - **z**: 位置zのパラメータ定義コンテナ
    - **angle**: 姿勢制御用のパラメータ定義コンテナ
      - **phi**: 位置phiのパラメータ定義コンテナ
      - **theta**: 位置thetaのパラメータ定義コンテナ
      - **psi**: 位置psiのパラメータ定義コンテナ

`position` と `angle` コンテナ内部のパラメータはPID制御パラメータを指定します（全て同じパラメータ名です）。

- **setpoint**: 目標値
  - positionの場合は、単位はメートルです。
  - angleの場合は、単位はdegreeです。
- **Kp**: Pゲイン
- **Ki**: Iゲイン
- **Kd**: Dゲイン




[English](README.md) ｜ 日本語

[![Build](https://github.com/toppers/hakoniwa-px4sim/actions/workflows/build.yml/badge.svg)](https://github.com/toppers/hakoniwa-px4sim/actions/workflows/build.yml)

このリポジトリは、PX4と連携可能なドローンのプラントモデル用シミュレーション環境です。このシミュレーション環境は、ドローンの動作を物理式ベースで正確にモデル化し、C言語で実装しています。

# 目次

- [特徴](#特徴)
- [環境](#環境)
- [事前準備](#事前準備)
- [hakoniwa-px4sim のインストール手順](#hakoniwa-px4sim-のインストール手順)
- [箱庭のインストール手順](#箱庭のインストール手順)
- [シミュレーション実行手順](#シミュレーション実行手順)
- [MATLAB連携](#MATLAB連携)
- [環境からの作用](#環境からの作用)
- [ヘッドレス・シミュレーション](#ヘッドレスシミュレーション)
- [ログリプレイ](#ログリプレイ)
- [コミュニティとサポート](#コミュニティとサポート)
- [本リポジトリの内容とライセンスについて](#本リポジトリの内容とライセンスについて)
- [貢献ガイドライン](#貢献ガイドライン)

# 特徴

1. **物理式ベースのプラントモデル:** ドローンの動作は、物理学に基づいた高精度なモデルで表現されています。これはC言語で開発されており、PX4とシームレスに連携します。詳細は[こちら](https://github.com/toppers/hakoniwa-px4sim/tree/main/drone_physics)を参照ください。障害物との衝突時の物理的反応をリアルタイムにシミュレートできます(`v1.1.0`)。詳細は[こちら](#障害物との衝突)。

2. **ゲームエンジンによるビジュアライズ:** ドローンのビジュアル表現は、ゲームエンジンを使用して実現しています。このビジュアライズは、物理シミュレーションの補助として機能し、主に可視化を目的としています。障害物との衝突時のビジュアル表現も可能です(`v1.1.0`)。詳細は[こちら](#障害物との衝突)

3. **エンジンの柔軟性:** 現時点ではUnityエンジンを[サポート](https://github.com/toppers/hakoniwa-unity-drone-model)していますが、アーキテクチャは他のゲームエンジンとの連携も可能に設計されています。Unreal Engine との連携可能なプラグインは[こちら](https://github.com/toppers/hakoniwa-unreal-simasset-plugin/tree/main)(`v1.1.0`)。

4. **MATLAB/Simulinkとの互換性:** 物理式モデルは、MATLAB/Simulinkで作成したモデルとも連携できるようになっています。詳細は[こちら](#MATLAB連携)

5. **センサモデルの整備:** センサモデルはアーキテクチャ内で整理され、明確な仕様に基づいています。これにより、ユーザーはセンサモデルを仕様に合わせて交換することが可能です。各センサ仕様は[こちら](https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs)を参照ください。ベンダ毎のセンサの作成方法については、[こちら](https://github.com/toppers/hakoniwa-px4sim/blob/main/drone_sensors/README-ja.md)を参照ください。

6. **ヘッドレス対応:** Unityなしでのシミュレーションが可能です。これにより、グラフィカルなインターフェースを必要としない環境でも、シミュレーションの実行が可能になります。詳細は[こちら](#ヘッドレスシミュレーション)。

7. **自動テストのサポート:** テストシナリオベースでの自動テストが可能です。これにより、繰り返しのテストや連続したテストの自動化が実現可能になり、開発プロセスの効率化が図れます。詳細は[こちら](https://github.com/toppers/hakoniwa-px4sim/tree/main/px4/auto-test)を参照ください。

8. **機体特性の外部パラメータ化:** ドローンの機体特性は外部からの[パラメータ化](https://github.com/toppers/hakoniwa-px4sim/tree/main/hakoniwa#機体のパラメータ説明)が可能です。これにより、さまざまな機体の特性に合わせたシミュレーションが実現でき、より幅広いテストシナリオへの対応が可能になります。パラメータ設定例は[こちら](https://github.com/toppers/hakoniwa-px4sim/blob/main/hakoniwa/config/drone_config.json)。

9. **複数機体対応:** ドローンの機体特性パラメータを複数用意することで、機体を複数同時にシミュレーションすることができます。詳細は[こちら](#複数機体対応)

10. **Python API対応:** PX4やQGCを利用せずに、Python API から機体操作することができます。詳細は[こちら](https://github.com/toppers/hakoniwa-px4sim/blob/main/drone_api/README-ja.md)。また、PS4コントローラでドローンを操作することができます。詳細は[こちら](https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/drone_control/ps4)。Python APIで駆動する制御プログラムの説明は[こちら](https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/drone_control)を参照ください。

11. **ログリプレイ:** ドローンシミュレーション実行中に記録されているログデータから、ドローンの飛行軌跡をリプレイすることができます。詳細は[こちら](#ログリプレイ)。

![スクリーンショット 2024-01-30 10 22 34](https://github.com/toppers/hakoniwa-px4sim/assets/164193/be993a09-ac40-4328-9602-6a593cd105b1)



# 環境

* サポートOS
  * Arm系Mac (M1Mac, M2Mac)
  * Windows 10/11
  * Windows WSL2
  * Ubuntu 22.0.4
* 利用する環境
  * Python 3.12
  * jq
  * cmake
  * googletest
  * Arm系Macの場合
      * Pythonはpyenvでインストールされたものを推奨
      * Jinja2 (`pip install -U jinja2`)
  * WSLの場合
    * [WSLの場合の箱庭構成例](https://github.com/toppers/hakoniwa-document/blob/main/architecture/examples/README-win.md)と同じです。
* チェックスクリプト:
  * 必要な全ての環境が正しくインストールされているかを確認するには、以下のコマンドを使用します:
    ```
    bash hako-setup-check.bash
    ```
* 利用するドローン
  * https://github.com/toppers/hakoniwa-unity-drone-model/tree/main
  * 下記のディレクトリ構成のように、本リポジトリと同じ階層でクローンしてください。
    ```
    hakoniwa-unity-drone-model/
    hakoniwa-px4sim/
    ```

# 事前準備

２つのリポジトリをクローンします。

```
git clone --recursive https://github.com/toppers/hakoniwa-px4sim.git
```

```
git clone --recursive https://github.com/toppers/hakoniwa-unity-drone-model.git
```

hakoniwa-unity-drone-model のインストール手順は、以下を参照ください。

https://github.com/toppers/hakoniwa-unity-drone-model


# hakoniwa-px4sim のインストール手順

以下の手順に従って、PX4 のインストールを実施します。

https://github.com/toppers/hakoniwa-px4sim/tree/main/px4


# 箱庭のインストール手順

以下の手順に従って、箱庭のインストールを実施します。

https://github.com/toppers/hakoniwa-px4sim/tree/main/hakoniwa

# シミュレーション実行手順

端末を２つ用意してください。

* 端末A：PX4のシミュレータ実行用
* 端末B：箱庭実行用

## 端末A

```
cd hakoniwa-px4sim/px4/PX4-Autopilot
```

PX4 on SITL をを起動します。

Windows の場合:
```
bash ../sim/win-simstart.bash
```

Windows 以外の場合:
```
bash ../sim/simstart.bash
```

成功すると、以下のように、TCPポートのコネクション待ちになります。


```
% bash ../sim/simstart.bash
[0/1] launching px4 none_iris (SYS_AUTOSTART=10016)

______  __   __    ___ 
| ___ \ \ \ / /   /   |
| |_/ /  \ V /   / /| |
|  __/   /   \  / /_| |
| |     / /^\ \ \___  |
\_|     \/   \/     |_/

px4 starting.

INFO  [px4] startup script: /bin/sh etc/init.d-posix/rcS 0
env SYS_AUTOSTART: 10016
INFO  [param] selected parameter default file parameters.bson
INFO  [param] importing from 'parameters.bson'
INFO  [parameters] BSON document size 568 bytes, decoded 568 bytes (INT32:14, FLOAT:13)
INFO  [param] selected parameter backup file parameters_backup.bson
INFO  [dataman] data manager file './dataman' size is 7868392 bytes
INFO  [init] PX4_SIM_HOSTNAME: localhost
INFO  [simulator_mavlink] Waiting for simulator to accept connection on TCP port 4560
```



ここから先は、端末Bです。

## 端末B

```
cd hakoniwa-px4sim/hakoniwa
```

WSLの場合は、以下のコマンドで docker コンテナに入ってください。

```
bash docker/run.bash <path/to/hakoniwa-unity-drone-model>
```

箱庭を起動するためのスクリプトを実行します。


```
bash run.bash 
```


成功すると、こうなります。

```
% bash run.bash 
HAKO_CAPTURE_SAVE_FILEPATH : ./capture.bin
HAKO_BYPASS_IPADDR : 127.0.0.1
HAKO_CUSTOM_JSON_PATH : ../config/custom.json
DRONE_CONFIG_PATH : ../config/drone_config.json
HAKO_BYPASS_PORTNO : 54001
INFO: shmget() key=255 size=1129352 
INFO: hako_master_init() success
Robot: DroneAvator, PduWriter: DroneAvator_drone_motor
channel_id: 0 pdu_size: 88
INFO: DroneAvator create_lchannel: logical_id=0 real_id=0 size=88
Robot: DroneAvator, PduWriter: DroneAvator_drone_pos
channel_id: 1 pdu_size: 48
INFO: DroneAvator create_lchannel: logical_id=1 real_id=1 size=48
WAIT START
INFO: px4 reciver start
INFO: COMMAND_LONG ack sended
```

この際、端末A側で、以下のように poll timeout のメッセージが出ますが、特に問題ないです。

```
ERROR [simulator_mavlink] poll timeout 0, 111
ERROR [simulator_mavlink] poll timeout 0, 111
ERROR [simulator_mavlink] poll timeout 0, 111
```

## Unity

この状態で、Unityのシミュレーションを開始してください。

![スクリーンショット 2024-01-26 9 34 16](https://github.com/toppers/hakoniwa-px4sim/assets/164193/1f6c417d-cb58-4c21-9dd0-7b59964eeadf)


そして、`START` ボタンを押下すると、シミュレーションが動き出します。


## 端末A

この時、端末Aでは以下のように、準備状態になります。

```
INFO  [lockstep_scheduler] setting initial absolute time to 1699681315573127 us
INFO  [commander] LED: open /dev/led0 failed (22)
WARN  [health_and_arming_checks] Preflight Fail: ekf2 missing data
INFO  [mavlink] mode: Normal, data rate: 4000000 B/s on udp port 18570 remote port 14550
INFO  [mavlink] mode: Onboard, data rate: 4000000 B/s on udp port 14580 remote port 14540
INFO  [mavlink] mode: Onboard, data rate: 4000 B/s on udp port 14280 remote port 14030
INFO  [mavlink] mode: Gimbal, data rate: 400000 B/s on udp port 13030 remote port 13280
INFO  [logger] logger started (mode=all)
INFO  [logger] Start file log (type: full)
INFO  [logger] [logger] ./log/2023-11-11/05_41_55.ulg	
INFO  [logger] Opened full log file: ./log/2023-11-11/05_41_55.ulg
INFO  [mavlink] MAVLink only on localhost (set param MAV_{i}_BROADCAST = 1 to enable network)
INFO  [mavlink] MAVLink only on localhost (set param MAV_{i}_BROADCAST = 1 to enable network)
INFO  [px4] Startup script returned successfully
pxh> INFO  [tone_alarm] home set
INFO  [tone_alarm] notify negative
INFO  [commander] Ready for takeoff!
```

ここで、端末Aで以下のコマンドを実行してください。

```
commander takeoff
```

成功すると、Unity上のドローンがホバリングしてくれます。

![スクリーンショット 2024-01-26 9 36 02](https://github.com/toppers/hakoniwa-px4sim/assets/164193/eba3b933-6789-4a2e-b742-de27a1ae1bce)


## シミュレーション停止方法

シミュレーションを停止するには、以下の順番で停止してください。

1. Unityのシミュレーションを止める
2. PX4のシミュレーションを　CTRL＋Cで止める
3. 箱庭のシミュレーションを CTRL+C で止める


## QGroundControlとの連携方法

[QGroundControl](http://qgroundcontrol.com/)をインストールすることで、QGC側から機体を操作できます。

QGCを起動した後に、PX4との接続設定が必要となります。

画面右上のロゴをクリックすると、下図のように「アプリケーション設定」ができますので、クリックします。

![image](https://github.com/toppers/hakoniwa-px4sim/assets/164193/b17cd3a1-23c3-4b0b-b46c-d4c98d375102)

次に、「通信リンク」をクリックし、「追加」ボタンを押下します。

![image](https://github.com/toppers/hakoniwa-px4sim/assets/164193/60389069-46a5-4801-aba5-af06f7582a53)

必要な設定をします。

![image](https://github.com/toppers/hakoniwa-px4sim/assets/164193/6d916332-7be6-4f33-855d-cc657919076b)

以下を設定しましょう。

* 名前：`hakoniwa` (お好きな名前を指定できます)
* タイプ：`UDP`
* ポート：`18570`
* サーバーアドレス：OSによって設定が違います
  * WSLの場合：WSL2上で、eth0 のIPアドレスを調べて設定してください。
  * WSL以外の場合：お使いのethernetのIPアドレスを調べて設定してください。

IPアドレスの調べ方（例）

```
$ ifconfig eth0
eth0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 172.29.246.4  netmask 255.255.240.0  broadcast 172.29.255.255
        inet6 fe80::215:5dff:feae:5d59  prefixlen 64  scopeid 0x20<link>
        ether 00:15:5d:ae:5d:59  txqueuelen 1000  (Ethernet)
        RX packets 2104410  bytes 2461696811 (2.4 GB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 1152573  bytes 1569239960 (1.5 GB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```

設定後、「サーバー追加」ボタンをおして、「OK」ボタンをクリックします。

![image](https://github.com/toppers/hakoniwa-px4sim/assets/164193/1da37afa-886d-4122-b115-b7f12808fb75)

最後に、「接続」ボタンをクリックすれば設定完了です。

![image](https://github.com/toppers/hakoniwa-px4sim/assets/164193/cb40933f-329c-4f86-bac8-782be71f7de2)

以下は、QGCの操作例です。

https://github.com/toppers/hakoniwa-px4sim/assets/164193/88934527-58dd-46f7-abc1-0e2d49c44935

1. 離陸
2. 東へ移動
3. 北へ移動

# MATLAB連携

バージョンv2.0.0以降、MATLAB/Simulinkで作成したモデルをコード生成し、箱庭（Hakoniwa）上でシミュレーションする機能がサポートされています。

現在サポートされている機能は以下の通りです。

- [X] ドローン物理モデル
- [ ] センサモデル
- [ ] アクチュエータモデル 

## 開発フローの概要

MATLAB/Simulinkから箱庭シミュレーション環境へのモデル移行プロセスは下図のとおりです。

現時点では、決められた箱庭のCインタフェースに対するドローン物理モデルを構築していくフローになっています。
今後のアップデートでは、インタフェース定義およびセンサ/アクチュエータ含めた開発フローを整備していく予定です。

![スクリーンショット 2024-01-31 13 49 04](https://github.com/toppers/hakoniwa-px4sim/assets/164193/be7e404f-c6b4-4b2a-8545-aec4239f7043)

* MATLAB/Simulink Hakoniwa Templates
  * MATLAB/Simulink のモデル向けに箱庭側でインタフェースを定義しています。詳細は[こちら](https://github.com/toppers/hakoniwa-px4sim/blob/main/matlab-if/README.md)。
  * MATLAB/Simulink のモデルを箱庭と接続するために、箱庭接続用のテンプレートを用意しています。詳細は[こちら](https://github.com/toppers/hakoniwa-px4sim/tree/main/matlab-if/model_template)。
* MATLAB/Simulink Process
  * テンプレートに箱庭の入出力インタフェースが定義されていますので、入力データから出力データに変換する処理をモデルとして作成します。
  * 作成したモデルをMATLAB上でシミュレーションし、チェックします。
  * MATLABシミュレーションチェック完了後、Cコード生成します。
* Hakoniwa Simulatin Process
  * 生成したCコードを箱庭(hakoniwa-px4sim)の `matlab-if` 直下に配置します。
  * `matlab-if` 直下の `CMakeLists.txt` を編集し、ビルド対象に含めます(`HAKONIWA_MATLAB_BUILD`ブロック内)。
  * 箱庭のビルドプロセスに従ってコンパイルします。
  * コンパイル成功したら、箱庭のシミュレーション実行プロセスに従って、シミュレーションを行います。


# 環境からの作用

v1.1.0以降のバージョンでは、シミュレーション環境において、ドローンは外部環からの作用が可能です。

例えば、ゲームエンジンを利用してドローンが障害物に衝突した際の物理的な影響をリアルタイムにシミュレートすることができます。

現時点のサポート状況は以下の通りです。

 - [X] 障害物との衝突
 - [ ] 風の影響
 - [ ] 直射日光による影響

## 障害物との衝突

ゲームエンジン上に障害物を配置することで、機体が障害物と干渉した情報を物理モデル側にフィードバックできます。

本機能を利用する場合は、機体パラメータ`droneDynamics` の`collision_detection` を `true` にします。

設定例：

```json
      "droneDynamics": {
          "physicsEquation": "BodyFrame",
          "collision_detection": true,
          "manual_control": false,
          "airFrictionCoefficient": [ 0.0001, 0.0 ],
          "inertia": [ 0.0000625, 0.00003125, 0.00009375 ],
          "mass_kg": 0.1,
          "body_size": [ 0.1, 0.1, 0.01 ],
          "position_meter": [ 0, 0, 0 ],
          "angle_degree": [ 0, 0, 0 ]
        },
```

実行例：



https://github.com/toppers/hakoniwa-px4sim/assets/164193/c1305966-d782-42f4-bd5b-13c57b1ff726




# ヘッドレス・シミュレーション

箱庭内のドローンは、UnityおよびPX4を利用せずに、PID 制御を試すことができます（ヘッドレス・シミュレーション）。

作成したドローンのダイナミクスを動作チェックしたい場合に便利です。

ヘッドレス・シミュレーションで作成するプログラムとしては以下の２方式があります。

1. [箱庭ビルドプロセスでビルドして組み込む方式](https://github.com/toppers/hakoniwa-px4sim/tree/main/hakoniwa/src/assets/drone/controller)
2. [ローダブルモジュールとして組み込む方式](https://github.com/toppers/hakoniwa-px4sim/tree/main/drone_control)

# 複数機体対応

ドローンの機体特性パラメータを複数用意することで、機体を複数同時にシミュレーションすることができます。

## 事前準備

複数機体を同時シミュレーションするには、以下の事前準備が必要です。

* [機体特性パラメータの設定](#機体特性パラメータの設定)
* [Unityの設定](#Unityの設定)
* [PX4の設定](#PX4の設定)
* [QGCの設定](#QGCの設定)

### 機体特性パラメータの設定

例：２台の場合
```
hakoniwa/config
├── drone_config_0.json
└── drone_config_1.json
```

機体パラメータファイルの書式は以下としてください。

* `drone_config_<index>.json`
* `<index>` は、0からの連番としてください。

各機体のパラメータとして、以下の項目を適切に設定してください。

* name
  * Unityの機体名と一致させてください。例：DroneAvator1
* components.droneDynamics.position_meter
  * 機体の初期位置を設定してください。他の機体と重複しない場所を指定ください。

### Unityの設定

1. Unityエディタで、機体パラメータ毎にドローンアバターを配置してください。ドローンアバター名と機体パラメータの "name" は必ず一致させてください。
2. Unityエディタで、`Generate` を実行し、`hakoniwa-unity-drone-model`直下に生成されている`custom.json を `hakoniwa/config/custom.json`にコピー配置してください。

### PX4の設定

PX4をマルチインスタンスで起動するには、PX4のファイルシステムを機体毎に用意する必要があります。

既存の build ディレクトリを以下の要領でコピー配置してください。

例：２台の場合
```
cd px4/PX4-Autopilot
cp -rp build build-0
cp -rp build build-1
```

buildディレクトリ名の書式は以下としてください。

* `build-<index>`
* `<index>` は、0からの連番としてください。

### QGCの設定

通信リンクの設定に２台目以降のポート番号を追加します。

ポート番号は、`18570 + <index>` としてください。`<index>` は、0からの連番です。

## シミュレーション実行方法

シミュレーション実行方法は、１台の場合と同じです。

ただし、PX4は、機体毎に起動する必要がありますので、以下のコマンドで機体番号を指定して起動してください。

```
bash ../sim/simstart.bash <index>
```

例：１台目の場合
```
bash ../sim/simstart.bash 0
```

例：２台目の場合
```
bash ../sim/simstart.bash 1
```

# ログリプレイ

箱庭ドローンシミュレータでは、シミュレーション実行すると、機体毎にログを記録します。

ログファイルは、`hakoniwa-px4sim/hakoniwa/drone_log0/drone_dynamics.csv`　に配置されています。

このログ情報を利用して、機体の飛行軌跡をリプレイすることができます。

ログリプレイを実行するディレクトリは、シミュレーション実行するディレクトリと同じ場所（`hakoniwa-px4sim/hakoniwa`）で行います。

また、hakoniwa-unity-drone-modelは、hakoniwa-px4simと同じディレクトリ階層であることを前提とします。

```
.
├── hakoniwa-unity-drone-model
└── hakoniwa-px4sim
```

ログリプレイ実行方法は以下の通りです。

```
bash replay.bash
```

hakoniwa-unity-drone-modelのUnityを起動し、シミュレーションを開始すると、シミュレーション実行時と同じ動きが再現されます。


# コミュニティとサポート

このプロジェクトに関する質問やディスカッションは、[箱庭コミュニティフォーラム](https://github.com/toppers/hakoniwa/discussions)で行われています。ここでは、プロジェクトに関する疑問の解消、アイデアの共有、フィードバックの提供ができます。また、プロジェクトの最新情報やアップデートについても、ここで情報共有されます。

プロジェクトに関する質問や提案がある場合、または同じ問題に直面している他のユーザーと意見交換をしたい場合は、遠慮なく[こちら](https://github.com/toppers/hakoniwa/discussions)に投稿してください。


# 本リポジトリの内容とライセンスについて

本リポジトリのコンテンツに関しては、各ファイルにライセンスが明記されている場合、そのライセンスに従います。特に明記されていないコンテンツについては、[TOPPERSライセンス](https://www.toppers.jp/license.html) に基づいて公開されています。

TOPPERSライセンスは、オープンソースプロジェクトのためのライセンスであり、ソフトウェアの使用、変更、配布に関する条件を定めています。ライセンスの詳細については、上記リンクを参照してください。

# 貢献ガイドライン

本プロジェクトへの貢献に興味を持っていただき、ありがとうございます。様々な形での貢献を歓迎しています。以下に、プロジェクトへの貢献方法についてのガイドラインを示します。

## イシューの報告

- バグの報告や新機能の提案などは、GitHubのIssuesを通じて行ってください。
- issue を作成する前に、同様の issue が既に存在しないかを確認してください。
- issue を作成する際は、できるだけ多くの情報を提供してください。再現手順、期待される挙動、実際の挙動、使用している環境などが含まれます。

## プルリクエスト

- 機能追加やバグ修正など、コードに関する貢献はプルリクエストを通じて行ってください。
- 新機能の追加や大きな変更の場合は、事前に関連する issue  で議論を行うことを推奨します。
- コードのスタイルやコーディング規約に一貫性を持たせるため、既存のコードスタイルに従ってください。

## コミュニケーション

- プロジェクトに関するディスカッションや質問は、[Discussions](https://github.com/toppers/hakoniwa/discussions)で行ってください。
- 他の貢献者とのコミュニケーションは、敬意を持って行ってください。

## その他の貢献

- ドキュメントの改善や翻訳など、コード以外の貢献も歓迎します。

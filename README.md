[![Build](https://github.com/toppers/hakoniwa-px4sim/actions/workflows/build.yml/badge.svg)](https://github.com/toppers/hakoniwa-px4sim/actions/workflows/build.yml)

# 環境

* サポートOS
  * Arm系Mac (M1Mac, M2Mac)
  * Windows 10/11
* 利用する環境
  * Arm系Macの場合
    * Python 3.10
      * pyenvでインストールされたものを推奨
      * Jinja2 (`pip install -U jinja2`)
  * Windowsの場合
    * [Windowsの場合の箱庭構成例](https://github.com/toppers/hakoniwa-document/blob/main/architecture/examples/README-win.md)と同じです。
* 利用するドローン
  * https://github.com/toppers/hakoniwa-unity-drone-model/tree/px4
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
git clone -b px4 --recursive https://github.com/toppers/hakoniwa-unity-drone-model.git
```

hakoniwa-unity-drone-model のインストール手順は、以下を参照ください。

https://github.com/toppers/hakoniwa-unity-drone-model/tree/px4


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

Windowsの場合は、以下のコマンドで docker コンテナに入ってください。

```
bash docker/run.bash
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

### Unity

この状態で、Unityのシミュレーションを開始してください。

![スクリーンショット 2023-11-11 14 40 27](https://github.com/toppers/hakoniwa-px4sim/assets/164193/d5d18be8-63bb-42e5-a134-a02121e329b1)

そして、`START` ボタンを押下すると、シミュレーションが動き出します。


### 端末B

この時、端末Bでは以下のように、準備状態になります。

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

ここで、端末Bで以下のコマンドを実行してください。

```
commander takeoff
```

成功すると、Unity上のドローンがホバリングしてくれます。

![スクリーンショット 2023-11-11 14 46 36](https://github.com/toppers/hakoniwa-px4sim/assets/164193/59a5bf25-8256-4a14-b901-4144e3fbcac8)

### シミュレーション停止方法

シミュレーションを停止するには、以下の順番で停止してください。

1. Unityのシミュレーションを止める
2. PX4のシミュレーションを　CTRL＋Cで止める
3. 箱庭のシミュレーションを CTRL+C で止める


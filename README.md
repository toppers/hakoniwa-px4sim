# hakoniwa-px4sim

## 環境

* サポートOS
  * Arm系Mac (M1Mac, M2Mac)
* 利用する環境
  * Docker Desktop
    * 本環境のインストールを実施する場合は、事前に、Docker Desktop を起動してください。
  * Python 3.10
    * pyenvでインストールされたものを推奨
    * Jinja2 (`pip install -U jinja2`)
* 利用するドローン
  * https://github.com/toppers/hakoniwa-unity-drone-model/tree/px4
  * 下記のディレクトリ構成のように、本リポジトリと同じ階層でクローンしてください。
    ```
    hakoniwa-unity-drone-model/
    hakoniwa-px4sim/
    ```

## 事前準備

２つのリポジトリをクローンします。

```
git clone --recursive https://github.com/toppers/hakoniwa-px4sim.git
```

```
git clone -b px4 --recursive https://github.com/toppers/hakoniwa-unity-drone-model.git
```

TODO: hakoniwa-unity-drone-model のインストール手順の説明を追加する


## hakoniwa-px4sim のインストール手順

### px4 の準備

```
cd hakoniwa-px4sim/px4
```

docker イメージを作成します。

```
bash docker/create-image.bash 
```

成功すると、以下のように、`docker images`コマンドで、作成された docker イメージを確認できます。

```
docker images
REPOSITORY                            TAG       IMAGE ID       CREATED        SIZE
toppersjp/hakoniwa-px4-runner         v1.0.0    bca5febae6f5   3 weeks ago    3.7GB
```

### 箱庭の準備

```
cd hakoniwa-px4sim/hakoniwa
```

箱庭環境をビルドします。

```
bash build.bash
```

成功すると、` cmake-build/src/hako-px4sim` というファイルが作成されます。

## シミュレーション実行手順

端末を２つ用意してください。

* 端末A：PX4のシミュレータ実行用
* 端末B：箱庭実行用

### 端末A

```
cd hakoniwa-px4sim/px4
```

PX4 on SITL を起動するための docker コンテナを起動します。

```
bash docker/run.bash
```

成功すると、こうなります。

```
% bash docker/run.bash 
arm64
Darwin
root@97fa2a50d6df:~/workspace/px4/PX4-Autopilot# 
```

次に、箱庭用の制御アプリを組み込むために、ビルドを実行します。
このオペレーションは、docker コンテナを起動するたびに行う必要があります。

```
bash /root/workspace/sim/build.bash 
```

ビルド終了後に、以下のコマンドで PX4 on SITL を起動します。

```
bash /root/workspace/sim/simstart.bash 
```

成功すると、以下のように、TCPポートのコネクション待ちになります。

```
root@97fa2a50d6df:~/workspace/px4/PX4-Autopilot# bash /root/workspace/sim/build.bash 
[0/1] Re-running CMake...
-- PX4 version: v1.14.0-rc2 (1.14.0)
-- PX4 config file: /root/workspace/px4/PX4-Autopilot/boards/px4/sitl/default.px4board
-- PLATFORM posix
-- ROMFSROOT px4fmu_common
-- ROOTFSDIR .
-- TESTING y
-- ETHERNET y
-- PX4 config: px4_sitl_default
-- PX4 platform: posix
-- PX4 lockstep: enabled
-- cmake build type: RelWithDebInfo
-- Could NOT find gz-transport (missing: gz-transport_DIR)
-- Could NOT find Java (missing: Java_JAVA_EXECUTABLE Java_JAR_EXECUTABLE Java_JAVAC_EXECUTABLE Java_JAVAH_EXECUTABLE Java_JAVADOC_EXECUTABLE) 
-- ROMFS: ROMFS/px4fmu_common
Architecture:  arm64
==> CPACK_INSTALL_PREFIX = @DEB_INSTALL_PREFIX@
-- Configuring done
-- Generating done
-- Build files have been written to: /root/workspace/px4/PX4-Autopilot/build/px4_sitl_default
[824/824] Linking CXX shared library s...s/dyn_hello/examples__dyn_hello.px4mod
root@97fa2a50d6df:~/workspace/px4/PX4-Autopilot# bash /root/workspace/sim/simstart.bash 
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
INFO  [param] selected parameter backup file parameters_backup.bson
  SYS_AUTOCONFIG: curr: 0 -> new: 1
  SYS_AUTOSTART: curr: 0 -> new: 10016
  CAL_ACC0_ID: curr: 0 -> new: 1310988
  CAL_GYRO0_ID: curr: 0 -> new: 1310988
  CAL_ACC1_ID: curr: 0 -> new: 1310996
  CAL_GYRO1_ID: curr: 0 -> new: 1310996
  CAL_ACC2_ID: curr: 0 -> new: 1311004
  CAL_GYRO2_ID: curr: 0 -> new: 1311004
  CAL_MAG0_ID: curr: 0 -> new: 197388
  CAL_MAG0_PRIO: curr: -1 -> new: 50
  CAL_MAG1_ID: curr: 0 -> new: 197644
  CAL_MAG1_PRIO: curr: -1 -> new: 50
* SENS_BOARD_X_OFF: curr: 0.0000 -> new: 0.0000
* SENS_DPRES_OFF: curr: 0.0000 -> new: 0.0010
INFO  [dataman] data manager file './dataman' size is 7866640 bytes
etc/init.d-posix/rcS: 39: [: Illegal number: 
INFO  [init] PX4_SIM_HOSTNAME: 192.168.65.254
INFO  [simulator_mavlink] using TCP on remote host 192.168.65.254 port 4560
WARN  [simulator_mavlink] Please ensure port 4560 is not blocked by a firewall.
INFO  [simulator_mavlink] Waiting for simulator to accept connection on TCP port 4560
```

ここから先は、端末Bです。

### 端末B

```
cd hakoniwa-px4sim/hakoniwa
```

箱庭を起動するためのスクリプトを起動します。

```
bash docker/run.bash
```

成功すると、こうなります。

```
bash run.bash 
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
Received data with length: 42
Decoded MAVLink message:
  Message ID: 76
  System ID: 1
  Component ID: 1
  Sequence: 0
  Type: COMMAND_LONG
  Target system: 0
  Target component: 0
  Command ID: 511
  Confirmation: 0
Received data with length: 21
Decoded MAVLink message:
  Message ID: 0
  System ID: 1
  Component ID: 1
  Sequence: 1
  Type: HEARTBEAT
  Custom mode: 0
  Base mode: 0
  System status: 0
  MAVLink version: 3
```

成功すると、端末A側で、以下のように poll timeout のメッセージが出ますが、特に問題ないです。

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
mc_rate_control stop
```

```
param set COM_DISARM_LAND -1
```

```
commander arm
```

これで準備OKです。では、テイクオフです！

```
px4_hakoniwa start
```

成功すると、Unity上のドローンがホバリングしてくれます。

![スクリーンショット 2023-11-11 14 46 36](https://github.com/toppers/hakoniwa-px4sim/assets/164193/59a5bf25-8256-4a14-b901-4144e3fbcac8)

### シミュレーション停止方法

シミュレーションを停止するには、以下の順番で停止してください。

1. Unityのシミュレーションを止める
2. PX4のシミュレーションを　CTRL＋Cで止める
3. 箱庭のシミュレーションを CTRL+C で止める


# MAVLINK仕様データ

MAVLINK仕様データは、以下を参照すること。

https://mavlink.io/en/messages/common.html

ただし、座標系が曖昧なものがあるので、ここで定義する。

(現状、仮説であるため、適宜変更対応予定)

## HIL_STATE_QUATERNION

|名前|座標系|備考|
|---|---|---|
|time_usec|-|-|
|rollspeed|航空|-|
|pitchspeed|航空|-|
|yawspeed|航空|-|
|vx|航空|-|
|vy|航空|-|
|vz|航空|-|
|xacc|航空|-|
|yacc|航空|-|
|zacc|航空|重力は含めない|
|attitude_quaternion|航空|-|
|lat|航空|-|
|lon|航空|-|
|alt|航空|-|
|ind_airspeed|-|-|
|true_airspeed|-|-|


## HIL_SENSOR

|名前|座標系|備考|
|---|---|---|
|time_usec|-|-|
|xacc|航空|-|
|yacc|航空|-|
|zacc|航空|重力を含む|
|xgyro|航空|-|
|ygyro|航空|-|
|zgyro|航空|-|
|xmag|-|-|
|ymag|-|-|
|zmag|-|-|
|abs_pressure|-|-|
|diff_pressure|-|-|
|pressure_alt|-|-|
|temperature|-|-|
|fields_updated|-|-|
|id|-|-|

## HIL_GPS

|名前|座標系|備考|
|---|---|---|
|time_usec|-|-|
|fix_type||-|
|lat|航空|-|
|lon|航空|-|
|alt|ROS|-|
|eph|-|-|
|epv|-|-|
|vel|-|-|
|vn|航空|-|
|ve|航空|-|
|vd|航空|-|
|cog|-|-|
|satelites_visible|-|-|
|id|-|-|
|yaw|-|-|

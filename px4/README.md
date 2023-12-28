# PX4のインストール手順

動作チェック状況

- [X] Mac
- [] Windows/WSL2
- [] Ubuntu


## 共通手順


```
cd px4
```


### 機体情報を箱庭ように差し替える

```
cp hakoniwa-apps/10016_none_iris PX4-Autopilot/ROMFS/px4fmu_common/init.d-posix/airframes/10016_none_iris 
```

```
cp hakoniwa-apps/rcS PX4-Autopilot/ROMFS/px4fmu_common/init.d-posix/rcS
```

### シミュレータ起動方法

前提条件：ビルド済みであること

```
 bash ../sim/simstart.bash
```

成功すると、以下のログが出力されます。

```
% make px4_sitl none_iris
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
  SENS_BOARD_X_OFF: curr: 0.0000 -> new: 0.0000
  SENS_DPRES_OFF: curr: 0.0000 -> new: 0.0010
INFO  [dataman] data manager file './dataman' size is 7868392 bytes
INFO  [init] PX4_SIM_HOSTNAME: localhost
INFO  [simulator_mavlink] Waiting for simulator to accept connection on TCP port 4560
```

## Mac向け PX4ビルド手順

```
cd PX4-Autopilot 
```

MacOS向けのセットアップ（最初の１回だけで実行すれば良いです）。

```
sh Tools/setup/macos.sh --no-nuttx --no-sim-tools
```

ビルド実行(変更が必要な場合は毎回実行する必要あります)。

```
make px4_sitl_default
```


クリーン方法：

```
make clean
```

参考にしたサイト：https://docs.px4.io/main/en/dev_setup/dev_env_mac.html


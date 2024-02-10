[English](README.md) ｜ 日本語

# PX4のインストール手順

動作チェック状況

- [X] Mac
- [X] Windows/WSL2
- [ ] Ubuntu


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

## Windows向け PX4ビルド手順

```
cd PX4-Autopilot 
```

セットアップ（最初の１回だけで実行すれば良いです）。

```
bash Tools/setup/ubuntu.sh --no-nuttx --no-sim-tools
```

ビルド実行(変更が必要な場合は毎回実行する必要あります)。

```
make px4_sitl_default
```

クリーン方法：

```
make clean
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

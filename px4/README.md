English ｜ [日本語](README-ja.md)

# PX4 Installation Instructions

Operation Check Status:

- [X] Mac
- [X] Windows/WSL2
- [X] Ubuntu

## Common Steps

Navigate to the PX4 directory:
```
cd px4
```

### Replace Aircraft Information for Hakoniwa

Copy the custom aircraft settings into the PX4-Autopilot folder:
```
cp hakoniwa-apps/10016_none_iris PX4-Autopilot/ROMFS/px4fmu_common/init.d-posix/airframes/10016_none_iris
```
```
cp hakoniwa-apps/rcS PX4-Autopilot/ROMFS/px4fmu_common/init.d-posix/rcS
```

## PX4 Build Instructions for Windows/WSL2 and Ubuntu

Navigate to the PX4-Autopilot directory:
```
cd PX4-Autopilot
```

Setup (needs to be executed only once):
```
bash Tools/setup/ubuntu.sh --no-nuttx --no-sim-tools
```

Execute the build (needs to be run every time changes are made):
```
make px4_sitl_default
```

To clean the build:
```
make clean
```

## PX4 Build Instructions for Mac

Navigate to the PX4-Autopilot directory:
```
cd PX4-Autopilot
```

Setup for MacOS (needs to be executed only once):
```
sh Tools/setup/macos.sh --no-nuttx --no-sim-tools
```

Execute the build (needs to be run every time changes are made):
```
make px4_sitl_default
```

To clean the build:
```
make clean
```

Reference site: https://docs.px4.io/main/en/dev_setup/dev_env_mac.html

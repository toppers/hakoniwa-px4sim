#!/bin/bash

cp -rp /root/workspace/px4/hakoniwa-apps/px4_hakoniwa /root/workspace/px4/PX4-Autopilot/src/examples/
cp /root/workspace/px4/hakoniwa-apps/default.px4board /root/workspace/px4/PX4-Autopilot/boards/px4/sitl/
cp /root/workspace/px4/hakoniwa-apps/10016_none_iris /root/workspace/px4/PX4-Autopilot/ROMFS/px4fmu_common/init.d-posix/airframes/

make px4_sitl_default
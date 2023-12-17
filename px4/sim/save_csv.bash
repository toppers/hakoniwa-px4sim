#!/bin/bash

if [ ! -d /root/workspace/sim/log ]
then
    mkdir log
fi

cp build/px4_sitl_default/rootfs/*.csv /root/workspace/sim/log/
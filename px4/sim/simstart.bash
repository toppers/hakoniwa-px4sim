#!/bin/bash

if [ -d /etc/hosts ]
then
    export PX4_SIM_HOST_ADDR=`cat /etc/hosts | grep $DOCKER_LOCAL_HOST_ADDR | awk '{print $1}'`
fi
if [ $# -eq 1 ]
then
    export PX4_SIM_HOST_ADDR=${1}
fi

make px4_sitl none_iris

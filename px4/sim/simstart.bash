#!/bin/bash

export PX4_SIM_HOST_ADDR=`cat /etc/hosts | grep $DOCKER_LOCAL_HOST_ADDR | awk '{print $1}'`
if [ $# -eq 1 ]
then
    export PX4_SIM_HOST_ADDR=${1}
fi

make px4_sitl none_iris

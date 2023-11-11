#!/bin/bash

export PX4_SIM_HOST_ADDR=`cat /etc/hosts | grep $DOCKER_LOCAL_HOST_ADDR | awk '{print $1}'`
make px4_sitl none_iris

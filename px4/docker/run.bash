#!/bin/bash

HOST_WORKDIR=`pwd`
DOCKER_DIR=/root/workspace
IMAGE_NAME=`cat docker/image_name.txt`
IMAGE_NAME_ARG=`cat docker/image_name.txt | awk -F/ '{print $2}'`
IMAGE_TAG=`cat docker/version.txt`
DOCKER_IMAGE=${IMAGE_NAME}:${IMAGE_TAG}

ARCH=`arch`
OS_TYPE=`uname`
echo $ARCH
echo $OS_TYPE
if [ $OS_TYPE = "Darwin" ]
then
    docker run \
		--platform linux/arm64 \
        -it --rm \
        --add-host=host.docker.internal:host-gateway \
        -v `pwd`/sim:${DOCKER_DIR}/sim \
        -v `pwd`/hakoniwa-apps:/root/workspace/px4/hakoniwa-apps \
        -p 14445:18570/udp \
        -p 14580:14580/udp \
        -p 14280:14280/udp \
        -p 13030:13030/udp \
        -p 14560:14560/udp \
        -e PX4_HOME_LAT=47.641468 \
        -e PX4_HOME_LON=-122.140165 \
        -e PX4_HOME_ALT=0.0 \
        --name ${IMAGE_NAME_ARG} ${DOCKER_IMAGE} 
else
	export RESOLV_IPADDR=`cat /etc/resolv.conf  | grep nameserver | awk '{print $NF}'`
	NETWORK_INTERFACE=$(route | grep '^default' | grep -o '[^ ]*$' | tr -d '\n')
	CORE_IPADDR=$(ifconfig "${NETWORK_INTERFACE}" | grep netmask | awk '{print $2}')
    docker run \
        -v `pwd`/sim:${DOCKER_DIR}/sim \
        -v `pwd`/hakoniwa-apps:/root/workspace/px4/hakoniwa-apps \
        -it --rm \
        --net host \
        -e PX4_SIM_HOSTNAME=${CORE_IPADDR} \
        -e PX4_HOME_LAT=47.641468 \
        -e PX4_HOME_LON=-122.140165 \
        -e PX4_HOME_ALT=0.0 \
        --name ${IMAGE_NAME_ARG} ${DOCKER_IMAGE} 
fi

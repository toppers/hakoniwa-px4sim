#!/bin/bash

HOST_WORKDIR=`pwd`
DOCKER_DIR=/root/workspace
IMAGE_NAME=`cat docker/image_name.txt`
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
        --name ${IMAGE_NAME} ${DOCKER_IMAGE} 
else
    docker run \
        -v ${HOST_WORKDIR}:${DOCKER_DIR} \
        -it --rm \
        -p 14560:14560/udp \
        --name ${IMAGE_NAME} ${DOCKER_IMAGE} 
fi

#!/bin/bash

source docker/env.bash

HAKONIWA_TOP_DIR=`pwd`
IMAGE_NAME=`cat docker/image_name.txt`
IMAGE_TAG=`cat docker/latest_version.txt`
DOCKER_IMAGE=toppersjp/${IMAGE_NAME}:${IMAGE_TAG}

ARCH=`arch`
OS_TYPE=`bash docker/utils/detect_os_type.bash`
echo $ARCH
echo $OS_TYPE
if [ $OS_TYPE = "Mac" ]
then
    if [ $ARCH = "arm64" ]
    then
        docker run \
            --platform linux/amd64 \
            -v ${HOST_WORKDIR}:${DOCKER_DIR} \
            -it --rm \
            -w ${DOCKER_DIR} \
            --net host \
            --name ${IMAGE_NAME} ${DOCKER_IMAGE} 
    else
        docker run \
            -v ${HOST_WORKDIR}:${DOCKER_DIR} \
            -it --rm \
            --net host \
            -w ${DOCKER_DIR} \
            --name ${IMAGE_NAME} ${DOCKER_IMAGE} 
    fi
else
    docker run \
        -v ${HOST_WORKDIR}:${DOCKER_DIR} \
        -it --rm \
        --net host \
        -w ${DOCKER_DIR} \
        --name ${IMAGE_NAME} ${DOCKER_IMAGE} \
        /bin/bash /root/ev3rt-athrill-v850e2m/sdk/src/build.bash
fi

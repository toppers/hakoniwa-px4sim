#!/bin/bash

if [ $# -ne 2 ]
then
    echo "Usage: $0 <hakoniwa-unity-drone-model location> <config directory>"
    exit 1
fi

export HAKO_CUSTOM_JSON_PATH=${1}/custom.json
export DRONE_CONFIG_PATH=${2}

if [ ! -f ${HAKO_CUSTOM_JSON_PATH} ]
then
    echo "ERROR: can not find ${HAKO_CUSTOM_JSON_PATH}"
    exit 1
fi
if [ ! -d ${DRONE_CONFIG_PATH} ]
then
    echo "ERROR: can not find ${DRONE_CONFIG_PATH}"
    exit 1
fi

cmake-build/src/hako-px4sim 127.0.0.1 4560 ext


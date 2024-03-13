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

if [ -z $HAKO_MASTER_DISABLE ]
then
    :
else
    source scripts_utils/hako-master.bash
    activate_master
fi

cmake-build/src/hako-px4sim 127.0.0.1 4560 pid


if [ -z $HAKO_MASTER_DISABLE ]
then
    :
else
    echo "INFO: NOW STOPPING SIMULATION" 

    kill_process
    echo "INFO: If you find that your keyboard input is not being displayed correctly, "
    echo "      try executing the 'reset' command to resolve the issue."
fi

exit 0

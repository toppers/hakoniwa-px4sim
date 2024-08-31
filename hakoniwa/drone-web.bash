#!/bin/bash
trap 'kill 0' EXIT

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

if [ -z "$HAKO_CONTROLLER_PARAM_FILE" ]
then
    grep moduleDirectory ${DRONE_CONFIG_PATH}/*.json | grep Flight > /dev/null
    if [ $? -eq 0 ]
    then
        export HAKO_CONTROLLER_PARAM_FILE=../drone_control/config/param-api.txt
    else
        export HAKO_CONTROLLER_PARAM_FILE=../drone_control/config/param-rc.txt
    fi
fi


REALTIME_SYNCHER_PID=
function run_realtime_syncher()
{
    CURR_DIR=`pwd`
    cd ../drone_api/sample
    if [ -f ~/myenv/bin/activate  ]
    then
        source ~/myenv/bin/activate
        PYTHON_BIN=python3.12
    else
        PYTHON_BIN=python
    fi
    ${PYTHON_BIN} real_time_syncher.py ../../../hakoniwa-unity-drone-model/custom.json  20 &
    REALTIME_SYNCHER_PID=$!
    cd $CURR_DIR
}


cmake-build/src/hako-px4sim 127.0.0.1 4560 ext &
EXT_PID=$!
sleep 1

run_realtime_syncher

sleep 1
hako-cmd start

sleep 2
echo "STOP FOR ENTER"
read

kill -s TERM ${REALTIME_SYNCHER_PID} ${EXT_PID}

wait ${REALTIME_SYNCHER_PID} ${EXT_PID}
exit 0

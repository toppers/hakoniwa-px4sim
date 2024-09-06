#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Usage: $0 {simple-demo|tojinbo|ar-demo}"
    exit 1
fi

ACT_MODE=$1

if [ "$ACT_MODE" != "simple-demo" ] && [ "$ACT_MODE" != "tojinbo" ] && [ "$ACT_MODE" != "ar-demo" ]
then
    echo "Error: Invalid argument. Please use one of the following: simple-demo, tojinbo, ar-demo."
    exit 1
fi

echo "Running in mode: $ACT_MODE"


DRONE_CONFIG=config/rc
if [ $ACT_MODE = "tojinbo" ]
then
    DRONE_CONFIG=config/rc-kanko
fi

HAKONIWA_RADIO_CTRL_PATH=drone_api/sample
HAKONIWA_PX4SIM_PATH=hakoniwa
HAKONIWA_VREAL_PATH=../hakoniwa-unity-drone-model

HAKO_PX4SIM_PID=
HAKO_VREAL_PID=
HAKO_CAMER_PID=
function kill_process()
{
    echo "trapped"

    if [ ! -z "$HAKO_CAMER_PID" ]
    then
        echo "KILLING: camera $HAKO_CAMER_PID"
        kill -s TERM "$HAKO_CAMER_PID" || echo "Failed to kill vreal"
    fi
    if [ ! -z "$HAKO_VREAL_PID" ]
    then
        echo "KILLING: vreal $HAKO_VREAL_PID"
        kill -s TERM "$HAKO_VREAL_PID" || echo "Failed to kill vreal"
    fi
    if [ ! -z "$HAKO_PX4SIM_PID" ]
    then
        echo "KILLING: px4sim $HAKO_PX4SIM_PID"
        kill -s TERM "$HAKO_PX4SIM_PID" || echo "Failed to kill px4sim"
    fi

    exit 0
}
function signal_handler() {
    echo "SIGINT"
}
trap signal_handler SIGINT

export PYTHONPATH="/usr/local/lib/hakoniwa:$PYTHONPATH"
export PYTHONPATH="/usr/local/lib/hakoniwa/py:$PYTHONPATH"
export PATH="/usr/local/bin/hakoniwa:${PATH}"
export LD_LIBRARY_PATH="/usr/local/lib/hakoniwa:${LD_LIBRARY_PATH}"
export DYLD_LIBRARY_PATH="/usr/local/lib/hakoniwa:${DYLD_LIBRARY_PATH}"


function activate_px4sim()
{
    CURR_DIR=`pwd`
    cd $HAKONIWA_PX4SIM_PATH
    bash drone-app.bash ../../hakoniwa-unity-drone-model/${ACT_MODE} ${DRONE_CONFIG} &
    HAKO_PX4SIM_PID=$!
    cd $CURR_DIR
}

function activate_vreal()
{
    CURR_DIR=`pwd`
    cd $HAKONIWA_VREAL_PATH
    bash plugin/activate_app.bash ${ACT_MODE} &
    HAKO_VREAL_PID=$!
    cd $CURR_DIR
}

function adjust_initial_pos()
{
    CONFIG_PATH=`pwd`/demo/xr_config.json
    AR_DEVICE_IPADDR=`jq -r '.server_url' ${CONFIG_PATH} | awk -F: '{print $1}'`
    CURR_DIR=`pwd`
    cd $HAKONIWA_VREAL_PATH
    echo "access: ${AR_DEVICE_IPADDR}:38528"
    python ${HAKONIWA_VREAL_PATH}/utils/xr_origin_tuning.py --input joystick ${CONFIG_PATH} ${AR_DEVICE_IPADDR}:38528
    cd $CURR_DIR
}

function radio_control()
{
    CURR_DIR=`pwd`
    cd $HAKONIWA_RADIO_CTRL_PATH
    if [ -f ~/myenv/bin/activate  ]
    then
        source ~/myenv/bin/activate
        PYTHON_BIN=python3.12
    else
        PYTHON_BIN=python
    fi
    ${PYTHON_BIN} rc.py ../../../hakoniwa-unity-drone-model/${ACT_MODE}/custom.json
    cd $CURR_DIR
}
function camera_control()
{
    CURR_DIR=`pwd`
    cd $HAKONIWA_RADIO_CTRL_PATH
    if [ -f ~/myenv/bin/activate  ]
    then
        source ~/myenv/bin/activate
        PYTHON_BIN=python3.12
    else
        PYTHON_BIN=python
    fi
    ${PYTHON_BIN} camera.py ../../../hakoniwa-unity-drone-model/${ACT_MODE}/custom.json &
    HAKO_CAMER_PID=$!
    cd $CURR_DIR
}
activate_px4sim

sleep 1

activate_vreal

if [ ${ACT_MODE} = "ar-demo" ]
then
    echo "Pleaser Enter key after clicking START button..."
    read
    echo "START ADJUST INITIAL POSITION"
    adjust_initial_pos
else
    sleep 5
    hako-cmd start
    sleep 1
    echo "START CAMERA"
    camera_control
fi

sleep 1
echo "START RADIO CONTROL"
radio_control

kill_process


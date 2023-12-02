#!/bin/bash
CURRENT_DIR=`pwd`
export PYTHONPATH=`pwd`/third-party/hakoniwa-python-simasset-plugin/src:${PYTHONPATH}

HAKO_RUN_PID=
HAKO_PYTHON_PID=
IS_OCCURED_SIGEVENT="FALSE"
function signal_handler()
{
    IS_OCCURED_SIGEVENT="TRUE"
    echo "trapped"
}
function kill_process()
{
    echo "INFO: STOPPING Simulation"
    if [ -z "$HAKO_RUN_PID" ]
    then
        exit 0
    fi
    cd $CURRENT_DIR
    if [ ! -z "$HAKO_PYTHON_PID" ]
    then
        echo "KILLING: UNITY $HAKO_PYTHON_PID"
        kill -9 $HAKO_PYTHON_PID
    fi
    kill -9 $HAKO_RUN_PID
    echo "INFO: STOP DONE"
    exit 0
}
trap signal_handler SIGINT

echo "INFO: Simulatin start"

cd cmake-build
./src/hako-px4sim 127.0.0.1 4560 phys &
HAKO_RUN_PID=$!

sleep 1

cd $CURRENT_DIR
python3 python/drone_pid_control.py python/config.json &
HAKO_PYTHON_PID=$!

echo "INFO: START SIMULATION"
sleep 1

hako-cmd start

while true; do
    echo "Press ENTER to stop..."
    read input
    if [ -z "$input" ]; then
        hako-cmd stop
        sleep 1
        hako-cmd reset
        sleep 1
        kill_process
        break
    fi
done



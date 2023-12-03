#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Usage: $0 <simtime sec>"
	exit 1
fi
SIMTIME_SEC=$1

CURRENT_DIR=`pwd`
export PYTHONPATH=`pwd`/third-party/hakoniwa-python-simasset-plugin/src:${PYTHONPATH}
export PYTHONPATH=/usr/local/lib/hakoniwa:${PYTHONPATH}
export PYTHONPATH=/usr/local/lib/hakoniwa/py:${PYTHONPATH}
export PATH=/usr/local/bin/hakoniwa:${PATH}
export LD_LIBRARY_PATH=/usr/local/lib/hakoniwa:${LD_LIBRARY_PATH}
export HAKO_CUSTOM_JSON_PATH=../config/custom_python.json

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

echo "wait ${SIMTIME_SEC} sec"
sleep ${SIMTIME_SEC}
hako-cmd stop
sleep 1
hako-cmd reset
sleep 1
kill_process


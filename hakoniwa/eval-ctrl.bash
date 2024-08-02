#!/bin/bash
trap 'kill 0' EXIT

export HAKO_CUSTOM_JSON_PATH=../../hakoniwa-unity-drone-model/custom.json
export DRONE_CONFIG_PATH=config/rc
export HAKO_CONTROLLER_PARAM_FILE=../drone_control/config/param-rc.txt

EXEC_SIM_TIME=5

HAKO_PID=
EVAL_PID=
if [ $# -ne 2 ]
then
    echo "Usage: $0 axis value"
    exit 1
fi
source ~/myenv/bin/activate

# start hakoniwa
cmake-build/src/hako-px4sim 127.0.0.1 4560 ext &
HAKO_PID=$!

CURR_DIR=`pwd`
# start eval-ctrl
cd ../drone_api/sample
python3.12 eval-ctrl.py ../../../hakoniwa-unity-drone-model/custom.json &
EVAL_PID=$!

cd ${CURR_DIR}

sleep 1

# hako start
hako-cmd start

sleep ${EXEC_SIM_TIME}

# kill hakoniwa
kill -s TERM ${HAKO_PID}

# kill eval-ctrl
kill -s TERM ${EVAL_PID}

# evaluation
python python/control_evaluate.py ./drone_log0/drone_dynamics.csv python/control_evaluate_sample.json

wait ${HAKO_PID} ${EVAL_PID}

echo "INFO: DONE"
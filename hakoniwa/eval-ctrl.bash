#!/bin/bash
trap 'kill 0' EXIT

export HAKO_CUSTOM_JSON_PATH=../../hakoniwa-unity-drone-model/custom.json

EXEC_SIM_TIME=5

HAKO_PID=
EVAL_PID=
if [ $# -ne 3 -a $# -ne 4 ]
then
    echo "Usage: $0 <stop_time> <tkey:tvalue> <key:value> <S:targetSpeed>"
    exit 1
fi
if [ ${1} -gt 0 ]
then
    STOP_TIME=`expr ${1} \* 1000000`
else
    STOP_TIME=-1
fi

SPEED_KEY_VALUE=
if [ $# -eq 4 ]
then
    SPEED_KEY_VALUE=${4}
    export DRONE_CONFIG_PATH=config/api_sample
    export HAKO_CONTROLLER_PARAM_FILE=../drone_control/config/param-api.txt
else
    export DRONE_CONFIG_PATH=config/rc
    export HAKO_CONTROLLER_PARAM_FILE=../drone_control/config/param-rc.txt
fi

TKEY_VALUE=${2}
KEY_VALUE=${3}
TKEY=`echo ${TKEY_VALUE} | awk -F: '{print $1}'`
TVALUE=`echo ${TKEY_VALUE} | awk -F: '{print $2}'`

KEY=`echo ${KEY_VALUE} | awk -F: '{print $1}'`
VALUE=`echo ${KEY_VALUE} | awk -F: '{print $2}'`

if [ "$TKEY" = "Rx" ] || [ "$TKEY" = "Ry" ]
then
    VAL=true
else
    VAL=false
fi
cp python/control_evaluate_sample.json python/tmp1.json
jq --argjson value ${VAL} '.CONVERT_TO_DEGREE = $value' python/tmp1.json > python/control_evaluate_sample.json
rm -f python/tmp1.json

if [ -f ~/myenv/bin/activate ]
then
    source ~/myenv/bin/activate
fi

# start hakoniwa
cmake-build/src/hako-px4sim 127.0.0.1 4560 ext &
HAKO_PID=$!

CURR_DIR=`pwd`
# start eval-ctrl
cd ../drone_api/sample
which python3.12
if [ $? -eq 0 ]
then
	PYTHON_BIN=python3.12
else
	PYTHON_BIN=python
	which python3
	if [ $? -eq 0 ]
	then
		PYTHON_BIN=python3
	fi
fi
${PYTHON_BIN} eval-ctrl.py ../../../hakoniwa-unity-drone-model/custom.json ${STOP_TIME} ${TKEY_VALUE} ${KEY_VALUE} ${SPEED_KEY_VALUE} &
EVAL_PID=$!

cd ${CURR_DIR}

sleep 1

# hako start
hako-cmd start

cat python/tmp.json

sleep ${EXEC_SIM_TIME}

# kill hakoniwa
kill -s TERM ${HAKO_PID}

# kill eval-ctrl
kill -s TERM ${EVAL_PID}

jq --arg axis "$TKEY" --argjson value "$TVALUE" '.AXIS = $axis | .TARGET_VALUE = $value' python/control_evaluate_sample.json > python/tmp.json

START_TIME=`cat /tmp/v.txt`
cp python/tmp.json python/tmp1.json
jq --argjson value "$START_TIME" '.EVALUATION_START_TIME = $value' python/tmp1.json > python/tmp.json
rm -f python/tmp1.json

# evaluation
${PYTHON_BIN} python/control_evaluate.py ./drone_log0/drone_dynamics.csv python/tmp.json

wait ${HAKO_PID} ${EVAL_PID}

echo "INFO: DONE"

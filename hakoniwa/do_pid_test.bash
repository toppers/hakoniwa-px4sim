#!/bin/bash

if [ $# -ne 2 ]
then
	echo "Usage: $0 <run-time-sec> <duration>"
	exit 1
fi

if [ ! -d python/results ]
then
	mkdir python/results
fi

RUN_TIME_SEC=${1}
DURATION=${2}

export DRONE_CONFIG_PATH=./config/drone_config.json
export HAKO_CUSTOM_JSON_PATH=./config/custom_c++.json

 ./cmake-build/src/hako-px4sim 127.0.01 4560 phys &
HAKO_PID=$!


sleep 1

hako-cmd start

sleep ${RUN_TIME_SEC}

kill -s TERM $HAKO_PID

wait ${HAKO_PID}


python3 python/hako_CausalAnalyzer.py ./python/results/height_data.csv ./python/results/phi_data.csv --dependent Height --independent Phi --graph_type line --duration ${DURATION}

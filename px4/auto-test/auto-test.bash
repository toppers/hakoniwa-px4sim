#!/bin/bash

if [ $# -ne 2 ]
then
    echo "Usage: $0 <test-resource-path> <test_scenario>"
    exit 1
fi

TEST_RESOURCE_PATH=${1}
TEST_SCENARIO=${2}
export DRONE_CONFIG_PATH="`pwd`/px4/auto-test/test_resource/work/drone_config.json"

if [ ! -f hako-start.bash ]
then
    echo "ERROR: not find hako-start.bash"
    exit 1
fi

function do_test()
{
    local result_name=${1}

    # DO TEST
    bash hako-start.bash ${TEST_RESOURCE_PATH}/configs/config.json `pwd`/${TEST_RESOURCE_PATH}/work/drone_config.json ${TEST_SCENARIO}

    # EVALUATE TEST RESULTS
    if [ ! -d ${TEST_RESOURCE_PATH}/eval/last_point ]
    then
        mkdir ${TEST_RESOURCE_PATH}/eval/last_point
    fi
    python3 px4/auto-test/evaluation/eval_data_mean_stdev.py  \
        px4/auto-test/evaluation_input/eval_height.json  ${TEST_RESOURCE_PATH}/logs \
        | tee ${TEST_RESOURCE_PATH}/eval/last_point/alt_${result_name}.txt
    python3 px4/auto-test/evaluation/eval_data_mean_stdev.py  \
        px4/auto-test/evaluation_input/eval_lat.json  ${TEST_RESOURCE_PATH}/logs \
        | tee ${TEST_RESOURCE_PATH}/eval/last_point/lat_${result_name}.txt
    python3 px4/auto-test/evaluation/eval_data_mean_stdev.py  \
        px4/auto-test/evaluation_input/eval_lon.json  ${TEST_RESOURCE_PATH}/logs \
        | tee ${TEST_RESOURCE_PATH}/eval/last_point/lon_${result_name}.txt
}
function do_test_move()
{
    local result_name=${1}

    # DO TEST
    bash hako-start.bash ${TEST_RESOURCE_PATH}/configs/config.json ${TEST_RESOURCE_PATH}/work/drone_config.json ${TEST_SCENARIO}

    # EVALUATE TEST RESULTS
    python3 px4/auto-test/evaluation/eval_data_mean_stdev.py  px4/auto-test/evaluation_input/eval_lon.json  ${TEST_RESOURCE_PATH}/logs \
            | tee ${TEST_RESOURCE_PATH}/eval/${result_name}.txt
}

function do_config()
{
    local p1=${1}
    local p2=${2}

    # PREPARE TEST CONDITIONS
    cp ${TEST_RESOURCE_PATH}/configs/drone_config_base.json ${TEST_RESOURCE_PATH}/work/drone_config.json 

    python3 hakoniwa/python/drone_config.py ${TEST_RESOURCE_PATH}/work/drone_config.json \
        components.rotor.Tr ${p1}
    echo "components.rotor.Tr ${p1}"

    I_XY="0$(echo "scale=4; ${p2} * 1" | bc)"
    I_Z="0$(echo "scale=4; ${p2} * 2" | bc)"
    python3 hakoniwa/python/drone_config.py ${TEST_RESOURCE_PATH}/work/drone_config.json \
        components.droneDynamics.inertia '['"${I_XY}"', '"${I_XY}"', '"${I_Z}"']'
    echo "components.droneDynamics.inertia '['"${I_XY}"', '"${I_XY}"', '"${I_Z}"']'"
}

function do_config_thruster()
{
    local p1=${1}
    local p2=${2}

    # PREPARE TEST CONDITIONS
    cp ${TEST_RESOURCE_PATH}/configs/drone_config_base.json ${TEST_RESOURCE_PATH}/work/drone_config.json 

    python3 hakoniwa/python/drone_config.py ${TEST_RESOURCE_PATH}/work/drone_config.json \
        components.thruster.parameterB "${p1}e-10"

    python3 hakoniwa/python/drone_config.py ${TEST_RESOURCE_PATH}/work/drone_config.json \
        components.thruster.parameterJr  "${p2}e-10"
}

# 以下はサンプルとしてのテストプログラムです。
for p1 in 1.0 2.0 3.0 4.0. 5.0 6.0 7.0 8.0 9.0 10.0
#for p1 in 1.4 1.5 1.6 1.7 1.8 1.9 2.0
#for p1 in 1.3
do
    for p2 in 1.0 2.0 4.0 6.0 10.0
#    for p2 in 0.6
    do
        echo "INFO: START TEST PARAM=${p1}-${p2}"
        do_config_thruster ${p1} ${p2}
        FILENAME=$(echo "result-p${p1}-p${p2}" | sed 's/\./_/g')
        do_test ${FILENAME}
        if [ ! -d  ${TEST_RESOURCE_PATH}/eval/logs ]
        then
            mkdir ${TEST_RESOURCE_PATH}/eval/logs
        fi
        cp ${TEST_RESOURCE_PATH}/logs/drone_dynamics.csv \
            ${TEST_RESOURCE_PATH}/eval/logs/drone_dynamics-p${p1}-p${p2}.csv
        sleep 5
        echo "INFO: END TEST"
    done
done

exit 0
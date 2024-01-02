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
    bash hako-start.bash ${TEST_RESOURCE_PATH}/configs/config.json ${TEST_RESOURCE_PATH}/work/drone_config.json ${TEST_SCENARIO}

    # EVALUATE TEST RESULTS
    python3 px4/auto-test/evaluation/eval_data_mean_stdev.py  px4/auto-test/evaluation_input/eval_height.json  ${TEST_RESOURCE_PATH}/logs \
            | tee ${TEST_RESOURCE_PATH}/eval/${result_name}.txt
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
        components.thruster.parameterB "${p1}e-09"
    echo "components.thruster.parameterB "${p1}e-09"

    python3 hakoniwa/python/drone_config.py ${TEST_RESOURCE_PATH}/work/drone_config.json \
        components.thruster.parameterJr  "${p2}e-08"
    echo "components.thruster.parameterJr  "${p2}e-08"
}

# 以下はサンプルとしてのテストプログラムです。
#for p1 in 0.05 0.1 0.15 0.2 0.25
#for p1 in 0.5 1.0 2.0 5.0 10.0 20.0
for p1 in 30.0 40.0 50.0 60.0 70.0 80.0
#for p1 in 0.1
do
#    for p2 in 0.001 0.005 0.01 0.015 0.02
    for p2 in 0.1 0.2 0.5 1.0 1.5 2.0 4.0
#    for p2 in 0.01
    do
        echo "INFO: START TEST PARAM=${p}"
        do_config_thruster ${p1} ${p2}
        FILENAME=$(echo "result-p${p1}-p${p2}" | sed 's/\./_/g')
        do_test_move ${FILENAME}
        sleep 5
        echo "INFO: END TEST"
    done
done

exit 0
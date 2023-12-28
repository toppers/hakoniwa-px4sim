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
    local config_path=${1}
    local new_value=${2}
    local result_name=${3}

    # PREPARE TEST CONDITIONS
    cp ${TEST_RESOURCE_PATH}/configs/drone_config_base.json ${TEST_RESOURCE_PATH}/work/drone_config.json 
    python3 hakoniwa/python/drone_config.py ${TEST_RESOURCE_PATH}/work/drone_config.json   ${config_path} "${new_value}"

    # DO TEST
    bash hako-start.bash ${TEST_RESOURCE_PATH}/configs/config.json ${TEST_RESOURCE_PATH}/work/drone_config.json ${TEST_SCENARIO}

    # EVALUATE TEST RESULTS
    python3 px4/auto-test/evaluation/eval_data_mean_stdev.py  px4/auto-test/evaluation_input/eval_height.json  ${TEST_RESOURCE_PATH}/logs \
            | tee ${TEST_RESOURCE_PATH}/eval/${result_name}.txt
}

#do_test components.droneDynamics.inertia '[0.015, 0.015, 0.03]' result01

# 以下はサンプルとしてのテストプログラムです。
for p in 0 1 2 3 4
do
    echo "INFO: START TEST PARAM=${p}"
    do_test components.droneDynamics.airFrictionCoefficient 0.${p} result_0${p}
    sleep 5
    echo "INFO: END TEST"
done

exit 0
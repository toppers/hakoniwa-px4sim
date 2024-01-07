#!/bin/bash

if [ $# -eq 1 ]
then
    export HAKO_CUSTOM_JSON_PATH=../config/custom_collision.json
fi

if [ ! -f cmake-build/src/hako-px4sim ]
then
    echo "ERROR: can not find ./src/hako-px4sim"
    exit 1
fi
OS_TYPE=`uname`
if [ ${OS_TYPE} = "Linux" ]
then
    uname -a | grep WSL2 > /dev/null
    if [ $? -eq 0 ]
    then
        bash run-win.bash
    else
        echo "ERROR: not supported OS: `uname -a`"
    fi
else
    bash run-mac.bash
fi


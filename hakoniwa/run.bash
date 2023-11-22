#!/bin/bash

cd cmake-build

if [ ! -f ./src/hako-px4sim ]
then
    echo "ERROR: can not find ./src/hako-px4sim"
    exit 1
fi
./src/hako-px4sim 127.0.0.1 4560 sim | tee log.txt
#./src/hako-px4sim 127.0.0.1 4560 bypass | tee log.txt

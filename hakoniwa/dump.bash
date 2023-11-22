#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Usage: $0 <capture path>"
    exit 1
fi

export HAKO_CAPTURE_SAVE_FILEPATH=${1}
./cmake-build/src/hako-px4sim 127.0.0.1 4560 replay_dump
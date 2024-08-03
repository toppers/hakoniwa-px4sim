#!/bin/bash
trap 'kill 0' EXIT

export HAKO_CUSTOM_JSON_PATH=../../hakoniwa-unity-drone-model/custom.json
export DRONE_CONFIG_PATH=config/rc

# start hakoniwa
cmake-build/src/hako-px4sim 127.0.0.1 4560 replay

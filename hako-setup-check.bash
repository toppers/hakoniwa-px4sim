#!/bin/bash

bash hakoniwa/third-party/hakoniwa-core-cpp-client/hako-setup-check.bash
if [ $? -ne 0 ]
then
    exit 1
fi

STATUS="OK"

commands=(
    "cmake"
    "jq"
    "python3"
)

OS=`uname`
if [ "$OS" = "Linux" ]; then
    LIB_EXT=".so"
    INC_DIR="/usr/include"
    ENV_PYTHONPATH="PYTHONPATH"
elif [ "$OS" = "Darwin" ]; then
    LIB_EXT=".dylib"
    INC_DIR="/opt/homebrew/include"
    ENV_PYTHONPATH="PYTHONPATH"
else
    LIB_EXT=".so"
    INC_DIR="/usr/include"
    ENV_PYTHONPATH="PYTHONPATH"
fi

env_paths=(
    "ENV_PYTHONPATH"
)

directories=(
    "${INC_DIR}/gtest"
    "drone_control/cmake-build/workspace"
    "drone_sensors/cmake-build/sensors"
)

files=(
    "hakoniwa/cmake-build/src/hako-px4sim"
)

for command in "${commands[@]}"; do
    which $command > /dev/null
    if [ $? -ne 0 ]; then
        echo "NG Missing command: $command"
        STATUS="NG"
    else
        echo "OK Command exists: $command"
    fi
done

for dir in "${directories[@]}"; do
    if [ ! -d "$dir" ]; then
        echo "NG Missing directory: $dir"
        STATUS="NG"
    else
        echo "OK Directory exists: $dir"
    fi
done

for file in "${files[@]}"; do
    if [ ! -e "$file" ]; then
        echo "NG Missing file: $file"
        STATUS="NG"
    else
        echo "OK File exists: $file"
    fi
done

for env_var_name in "${env_paths[@]}"; do
    actual_env_var_name=$(eval echo \$$env_var_name)
    echo "$actual_env_var_name: $(eval echo \$$actual_env_var_name)" | grep hakoniwa > /dev/null
    if [ $? -ne 0 ]; then
        echo "Missing $actual_env_var_name: hakoniwa..."
        exit 1
    else
        echo "OK Environment variable: $actual_env_var_name"
    fi
done

if [ $STATUS = "OK" ]
then
    echo "Check complete."
    exit 0
else
    echo "Check failed."
    exit 1
fi

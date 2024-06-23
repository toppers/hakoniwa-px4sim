#!/bin/bash

if [ $# -ne 3 ]
then
    echo "Usage: $0 <to-path> <hakoniwa-px4sim-out-path> <hakoniwa-unity-exe-path>"
    exit 1
fi

TO_DIR=${1}
PX4OUT_DIR=${2}
UNITY_EXE_DIR=${3}

if [ ! -d hakoniwa ]
then
    echo "ERROR: can not find directory: hakoniwa"
    echo "please execute this tool on top directory of hakoniwa-px4sim"
    exit 1
fi

if [ ! -d ${TO_DIR} ]
then
    echo "ERROR: can not find directory: ${TO_DIR}"
    exit 1
fi

if [ ! -d ${UNITY_EXE_DIR} ]
then
    echo "ERROR: can not find unity exe dir: ${UNITY_EXE_DIR}"
    exit 1
fi

if [ ! -f ${UNITY_EXE_DIR}/model.exe ]
then
    echo "ERROR: can not find unity exe: ${UNITY_EXE_DIR}"
    exit 1
fi

if [ -d ${TO_DIR}/hakoniwa-px4-win ]
then
    rm -rf  ${TO_DIR}/hakoniwa-px4-win/*
else
    mkdir ${TO_DIR}/hakoniwa-px4-win
fi

# README
cp tools/win/template/hakoniwa-px4-win/README* ${TO_DIR}/hakoniwa-px4-win/

mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa

# hakoniwa/DroneAppWin
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/DroneAppWin
cp -rp ${UNITY_EXE_DIR}/* ${TO_DIR}/hakoniwa-px4-win/hakoniwa/DroneAppWin/

# hakoniwa/apps
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/apps
cp -rp tools/win/template/hakoniwa-px4-win/hakoniwa/apps/* ${TO_DIR}/hakoniwa-px4-win/hakoniwa/apps/
cp drone_api/sample/camera.py ${TO_DIR}/hakoniwa-px4-win/hakoniwa/apps/
cp drone_api/sample/rc.py     ${TO_DIR}/hakoniwa-px4-win/hakoniwa/apps/
cp drone_api/sample/sample.py ${TO_DIR}/hakoniwa-px4-win/hakoniwa/apps/

# hakoniwa/bin
# hakoniwa/config
# hakoniwa/config_api
# hakoniwa/config_api2
# hakoniwa/py
# hakoniwa/out
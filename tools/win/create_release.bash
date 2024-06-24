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
OUT_WORKSPACE=build/x64-Release/drone_control/workspace
OUT_HAKOSRC=build/x64-Release/hakoniwa/src

if [ ! -f ${PX4OUT_DIR}/${OUT_WORKSPACE}/DroneAvator/DroneAvator.dll  ]
then
    echo "ERROR: can not find ${PX4OUT_DIR}/${OUT_WORKSPACE}/DroneAvator/DroneAvator.dll"
    exit 1
fi
if [ ! -f ${PX4OUT_DIR}/${OUT_WORKSPACE}/FlightController/FlightController.dll  ]
then
    echo "ERROR: can not find ${PX4OUT_DIR}/${OUT_WORKSPACE}/FlightController/FlightController.dll"
    exit 1
fi
if [ ! -f ${PX4OUT_DIR}/${OUT_WORKSPACE}/RadioController/RadioController.dll  ]
then
    echo "ERROR: can not find ${PX4OUT_DIR}/${OUT_WORKSPACE}/RadioController/RadioController.dll"
    exit 1
fi
if [ ! -f ${PX4OUT_DIR}/${OUT_HAKOSRC}/hako-px4sim.exe  ]
then
    echo "ERROR: can not find ${PX4OUT_DIR}/${OUT_HAKOSRC}/hako-px4sim.exe"
    exit 1
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
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/bin
cp -rp tools/win/template/hakoniwa-px4-win/hakoniwa/bin/* ${TO_DIR}/hakoniwa-px4-win/hakoniwa/bin/

# hakoniwa/config
# hakoniwa/config_api
# hakoniwa/config_api2

# hakoniwa/py
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/py
cp -rp hakoniwa/third-party/hakoniwa-core-cpp-client/bindings/python/hako_binary ${TO_DIR}/hakoniwa-px4-win/hakoniwa/py/
cp hakoniwa/third-party/hakoniwa-core-cpp-client/bindings/python/hako_pdu.py ${TO_DIR}/hakoniwa-px4-win/hakoniwa/py/
cp drone_api/libs/*.py ${TO_DIR}/hakoniwa-px4-win/hakoniwa/py/

# hakoniwa/out
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/build
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/build/x64-Release
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/build/x64-Release/drone_control
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/${OUT_WORKSPACE}

mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/${OUT_WORKSPACE}/DroneAvator
cp ${PX4OUT_DIR}/${OUT_WORKSPACE}/DroneAvator/DroneAvator.dll ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/${OUT_WORKSPACE}/DroneAvator/

mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/${OUT_WORKSPACE}/FlightController
cp ${PX4OUT_DIR}/${OUT_WORKSPACE}/FlightController/FlightController.dll ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/${OUT_WORKSPACE}/DroneAvator/

mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/${OUT_WORKSPACE}/RadioController
cp ${PX4OUT_DIR}/${OUT_WORKSPACE}/RadioController/RadioController.dll ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/${OUT_WORKSPACE}/RadioController/

mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/build/x64-Release/hakoniwa
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/${OUT_HAKOSRC}
cp -rp ${PX4OUT_DIR}/${OUT_HAKOSRC}/hako-px4sim.exe ${TO_DIR}/hakoniwa-px4-win/hakoniwa/out/${OUT_HAKOSRC}/

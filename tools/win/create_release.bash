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
cp drone_api/sample/rc-custom.py ${TO_DIR}/hakoniwa-px4-win/hakoniwa/apps/
cp drone_api/sample/return_to_home.py ${TO_DIR}/hakoniwa-px4-win/hakoniwa/apps/
cp -a drone_api/sample/rc_utils ${TO_DIR}/hakoniwa-px4-win/hakoniwa/apps/
cp -a drone_api/sample/rc_config ${TO_DIR}/hakoniwa-px4-win/hakoniwa/apps/
cp -a drone_api/sample/rc_debug ${TO_DIR}/hakoniwa-px4-win/hakoniwa/apps/


# hakoniwa/bin
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/bin
cp -rp tools/win/template/hakoniwa-px4-win/hakoniwa/bin/* ${TO_DIR}/hakoniwa-px4-win/hakoniwa/bin/
rm ${TO_DIR}/hakoniwa-px4-win/hakoniwa/bin/run-api*
rm ${TO_DIR}/hakoniwa-px4-win/hakoniwa/bin/run-win.bat

# hakoniwa/config_px4
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/config_px4
cp tools/win/template/hakoniwa-px4-win/hakoniwa/config/cpp_core_config.json ${TO_DIR}/hakoniwa-px4-win/hakoniwa/config_px4/
#cp tools/win/template/hakoniwa-px4-win/hakoniwa/config/custom.json ${TO_DIR}/hakoniwa-px4-win/hakoniwa/config_px4/
cp tools/win/template/hakoniwa-px4-win/hakoniwa/config/drone_config_0.json ${TO_DIR}/hakoniwa-px4-win/hakoniwa/config_px4/

# hakoniwa/config_rc
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/config_rc
cp tools/win/template/hakoniwa-px4-win/hakoniwa/config/cpp_core_config.json ${TO_DIR}/hakoniwa-px4-win/hakoniwa/config_rc/
#cp tools/win/template/hakoniwa-px4-win/hakoniwa/config/custom.json ${TO_DIR}/hakoniwa-px4-win/hakoniwa/config_rc/
jq '.controller.moduleDirectory = "..\\..\\out\\build\\x64-Release\\drone_control\\workspace\\RadioController"' \
    hakoniwa/config/rc/drone_config_0.json > ${TO_DIR}/hakoniwa-px4-win/hakoniwa/config_rc/drone_config_0.json

# hakoniwa/config_fc
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/config_fc
cp tools/win/template/hakoniwa-px4-win/hakoniwa/config/cpp_core_config.json ${TO_DIR}/hakoniwa-px4-win/hakoniwa/config_fc/
#cp tools/win/template/hakoniwa-px4-win/hakoniwa/config/custom.json ${TO_DIR}/hakoniwa-px4-win/hakoniwa/config_fc/
jq '.controller.moduleDirectory = "..\\..\\out\\build\\x64-Release\\drone_control\\workspace\\FlightController" ' \
    hakoniwa/config/api_sample/drone_config_0.json > ${TO_DIR}/hakoniwa-px4-win/hakoniwa/config_fc/drone_config_0.json

# drone PID config
mkdir -p ${TO_DIR}/hakoniwa-px4-win/hakoniwa/drone_control/config
cp drone_control/config/param-api-mixer.txt ${TO_DIR}/hakoniwa-px4-win/hakoniwa/drone_control/config/

# hakoniwa/py
mkdir  ${TO_DIR}/hakoniwa-px4-win/hakoniwa/py
cp -rp hakoniwa/third-party/hakoniwa-core-cpp-client/bindings/python/hako_binary ${TO_DIR}/hakoniwa-px4-win/hakoniwa/py/
cp hakoniwa/third-party/hakoniwa-core-cpp-client/bindings/python/hako_pdu.py ${TO_DIR}/hakoniwa-px4-win/hakoniwa/py/
cp drone_api/libs/*.py ${TO_DIR}/hakoniwa-px4-win/hakoniwa/py/

# hakopy
wget https://github.com/toppers/hakoniwa-core-cpp-client/releases/download/v2.0.0/hako_asset_python.dll
mv hako_asset_python.dll ${TO_DIR}/hakoniwa-px4-win/hakoniwa/py/hakopy.pyd

# out
mkdir  ${TO_DIR}/hakoniwa-px4-win/out
mkdir  ${TO_DIR}/hakoniwa-px4-win/out/build
mkdir  ${TO_DIR}/hakoniwa-px4-win/out/build/x64-Release
mkdir  ${TO_DIR}/hakoniwa-px4-win/out/build/x64-Release/drone_control
mkdir  ${TO_DIR}/hakoniwa-px4-win/out/${OUT_WORKSPACE}

mkdir  ${TO_DIR}/hakoniwa-px4-win/out/${OUT_WORKSPACE}/FlightController
cp ${PX4OUT_DIR}/${OUT_WORKSPACE}/FlightController/FlightController.dll ${TO_DIR}/hakoniwa-px4-win/out/${OUT_WORKSPACE}/FlightController/

mkdir  ${TO_DIR}/hakoniwa-px4-win/out/${OUT_WORKSPACE}/RadioController
cp ${PX4OUT_DIR}/${OUT_WORKSPACE}/RadioController/RadioController.dll ${TO_DIR}/hakoniwa-px4-win/out/${OUT_WORKSPACE}/RadioController/

mkdir  ${TO_DIR}/hakoniwa-px4-win/out/build/x64-Release/hakoniwa
mkdir  ${TO_DIR}/hakoniwa-px4-win/out/${OUT_HAKOSRC}
cp -rp ${PX4OUT_DIR}/${OUT_HAKOSRC}/hako-px4sim.exe ${TO_DIR}/hakoniwa-px4-win/out/${OUT_HAKOSRC}/


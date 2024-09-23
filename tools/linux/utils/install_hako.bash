#!/bin/bash

CURR_DIR=`pwd`
HAKO_CORE_BUILD_DIR=hakoniwa-px4sim/hakoniwa/cmake-build/third-party/hakoniwa-core-cpp-client

# install hakoniwa
cp ${HAKO_CORE_BUILD_DIR}/core/sample/base-procs/hako-cmd/hako-cmd ${ROOT_DIR}/usr/local/bin/hakoniwa/
cp ${HAKO_CORE_BUILD_DIR}/src/proxy/hako-proxy ${ROOT_DIR}/usr/local/bin/hakoniwa/
cp ${HAKO_CORE_BUILD_DIR}/src/hakoc/libhakoarun.* ${ROOT_DIR}/usr/local/lib/hakoniwa/
cp ${HAKO_CORE_BUILD_DIR}/src/hakoc/libshakoc.* ${ROOT_DIR}/usr/local/lib/hakoniwa/
cp ${HAKO_CORE_BUILD_DIR}/src/hakoc/libshakoc.* ${ROOT_DIR}/usr/local/lib/hakoniwa/hakoc.so
cp ${HAKO_CORE_BUILD_DIR}/src/assets/libassets.* ${ROOT_DIR}/usr/local/lib/hakoniwa/
cp ${HAKO_CORE_BUILD_DIR}/src/conductor/libconductor.* ${ROOT_DIR}/usr/local/lib/hakoniwa/
cp ${HAKO_CORE_BUILD_DIR}/bindings/python/libhako_asset_python.so ${ROOT_DIR}/usr/local/lib/hakoniwa/py/hakopy.so

cp hakoniwa-px4sim/hakoniwa/third-party/hakoniwa-core-cpp-client/src/include/*.h ${ROOT_DIR}/usr/local/include/hakoniwa/
cp -rp hakoniwa-px4sim/hakoniwa/third-party/hakoniwa-core-cpp-client/bindings/python/hako_binary/offset ${ROOT_DIR}/usr/local/lib/hakoniwa/hako_binary/
cp -rp hakoniwa-px4sim/hakoniwa/third-party/hakoniwa-core-cpp-client/bindings/python/hako_binary/*.py ${ROOT_DIR}/usr/local/lib/hakoniwa/py/hako_binary/
cp     hakoniwa-px4sim/hakoniwa/third-party/hakoniwa-core-cpp-client/bindings/python/hako_pdu.py ${ROOT_DIR}/usr/local/lib/hakoniwa/py/

# ディレクトリのパーミッションを適切に設定
chmod -R 755 ${ROOT_DIR}/var/lib/hakoniwa

# install hako-px4sim
cp -rp hakoniwa-px4sim/hakoniwa/third-party/hakoniwa-ros2pdu/pdu/offset  ${ROOT_DIR}/usr/local/lib/hakoniwa/hako_binary/
cp hakoniwa-px4sim/hakoniwa/cmake-build/src/hako-px4sim ${ROOT_DIR}/usr/local/bin/hakoniwa/

# install drone_api
cp hakoniwa-px4sim/drone_api/libs/*.py ${ROOT_DIR}/usr/local/lib/hakoniwa/py/

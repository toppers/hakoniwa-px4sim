#!/bin/bash

CURR_DIR=`pwd`
cd third-party/hakoniwa-core-cpp-client

bash build.bash
bash install.bash
cd ${CURR_DIR}
sudo cp -rp third-party/hakoniwa-ros2pdu/pdu/offset /usr/local/lib/hakoniwa/hako_binary/
sudo bash third-party/hakoniwa-core-cpp-client/hako-mmap-set.bash -p /var/lib/hakoniwa/mmap


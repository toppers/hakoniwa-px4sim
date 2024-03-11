#!/bin/bash


cd third-party/hakoniwa-core-cpp-client

bash build.bash
bash install.bash
sudo bash third-party/hakoniwa-core-cpp-client/hako-mmap-set.bash -p /var/lib/hakoniwa/mmap


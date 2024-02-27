#!/bin/bash

cd cmake-build
echo "./src/hako-px4sim 127.0.0.1 $((4560+${MULTI_INSTANCE_ID})) sim"
./src/hako-px4sim 127.0.0.1 $((4560+${MULTI_INSTANCE_ID})) sim | tee log.txt

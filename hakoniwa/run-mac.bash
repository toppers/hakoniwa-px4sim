#!/bin/bash

cd cmake-build
./src/hako-px4sim 127.0.0.1 4560 sim | tee log.txt
#./src/hako-px4sim 127.0.0.1 4560 bypass | tee log.txt

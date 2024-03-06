#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Usage: $0 <instance-num>"
    exit 1
fi
I_NUM=${1}

cd cmake-build
cmake ..
make

COUNT=1
while [ $COUNT -lt $I_NUM ]
do
    cp workspace/DroneFlightController//libDroneFlightController.so workspace/DroneFlightController//libDroneFlightController${COUNT}.so
    COUNT=`expr $COUNT + 1`
done

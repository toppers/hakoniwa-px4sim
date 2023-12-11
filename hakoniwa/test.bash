#!/bin/bash

OS_TYPE=`uname`

cd cmake-build
if [ ${OS_TYPE} = "Linux" ]
then
    uname -a | grep WSL2 > /dev/null
    if [ $? -eq 0 ]
    then
        cmake .. -D HAKO_CLIENT_OPTION_FILEPATH=`pwd`/../win-cmake-options.cmake -DDO_TEST=true
    else
        cmake .. -D HAKO_CLIENT_OPTION_FILEPATH=`pwd`/../linux-cmake-options.cmake -DDO_TEST=true
    fi
else
    cmake ..  -DDO_TEST=true
fi
make
./test/hako-px4sim-test

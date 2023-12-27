#!/bin/bash

OS_TYPE=`uname`

cd cmake-build
if [ ${OS_TYPE} = "Linux" ]
then
    uname -a | grep WSL2 > /dev/null
    if [ $? -eq 0 ]
    then
        cmake .. -D DO_TEST=true -D HAKO_CLIENT_OPTION_FILEPATH=`pwd`/../../cmake-options/win-cmake-options.cmake
    else
        cmake .. -D DO_TEST=true -D HAKO_CLIENT_OPTION_FILEPATH=`pwd`/../../cmake-options/linux-cmake-options.cmake
    fi
else
    cmake .. -D DO_TEST=true
fi
make

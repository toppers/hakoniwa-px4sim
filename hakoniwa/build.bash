#!/bin/bash

for arg in "$@"
do
    ADDITIONAL_CMAKE_OPTIONS="$ADDITIONAL_CMAKE_OPTIONS -D $arg"
done

OS_TYPE=`uname`

cd cmake-build
if [ ${OS_TYPE} = "Linux" ]
then
    uname -a | grep WSL2 > /dev/null
    if [ $? -eq 0 ]
    then
        cmake .. -D HAKONIWA_BUILD=true -D DO_TEST=true -D HAKO_CLIENT_OPTION_FILEPATH=`pwd`/../../cmake-options/win-cmake-options.cmake $ADDITIONAL_CMAKE_OPTIONS
    else
        cmake .. -D HAKONIWA_BUILD=true -D DO_TEST=true -D HAKO_CLIENT_OPTION_FILEPATH=`pwd`/../../cmake-options/linux-cmake-options.cmake $ADDITIONAL_CMAKE_OPTIONS
    fi
else
    cmake .. -D HAKONIWA_BUILD=true -D DO_TEST=true $ADDITIONAL_CMAKE_OPTIONS
fi
make

exit $?
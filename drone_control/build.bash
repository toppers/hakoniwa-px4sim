#!/bin/bash

if [ -d cmake-build ]
then
    :
else
    mkdir cmake-build
fi
cd cmake-build
cmake ..
make


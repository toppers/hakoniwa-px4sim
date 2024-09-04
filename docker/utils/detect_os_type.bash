#!/bin/bash

if [[ "$(uname -r)" == *microsoft* ]]
then
    OS_TYPE=wsl2
elif [[ "$(uname)" == "Darwin" ]]
then
    OS_TYPE=Mac
else
    OS_TYPE=Linux
fi

echo ${OS_TYPE}

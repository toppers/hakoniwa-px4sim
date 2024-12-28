#!/bin/bash

MULTI_INSTANCE_ID=
if [ $# -eq 1 ]
then
    MULTI_INSTANCE_ID=$1
fi

ARCH=`arch`
OS_TYPE=`uname`
WIN_WSL="OFF"
uname -a | grep microsoft
if [ $? -eq 0 ]
then
    WIN_WSL="ON"
fi
export PX4_HOME_LAT=47.641468
export PX4_HOME_LON=-122.140165
export PX4_HOME_ALT=0.0
export PX4_SIM_HOSTNAME=127.0.0.1

if [ ${OS_TYPE} = "Linux" ]
then
    if [ ${WIN_WSL} = "ON" ]
    then
        RESOLV_IPADDR=`cat /etc/resolv.conf  | grep nameserver | awk '{print $NF}'`
        NETWORK_INTERFACE=$(route | grep '^default' | grep -o '[^ ]*$' | tr -d '\n')
        CORE_IPADDR=$(ifconfig "${NETWORK_INTERFACE}" | grep netmask | awk '{print $2}')
        export PX4_SIM_HOSTNAME=${CORE_IPADDR}
    fi
fi

if [ -z ${MULTI_INSTANCE_ID} ]
then
    echo "INFO: SINGLE INSTANCE MODE"
    make px4_sitl none_iris
else
    echo "INFO: MULTI INSTANCE MODE"
     PX4_SIM_MODEL=none_iris ./build-${MULTI_INSTANCE_ID}/px4_sitl_default/bin/px4 -i ${MULTI_INSTANCE_ID}
fi

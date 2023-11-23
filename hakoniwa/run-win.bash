#!/bin/bash

HAKO_MASTER_PID=
function signal_handler()
{
    echo "TRAPPED"
}

function kill_process()
{
    echo "INFO: STOPPING Simulation"
    if [ -z "$HAKO_MASTER_PID" ]
    then
        exit 0
    fi
    echo "INFO: killing HAKO_MASTER_PID: $HAKO_MASTER_PID"
    kill -9 $HAKO_MASTER_PID

    while [ 1 ]
    do
        NUM=$(ps aux | grep "hako-master" | grep -v grep | wc -l)
        if [ $NUM -eq 0 ]
        then
            break
        fi
        echo "INFO: waiting hako-master is killed"
        sleep 1
    done
    echo "INFO: DONE simulation"
}
trap signal_handler SIGINT

export PATH=/usr/local/bin/hakoniwa:${PATH}
export LD_LIBRARY_PATH=/usr/local/lib/hakoniwa:${LD_LIBRARY_PATH}

UDP_SRV_PORT=54001
UDP_SND_PORT=54002
GRPC_PORT=50051
MQTT_PORT=
echo "INFO: ACTIVATING HAKO-CONDUCTOR"
hako-master-rust ${DELTA_MSEC} ${MAX_DELAY_MSEC} ${CORE_IPADDR}:${GRPC_PORT} ${UDP_SRV_PORT} ${UDP_SND_PORT} ${MQTT_PORT} & 
HAKO_MASTER_PID=$!

sleep 1

echo "INFO: ACTIVATING hako-px4sim"
cd cmake-build
./src/hako-px4sim ${CORE_IPADDR} 4560 sim

echo "INFO: NOW STOPPING SIMULATION" 

kill_process
echo "INFO: If you find that your keyboard input is not being displayed correctly, "
echo "      try executing the 'reset' command to resolve the issue."

exit 0

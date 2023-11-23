#!/bin/bash

export PATH=/usr/local/bin/hakoniwa:${PATH}
export LD_LIBRARY_PATH=/usr/local/lib/hakoniwa:${LD_LIBRARY_PATH}

UDP_SRV_PORT=54001
UDP_SND_PORT=54002
GRPC_PORT=50051
MQTT_PORT=
echo "INFO: ACTIVATING HAKO-CONDUCTOR"
hako-master ${DELTA_MSEC} ${MAX_DELAY_MSEC} ${CORE_IPADDR}:${GRPC_PORT} ${UDP_SRV_PORT} ${UDP_SND_PORT} ${MQTT_PORT} & 

sleep 1

echo "INFO: ACTIVATING hako-px4sim"
cd cmake-build
./src/hako-px4sim ${CORE_IPADDR} 4560 sim

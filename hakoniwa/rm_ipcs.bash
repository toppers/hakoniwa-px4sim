#!/bin/bash

for key in 0x000000ff 0x00000100 
do
    echo "REMOVING RESOURCES: ${key}"
    ipcs -s | grep $key | awk '{print $2}' | xargs ipcrm -s
    ipcs -m | grep $key | awk '{print $2}' | xargs ipcrm -m
done

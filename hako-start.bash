#!/bin/bash

# プロセス終了のハンドラ
terminate_processes() {
    echo "INFO: Stopping processes..."
    for pid in ${pids[@]}; do
        echo "kill -TERM "$pid""
        kill -TERM "$pid"
    done
    wait
    echo "INFO: All processes stopped."
    exit
}

# SIGTERM と SIGINT のシグナルハンドラを設定
trap 'terminate_processes' TERM INT

# バックグラウンドプロセスのPIDを格納する配列
declare -a pids

# run hakoniwa
CURR_DIR=`pwd`
echo "INFO: Activating hakoniwa"
cd hakoniwa/cmake-build
./src/hako-px4sim 127.0.0.1 4560 asim &
pids+=($!)
cd ${CURR_DIR}

sleep 2

# run px4
echo "INFO: Activating PX4"
cd px4/PX4-Autopilot
bash ../sim/simstart.bash &
pids+=($!)
cd ${CURR_DIR}

sleep 2

hako-cmd start

# 全てのバックグラウンドプロセスの終了を待つ
wait

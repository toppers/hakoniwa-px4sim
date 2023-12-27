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

# 引数のチェック
if [ "$#" -ne 3 -a "$#" -ne 4 ]; then
    echo "Usage: $0 <config.json> <drone_config.json> <scenario.json> [unity]"
    exit 1
fi

# 引数から設定ファイルのパスを取得
config_json=$1
drone_config_json=$2
scenario_json=$3
use_unity="OFF"
if [ $# -eq 4 ]
then
    use_unity="ON"
fi

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

if [ "$use_unity" = "ON" ]
then
    echo "Please start Unity and enter key"
    read
fi
sleep 1
echo "INFO: start simulation"

hako-cmd start


# run test scenario

python3 px4/auto-test/hako_QgcStub.py "$config_json" "$drone_config_json" "$scenario_json" &
pids+=($!)

# 全てのバックグラウンドプロセスの終了を待つ
wait

#!/bin/bash

# シミュレーション実行時間
SIMULATION_TIME=3

# 初期集団を生成
python python/adjust_pid_params.py --generate_initial_population > population.txt

# シミュレーションと評価のループ
for i in {1..10}; do
    echo "Generation $i"

    # 現在の集団から各個体を取り出しシミュレーションを実行
    while IFS= read -r line; do
        Kp=$(echo $line | awk '{print $1}')
        Ki=$(echo $line | awk '{print $2}')
        Kd=$(echo $line | awk '{print $3}')
        echo "Individual: Kp=$Kp, Ki=$Ki, Kd=$Kd"
        
        # PIDパラメータファイルの更新
        echo "DroneAvator_Kp=$Kp" > drone_control_params.txt
        echo "DroneAvator_Ki=$Ki" >> drone_control_params.txt
        echo "DroneAvator_Kd=$Kd" >> drone_control_params.txt

        sleep 1

        # シミュレーションの開始
        hako-cmd start
        
        # 指定時間待機
        sleep $SIMULATION_TIME
        
        # シミュレーションの停止
        hako-cmd stop
        
        # シミュレーション結果の評価
        python python/control_evaluate.py ./drone_log0/drone_dynamics.csv > evaluation_result.txt
        cat evaluation_result.txt

        # 評価結果に基づき新しい集団を生成
        python python/adjust_pid_params.py evaluation_result.txt drone_control_params.txt > new_population.txt

        # 集団を更新
        mv new_population.txt population.txt
        
        # シミュレーションのリセット
        hako-cmd reset

    done < population.txt
done

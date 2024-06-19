#!/bin/bash

# シミュレーション実行時間
SIMULATION_TIME=5

# 初期集団サイズ
POPULATION_SIZE=12  # 必要に応じて変更
DRONE_NUM=4
LOOP_NUM=$((POPULATION_SIZE / DRONE_NUM))

# 初期集団を生成
for k in $(seq 1 $LOOP_NUM); do
    python python/genetic_algorithm.py --generate_initial_population $DRONE_NUM > population${k}.txt
done

# シミュレーションと評価のループ
for i in {1..10}; do
    echo "Generation $i"

    # 現在の集団から各個体を取り出しシミュレーションを実行
    rm -f fitness_values.txt
    rm -f drone_control_params_all.txt
    n=0
    for k in $(seq 1 $LOOP_NUM); do
        rm -f drone_control_params.txt
        index=0
        while IFS= read -r line; do
            Kp=$(echo $line | awk '{print $1}')
            Ki=$(echo $line | awk '{print $2}')
            Kd=$(echo $line | awk '{print $3}')
            echo "Individual: Kp=$Kp, Ki=$Ki, Kd=$Kd"
            
            # PIDパラメータファイルの更新
            echo "DroneAvator${index}_Kp=$Kp" >> drone_control_params.txt
            echo "DroneAvator${index}_Ki=$Ki" >> drone_control_params.txt
            echo "DroneAvator${index}_Kd=$Kd" >> drone_control_params.txt

            echo "DroneAvator${n}_Kp=$Kp" >> drone_control_params_all.txt
            echo "DroneAvator${n}_Ki=$Ki" >> drone_control_params_all.txt
            echo "DroneAvator${n}_Kd=$Kd" >> drone_control_params_all.txt
            index=$((index + 1))
            n=$((n + 1))
        done < population${k}.txt

        sleep 1

        # 各個体のシミュレーションの開始
        hako-cmd start
        
        # 指定時間待機
        sleep $SIMULATION_TIME
        
        # シミュレーションの停止
        hako-cmd stop
        # 各個体の評価結果を収集
        index=0
        while IFS= read -r line; do
            echo "#### index = $index #####"
            # シミュレーション結果の評価
            python python/evaluate_individual.py ./drone_log${index}/drone_dynamics.csv fitness_values.txt

            python python/control_evaluate.py ./drone_log${index}/drone_dynamics.csv
            index=$((index + 1))
        done < population${k}.txt

        # シミュレーションのリセット
        hako-cmd reset
    done

    # 評価結果に基づき新しい集団を生成
    python python/genetic_algorithm.py fitness_values.txt drone_control_params_all.txt > new_population.txt

    # 集団を分割して更新
    split -l $((POPULATION_SIZE / LOOP_NUM)) new_population.txt new_population_

    suffixes=(aa ab ac ad)
    for k in $(seq 0 $((LOOP_NUM - 1))); do
        mv new_population_${suffixes[$k]} population$((k + 1)).txt
    done


done

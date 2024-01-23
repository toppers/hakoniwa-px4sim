# GQC <-> PX4 のセンサーキャリブレーションとアクチュエータの設定

[PX4のページ](https://docs.px4.io/main/en/config/)を参照のこと．

ここに，QGCを使った「機体」「センサー」「アクチュエータ」の「セットアップ」について書かれている．セットアップには，「ファームウェア」，「ジョイスティック」，「バッテリー」なども含まれるが，センサーとアクチュエーターについて抜き出す．

- セットアップ
    - 形状(Airframe)
       ![形状](https://docs.px4.io/main/assets/img/airframe_px4.3c810c47.jpg)
    - センサー向き(Sensor Orientation)
        ![センサー全体の向き](https://docs.px4.io/main/assets/img/yaw_rotation.c0ff8b97.png)
    - コンパスのキャリブレーション(Compass)
        ![コンパス](https://docs.px4.io/main/assets/img/sensor_compass_calibrate_px4.a111cc6c.jpg)
    - ジャイロのキャリブレーション(Gyroscope)
        ![ジャイロ](https://docs.px4.io/main/assets/img/gyroscope_calibrate_progress_px4.67d944fd.jpg)
    - 加速度計のキャリブレーション(Accelerometer)
        ![加速度計](https://docs.px4.io/main/assets/img/accelerometer_positions_px4.94b81dab.jpg)
    - ESC(Electric Speed Controller)バッテリー
    - アクチュエータの位置情報設定(Actualtors)
        ![アクチュエータ](https://docs.px4.io/main/assets/img/qgc_actuators_mc_aux.d310bb18.png)
    - モータの出力(Motor Configuration)
        ![モーター](https://docs.px4.io/main/assets/img/identify_motors_in_progress.5ba3f3bb.png)
    


|対象 |(英語) | 関連定数 | 手法|補足|
|-----|----- |--------------|---------|----|
|機体 ![形状](https://docs.px4.io/main/assets/img/airframe_px4.3c810c47.jpg)  | Airframe | - | パネルUI ||
|センサー向き ![センサー全体](https://docs.px4.io/main/assets/img/yaw_rotation.c0ff8b97.png) Sensor Orientation | - | パネルUI |センサーボード全体|
|コンパス  ![コンパス](https://docs.px4.io/main/assets/img/sensor_compass_calibrate_px4.a111cc6c.jpg) | Compass | mag | 機体を各方向に回す |地上と機体姿勢の関係認識 |
|ジャイロ  ![ジャイロ](https://docs.px4.io/main/assets/img/gyroscope_calibrate_progress_px4.67d944fd.jpg) | Gyroscope | gyro($\phi,\theta$) | 正姿勢静止させる |姿勢の認識|
|加速度計 ![加速度計](https://docs.px4.io/main/assets/img/accelerometer_positions_px4.94b81dab.jpg) | Accelerometer | gのかかる($\phi,\theta,\psi$) | 各方向に向かせて静止 |重力加速度方向の機体での方向認識|
|アクチュエータ ![アクチュエータ](https://docs.px4.io/main/assets/img/qgc_actuators_mc_aux.d310bb18.png) | Actuators | ロータ位置など| パネル設定と実際に- | |
|モーター ![モーター](https://docs.px4.io/main/assets/img/identify_motors_in_progress.5ba3f3bb.png) | Motor Configuration | モーター電圧と回転 | パネル設定と実際に回転させる | $Kr, Tr$ ? |


なお，[オートチューン](https://docs.px4.io/main/en/config/autotune.html)には，「実際にある程度安定飛行できる状態であれば」オートチューニングがフライト中に行われると書かれている（auto-tune enable/disable スイッチがある）．

また，オートチューン手順は，

1. pre-tuning テストを行う（安定飛行できる状態であることを確認する）
1. RC control でテイクオフする
 1.1 マルチコプターの場合は，リモコンで altitude モードでテイクオフする．ホバー状態にする(4m から 20m の高度で)．
1. オートチューンを有効にする．TIP: オートチューンスイッチがあれば，それを有効にする．
1.1  QGC では，Vehicle setup > PID Tuning > Autotune Enabled で有効にする．
1.2 Rate Controller と Attitude Controller のタブを選択する．
1.3 Autotune Enabled ボタンを有効になっていることを確認する（これにより，Autotune ボタンが表示され，手動チューニングセレクタが削除される）．
1.4 Popup を読み，OK をクリックする．
1. ドローンは最初に roll ，その後 pitch, yaw の動きを行う．
1. チューニングを適用する．
1.1 マニュアルで着陸すると，新しいチューニングパラメータが適用される．その後，再度離陸して安定していることを確認．
1. 激しく振動したりした場合，トラブルシューティングへ．

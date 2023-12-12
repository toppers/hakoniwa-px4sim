# PX4とAirsmの間で流れるMAVLINKデータの解析結果

|送信元|メッセージ|送信周期|
|---|---|---|
|PX4|COMMAND_LONG|1回のみ|
|PX4|HEARTBEAT|1回のみ|
|AirSim|COMMAND_LONG|1回のみ|
|AirSim|HIL_SENSOR|3msec|
|PX4|HIL_ACTUATOR_CONTROLS|3msec|
|AirSim|SYSTEM_TIME|3msec|
|AirSim|HIL_GPS|21msec|

# 通信シーケンス

|PX4|メッセージ|AirSim|
|---|---|---|
|=>|COMMAND_LONG|>=|
|=>|HEARTBEAT|>=|
|=<|COMMAND_LONG|<=|
|=<|HIL_SENSOR|<=|
|=<|SYSTEM_TIME|<=|
|=<|HIL_GPS|<=|
|=<|:|<=|
|=>|HIL_ACTUATOR_CONTROLS|>=|
|=<|HIL_SENSOR|<=|
|=<|SYSTEM_TIME|<=|
|=<|HIL_GPS|<=|
|=<|:|<=|
|=>|HIL_ACTUATOR_CONTROLS|>=|
|=<|HIL_SENSOR|<=|
|=<|SYSTEM_TIME|<=|
|=<|HIL_GPS|<=|


# 通信データの代表値

## PX4

### COMMAND_LONG

```
Message Owner: Controller: 0
Decoded MAVLink message:
  Message ID: 76
  System ID: 1
  Component ID: 1
  Sequence: 0
  Message.type: 2
  Type: COMMAND_LONG
  Target system: 0
  Target component: 0
  Command ID: 511
  Confirmation: 0
  Param1: 115
  Param2: 5000
  Param3: 0
  Param4: 0
  Param5: 0
  Param6: 0
  Param7: 0
```


### HEARTBEAT

```
Message Owner: Controller: 0
Decoded MAVLink message:
  Message ID: 0
  System ID: 1
  Component ID: 1
  Sequence: 1
  Message.type: 1
  Type: HEARTBEAT
  Base mode: 0
  Custom mode: 0
  System status: 0
  MAVLink version: 3
```

### HIL_ACTUATOR_CONTROLS

```
Average of TIME_diff: 3002.7064792305255 μs
Standard Deviation of TIME_diff: 124.62584573916502 μs
```


静止状態：


```
Message Owner: Controller: 0
Decoded MAVLink message:
  Message ID: 93
  System ID: 1
  Component ID: 1
  Sequence: 2
  Message.type: 8
  Type: HIL_ACTUATOR_CONTROLS
  Time_usec: 1700626138321025
  Mode: 1
  Flags: 1
  Controls: 0 0 0 0 0 0 0 0 
```

ホバリング状態：

```
Message Owner: Controller: 0
Decoded MAVLink message:
  Message ID: 93
  System ID: 1
  Component ID: 1
  Sequence: 1
  Message.type: 8
  Type: HIL_ACTUATOR_CONTROLS
  Time_usec: 1700626174495797
  Mode: 129
  Flags: 1
  Controls: 0.535 0.528 0.454 0.447 0 0 0 0 
```

#### controls[0-3]

<img width="878" alt="スクリーンショット 2023-12-13 6 56 40" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/c083ec6e-d45e-42e3-a3f8-4bc89bcc672c">

#### mode

<img width="869" alt="スクリーンショット 2023-12-13 6 58 33" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/1cd8e7d0-9522-4756-a081-d1e46bc1f173">

#### flags

<img width="877" alt="スクリーンショット 2023-12-13 6 59 07" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/c31979cc-c327-4ee8-be2f-efd76a38aa07">


## AirSim

### COMMAND_LONG

```
Message Owner: Physics: 1
Decoded MAVLink message:
  Message ID: 76
  System ID: 142
  Component ID: 42
  Sequence: 0
  Message.type: 2
  Type: COMMAND_LONG
  Target system: 1
  Target component: 1
  Command ID: 520
  Confirmation: 1
  Param1: 0
  Param2: 0
  Param3: 0
  Param4: 0
  Param5: 0
  Param6: 0
  Param7: 0
```

### SYSTEM_TIME

```
Message Owner: Physics: 1
Decoded MAVLink message:
  Message ID: 2
  System ID: 142
  Component ID: 42
  Sequence: 2
  Message.type: 6
  Type: SYSTEM_TIME
  Unix time: 1700626203511011
  Boot time: 4114056359
```

### HIL_SENSOR

```
Average of TIME_diff: 2999.958494548013 μs
Standard Deviation of TIME_diff: 17.794393454408848 μs
```


静止状態：

```
Message Owner: Physics: 1
Decoded MAVLink message:
  Message ID: 107
  System ID: 142
  Component ID: 42
  Sequence: 1
  Message.type: 4
  Type: HIL_SENSOR
  Time stamp: 1700626203510982
  Xacc: 0.0279299
  Yacc: -0.00230983 
  Zacc: -9.8091
  Xgyro: 0.00116225
  Ygyro: -0.000402902
  Zgyro: -0.00136381
  Xmag: 0.249532
  Ymag: 0.0363823
  Zmag: 0.371417
  Abs_pressure: 998.718
  Diff_pressure: 0
  Pressure_Alt: 121.626
  temparature: 0
  fileds_updated: 2147483648
 id: 0x0 
```

ホバリング状態：

```
Message Owner: Physics: 1
Decoded MAVLink message:
  Message ID: 107
  System ID: 142
  Component ID: 42
  Sequence: 217
  Message.type: 4
  Type: HIL_SENSOR
  Time stamp: 1700626259177379
  Xacc: 0.0369189
  Yacc: -0.0343275
  Zacc: -9.80304
  Xgyro: -0.00549784
  Ygyro: -0.00566039
  Zgyro: 0.00185995
  Xmag: -0.0137779
  Ymag: -0.250163
  Zmag: 0.3654
  Abs_pressure: 997.667
  Diff_pressure: 0
  Pressure_Alt: 130.476
  temparature: 0
  fileds_updated: 7167
 id: 0x0

```
#### [xyz]acc

<img width="917" alt="スクリーンショット 2023-12-13 7 02 17" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/a2a5ee2a-ec68-4770-b498-d93f09becb2c">

<img width="894" alt="スクリーンショット 2023-12-13 7 03 18" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/413e05aa-dba7-458f-bfd3-37c630ca84ba">

#### [xyz]gyro

<img width="911" alt="スクリーンショット 2023-12-13 7 04 03" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/69cdca58-4e16-4910-99f3-59b3003ff7ee">

#### [xyz]mag

<img width="873" alt="スクリーンショット 2023-12-13 7 04 38" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/1cdfab44-83ac-40e9-b1c9-9a0e439545d8">

### preassure

<img width="880" alt="スクリーンショット 2023-12-13 7 05 46" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/12f1b963-a5c5-495c-831d-ea04c08e8c81">

<img width="900" alt="スクリーンショット 2023-12-13 7 06 16" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/15565a70-ea78-4fc8-b44f-aa85d4ee554b">

<img width="900" alt="スクリーンショット 2023-12-13 7 06 46" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/a614321d-8bb5-4c31-b13e-19af321397db">


### HIL_GPS


```
Average of TIME_diff: 20992.981333997013 μs
Standard Deviation of TIME_diff: 360.4148943809385 μs
```

静止状態：

```
Message Owner: Physics: 1
Decoded MAVLink message:
  Message ID: 113
  System ID: 142
  Component ID: 42
  Sequence: 3
  Message.type: 7
  Type: HIL_GPS 
  Time stamp: 1700626203511030
  fix_type: 0x3
  Latitude: 476414679
  Longitude: -1221401649
  alt: 121321
  eph: 129
  epv: 129
  vn: 0
  vel: 0
  vd: 0
  cog: 0
  id: 0x0
  yaw: 0
```

ホバリング状態：

```
Message Owner: Physics: 1
Decoded MAVLink message:
  Message ID: 113
  System ID: 142
  Component ID: 42
  Sequence: 212 
  Message.type: 7
  Type: HIL_GPS 
  Time stamp: 1700626259168393
  fix_type: 0x3
  Latitude: 476414531
  Longitude: -1221395265
  alt: 130551
  eph: 10 
  epv: 10
  vn: -1
  vel: 4
  vd: 0
  cog: 24593
  id: 0x0
```

### lat, long, alt

<img width="886" alt="スクリーンショット 2023-12-13 7 09 28" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/27132984-0728-4e69-9192-8a045ad1c14e">

<img width="884" alt="スクリーンショット 2023-12-13 7 09 51" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/26744c75-962c-4394-94a3-169e3db0b9fa">

<img width="906" alt="スクリーンショット 2023-12-13 7 10 21" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/33676810-80b9-436b-a192-bbc127b20ef0">


### eph, epv

<img width="889" alt="スクリーンショット 2023-12-13 7 11 00" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/ca678bb4-2a67-4711-a224-a3f7c5ae8f4e">

<img width="879" alt="スクリーンショット 2023-12-13 7 11 17" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/ebca91fa-c817-4f4c-bb6b-40834a96eab7">

### vel, vn, ve, vd, cog

<img width="910" alt="スクリーンショット 2023-12-13 7 11 53" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/7af20e3b-1f2d-41dc-879c-b19ba3403bf8">

<img width="901" alt="スクリーンショット 2023-12-13 7 12 26" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/81fbb22b-e708-4a01-b8ee-653a796f052b">

<img width="884" alt="スクリーンショット 2023-12-13 7 13 05" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/4898846b-1444-4272-aa6f-d460ece7012a">

<img width="909" alt="スクリーンショット 2023-12-13 7 13 32" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/bb13ef84-e937-4811-b4ea-e019d20e5cf1">

<img width="891" alt="スクリーンショット 2023-12-13 7 13 54" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/256dac8b-1661-41e7-adeb-4cfe74d9cf95">


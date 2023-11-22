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

### HIL_GPS


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

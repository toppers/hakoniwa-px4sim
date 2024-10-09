# MultirotorClient API Reference

`MultirotorClient` クラスは、ドローンシミュレーション環境とのインタフェースを提供します。以下のAPIを通じて、ドローンの制御や状態の取得が可能です。

## クラスメソッド

### `__init__(config_path)`
- **概要**: コンストラクタ。設定ファイルのパスを指定して、クライアントを初期化します。
- **引数**:
  - `config_path` (str): PDU設定ファイルのパス。

### `confirmConnection()`
- **概要**: シミュレーション環境との接続を確認します。成功時に `True` を、失敗時に `False` を返します。

### `enableApiControl(v, vehicle_name=None)`
- **概要**: APIを通じたドローンの制御を有効/無効にします。
- **引数**:
  - `v` (bool): 制御を有効にする場合は `True`、無効にする場合は `False`。
  - `vehicle_name` (str, optional): 操作対象のドローン名。指定しない場合はデフォルトのドローンが対象。

### `armDisarm(v, vehicle_name=None)`
- **概要**: ドローンのアーム/ディスアームを制御します。
- **引数**:
  - `v` (bool): アームする場合は `True`、ディスアームする場合は `False`。
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `simGetVehiclePose(vehicle_name=None)`
- **概要**: 指定されたドローンのROS座標系での現在位置と姿勢を取得します。
- **引数**:
  - `vehicle_name` (str, optional): 対象のドローン名。

### `simGetVehiclePoseUnityFrame(vehicle_name=None)`
- **概要**: 指定されたドローンのUnity座標系での現在位置と姿勢を取得します。
- **引数**:
  - `vehicle_name` (str, optional): 対象のドローン名。

### `takeoff(height, vehicle_name=None)`
- **概要**: ドローンを離陸させます。
- **引数**:
  - `height` (float): 離陸後の高さ。
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `moveToPosition(x, y, z, speed, yaw_deg=None, timeout_sec=-1, vehicle_name=None)`
- **概要**: 指定された位置へドローンを移動させます。
- **引数**:
  - `x`, `y`, `z` (float): ROS座標系での目標位置の座標。
  - `speed` (float): 移動速度。
  - `yaw_deg` (float, optional): 移動時のyaw角度（度）。指定しない場合は現在の姿勢を維持します。
  - `timeout_sec` (int, optional): 操作が完了するまでの最大待機時間。デフォルトは無制限。
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `moveToPositionUnityFrame(x, y, z, speed, yaw_deg=None, timeout_sec=-1, vehicle_name=None)`
- **概要**: Unity座標系に基づいてドローンを指定された位置へ移動させます。
- **引数**:
  - `x`, `y`, `z` (float): Unity座標系での目標位置の座標。
  - `speed` (float): 移動速度。
  - `yaw_deg` (float, optional): 移動時のyaw角度（度）。
  - `timeout_sec` (int, optional): 操作が完了するまでの最大待機時間。デフォルトは無制限。
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `land(vehicle_name=None)`
- **概要**: ドローンを着陸させます。
- **引数**:
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `grab_baggage(grab, timeout_sec=-1, vehicle_name=None)`
- **概要**: 荷物をつかむ/離す操作を制御します。
- **引数**:
  - `grab` (bool): つかむ場合は `True`、離す場合は `False`。
  - `timeout_sec` (int, optional): 操作が完了するまでの最大待機時間。デフォルトは無制限。
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `simGetImage(id, image_type, vehicle_name=None)`
- **概要**: ドローンのカメラから画像を取得します。
- **引数**:
  - `id` (int): カメラのID。
  - `image_type` (str): 画像のタイプ（例: "png"）。
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `simSetCameraOrientation(id, degree, vehicle_name=None)`
- **概要**: ドローンのカメラの向きを設定します。
- **引数**:
  - `id` (int): カメラのID。
  - `degree` (float): カメラのピッチ角度（度）。
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `getLidarData(vehicle_name=None)`
- **概要**: Lidarデータを取得します。
- **引数**:
  - `vehicle_name` (str, optional): Lidarデータを取得するドローン名。

### `getGameJoystickData(vehicle_name=None)`
- **概要**: ゲームパッドのデータを取得します。
- **引数**:
  - `vehicle_name` (str, optional): データを取得するドローン名。

### `putGameJoystickData(data, vehicle_name=None)`
- **概要**: ゲームパッドのデータを送信します。
- **引数**:
  - `data`: 送信するゲームパッドのデータ。
  - `vehicle_name` (str, optional): データを送信するドローン名。

# MultirotorClient API Reference

`MultirotorClient`クラスは、ドローンシミュレーション環境とのインタフェースを提供します。以下のAPIを通じて、ドローンの制御や状態の取得が可能です。

## クラスメソッド

### `__init__(config_path)`
- 概要: コンストラクタ。設定ファイルのパスを指定して、クライアントを初期化します。
- 引数:
  - `config_path` (str): PDU設定ファイルのパス。

### `confirmConnection()`
- 概要: シミュレーション環境との接続を確認します。成功時に`True`を、失敗時に`False`を返します。

### `enableApiControl(v, vehicle_name=None)`
- 概要: APIを通じたドローンの制御を有効/無効にします。
- 引数:
  - `v` (bool): 制御を有効にする場合は`True`、無効にする場合は`False`。
  - `vehicle_name` (str, optional): 操作対象のドローン名。指定しない場合はデフォルトのドローンが対象。

### `armDisarm(v, vehicle_name=None)`
- 概要: ドローンのアーム/ディスアームを制御します。
- 引数:
  - `v` (bool): アームする場合は`True`、ディスアームする場合は`False`。
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `simGetVehiclePose(vehicle_name=None)`
- 概要: 指定されたドローンの現在位置と姿勢を取得します。
- 引数:
  - `vehicle_name` (str, optional): 対象のドローン名。

### `takeoff(height, vehicle_name=None)`
- 概要: ドローンを離陸させます。
- 引数:
  - `height` (float): 離陸後の高さ。
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `moveToPosition(x, y, z, speed, vehicle_name=None)`
- 概要: 指定された位置へドローンを移動させます。
- 引数:
  - `x`, `y`, `z` (float): 目標位置の座標。
  - `speed` (float): 移動速度。
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `land(vehicle_name=None)`
- 概要: ドローンを着陸させます。
- 引数:
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `grab_baggage(grab, vehicle_name=None)`
- 概要: 荷物をつかむ/離す操作を制御します。
- 引数:
  - `grab` (bool): つかむ場合は`True`、離す場合は`False`。
  - `vehicle_name` (str, optional): 操作対象のドローン名。

### `simGetImage(id, image_type, vehicle_name=None)`
- 概要: ドローンのカメラから画像を取得します。
- 引数:
  - `id` (int): カメラのID。
  - `image_type` (str): 画像のタイプ（例: "png"）。
  - `vehicle_name` (str, optional): 操作対象のドローン名。

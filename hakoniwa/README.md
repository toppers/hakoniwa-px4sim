# 箱庭のインストール手順

```
cd hakoniwa-px4sim/hakoniwa
```

## Windows の場合

Windowsの場合は、docker コンテナ内での作業になります。

事前に以下のインストールをしてください。

```
sudo apt  install docker.io
```

```
sudo apt install net-tools
```

インストールが終わったら、docker を起動します。

```
sudo service docker start
```

そして、以下のコマンドで docker イメージを pull してください。

```
bash docker/pull-image.bash 
```

なお、以下のエラーが出た場合は、`/var/run/docker.sock` に所有権を与えてください。

```
permission denied while trying to connect to the Docker daemon socket at unix:///var/run/docker.sock: Post "http://%2Fvar%2Frun%2Fdocker.sock/v1.24/images/create?fromImage=toppersjp%2Fhakoniwa-px4sim&tag=v1.0.1": dial unix /var/run/docker.sock: connect: permission denied
```

成功した場合は、以下のように`toppersjp/hakoniwa-px4sim`が作成されます。

```
$ docker images
REPOSITORY                        TAG       IMAGE ID       CREATED             SIZE
toppersjp/hakoniwa-px4sim         v1.0.1    c34c696b8007   About an hour ago   3.98GB
```

なお、イメージは一度作成できればWindows内に残り続けますので、一度だけ実行すればOKです。

次に、以下のコマンドで docker コンテナに入ってください。

```
bash docker/run.bash 
```

箱庭環境をビルドします。

```
bash build.bash
```

成功すると、` cmake-build/src/hako-px4sim` というファイルが作成されます。

## Mac の場合

箱庭環境をビルドします。

```
bash build.bash
```

成功すると、` cmake-build/src/hako-px4sim` というファイルが作成されます。


# 機体のパラメータ説明

機体のパラメータは、[drone_config.json](https://github.com/toppers/hakoniwa-px4sim/blob/main/hakoniwa/config/drone_config.json)で設定できます。

各項目の設定内容は以下のとおりです。

## シミュレーション設定
- **lockstep**: シミュレーションのロックステップモード。`true` で同期モードに設定されます。
- **timeStep**: シミュレーションのタイムステップ間隔。単位は秒(`s`)。例: `0.003`。
- **logOutputDirectory**: ログファイルの出力ディレクトリへのパス。例: `"./"`。
- **logOutput**: 各種センサーとMAVLinkのログ出力の有効/無効。
  - **sensors**: 各センサーのログ出力設定。`true` または `false`。
  - **mavlink**: MAVLinkメッセージのログ出力設定。`true` または `false`。
- **mavlink_tx_period_msec**: MAVLinkメッセージの送信周期。単位はミリ秒(`ms`)。
- **location**: シミュレーションの地理的位置。
  - **latitude**: 緯度。単位は度(`deg`)。
  - **longitude**: 経度。単位は度(`deg`)。
  - **altitude**: 高度。単位はメートル(`m`)。
  - **magneticField**: 地磁気の強度と方向。
    - **intensity_nT**: 地磁気の強度。単位はナノテスラ(`nT`)。
    - **declination_deg**: 地磁気の偏角。単位は度(`deg`)。
    - **inclination_deg**: 地磁気の傾斜角。単位は度(`deg`)。

## コンポーネント設定
- **droneDynamics**: ドローンの動力学モデル。
  - **physicsEquation**: 運動方程式のタイプ(BodyFrameのみ対応しています)。
  - **airFrictionCoefficient**: 空気抵抗係数。
  - **inertia**: 慣性モーメントのリスト。単位はキログラム・メートル二乗(`kg*m^2`)。
  - **mass_kg**: ドローンの質量。単位はキログラム(`kg`)。
  - **position_meter**: 機体の初期位置。単位はメートル(`m`)。
  - **angle_degree**: 機体の初期角度。単位は度(`deg`)。
- **rotor**: ローターの設定。
  - **Tr**: ローターの応答時間。単位は秒(`s`)。
  - **Kr**: ローターの力定数。
  - **rpmMax**: ローターの最大回転数。単位は回転/分(`rpm`)。
- **thruster**: スラスターの設定。
  - **rotorPositions**: ローターの位置と回転方向。単位はメートル(`m`)。rotationDirectionはローターの回転方向(CW:-1.0, CCW: 1.0)
  - **HoveringRpm**: ホバリング時の回転数。単位は回転/分(`rpm`)。
  - **parameterB**: スラスターのパラメータB。
  - **parameterJr**: スラスターの慣性モーメントパラメータ。
- **sensors**: 各種センサーの設定。
  - **sampleCount**: サンプル数
  - **noise**:ノイズレベル(標準偏差)。ノイズ未設定の場合は0。

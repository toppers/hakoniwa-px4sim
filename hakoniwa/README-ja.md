[English](README.md) ｜ 日本語

# 箱庭のインストール手順

```
cd hakoniwa-px4sim/hakoniwa
```

## Windows の場合

Windowsの場合は、以下のツールを利用しますので、インストールください。

* Visual Studio(C++)
  * ビルドモードは、x64-Release としてください
* [Python 3.12](https://www.python.org/)

## WSL2 の場合

WSL2の場合は、docker コンテナ内での作業になります。

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

## Mac/Ubuntu の場合

箱庭コア機能をインストールします。

```
bash install.bash
```

インストールが成功している場合は、以下のコマンドでチェックできます。

```
bash third-party/hakoniwa-core-cpp-client/hako-setup-check.bash
```

成功している場合は、以下のログが出力されます。
```
OK Directory exists: /usr/local/bin
OK Directory exists: /usr/local/bin/hakoniwa
OK Directory exists: /usr/local/lib
OK Directory exists: /usr/local/lib/hakoniwa
OK Directory exists: /etc/hakoniwa
OK Directory exists: /var/lib/hakoniwa
OK Directory exists: /var/lib/hakoniwa/mmap
OK File exists: /etc/hakoniwa/cpp_core_config.json
OK File exists: /usr/local/bin/hakoniwa/hako-cmd
OK File exists: /usr/local/lib/hakoniwa/libhakoarun.a
OK File exists: /usr/local/lib/hakoniwa/libshakoc.dylib
OK File exists: /usr/local/lib/hakoniwa/hakoc.so
OK File exists: /usr/local/lib/hakoniwa/libassets.dylib
OK File exists: /usr/local/lib/hakoniwa/libconductor.dylib
OK File exists: /usr/local/lib/hakoniwa/py
OK File exists: /usr/local/bin/hakoniwa/hako-proxy
Check complete.
```

# 箱庭のビルド手順（WSL2/Mac/Ubuntu）

箱庭環境をビルドします。ビルド方法としては、２パターンあります。


## MATLAB生成コードを利用しない場合

```
bash build.bash
```

## MATLAB生成コードを利用する場合

```
bash build.bash HAKONIWA_MATLAB_BUILD=true
```

成功すると、` cmake-build/src/hako-px4sim` というファイルが作成されます。


# 箱庭のビルド手順（Windows）

1. Visual Stuido　を起動し、「ローカルフォルダーを開く」を選択します。
2. hakoniwa-px4sim を選択します。
3. 「ビルド」→「すべてビルド」を選択するとビルドが始まります。

# 機体のパラメータ説明

機体のパラメータは、[drone_config.json](https://github.com/toppers/hakoniwa-px4sim/blob/main/hakoniwa/config/api_sample/drone_config_0.json)で設定できます。

各項目の設定内容は以下のとおりです。

## シミュレーション設定
- **name**: 機体名
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
  - **physicsEquation**: 運動方程式のタイプを指定します。
    - BodyFrame: 箱庭のデフォルト物理モデルを利用する場合は、この値を設定してください。
    - BodyFrameMatlab: MATLABで生成した物理モデルのコードを利用する場合は、この値を設定してください。
  - **collision_detection**: 障害物との衝突を検出して物理式にフィードバックする場合は`true`。非検出とする場合は、`false`。
  - **enable_disturbance**: 風や温度などの外乱を物理/センサモデルにフィードバックする場合は`true`。非検出とする場合は、`false`。
  - **manual_control**:　センサキャリブレーションで機体を手動で操作した場合に利用します。`true`にすると、外部操作が可能になります。通常は`false`として下さい。
  - **airFrictionCoefficient**: 空気抵抗係数。空気抵抗の１次項と２次項を配列で指定します。
  - **inertia**: 慣性モーメントのリスト。単位はキログラム・メートル二乗(`kg*m^2`)。
  - **mass_kg**: ドローンの質量。単位はキログラム(`kg`)。
  - **body_size**: 機体のサイズ（x, y, z）を配列で指定します。単位はメートル(`m`)。
  - **position_meter**: 機体の初期位置。単位はメートル(`m`)。
  - **angle_degree**: 機体の初期角度。単位は度(`deg`)。
- **rotor**: ローターの設定。
  - **vendor**: ベンダ名を指定します。現状は`None`を設定して下さい。
  - **dynamics_constants**: ローターの動力学定数。
    - **R**: 電気抵抗。単位はオーム(`Ω`)。
    - **Cq**: トルク係数。単位は(`Nms^2/rad^2`)。
    - **D**: 動粘性摩擦係数。単位は(`Nms/rad`)。
    - **K**: 逆起電力定数。単位はラジアン/秒・ボルト(`Nm/A`)。
    - **J**: モーターイナーシャ。単位はキログラム・メートル二乗(`kg*m^2`)。
- **thruster**: スラスターの設定。
  - **vendor**: ベンダ名を指定します。現状は`None`を設定して下さい。
  - **rotorPositions**: ローターの位置と回転方向。単位はメートル(`m`)。rotationDirectionはローターの回転方向(CW:-1.0, CCW: 1.0)
  - **Ct**: 推力係数。単位は(`Ns^2/rad^2`)。
- **sensors**: 各種センサーの設定。
  - **sampleCount**: サンプル数
  - **noise**:ノイズレベル(標準偏差)。ノイズ未設定の場合は0。


## コントローラ設定

**controller**セクションでは、ドローンのフライトコントローラーモジュールに関連する設定を行います。以下のパラメータが設定可能です。

- **moduleDirectory**: フライトコントローラーモジュールが格納されているディレクトリパスを指定します。例えば、`"../src/drone_control/cmake-build/workspace/FlightController"` のようにモジュールのパスを指定します。
  
- **moduleName**: 使用するフライトコントローラーモジュールの名前を指定します。これにより、シミュレーション内でどのフライトコントローラーモジュールが使用されるかを決定します。例: `"FlightController"`。

- **direct_rotor_control**: ローターの直接制御を有効にするかどうかを指定します。`true`に設定すると、ローターを直接操作する制御が有効になります。通常は`false`に設定します。

- **mixer**: ドローンのミキサー設定を指定します。本セクション未設定の場合は、直接、推力とトルクが物理モデルに入力されます。
  - **vendor**: ミキサーのベンダ名を指定します。現状では`"None"`と`"linear"`を指定できます。
  - **enableDebugLog**: デバッグログの出力を有効にするかどうかを指定します。`true`でデバッグログが有効になります。
  - **enableErrorLog**: エラーログの出力を有効にするかどうかを指定します。`true`でエラーログが有効になります。


# 箱庭コマンドおよびライブラリのインストール手順(WSL2/Mac/Ubuntu）

箱庭にはコマンド(`hako-cmd`) と共有ライブラリ(`libshakoc.[so|dylib]`)があります。Unityを使用せずにシミュレーションを実行する場合に利用します。

以下、インストール手順を示します。

**ディレクトリ移動:**

```
cd hakoniwa/third-party/hakoniwa-core-cpp-client
```

**ビルド：**

```
bash build.bash
```

**インストール：**

```
bash install.bash
```

インストール時に、/usr/local/lib/hakoniwa と /usr/local/bin/hakoniwa へのアクセス許可が必要になることがあります。この許可を与えると、箱庭のライブラリやコマンドがこれらのディレクトリに配置されます。


**環境変数の設定:**

インストールが成功した後、これらのパスを環境変数に設定してください。

Ubuntuの場合:

~/.bashrc ファイルに以下の行を追加してください。

```sh
export LD_LIBRARY_PATH=/usr/local/lib/hakoniwa:$LD_LIBRARY_PATH
export PATH=/usr/local/bin/hakoniwa:$PATH
```

変更を反映させるために、次のコマンドを実行します。

```sh
source ~/.bashrc
```

macOSの場合：

使用しているシェルに応じて ~/.bash_profile または ~/.zshrc に以下の行を追加します。

```sh
export DYLD_LIBRARY_PATH=/usr/local/lib/hakoniwa:$DYLD_LIBRARY_PATH
export PATH=/usr/local/bin/hakoniwa:$PATH
```

変更を反映させるために、次のコマンドを実行します。

```sh
source ~/.bash_profile  # Bashの場合
source ~/.zshrc         # Zshの場合
```

# 箱庭コマンドおよびライブラリのインストール手順(Windows）

Windowsコンソールを開き、`hakoniwa-px4sim/haoniwa/third-party/hakoniwa-core-cpp-client` に移動し、インストールコマンドを実行します。

```
 .\install.bat
```

成功すると、以下の環境変数が設定されます。

* HAKO_CONFIG_PATH
  * <path/to>\hakoniwa-core-cpp-client\cpp_core_config.json
* PATH
  * <path/to>\hakoniwa-core-cpp-client\out\build\x64-Release\core\sample\base-procs\hako-cmd

もし、設定されていない場合は、手動で設定してください。

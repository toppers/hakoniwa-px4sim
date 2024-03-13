
[English](README.md) ｜ 日本語

# これは何？

ここでは、ベンダ毎のセンサモデルを作成するためのコードを管理しています。

# 対象センサ

現在サポートしているセンサは以下のものです。

- [ ] 加速度センサ
- [X] ジャイロセンサ
- [ ] GPS
- [ ] 磁気センサ
- [ ] 気圧センサ

ここで作成したセンサモジュールは、ダイナミックリンクライブラリとしてビルドされます。
作成されたライブラリは、箱庭の機体特性パラメータファイルでパス指定することで大ナミックローディングされます。

# ジャイロセンサ

## 作成方法

ジャイロセンサを作成するためには、以下のインタフェース関数を実装することで実現しています。

定義ファイル：hako_module_drone_sensor_gyro.h
```
typedef struct {
    int (*init) (mi_drone_sensor_gyro_context_t* context);
    mi_drone_sensor_gyro_out_t (*run) (mi_drone_sensor_gyro_in_t *in);
} HakoModuleDroneSensorGyroType;
```

実装する場所は、`sensors/gyro/vendor` 直下です。ベンダ毎のディレクトリ名を作成して、その直下に必要なファイルを配置してください。

例：箱庭のサンプルジャイロセンサ
```
./sensors/gyro/vendor
└── hakogyro
    ├── CMakeLists.txt
    ├── config_parser.hpp
    └── hako_module_drone_sensor_gyro_impl.cpp
```

CMAkeLists.txt の設定例：

```
# ローダブルモジュールのビルド設定
project(hakogyro)

# ソースファイル
set(SOURCE_FILES hako_module_drone_sensor_gyro_impl.cpp)

# ローダブルモジュールとして動的ライブラリを生成
add_library(
    ${PROJECT_NAME} MODULE 
    ${SOURCE_FILES}
    ../hako_module_drone_sensor_gyro.c
)

target_include_directories(
    ${PROJECT_NAME} 
    PRIVATE ../../include
    PRIVATE ..
)
```

## ビルド方法


### Windows の場合

[hakoniwa](https://github.com/toppers/hakoniwa-px4sim/tree/main/hakoniwa)のビルドで自動的にビルドされます。

### Windows 以外の場合(WSL2/Mac)

センサモジュールのビルドは、`drone_sensors` 直下に移動して、以下のコマンドを実行することで自動ビルドされます。

```
bash build.bash
```

実行ログ：
```
-- The C compiler identification is AppleClang 15.0.0.15000100
-- The CXX compiler identification is AppleClang 15.0.0.15000100
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (0.6s)
-- Generating done (0.0s)
-- Build files have been written to: /Users/tmori/project/release/hakoniwa-px4sim/drone_sensors/cmake-build
[ 33%] Building CXX object sensors/gyro/vendor/hakogyro/CMakeFiles/hakogyro.dir/hako_module_drone_sensor_gyro_impl.cpp.o
[ 66%] Building C object sensors/gyro/vendor/hakogyro/CMakeFiles/hakogyro.dir/__/hako_module_drone_sensor_gyro.c.o
[100%] Linking CXX shared module libhakogyro.so
[100%] Built target hakogyro
```

## サンプル実装説明

ジャイロセンサのサンプル実装例として、`hakogyro` を用意しています。

インタフェース関数の実装内容は以下のとおりです。

* 概要
  * このセンサは、温度ドリフトによってセンサ値に偏りが生じます。
  * 温度ドリフトのパラメータとしては、以下の３種類です。
    * TEMP_MIN: 許容可能な温度の下限値
    * TEMP_MAX: 許容可能な温度の上限値
    * TEMP_DRIFT: 許容範囲外になった場合のドリフトする割合
  * 温度ドリフトのパラメータは外部ファイルで定義します。
* hako_module_drone_sensor_gyro_impl_init
  * 温度ドリフトのパラメータファイルを取得し、各パラメータをコンテキストとして保存します。
* hako_module_drone_sensor_gyro_impl_run
  * 箱庭から、シミュレーションで計算された各加速（機体座標系）と機体温度が
  * 入力データとして指定されますので、その値をベースに、センサ値にドリフトした値を返します。

## 機体パラメータでのジャイロセンサの設定項目

ジャイロセンサの機体パラメータの設定項目は、以下の２つです。

* gyro
  * vendor: 作成したセンサのディレクトリパスを指定します。
  * context
    * moduleName：作成したセンサのモジュール名を指定します。
    * file：温度ドリフトパラメータ等、センサのパラメータファイルを指定します。


設定例：
```json
"gyro": {
    "vendor": "../drone_sensors/cmake-build/sensors/gyro/vendor/hakogyro",
    "context": {
        "moduleName": "hakogyro",
        "file": "./config/hard-maker-demo/sensor0.txt"
    },
    "sampleCount": 1,
    "noise": 0.0
}
```
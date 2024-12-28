[English](README.md) ｜ 日本語

# 箱庭ドローン制御

## これは何？

ここでは、箱庭の[ドローン物理モデル](https://github.com/toppers/hakoniwa-px4sim/blob/main/drone_physics/README-ja.md)をPID制御するプログラムです。

PID制御プログラムとして、以下の２つのプログラムが用意されています。

- FlightController
  - Python APIで箱庭ドローンを操作できるようにするための制御プログラムです。
- RadioController
  - PS4コントローラで箱庭ドローンを操作できるようにするための制御プログラムです。

これらのプログラムは、ビルドするとローダブルモジュールとして作成され、箱庭ドローンシミュレーが自動的にローディングしてくれます。


# ビルド方法

## Windows の場合

[hakoniwa](https://github.com/toppers/hakoniwa-px4sim/tree/main/hakoniwa)のビルドで自動的にビルドされます。

## Windows 以外の場合(WSL2/Mac)

`cmake-build` に移動して、以下のコマンドを実行してください。

```
cmake ..
make
```

成功すると、ローダブルモジュールが作成されます。

例：lib<ディレクトリ名>.so が作成されます
```
cmake-build/workspace/FlightController/libFlightController.so 
cmake-build/workspace/RadioController/libRadioController.so 
```

# 制御パラメータファイル

ドローンの動作は、さまざまな制御パラメータで構成されています。これらのパラメータはテキストファイルとして定義され、ユーザーはそれをカスタマイズすることが可能です。

もしドローンの動きに調整が必要な場合、このパラメータファイルの値を変更することで、動作の微調整を行うことができます。

現在のデフォルト設定は、こちらで確認できます：
[ドローン制御パラメータ設定ファイル](https://github.com/toppers/hakoniwa-px4sim/blob/main/drone_control/config/param-api-mixer.txt)


- パラメータファイルを正しく読み込むためには、環境変数 `HAKO_CONTROLLER_PARAM_FILE` にそのファイルのパスを設定する必要があります。
- ラジオコントロールの場合、`ANGLE_CONTROL_ENABLE` を  `1` にすると、ATTIモードで操作できます。`0` にすると、GPSモードで操作できます。
- PID制御パラメータは、シミュレーションをリセットすると再ローディングされます。

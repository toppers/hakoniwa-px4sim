# Matlab と Hakoniwa Drone の連携

## これは何？

Matlab で定義された制御アルゴリズムを Hakoniwa Drone に組み込むための最初の一歩です。
初回として，drone_physics 関数を Matlab でモデリングし，Hakoniwa Drone とリンクして
動作させることを目標とします．

## インターフェイス

このディレクトリにある，drone_physics_matlab.h がインターフェイスです．
そこに，in/out の構造体の定義，および作ってほしい関数宣言があります．

- `mi_drone_acceleration_in_t` が入力構造体
- `mi_drone_acceleration_out_t` が出力構造体
- `mi_drone_acceleration()` が matlab で作る関数です（もしくはここから呼び出す）

最初のテストプログラムを，acctest.cpp に書きました．matlab で検索して，コメントアウトしてある
部分が通るようにつなぎます．acctest.cpp には，drone_physics 側の元関数を呼び出して実装した，
同じインターフェイスのものが定義されています．それとの答え合わせをしていきたいと思います．

- `drone_acceleration_by_physics()` が元関数を使った同じインターフェイスの関数

これと答え合わせします．

## ディレクトリ

* matlab-if/
  * README.md - このファイル
  * drone_physics_matlab.h - インターフェイスヘッダー
  * acctest.cpp - インターフェイスのテストプログラム

将来的には Matlab のいろんな実装をサブディレクトリを切って配置していく予定です．また，Matlab 実装との成果物連携をやりやすくするために，ごそっとディレクトリを切って（matlab-if/drone_physics），その中に Matlab の実装やインターフェイス仕様書(html)を手をかけずにそのまま入れるようにしたいです．

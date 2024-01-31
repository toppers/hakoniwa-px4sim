# Matlab と Hakoniwa Drone の連携

## これは何？

Matlab で定義された制御アルゴリズムを Hakoniwa Drone に組み込むための最初の一歩です．
初回として，機体の動力学を記述する drone_physics ディレクトリの関数を Matlab でモデリングし，Hakoniwa Drone とリンクして動作させています．

## インターフェイス

このディレクトリにある，drone_physics_matlab.h がインターフェイスです．
そこに，in/out の構造体の定義，および実際に Matlab で作る関数宣言があります．

- `mi_drone_acceleration_in_t` が入力構造体
- `mi_drone_acceleration_out_t` が出力構造体
- `mi_drone_acceleration()` が matlab を呼び出すエントリポイント関数

このインターフェイスを使って，Hakoniwa Drone と連携します．
既存のC言語での実装との整合テストを行うために，`acctest.cpp` を用意し，実際にテストしました．
acctest.cpp には，drone_physics 側の既存関数を呼び出して実装した，同じインターフェイスのものが定義されており，
それとの答え合わせするコードが書かれています．

- `drone_acceleration_by_physics()` が元関数を使った同じインターフェイスの関数

そして，実際に単体レベルでテストが行われて通過しています．また，Hakoniwa Drone とリンクして動作させてもいます．

## ディレクトリ

* matlab-if/
  * README.md - このファイル
  * drone_physics_matlab.h - インターフェイスヘッダー
  * acctest.cpp - インターフェイスのテストプログラム
  * drone_sample/ -  今後，他の matlab モデル作るためのサンプルが配置されています．
  * model_template/ - モデルテンプレートです．
     * README.md - モデルテンプレートの説明
     * drone_system.slx - 最上位モデル（編集不可）
     * drone_impl.slx - Droneの処理を実装したモデル（編集可）
     * data.sldd - モデル内で使用するデータ定義ファイル
  * drone_system/ - matlab の実際の実装です（非公開）

matlab 側について，詳しくは，[drone_system/README.md](model_template/README.md) を参照してください．

## 今後
将来的には Matlab のいろんな実装をサブディレクトリを切って配置していく予定です．

例：
* センサーのモデル
* アクチュエータのモデル
* コントローラー側のモデル
* その他，制御アルゴリズムのモデル
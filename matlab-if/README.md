# Matlab と Hakoniwa Drone の連携

## これは何？
Matlab で定義された制御アルゴリズムを Hakoniwa Drone に組み込むための最初の一歩です．
初回として，機体の動力学を記述する drone_physics ディレクトリの関数を matlab でモデリングし，Hakoniwa Drone とリンクして動作させています．

## インターフェイス
このディレクトリにある，`drone_physics_matlab.h` がインターフェイスです．
そこに，drone の加速（並進および回転）を推力とトルクから求める力学の数式につかう，
in/out の「構造体の定義」，および「計算関数」があります．この計算関数を Matlab で実装します．

### `drone_physics_matlab.h`
- `mi_drone_acceleration_in_t` - 入力構造体
- `mi_drone_acceleration_out_t` - 出力構造体
- `mi_drone_acceleration()` - matlab を呼び出すエントリポイント関数

このインターフェイスを使って，Hakoniwa Drone と連携します．

### `drone_physics_matlab_sample.cpp`
matlab と同じプロトタイプ宣言をもつ関数の仮実装です．
- `ml_drone_acceleration()` - 旧実装呼び出しの仮実装（matlab 実装に置き換える）
- `ml_drone_acceleration_initialize()` - 何もしない関数として仮実装
このファイルはサンプルであり，リンクされません．

### `acctest.cpp`
既存のC言語での実装との突合テストを行うために，`acctest.cpp` を用意し，実際にテストしました．
`acctest.cpp` には，drone_physics 側の既存関数を呼び出して実装した，
同じインターフェイスのものが定義されており，それとの答え合わせするテストコードが書かれています．

- `drone_acceleration_by_physics()` - 元関数を使った同じインターフェイスの関数
- `main()` - テストコード

実際に単体レベルでテストが行われて通過しています．また，Hakoniwa Drone とリンクして動作させてもいます．

## ディレクトリ
このディレクトリの構成は以下の通りです．

* matlab-if/
  * README.md - このファイル
  * drone_physics_matlab.h - インターフェイスヘッダー
  * drone_physics_matlab_sample.cpp - 実装例（旧コードを呼び出す仮実装：matlab実装で置き換える）
  * acctest.cpp - インターフェイスのテストプログラム
  * drone_sample/ -  今後，他の matlab モデル作るためのサンプルが配置されています．
  * model_template/ - モデルテンプレートです．
     * README.md - モデルテンプレートの説明
     * drone_system.slx - 最上位モデル（編集不可）
     * drone_impl.slx - Droneの処理を実装したモデル（編集可）
     * data.sldd - モデル内で使用するデータ定義ファイル
  * drone_system/ - matlab の実際の実装です（非公開）

matlab 側について，詳しくは，[drone_template/README.md](model_template/README.md) を参照してください．

## 今後
将来的には Matlab のいろんな実装をサブディレクトリを切って配置していく予定です．

例：
* センサーのモデル
* アクチュエータのモデル
* コントローラー側のモデル
* その他，制御アルゴリズムのモデル
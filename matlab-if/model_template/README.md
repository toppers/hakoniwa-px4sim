# Toppers Hakoniwa と連携するSimulinkモデルのテンプレート

## これは何？
Hakoniwa Droneで利用するSimulinkモデルのテンプレート一式を提供します。

## モデルの構成
基本的な構成は以下の通りです。
drone_systemは、箱庭側のInterfaceと合わせる設定がされています。
モデルを作成する際は、drone_systemは変更せず、drone_implに実装してください。
(drone_implは、drone_systemからモデル参照の形で呼び出されています)

* drone_system.slx - 最上位モデル（編集不可）
* drone_impl.slx - Droneの処理を実装したモデル（編集可）
* data.sldd - モデル内で使用するデータ定義ファイル

## 動作確認バージョンと、必要なライセンス
MATLAB R2023aで動作確認しています。
利用には以下のライセンスが必要ですが、drone_implに実装する処理によっては、
これ以外のライセンスも必要な場合があるので、ご注意ください。

* MATLAB
* Simulink
* MATLAB Coder
* Simulink Coder
* Embedded Coder

## モデルビルド方法
標準的なモデルビルド方法でOKです。
具体的には
* モデルを開く（ダブルクリック）してからCtrl＋Dを押す
* MATLABのコマンドプロンプト上でslbuild('drone_system')と実行する

の何れかを実施ください。

ビルドが完了すると、ビルドディレクトリ（通常は、モデルと同じディレクトリ）に、
drone_system.zipが生成されます。

### drone_system.zipの構成
drone_system.zipの中身は以下の通りです。
実装したDroneモデルの実装は、基本的に23a以下のファイルとして実装されます。
R2023aディレクトリは基本的には使用しません。
（OSの支援を受けるような機能を利用する場合などに必要になります）

なお、Droneモデルの実装方法や設定によって、これ以外のファイルが生成される場合がありますので、ご注意ください。

* 23a
  * drone_system_ert_rtw - 親モデル（drone_system.slx）に対応するコード一式
  * slprj
    * ert
      * ~~_htmlliib~~ - 使いません
      * _sharedutils - 共有ユーティリティファイル
      * drone_impl - 子モデル（drone_impl.slx）に対応するコード一式
* R2023a
  * extern
  * rtw
  * simulink

## drone_implを実装する際の注意点
* drone_impl.slxの入力ポートと出力ポートの設定は変更しないでください（上位モデルとのInterfaceに齟齬が発生し、エラーになります）
* MATLAB側の制約上、生成する関数名（mi_drone_accelerationなど）と同じ名称をモデル名とすることはできません。（現在は、モデルに対してエントリポイント関数と異なる名称を与えています）
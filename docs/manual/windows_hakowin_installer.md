<div class="box-title">
    <p>
    <div style="font-size:18pt;font-weight:bold;text-align:center;margin-top:150px"><span class="title">箱庭ドローンシミュレータ Windows版インストーラ利用方法
    利用</span></div>
    </p>
    <p>
    <div style="font-size:14pt;font-weight:bold;text-align:center;margin-top:20px"><span class="sub-title">インストーラ利用とサンプルアプリ動作確認</span></div>
    </p>
    <p>
    <div style="font-size:12pt;font-weight:bold;text-align:center;margin-top:500px"><span class="author">ドローンWG</span></div>
    </p>
    <p>
    <div style="font-size:12pt;font-weight:bold;text-align:center;margin-top:10px"><span class="date">2024年08月24日</span></div>
    </p>
</div>

<!-- 改ページ -->
<div style="page-break-before:always"></div>

<div style="font-size:18pt;font-weight:bold;text-align:left;"><span class="contents">目次</span></div>
<!-- TOC -->

- [1. 本ドキュメントについて](#1-本ドキュメントについて)
  - [1.1. 対象環境](#11-対象環境)
  - [1.2. 前提条件](#12-前提条件)
- [2. 箱庭ドローンシミュレータ Windows用のインストーラについて](#2-箱庭ドローンシミュレータ-windows用のインストーラについて)
- [3. 箱庭ドローンシミュレータ Windows用のインストール手順](#3-箱庭ドローンシミュレータ-windows用のインストール手順)
  - [3.1. 箱庭ドローンシミュレータ Windows用のインストーラ実行](#31-箱庭ドローンシミュレータ-windows用のインストーラ実行)
- [4. Python API用のサンプルアプリ実行](#4-python-api用のサンプルアプリ実行)
  - [4.1. hakowinフォルダの内容](#41-hakowinフォルダの内容)
  - [4.2. サンプリアプリの起動確認](#42-サンプリアプリの起動確認)
    - [4.2.1. 箱庭コア機能の起動](#421-箱庭コア機能の起動)
    - [4.2.2. 箱庭ドローンシミュレータ用のUnityアプリ起動](#422-箱庭ドローンシミュレータ用のunityアプリ起動)
    - [4.2.3. Python API サンプルアプリの起動](#423-python-api-サンプルアプリの起動)
- [5. トラブルシューティング](#5-トラブルシューティング)
  - [5.1. 環境変数が設定できていない場合](#51-環境変数が設定できていない場合)
  - [5.2. Ramdiskが設定できてない場合](#52-ramdiskが設定できてない場合)

<!-- /TOC -->
<!-- 改ページ -->
<div style="page-break-before:always"></div>


<div style="font-size:18pt;font-weight:bold;text-align:left;"><span class="contents">用語集・改版履歴</span></div>


|略語|用語|意味|
|:---|:---|:---|
||||


|No|日付|版数|変更種別|変更内容|
|:---|:---|:---|:---|:---|
|1|2024/08/24|0.1|新規|新規作成|
||||||
||||||

<!-- 改ページ -->
<div style="page-break-before:always"></div>

# 1. 本ドキュメントについて

本ドキュメントは、箱庭ドローンシミュレータ用のWindows用のインストーラの利用方法とPython APIを使ったサンプルアプリ動作の確認をする解説をします。

## 1.1. 対象環境

本ドキュメントでは、以下の環境を対象としています。

本ドキュメントでは、以下のOSバージョンとPC環境を想定としています。

|No|対象|内容|
|:---|:---|:---|
|1|OS|Windows10/11|
|2|PC|64bit環境|
|3|PC|Corei7 9th以降|
|4|PC|32Gbyteのメモリ推奨|
|5|PC|SSD 512Gbyte以上|
|6|PC|Graphicsアクセラレータ推奨|

## 1.2. 前提条件

Ramdiskのインストールが完了していることが必要です。Ramdiskのインストールについては、windows_preinstall手順を確認して、インストールを完了してください。

# 2. 箱庭ドローンシミュレータ Windows用のインストーラについて

箱庭ドローンシミュレータ Windows用のインストーラは、Microsoft社のVisual Studio 2022 Community版の統合開発環境を利用して開発されています。
利用用途は、オープンソース且つ、個人的な利用に限られます。詳細なライセンス条項等は、Microsoft社のライセンスに則るものとします。

[Visual Studio 2022 Community版ライセンス条項](https://visualstudio.microsoft.com/ja/license-terms/vs2022-ga-community/)

[Visual Studio 2022 Community版再配布条項](https://learn.microsoft.com/ja-jp/visualstudio/releases/2022/redistribution)



# 3. 箱庭ドローンシミュレータ Windows用のインストール手順

Releasページにあるzipファイルを入手します。入手後、適当なフォルダに展開してください。

## 3.1. 箱庭ドローンシミュレータ Windows用のインストーラ実行

zipファイルを展開したフォルダに移動します。setup.exe, hakowin.msiが展開されています。各ファイルの内容は、以下の通りです。

|No|ファイル名|内容|
|:---|:---|:---|
|1|setup.exe|箱庭ドローンシミュレータ動作環境インストール用のセットアップ実行ファイル|
|2|hakowin.msi|箱庭ドローンシミュレータ動作環境パッケージ一式|

上記のファイルが展開されていることが確認できたら、setup.exeを右クリックして、`管理者として実行`をクリックします。

![箱庭ドローンシミュレータインストーラ1](./hakoinst/inst1.png)

hakowinセットアップウィザードの画面が起動しますので、次をクリックします。

![箱庭ドローンシミュレータインストーラ2](./hakoinst/inst2.png)

インストールフォルダの選択画面が出てきます。次をクリックします。
- 注意
  インストールフォルダの選択画面ではインストール先のフォルダや、ユーザが選択できますが、インストールフォルダ,ユーザ選択は変更ぜずにインストールをしてください。

hakoniwa drone config fileの画面が出てきますので、コンフィグファイルの指定ラジオボタンは、デフォルトのまま、次をクリックします。

![箱庭ドローンシミュレータインストーラ3](./hakoinst/inst3.png)

インストールの確認画面が出てきますので、次をクリックします。次の画面でhakowinのインストール画面が出てきますので、プログレスバーが完了するまで待ちます。
プログレスバーが完了したら、次をクリックします。次の画面でインストールの完了画面が出てきますので、閉じるをクリックして終了してください。

![箱庭ドローンシミュレータインストーラ4](./hakoinst/inst4.png)

これで箱庭ドローンシミュレータ環境の動作環境がインストールされました。

インストール先は、以下のフォルダにインストールされています。

```txt
C:\Users\”User名”\Documents\hakoniwa\hakowin\hakoniwa-px4-win
```


# 4. Python API用のサンプルアプリ実行

インストーラでのインストールが完了すると、ディスクトップ上にhakowinフォルダが作成されます。このフォルダに箱庭ドローンシミュレータ環境やPython API用のサンプルアプリなどのショートカットが配置されています。

![箱庭ドローンシミュレータサンプルアプリ1](./hakoinst/inst5.png)

## 4.1. hakowinフォルダの内容

箱庭ドローンシミュレータで利用する箱庭コア機能、Unityのドローンモデル、サンプルアプリなどが配置されています。

![箱庭ドローンシミュレータサンプルアプリ2](./hakoinst/inst6.png)

|No|ファイル名|種類|用途|
|:---|:---|:---|:---|
|1|create_mmap.batのショートカット|mmap|mmap確認用のツール|
|2|run-win.batのショートカット|箱庭コア機能|PX4用の箱庭コア機能起動用|
|3|run-api.batのショートカット|箱庭コア機能|Python APIを使った箱庭コア機能起動用|
|4|run-api2.batのショートカット|箱庭コア機能|Python APIを使った箱庭コア機能起動用|
|5|run-sample.batのショートカット|Pythonアプリ機能|Python APIを使ったサンプルアプリ起動用|
|6|run-camera.batのショートカット|Pythonアプリ機能|Python APIを使ったカメラアプリ起動用|
|7|run-rc.batのショートカット|Pythonアプリ機能|Python APIを使ったプロポアプリ起動用|
|8|unity_model.exeのショートカット|Unityアプリ機能|箱庭ドローンシミュレータ用のUnityビジュアライズアプリ起動用|

## 4.2. サンプリアプリの起動確認

正しくインストールできていること確認するため、Python APIを使ったサンプルアプリを起動して動作確認をします。

### 4.2.1. 箱庭コア機能の起動

最初に箱庭コア機能を起動します。hakowinフォルダ内の`run-api2.batのショートカット`をダブルクリックして起動します。

![箱庭ドローンシミュレータサンプルアプリ動作確認1](./hakoinst/inst7.png)

WAIT STARTと表示され、待ち状態になっていることを確認します。

### 4.2.2. 箱庭ドローンシミュレータ用のUnityアプリ起動

次に箱庭ドローンシミュレータでビジュアライズをするためのUnityアプリを起動します。`drone_model.exeのショートカット`をダブルクリックします。

![箱庭ドローンシミュレータサンプルアプリ動作確認2](./hakoinst/inst8.png)

Unityアプリが起動したら、STARTボタンをクリックして待ちます。

### 4.2.3. Python API サンプルアプリの起動

最後にPython APIの動作確認のため、サンプルアプリを起動します。`run-sample.batのショートカット`をダブルクリックして起動します。

![箱庭ドローンシミュレータサンプルアプリ動作確認3](./hakoinst/inst9.png)

サンプルアプリ起動後にUnityアプリ上でドローンが正しく飛んでいれば、インストールは正常にできています。

# 5. トラブルシューティング

インストーラを使った箱庭ドローンシミュレータ動作環境では、いくつかのトラブルが発生する可能性があります。以下のようなトラブルになった場合には、環境を見直してください。

## 5.1. 環境変数が設定できていない場合

管理者権限でのインストールをしていない場合には、Windows側のシステム上、環境変数が設定されません。環境変数が設定できていないと、以下のようなエラーが発生する場合があります。

![箱庭ドローンシミュレータサンプルトラブルシュート1](./hakoinst/troubleshooting1.png)

この場合は、箱庭ドローンシミュレータのインストーラが管理者権限で実行されてないために置きますので、一度、アンインストールして、管理者権限で再度インストールし直してください。

## 5.2. Ramdiskが設定できてない場合

Ramdiskが正しく設定できていない場合、以下のようなエラーが発生する場合があります。

![箱庭ドローンシミュレータサンプルトラブルシュート2](./hakoinst/troubleshooting2.png)

この場合、Ramdiskの設定を見直すため、windows_preinstallの手順を良く見直して、設定を確認してください。

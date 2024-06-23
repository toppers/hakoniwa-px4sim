このツールは、Windows向けの箱庭ドローンシミュレータです。

■セットアップ手順

◆環境変数
環境変数に以下を追加してください。

環境変数名：HAKO_CONFIG_PATH
設定値：本フォルダ配下にあるhakoniwa\config\cpp_core_config.jsonを絶対パスで指定してください
設定例：E:\hakoniwa-px4-win\hakoniwa\config\cpp_core_config.json

◆RAMDISK
箱庭では64MBのRAMDISKを利用しますので、Windows用のRAMDISKを作成してください。
本説明では、ZドライブをRAMDISKとしています。

参考：
* https://avalon-studio.work/blog/windows/ram-disk-configration/
* ImDiskダウンロード場所：https://sourceforge.net/projects/imdisk-toolkit/
* 高速スタートアップの設定：https://www.fmworld.net/cs/azbyclub/qanavi/jsp/qacontents.jsp?PID=2111-3152

◆RAMDISKのフォルダ設定
RAMDISK作成後、mmap という空フォルダを作成してください。

◆コンフィグファイルの設定
hakoniwa\config\cpp_core_config.jsonファイルを開き、core_mmap_path に、先ほど作成した mmap フォルダを絶対パスとして設定してください。

設定例：Zドライブ直下にmmapフォルダを作成した場合
{
    "shm_type":  "mmap",
    "core_mmap_path":  "Z:\\mmap"
}

◆PX4の準備
こちらを参照してください。
https://github.com/toppers/hakoniwa-px4sim/blob/main/px4/README-ja.md



■PX4連携シミュレーション実行手順

1. hakoniwa\bin フォルダをエクスプローラで開き、run-win.bat をダブルクリックしてください。
2. hakoniwa\DroneWinNative フォルダをエクスプローラで開き、model.exe をダブルクリックしてください。
3. Windowsネイティブ向けのPX4を起動します。
  https://github.com/toppers/hakoniwa-px4sim/blob/main/README-ja.md#%E7%AB%AF%E6%9C%ABa


■Python連携シミュレーション実行手順
◆PS4コントローラで操作する場合

1. hakoniwa\bin フォルダをエクスプローラで開き、run-api.bat をダブルクリックしてください。
2. hakoniwa\DroneWinNative フォルダをエクスプローラで開き、model.exe をダブルクリックして、STARTしてください。
3. hakoniwa\apps\run-rc.batをダブルクリックしてください。
4. hakoniwa\apps\run-camera.batをダブルクリックしてください。

◆Pythonプログラムで制御する場合

1. hakoniwa\bin フォルダをエクスプローラで開き、run-api2.bat をダブルクリックしてください。
2. hakoniwa\DroneWinNative フォルダをエクスプローラで開き、model.exe をダブルクリックして、STARTしてください。
3. hakoniwa\apps\run-sample.batをダブルクリックしてください。


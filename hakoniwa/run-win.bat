@echo off
setlocal

:: WSL IPアドレスの取得
for /f "tokens=3" %%a in ('netsh interface ip show address "vEthernet (WSL (Hyper-V firewall))" ^| findstr "IP Address"') do set "WSL_IP=%%a"

:: IPアドレスの確認
if not defined WSL_IP (
    echo WSL IP address could not be found.
    exit /b 1
)

echo WSL IP Address: %WSL_IP%

:: 実行ディレクトリを cmake-build に変更
pushd cmake-build

:: コマンドの実行
..\..\out\build\x64-Debug\hakoniwa\src\hako-px4sim.exe %WSL_IP% 4560 sim

:: 元のディレクトリに戻る
popd

endlocal

@echo off
if "%~2"=="" (
    echo Usage: %0 "<hakoniwa-unity-drone-model location>" "<config directory>"
    exit /b 1
)

setlocal

:: WSL IPアドレスの取得
for /f "tokens=3" %%a in ('netsh interface ip show address "vEthernet (WSL (Hyper-V firewall))" ^| findstr "IP Address"') do set "WSL_IP=%%a"

:: IPアドレスの確認
if not defined WSL_IP (
    echo WSL IP address could not be found.
    exit /b 1
)

echo WSL IP Address: %WSL_IP%



set "HAKO_CUSTOM_JSON_PATH=%~1\custom.json"
set "DRONE_CONFIG_PATH=%~2"


:: コマンドの実行
..\out\build\x64-Release\hakoniwa\src\hako-px4sim.exe %WSL_IP% 4560 sim

:: 元のディレクトリに戻る
popd

endlocal

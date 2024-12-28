@echo off
setlocal

set "HAKO_CUSTOM_JSON_PATH=..\DroneAppWin\custom.json"
set "DRONE_CONFIG_PATH=..\config_px4"

if not exist "%HAKO_CUSTOM_JSON_PATH%" (
    echo ERROR: can not find "%HAKO_CUSTOM_JSON_PATH%"
    exit /b 1
)

if not exist "%DRONE_CONFIG_PATH%\" (
    echo ERROR: can not find "%DRONE_CONFIG_PATH%"
    exit /b 1
)

REM create_mmap.bat を呼び出す
call create_mmap.bat

for /f "tokens=3" %%a in ('netsh interface ip show address "vEthernet (WSL)" ^| findstr "IP Address"') do set "WSL_IP=%%a"

if not defined WSL_IP (
    echo WSL IP address could not be found.
    exit /b 1
)

echo WSL IP Address: %WSL_IP%


..\..\out\build\x64-Release\hakoniwa\src\hako-px4sim.exe %WSL_IP% 4560 sim

popd

endlocal

@echo off

if "%~2"=="" (
    echo Usage: %0 "<hakoniwa-unity-drone-model location>" "<config directory>"
    exit /b 1
)

set "HAKO_CUSTOM_JSON_PATH=%~1\custom.json"
set "DRONE_CONFIG_PATH=%~2"

if not exist "%HAKO_CUSTOM_JSON_PATH%" (
    echo ERROR: can not find "%HAKO_CUSTOM_JSON_PATH%"
    exit /b 1
)

if not exist "%DRONE_CONFIG_PATH%\" (
    echo ERROR: can not find "%DRONE_CONFIG_PATH%"
    exit /b 1
)

..\out\build\x64-Release\hakoniwa\src\hako-px4sim.exe 127.0.0.1 4560 pid

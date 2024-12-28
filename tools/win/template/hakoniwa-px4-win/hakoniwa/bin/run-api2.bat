@echo off
setlocal

set "HAKO_CUSTOM_JSON_PATH=..\config_api2\custom.json"
set "DRONE_CONFIG_PATH=..\config_api2"

REM create_mmap.bat を呼び出す
call create_mmap.bat

if not exist "%HAKO_CUSTOM_JSON_PATH%" (
    echo ERROR: can not find "%HAKO_CUSTOM_JSON_PATH%"
    exit /b 1
)

if not exist "%DRONE_CONFIG_PATH%\" (
    echo ERROR: can not find "%DRONE_CONFIG_PATH%"
    exit /b 1
)

..\..\out\build\x64-Release\hakoniwa\src\hako-px4sim.exe 127.0.0.1 4560 ext

popd

endlocal

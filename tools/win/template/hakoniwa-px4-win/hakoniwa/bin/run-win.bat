@echo off
setlocal

REM create_mmap.bat を呼び出す
call create_mmap.bat

for /f "tokens=3" %%a in ('netsh interface ip show address "vEthernet (WSL (Hyper-V firewall))" ^| findstr "IP Address"') do set "WSL_IP=%%a"

if not defined WSL_IP (
    echo WSL IP address could not be found.
    exit /b 1
)

echo WSL IP Address: %WSL_IP%


..\..\out\build\x64-Release\hakoniwa\src\hako-px4sim.exe %WSL_IP% 4560 sim

popd

endlocal

@echo off

if "%~1"=="" (
    echo Usage: install.bat [Destination Directory]
    goto :eof
)

set "DEST_DIR=%~1"

echo Copying Python files to %DEST_DIR%...

if not exist "%DEST_DIR%\py" mkdir "%DEST_DIR%\py"

copy "libs\*.py" "%DEST_DIR%\py"
copy "..\hakoniwa\third-party\hakoniwa-core-cpp-client\bindings\python\*.py"  "%DEST_DIR%\py"

if not exist "%DEST_DIR%\py\hako_binary" mkdir "%DEST_DIR%\py\hako_binary"
set "SRC_DIR=..\hakoniwa\third-party\hakoniwa-core-cpp-client\bindings\python\hako_binary\"
xcopy "%SRC_DIR%" "%DEST_DIR%\py\hako_binary" /E /I /Y


set "SRC_FILE=..\out\build\x64-Release\hakoniwa\third-party\hakoniwa-core-cpp-client\bindings\python\hako_asset_python.dll"
copy "%SRC_FILE%" "%DEST_DIR%\py\hakopy.pyd" /Y

setx HAKO_BINARY_PATH "%DEST_DIR%\py\hako_binary\offset"
setx PYTHONPATH "%DEST_DIR%\py

echo Copy operation completed.

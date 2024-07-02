@echo off
setlocal


REM HAKO_CONFIG_PATH環境変数からパスを取得
set "HAKO_CONFIG_PATH=%HAKO_CONFIG_PATH%"

for /f "tokens=*" %%i in ('powershell -Command "Get-Content %HAKO_CONFIG_PATH% | ConvertFrom-Json | Select-Object -ExpandProperty core_mmap_path"') do (
    set CORE_MMAP_PATH=%%i
)

rem 抽出した値を表示
echo %CORE_MMAP_PATH%

for /f "tokens=1* delims=\\" %%i in ("%CORE_MMAP_PATH%") do (
    set DRIVE_LETTER=%%i
    set MMAP_PATH=%%j
)

echo CoreMmapPath: %CORE_MMAP_PATH%
echo DriveLetter: %DRIVE_LETTER%
echo MmapDir: %MMAP_PATH%

REM ドライブに移動
cd /d %DRIVE_LETTER% || (
    echo Failed to change to drive %DRIVE_LETTER%. Drive may not be available.
    exit /b 1
)

REM ディレクトリが存在するか確認
if exist "%MMAP_PATH%" (
    echo Directory already exists: %MMAP_PATH%
) else (
    mkdir "%MMAP_PATH%"
    echo Created directory: %CORE_MMAP_PATH%
)

endlocal

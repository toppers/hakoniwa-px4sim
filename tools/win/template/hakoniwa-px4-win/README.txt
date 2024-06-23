This tool is a Hakoniwa drone simulator for Windows.

■ Setup Instructions

◆ Environment Variables
Please add the following to your environment variables.

Environment Variable Name: HAKO_CONFIG_PATH
Value: Specify the absolute path to hakoniwa\config\cpp_core_config.json under this folder
Example Setting: E:\hakoniwa-px4-win\hakoniwa\config\cpp_core_config.json

◆ RAMDISK
The Hakoniwa uses a 64MB RAMDISK, so please create a RAMDISK for Windows.
In this explanation, the Z drive is used as the RAMDISK.

References:
* https://avalon-studio.work/blog/windows/ram-disk-configration/
* ImDisk Download Location: https://sourceforge.net/projects/imdisk-toolkit/
* Fast Startup Setting: https://www.fmworld.net/cs/azbyclub/qanavi/jsp/qacontents.jsp?PID=2111-3152

◆ RAMDISK Folder Setting
After creating the RAMDISK, please create an empty folder named mmap.

◆ Config File Setting
Open the hakoniwa\config\cpp_core_config.json file and set the core_mmap_path to the absolute path of the mmap folder you just created.

Example Setting: If you created the mmap folder directly under the Z drive
{
    "shm_type":  "mmap",
    "core_mmap_path":  "Z:\\mmap"
}

◆ Preparation for PX4
Please refer to the following.
https://github.com/toppers/hakoniwa-px4sim/blob/main/px4/README-ja.md

■ PX4 Simulation Execution Procedure

1. Open the hakoniwa\bin folder in Explorer and double-click on run-win.bat.
2. Open the hakoniwa\DroneWinNative folder in Explorer and double-click on model.exe.
3. Start PX4 for Windows Native.
  https://github.com/toppers/hakoniwa-px4sim/tree/main?tab=readme-ov-file#terminal-a


■ Python Simulation Execution Procedure
◆If controlling with a PS4 controller:

1. Open the hakoniwa\bin folder in Explorer and double-click on run-api.bat.
2. Open the hakoniwa\DroneWinNative folder in Explorer and double-click on model.exe.
3. Double-click hakoniwa\apps\run-rc.bat.
4. Double-click hakoniwa\apps\run-camera.bat.

◆If controlling with a Python program:

1. Open the hakoniwa\bin folder in Explorer and double-click on run-api2.bat.
2. Open the hakoniwa\DroneWinNative folder in Explorer and double-click on model.exe.
3. Double-click hakoniwa\apps\run-sample.bat.


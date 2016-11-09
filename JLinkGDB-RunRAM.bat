@echo off
PATH=D:\MCU\GNU_Tools_ARM_Embedded\5.2_2015q4\bin;D:\MCU\SEGGER\JLink_V610a;%PATH%
@if exist build\obj\build.axf goto run
echo File 'build\obj\build.axf' not found!
echo Build project...
mingw32-make.exe -f Makefile all
@if not exist build\obj\build.axf goto err
:run
start JLinkGDBServer.exe -device Cortex-M3 -if SWD -ir -endian little -speed 1000 
arm-none-eabi-gdb.exe -x flasher/gdb_run_ram.jlink
taskkill /F /IM JLinkGDBServer.exe
goto end
:err
echo Error!
:end
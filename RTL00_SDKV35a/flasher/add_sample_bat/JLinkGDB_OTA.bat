@echo off
call paths.bat
@if exist build\bin\ota.bin goto run
echo File 'build\obj\ota.bin' not found!
echo Build project...
mingw32-make.exe -f Makefile all
@if not exist build\bin\ota.bin goto err
:run
start start JLinkGDBServer.exe -device Cortex-M3 -if SWD -ir -endian little -speed 1000 
arm-none-eabi-gdb.exe -x flasher/gdb_ota.jlink
taskkill /F /IM JLinkGDBServer.exe
goto end
:err
echo Error!
:end
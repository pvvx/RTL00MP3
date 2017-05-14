@echo off
call paths.bat
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
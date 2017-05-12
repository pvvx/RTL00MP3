@echo off
call paths.bat
start JLinkGDBServer.exe -device Cortex-M3 -if SWD -ir -endian little -speed 1000 
arm-none-eabi-gdb.exe -x flasher/gdb_rdflash.jlink
taskkill /F /IM JLinkGDBServer.exe


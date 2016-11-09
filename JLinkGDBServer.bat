@echo off
PATH=D:\MCU\GNU_Tools_ARM_Embedded\5.2_2015q4\bin;D:\MCU\SEGGER\JLink_V610a;%PATH%
start JLinkGDBServer.exe -device Cortex-M3 -if SWD -ir -endian little -speed 1000 
arm-none-eabi-gdb.exe -x flasher/gdb_init.jlink
taskkill /F /IM JLinkGDBServer.exe


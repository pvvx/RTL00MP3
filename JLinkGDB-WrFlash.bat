@echo off
call paths.bat
@if %1x==x goto xxx
set img_file=%1
goto run
:xxx
set img_file=build/bin/ram_all.bin
:run
echo define call1>flasher/flash_file.jlink
echo SetFirwareSize %img_file%>>flasher/flash_file.jlink
echo end>>flasher/flash_file.jlink
echo define call2>>flasher/flash_file.jlink
echo FlasherWrite %img_file% 0 $Image1Size>>flasher/flash_file.jlink
echo end>>flasher/flash_file.jlink
echo define call3>>flasher/flash_file.jlink
echo FlasherWrite %img_file% $Image2Addr $Image2Size>>flasher/flash_file.jlink
echo end>>flasher/flash_file.jlink
start JLinkGDBServer.exe -device Cortex-M3 -if SWD -ir -endian little -speed 3500 
arm-none-eabi-gdb.exe -x flasher/gdb_wrflash.jlink
taskkill /F /IM JLinkGDBServer.exe


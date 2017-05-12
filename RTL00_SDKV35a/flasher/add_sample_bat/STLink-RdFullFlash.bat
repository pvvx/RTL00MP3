@echo off
call paths.bat
cd flasher
openocd -f interface/stlink-v2.cfg -c "adapter_khz 1000" -f rtl8710.ocd -f cortex.ocd -c "init" -c "reset halt" -c "rtl8710_flash_read_id" -c "adapter_khz 5000" -c "rtl8710_flash_read ../fullflash.bin 0 1048576" -c "shutdown"
echo flash read fullflash.bin
pause


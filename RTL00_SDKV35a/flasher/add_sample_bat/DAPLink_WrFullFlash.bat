@echo off
call paths.bat
cd flasher
openocd -f interface/cmsis-dap.cfg -c "adapter_khz 3500" -f rtl8710.ocd -f cortex.ocd -c "init" -c "reset halt" -c "rtl8710_flash_read_id" -c "rtl8710_flash_auto_erase 1" -c "rtl8710_flash_auto_verify 1" -c "rtl8710_flash_write fullflash.bin 0" -c "shutdown"
pause


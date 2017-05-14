@echo off
call paths.bat
cd flasher
openocd -f interface/cmsis-dap.cfg -c "adapter_khz 1000" -f rtl8710.ocd -f cortex.ocd -c "init" -c "reset halt" -c "load_ram_binary RTL00Console_ROM.bin 0x10000BA8" -c "exit"
rem -c "shutdown"



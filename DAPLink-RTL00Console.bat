@echo off
cd flasher
openocd -f interface/cmsis-dap.cfg -c "adapter_khz 1000" -f rtl8710.ocd -f cortex.ocd -c "init" -c "reset halt" -c "load_ram_binary RTL00Console_ROM.bin" -c "exit"
rem -c "shutdown"



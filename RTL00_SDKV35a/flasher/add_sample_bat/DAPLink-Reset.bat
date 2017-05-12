@echo off
call paths.bat
@cd flasher
openocd -f interface/cmsis-dap.cfg -c "adapter_khz 1000" -f rtl8710.ocd -f cortex.ocd -c "init" -c "reset halt" -c "restart_from_falsh" -c "shutdown"
rem 


@echo off
PATH=D:\MCU\OpenOCD;%PATH%
taskkill /F /IM openocd.exe
start openocd -f interface\Jlink.cfg -f flasher\ameba1.cfg

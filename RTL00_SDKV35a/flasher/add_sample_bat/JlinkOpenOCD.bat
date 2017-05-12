@echo off
call paths.bat
taskkill /F /IM openocd.exe
start openocd -f interface\Jlink.cfg -f flasher\ameba1.cfg

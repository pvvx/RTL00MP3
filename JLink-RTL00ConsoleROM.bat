@echo off
PATH=D:\MCU\SEGGER\JLink_V610a;%PATH%
start JLink.exe -Device CORTEX-M3 -If SWD -Speed 4000 flasher\RTL00ConsoleROM.JLinkScript

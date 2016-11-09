@echo off
PATH=D:\MCU\SEGGER\JLink_V610a;%PATH%
JLink.exe -Device CORTEX-M3 -If SWD -Speed 1000 flasher\RTL_Reset.JLinkScript

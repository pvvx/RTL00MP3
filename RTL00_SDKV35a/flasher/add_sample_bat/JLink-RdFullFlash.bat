@echo off
call paths.bat
JLink.exe -Device CORTEX-M3 -If SWD -Speed 10000 flasher/RTL_FFlash.JLinkScript

set libname=lib_wlan
del %libname%_new.a
md %libname%.lib
cd %libname%.lib
PATH=D:\MCU\GNU_Tools_ARM_Embedded\5.2_2015q4\bin;%PATH%
rem arm-none-eabi-ar.exe x ..\%libname%.a
arm-none-eabi-objcopy.exe --globalize-symbol rtw_flash_map_update.part.12 rtl8195a_hal_init.o
rem rtw_flash_map_update
arm-none-eabi-ar.exe ru ..\%libname%_new.a *.o
cd ..
rem rd /q /s %libname%.lib

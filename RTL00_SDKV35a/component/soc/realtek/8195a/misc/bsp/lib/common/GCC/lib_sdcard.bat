set libname=lib_sdcard_v2
del %libname%_new.a
md %libname%.lib
cd %libname%.lib
PATH=D:\MCU\GNU_Tools_ARM_Embedded\5.2_2015q4\bin;%PATH%
arm-none-eabi-ar.exe x ..\%libname%.a
del sd.o
del sdio_host.o
del hal_sdio_host.o
arm-none-eabi-ar.exe ru ..\%libname%_new.a *.o
cd ..
rem rd /q /s %libname%.lib

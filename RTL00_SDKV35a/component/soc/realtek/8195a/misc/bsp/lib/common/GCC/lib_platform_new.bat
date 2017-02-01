set libname=lib_platform
del %libname%_new.a
md %libname%.lib
cd %libname%.lib
PATH=D:\MCU\GNU_Tools_ARM_Embedded\5.2_2015q4\bin;%PATH%
arm-none-eabi-ar.exe x ..\%libname%.a
del hal_efuse.o
del hal_common.o
del freertos_pmu_8195a.o
del hal_soc_ps_monitor.o
del app_start.o
del hal_log_uart.o
del hal_pinmux.o
del hal_misc.o
del startup.o
rem del hal_spi_flash_ram.o
arm-none-eabi-ar.exe ru ..\%libname%_new.a *.o
cd ..
rd /q /s %libname%.lib

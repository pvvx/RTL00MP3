set libname=lib_rtlstd
del %libname%_new.a
md %libname%.lib
cd %libname%.lib
PATH=D:\MCU\GNU_Tools_ARM_Embedded\5.2_2015q4\bin;%PATH%
arm-none-eabi-ar.exe x ..\%libname%.a
del ram_libc.o
rem del ram_libgloss_retarget.o
rem del rtl_eabi_cast_ram.o
rem del rtl_math_ram.o
arm-none-eabi-ar.exe ru ..\%libname%_new.a *.o
cd ..
rem rd /q /s %libname%.lib

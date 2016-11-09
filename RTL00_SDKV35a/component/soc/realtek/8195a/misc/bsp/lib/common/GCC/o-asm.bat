for %%a in (*.a) do (
md %%a.o
cd %%a.o
arm-none-eabi-ar x ..\%%a
for %%o in (*.o) do arm-none-eabi-objdump -S %%o > %%o.asm
cd ..
)


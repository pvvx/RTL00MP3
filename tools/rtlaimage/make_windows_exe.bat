rem pip install wheel
rem pip install pyinstaller
pyinstaller -c --onedir --onefile -n rtlaimage rtlaimage.py
rem copy /b dist\rtlaimage.exe ..\..\RTL00_SDKV35a\component\soc\realtek\8195a\misc\iar_utility\common\tools\rtlaimage.exe
copy /b dist\rtlaimage.exe .\
del /Q rtlaimage.spec
rm -rf dist build

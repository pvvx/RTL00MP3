#=============================================
# User defined
#=============================================
SDK_PATH = RTL00_SDKV35a/
#GCC_PATH = d:/MCU/GNU_Tools_ARM_Embedded/6.2017-q1-update/bin/# + or set in PATH
#OPENOCD_PATH = D:/MCU/OpenOCD/bin/# + or set in PATH
#TOOLS_PATH ?= $(SDK_PATH)../tools/
PYTHON ?= C:/Python27/python
IMAGETOOL ?= $(PYTHON) $(TOOLS_PATH)rtlaimage/rtlaimage.py
FLASHER_TYPE ?= Jlink
#FLASHER_TYPE ?= cmsis-dap
FLASHER_SPEED = 3500
FLASHER_PATH = flasher/
JLINK_PATH ?= D:/MCU/SEGGER/JLink_V612i/
JLINK_GDBSRV ?= JLinkGDBServer.exe

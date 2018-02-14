#=============================================
# User defined
#=============================================
#SDK_PATH = ../RTL00MP3/RTL00_SDKV35a/
SDK_PATH = RTL00_SDKV35a/
#GCC_PATH = d:/MCU/GNU_Tools_ARM_Embedded/6.2017-q1-update/bin/# + or set in PATH
#OPENOCD_PATH = D:/MCU/OpenOCD/bin/# + or set in PATH
#TOOLS_PATH ?= $(SDK_PATH)component/soc/realtek/8195a/misc/iar_utility/common/tools/
FLASHER_TYPE = Jlink
#FLASHER_TYPE = cmsis-dap
FLASHER_SPEED = 3500
FLASHER_PATH = flasher/
JLINK_PATH ?= D:/MCU/SEGGER/JLink_V612i/
JLINK_GDBSRV ?= JLinkGDBServer.exe

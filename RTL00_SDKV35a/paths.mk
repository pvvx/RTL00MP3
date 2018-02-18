#---------------------------
# User defined (in userset.mk)
#---------------------------
SDK_PATH ?= ../RTL00MP3/RTL00_SDKV35a/
#GCC_PATH = d:/MCU/GNU_Tools_ARM_Embedded/6.2017-q1-update/bin/# + or set in PATH
#OPENOCD_PATH = d:/MCU/OpenOCD/bin/# + or set in PATH
FLASHER_TYPE ?= Jlink
FLASHER_PATH ?= $(SDK_PATH)flasher/
JLINK_PATH ?= D:/MCU/SEGGER/JLink_V612i/
JLINK_GDBSRV ?= JLinkGDBServer.exe
PYTHON ?= C:/Python27/python
#---------------------------
# Default
#---------------------------
# Compilation (GCC) tools
CROSS_COMPILE = $(GCC_PATH)arm-none-eabi-
AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
NM = $(CROSS_COMPILE)nm
LD = $(CROSS_COMPILE)gcc
GDB = $(CROSS_COMPILE)gdb
SIZE = $(CROSS_COMPILE)size
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

# Binary tools
TOOLS_PATH ?= $(SDK_PATH)../tools/

# openocd tools
OPENOCD ?= $(OPENOCD_PATH)openocd

# TARGET dirs
TARGET ?= build
OBJ_DIR ?= $(TARGET)/obj
BIN_DIR ?= $(TARGET)/bin
ELFFILE ?= $(OBJ_DIR)/$(TARGET).axf


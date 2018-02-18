# RTL8710 Flasher v0.0.alfa
# pvvx 21.09.2016
include userset.mk
include $(SDK_PATH)paths.mk
include project.mk
#---------------------------
#FLASHER = stlink-v2-1
#FLASHER = stlink-v2
FLASHER ?= Jlink
JLINK_PATH ?= D:/MCU/SEGGER/JLink_V612i/
#---------------------------
# Default
#---------------------------
# TARGET dirs
TARGET ?= build
OBJ_DIR ?= build/obj
BIN_DIR ?= build/bin
ELFFILE ?= $(OBJ_DIR)/$(TARGET).axf
#---------------------------
# Compilation tools
CROSS_COMPILE ?= $(GCC_PATH)arm-none-eabi-
AR ?= $(CROSS_COMPILE)ar
CC ?= $(CROSS_COMPILE)gcc
AS ?= $(CROSS_COMPILE)as
NM ?= $(CROSS_COMPILE)nm
LD ?= $(CROSS_COMPILE)gcc
GDB ?= $(CROSS_COMPILE)gdb
SIZE ?= $(CROSS_COMPILE)size
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump

# Make bunary tools
TOOLS_PATH ?= $(SDK_PATH)../tools/
ifneq ($(shell uname), Linux)
EXE = .exe
endif

IMAGETOOL ?= $(PYTHON) $(TOOLS_PATH)rtlaimage/rtlaimage.py

# openocd tools
OPENOCD = $(OPENOCD_PATH)openocd.exe

JLINK_GDB ?= JLinkGDBServer.exe
JLINK_EXE ?= JLink.exe

ifeq ($(FLASHER), Jlink)
# Jlink FLASHER_SPEED ..4000 kHz
FLASHER_SPEED ?= 3500
else 
ifeq ($(FLASHER),stlink-v2)
# stlink-v2 FLASHER_SPEED ..1800 kHz
FLASHER_SPEED ?= 1800
else
# over FLASHER_SPEED ..1000 kHz ?
FLASHER_SPEED ?= 1000
endif
endif

# COMPILED_BOOT if defined -> extract image1, boot head in elf
COMPILED_BOOT=1
# COMPILED_BOOT_BIN if !defined -> use source startup boot
#COMPILED_BOOT_BIN=1
# PADDINGSIZE defined -> image2 OTA
PADDINGSIZE =44k

NMAPFILE = $(OBJ_DIR)/$(TARGET).nmap

#FLASHER_PATH ?= flasher/

#RAM_IMAGE?= $(BIN_DIR)/ram.bin

RAM1_IMAGE ?= $(BIN_DIR)/ram_1.bin
RAM1P_IMAGE ?= $(BIN_DIR)/ram_1.p.bin

RAM2_IMAGE = $(BIN_DIR)/ram_2.bin
RAM2P_IMAGE = $(BIN_DIR)/ram_2.p.bin

RAM3_IMAGE = $(BIN_DIR)/sdram.bin
RAM3P_IMAGE = $(BIN_DIR)/sdram.p.bin

FLASH_IMAGE = $(BIN_DIR)/ram_all.bin
OTA_IMAGE = $(BIN_DIR)/ota.bin

#all: FLASH_IMAGE = $(BIN_DIR)/ram_all.bin
#all: OTA_IMAGE = $(BIN_DIR)/ota.bin
mp: FLASH_IMAGE = $(BIN_DIR)/ram_all_mp.bin
mp: OTA_IMAGE = $(BIN_DIR)/ota_mp.bin


.PHONY: genbin flashburn reset test readfullflash flashboot flashwebfs flash_OTA runram runsdram
.NOTPARALLEL: all mp genbin flashburn reset test readfullflash _endgenbin flashwebfs flash_OTA

all: $(ELFFILE) $(FLASH_IMAGE) _endgenbin
mp: $(ELFFILE) $(FLASH_IMAGE) _endgenbin

genbin: $(ELFFILE) $(FLASH_IMAGE) _endgenbin

$(ELFFILE):
	$(error Falsher: file $@ not found)

$(NMAPFILE): $(ELFFILE)
	@echo "==========================================================="
	@echo "Build names map file"
	@echo $@
	@$(NM) $< | sort > $@
#	@echo "==========================================================="

$(FLASH_IMAGE):$(ELFFILE)
	@echo "==========================================================="
	$(IMAGETOOL) -a -r -o $(BIN_DIR)/ $(ELFFILE) 
 
_endgenbin:
	@echo "-----------------------------------------------------------"
	@echo "Image ($(OTA_IMAGE)) size $(shell printf '%d\n' $$(( $$(stat --printf="%s" $(OTA_IMAGE)) )) ) bytes"
	@echo "Image ($(FLASH_IMAGE)) size $(shell printf '%d\n' $$(( $$(stat --printf="%s" $(FLASH_IMAGE)) )) ) bytes"
	@echo "==========================================================="  

ifeq ($(FLASHER_TYPE), Jlink)

reset:
	@$(JLINK_PATH)$(JLINK_EXE) -Device CORTEX-M3 -If SWD -Speed 1000 $(FLASHER_PATH)RTL_Reset.JLinkScript

runram:
	$(JLINK_PATH)$(JLINK_EXE) -Device CORTEX-M3 -If SWD -Speed 1000 $(FLASHER_PATH)RTL_RunRAM.JLinkScript

runsdram:
	$(JLINK_PATH)$(JLINK_EXE) -Device CORTEX-M3 -If SWD -Speed 1000 $(FLASHER_PATH)RTL_RunRAM_SDR.JLinkScript

readfullflash:
	@$(JLINK_PATH)$(JLINK_EXE) -Device CORTEX-M3 -If SWD -Speed 1000 $(FLASHER_PATH)RTL_FFlash.JLinkScript


flashburn:
	@echo define call1>$(FLASHER_PATH)flash_file.jlink
	@echo SetFirwareSize build/bin/ram_all.bin>>$(FLASHER_PATH)flash_file.jlink
	@echo end>>$(FLASHER_PATH)flash_file.jlink
	@echo define call2>>$(FLASHER_PATH)flash_file.jlink
	@echo FlasherWrite build/bin/ram_all.bin 0 '$$'Image1Size>>$(FLASHER_PATH)flash_file.jlink
	@echo end>>$(FLASHER_PATH)flash_file.jlink
	@echo define call3>>$(FLASHER_PATH)flash_file.jlink
	@echo FlasherWrite build/bin/ram_all.bin '$$'Image2Addr '$$'Image2Size>>$(FLASHER_PATH)flash_file.jlink
	@echo end>>$(FLASHER_PATH)flash_file.jlink
	@cmd /K start $(JLINK_PATH)$(JLINK_GDBSRV) -device Cortex-M3 -if SWD -ir -endian little -speed 1000 
	@$(GDB) -x $(FLASHER_PATH)gdb_wrflash.jlink
	#@taskkill /F /IM $(JLINK_GDBSRV)

flashboot:
	@echo define call1>$(FLASHER_PATH)file_info.jlink
	@echo set '$$'ImageSize = $(shell printf '0x%X\n' $$(stat --printf="%s" $(BIN_DIR)/ram_1.p.bin))>>$(FLASHER_PATH)file_info.jlink
	@echo set '$$'ImageAddr = 0x000000>>$(FLASHER_PATH)file_info.jlink
	@echo end>>$(FLASHER_PATH)file_info.jlink
	@echo define call2>>$(FLASHER_PATH)file_info.jlink
	@echo FlasherWrite $(BIN_DIR)/ram_1.p.bin '$$'ImageAddr '$$'ImageSize>>$(FLASHER_PATH)file_info.jlink
	@echo end>>$(FLASHER_PATH)file_info.jlink
	@cmd /K start $(JLINK_PATH)$(JLINK_GDBSRV) -device Cortex-M3 -if SWD -ir -endian little -speed 1000 
	@$(GDB) -x $(FLASHER_PATH)gdb_wrfile.jlink

flashwebfs:
	@echo define call1>$(FLASHER_PATH)file_info.jlink
	@echo set '$$'ImageSize = $(shell printf '0x%X\n' $$(stat --printf="%s" $(BIN_DIR)/WEBFiles.bin))>>$(FLASHER_PATH)file_info.jlink
	@echo set '$$'ImageAddr = 0x0D0000>>$(FLASHER_PATH)file_info.jlink
	@echo end>>$(FLASHER_PATH)file_info.jlink
	@echo define call2>>$(FLASHER_PATH)file_info.jlink
	@echo FlasherWrite $(BIN_DIR)/WEBFiles.bin '$$'ImageAddr '$$'ImageSize>>$(FLASHER_PATH)file_info.jlink
	@echo end>>$(FLASHER_PATH)file_info.jlink
	@cmd /K start $(JLINK_PATH)$(JLINK_GDBSRV) -device Cortex-M3 -if SWD -ir -endian little -speed 1000 
	@$(GDB) -x $(FLASHER_PATH)gdb_wrfile.jlink
	#@taskkill /F /IM $(JLINK_GDBSRV)

flashespfs:
	@echo define call1>$(FLASHER_PATH)file_info.jlink
	@echo set '$$'ImageSize = $(shell printf '0x%X\n' $$(stat --printf="%s" $(BIN_DIR)/webpages.espfs))>>$(FLASHER_PATH)file_info.jlink
	@echo set '$$'ImageAddr = 0x0D0000>>$(FLASHER_PATH)file_info.jlink
	@echo end>>$(FLASHER_PATH)file_info.jlink
	@echo define call2>>$(FLASHER_PATH)file_info.jlink
	@echo FlasherWrite $(BIN_DIR)/webpages.espfs '$$'ImageAddr '$$'ImageSize>>$(FLASHER_PATH)file_info.jlink
	@echo end>>$(FLASHER_PATH)file_info.jlink
	@cmd /K start $(JLINK_PATH)$(JLINK_GDBSRV) -device Cortex-M3 -if SWD -ir -endian little -speed 1000 
	@$(GDB) -x $(FLASHER_PATH)gdb_wrfile.jlink
	#@taskkill /F /IM $(JLINK_GDBSRV)


flash_OTA:
	@cmd /K start $(JLINK_PATH)$(JLINK_GDBSRV) -device Cortex-M3 -if SWD -ir -endian little -speed 1000 
	@$(GDB) -x $(FLASHER_PATH)gdb_ota.jlink
	#@taskkill /F /IM $(JLINK_GDBSRV)

else
ifeq ($(FLASHER_TYPE),cmsis-dap)
FLASHER:=cmsis-dap

flashboot:
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c 'transport select swd' -c 'adapter_khz 1000' \
	-f $(FLASHER_PATH)rtl8710.ocd -c 'init' -c 'reset halt' -c 'adapter_khz $(FLASHER_SPEED)' \
	-c 'rtl8710_flash_auto_erase 1' -c 'rtl8710_flash_auto_verify 1' \
	-c 'rtl8710_flash_write $(RAM1P_IMAGE) 0' \
	-c 'rtl8710_reboot' -c 'reset run' -c shutdown

flashburn:
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c 'transport select swd' -c 'adapter_khz 1000' \
	-f $(FLASHER_PATH)rtl8710.ocd -c 'init' -c 'reset halt' -c 'adapter_khz $(FLASHER_SPEED)' \
	-c 'rtl8710_flash_auto_erase 1' -c 'rtl8710_flash_auto_verify 1' \
	-c 'rtl8710_flash_write $(RAM1P_IMAGE) 0' \
	-c 'rtl8710_flash_write $(RAM2P_IMAGE) 0xb000' \
	-c 'rtl8710_reboot' -c 'reset run' -c shutdown

flashimage2p: 
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c 'transport select swd' -c 'adapter_khz 1000' \
	-f $(FLASHER_PATH)rtl8710.ocd -c 'init' -c 'reset halt' -c 'adapter_khz $(FLASHER_SPEED)' \
	-c 'rtl8710_flash_auto_erase 1' -c 'rtl8710_flash_auto_verify 1' \
	-c 'rtl8710_flash_write $(RAM2P_IMAGE) 0xb000' \
	-c 'rtl8710_reboot' -c 'reset run' -c shutdown

flash_OTA:
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c 'transport select swd' -c 'adapter_khz 1000' \
	-f $(FLASHER_PATH)rtl8710.ocd -c 'init' -c 'reset halt' -c 'adapter_khz $(FLASHER_SPEED)' \
	-c 'rtl8710_flash_auto_erase 1' -c 'rtl8710_flash_auto_verify 1' \
	-c 'rtl8710_flash_write $(RAM2P_IMAGE) 0x80000' \
	-c 'rtl8710_reboot' -c 'reset run' -c shutdown
	
flashwebfs:
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c 'transport select swd' -c 'adapter_khz 1000' \
	-f $(FLASHER_PATH)rtl8710.ocd -c 'init' -c 'reset halt' -c 'adapter_khz $(FLASHER_SPEED)' \
	-c 'rtl8710_flash_auto_erase 1' -c 'rtl8710_flash_auto_verify 1' \
	-c 'rtl8710_flash_write $(BIN_DIR)/WEBFiles.bin 0xd0000' \
	-c 'rtl8710_reboot' -c 'reset run' -c shutdown

flashespfs:
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c 'transport select swd' -c 'adapter_khz 1000' \
	-f $(FLASHER_PATH)rtl8710.ocd -c 'init' -c 'reset halt' -c 'adapter_khz $(FLASHER_SPEED)' \
	-c 'rtl8710_flash_auto_erase 1' -c 'rtl8710_flash_auto_verify 1' \
	-c 'rtl8710_flash_write $(BIN_DIR)/webpages.espfs 0xd0000' \
	-c 'rtl8710_reboot' -c 'reset run' -c shutdown
	
reset:
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c 'transport select swd' -c 'adapter_khz 1000' \
	-f $(FLASHER_PATH)rtl8710.ocd -c 'init' -c 'reset halt' -c 'adapter_khz $(FLASHER_SPEED)' \
	-c 'mww 0x40000210 0x111157' -c 'rtl8710_reboot' -c shutdown
	
runram:
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c 'transport select swd' -c 'adapter_khz 1000' \
	-f $(FLASHER_PATH)rtl8710.ocd -c 'init' -c 'reset halt' -c 'adapter_khz $(FLASHER_SPEED)' \
	-c 'load_image $(RAM1_IMAGE) 0x10000bc8 bin' \
	-c 'load_image $(RAM2_IMAGE) 0x10006000 bin' \
	-c 'mww 0x40000210 0x20111157' -c 'rtl8710_reboot' -c shutdown

runsdram:
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c 'transport select swd' -c 'adapter_khz 1000' \
	-f $(FLASHER_PATH)rtl8710.ocd -c 'init' -c 'reset halt' -c 'adapter_khz $(FLASHER_SPEED)' \
	-c 'load_image $(RAM1_IMAGE) 0x10000bc8 bin' \
	-c 'load_image $(RAM2_IMAGE) 0x10006000 bin' \
	-c 'boot_load_srdam $(RAM3_IMAGE) 0x30000000' \
	-c shutdown

endif
endif

clean:
	@rm -f $(BIN_DIR)/*.bin
	
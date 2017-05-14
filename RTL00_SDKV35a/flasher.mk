# RTL8710 Flasher v0.0.alfa
# pvvx 21.09.2016
include userset.mk
include $(SDK_PATH)paths.mk
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
TOOLS_PATH ?= component/soc/realtek/8195a/misc/iar_utility/common/tools/
ifneq ($(shell uname), Linux)
EXE = .exe
endif
PICK = $(TOOLS_PATH)pick$(EXE)
PADDING = $(TOOLS_PATH)padding$(EXE)
CHCKSUM = $(TOOLS_PATH)checksum$(EXE)

# openocd tools
OPENOCD = $(OPENOCD_PATH)openocd

JLINK_GDB ?= JLinkGDBServer.exe
JLINK_EXE ?= JLink.exe

ifeq ($(FLASHER), Jlink)
# Jlink FLASHER_SPEED ..4000 kHz
FLASHER_SPEED = 3500
else 
ifeq ($(FLASHER),stlink-v2)
# stlink-v2 FLASHER_SPEED ..1800 kHz
FLASHER_SPEED = 1800
else
# over FLASHER_SPEED ..500 kHz ?
FLASHER_SPEED = 500
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
RAM1R_IMAGE ?= $(BIN_DIR)/ram_1.r.bin

RAM2_IMAGE = $(BIN_DIR)/ram_2.bin
RAM2P_IMAGE = $(BIN_DIR)/ram_2.p.bin
RAM2NS_IMAGE = $(BIN_DIR)/ram_2.ns.bin

RAM3_IMAGE = $(BIN_DIR)/sdram.bin
RAM3P_IMAGE = $(BIN_DIR)/sdram.p.bin

FLASH_IMAGE = $(BIN_DIR)/ram_all.bin
OTA_IMAGE = $(BIN_DIR)/ota.bin

#all: FLASH_IMAGE = $(BIN_DIR)/ram_all.bin
#all: OTA_IMAGE = $(BIN_DIR)/ota.bin
mp: FLASH_IMAGE = $(BIN_DIR)/ram_all_mp.bin
mp: OTA_IMAGE = $(BIN_DIR)/ota_mp.bin

TST_IMAGE = $(BIN_DIR)/ram_2.bin

.PHONY: genbin1 genbin23 flashburn reset test readfullflash flashwebfs flash_OTA
.NOTPARALLEL: all mp genbin1 genbin23 flashburn reset test readfullflash _endgenbin flashwebfs flash_OTA

all: $(ELFFILE) $(OTA_IMAGE) $(FLASH_IMAGE) _endgenbin
mp: $(ELFFILE) $(OTA_IMAGE) $(FLASH_IMAGE) _endgenbin

genbin1: $(ELFFILE) $(RAM1P_IMAGE) 

genbin23: $(ELFFILE) $(OTA_IMAGE) $(FLASH_IMAGE) _endgenbin 


_endgenbin:
	@echo "-----------------------------------------------------------"
	@echo "Image ($(OTA_IMAGE)) size $(shell printf '%d\n' $$(( $$(stat --printf="%s" $(OTA_IMAGE)) )) ) bytes"
	@echo "Image ($(FLASH_IMAGE)) size $(shell printf '%d\n' $$(( $$(stat --printf="%s" $(FLASH_IMAGE)) )) ) bytes"
	@echo "==========================================================="  

ifeq ($(FLASHER_TYPE), Jlink)

reset:
	@$(JLINK_PATH)$(JLINK_EXE) -Device CORTEX-M3 -If SWD -Speed 1000 $(FLASHER_PATH)RTL_Reset.JLinkScript

runram:
	@$(JLINK_PATH)$(JLINK_EXE) -Device CORTEX-M3 -If SWD -Speed 1000 $(FLASHER_PATH)RTL_RunRAM.JLinkScript

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

flashburn:
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c "transport select swd" -f $(FLASHER_PATH)rtl8710.ocd -c "init" -c "adapter_khz $(FLASHER_SPEED)" -c "reset halt" \
	-c "rtl8710_flash_auto_erase 1" -c "rtl8710_flash_auto_verify 1" \
	-c "rtl8710_flash_write $(RAM1P_IMAGE) 0" \
	-c "rtl8710_flash_write $(RAM2P_IMAGE) 0xb000" \
	-c "rtl8710_reboot" -c "reset run" -c shutdown

flashimage2p: 
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c "transport select swd" -f $(FLASHER_PATH)rtl8710.ocd -c "init" -c "adapter_khz $(FLASHER_SPEED)" -c "reset halt" \
	-c "rtl8710_flash_auto_erase 1" -c "rtl8710_flash_auto_verify 1" \
	-c "rtl8710_flash_write $(RAM2P_IMAGE) 0xb000" \
	-c "rtl8710_reboot" -c "reset run" -c shutdown
	
flashwebfs:
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c "transport select swd" -f $(FLASHER_PATH)rtl8710.ocd -c "init" -c "adapter_khz $(FLASHER_SPEED)" -c "reset halt" \
	-c "rtl8710_flash_auto_erase 1" -c "rtl8710_flash_auto_verify 1" \
	-c "rtl8710_flash_write $(BIN_DIR)/WEBFiles.bin 0xd0000" \
	-c "rtl8710_reboot" -c "reset run" -c shutdown

flashespfs:
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c "transport select swd" -f $(FLASHER_PATH)rtl8710.ocd -c "init" -c "adapter_khz $(FLASHER_SPEED)" -c "reset halt" \
	-c "rtl8710_flash_auto_erase 1" -c "rtl8710_flash_auto_verify 1" \
	-c "rtl8710_flash_write $(BIN_DIR)/webpages.espfs 0xd0000" \
	-c "rtl8710_reboot" -c "reset run" -c shutdown
	
reset:
#	@$(JLINK_PATH)$(JLINK_EXE) -Device CORTEX-M3 -If SWD -Speed $(FLASHER_SPEED) flasher/RTLreset.JLinkScript
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c "transport select swd" -f $(FLASHER_PATH)rtl8710.ocd -c "init" -c "adapter_khz $(FLASHER_SPEED)" -c "reset halt" \
	-c "rtl8710_reboot" -c shutdown

runram:
#	@$(JLINK_PATH)$(JLINK_GDB) -device Cortex-M3 -if SWD -ir -endian little -speed $(FLASHER_SPEED)
#	@$(GDB) -x flasher/gdb_run_ram.jlink
#	@taskkill.exe -F -IM $(JLINK_GDB)
	@$(OPENOCD) -f interface/$(FLASHER).cfg -c "transport select swd" -f $(FLASHER_PATH)rtl8710.ocd -c "init" -c "adapter_khz $(FLASHER_SPEED)" -c "reset halt" \
	-c "load_image $(RAM1R_IMAGE) 0x10000bc8 bin" \
	-c "load_image $(RAM2_IMAGE) 0x10006000 bin" \
	-c "mww 0x40000210 0x20200113" \
	-c "reset run" -c shutdown

endif

$(NMAPFILE): $(ELFFILE)
	@echo "==========================================================="
	@echo "Build names map file"
	@echo $@
	@$(NM) $< | sort > $@
#	@echo "==========================================================="

$(FLASH_IMAGE): $(RAM1P_IMAGE) $(RAM2P_IMAGE) $(RAM3P_IMAGE)
	@echo "==========================================================="
	@echo "Make Flash image ($(FLASH_IMAGE))" 
#	@echo "==========================================================="
	@mkdir -p $(BIN_DIR)
	@rm -f $(FLASH_IMAGE) 
	@cat $(RAM1P_IMAGE) > $(FLASH_IMAGE)
#	@chmod 777 $(FLASH_IMAGE)
ifdef PADDINGSIZE
	@$(PADDING) $(PADDINGSIZE) 0xFF $(FLASH_IMAGE)
endif	
	@cat $(RAM2P_IMAGE) >> $(FLASH_IMAGE)
	@cat $(RAM3P_IMAGE) >> $(FLASH_IMAGE)
#	@echo "Image ($(FLASH_IMAGE)) size $(shell printf '%d\n' $$(( $$(stat --printf="%s" $(FLASH_IMAGE)) )) ) bytes"
#	@echo "==========================================================="
#	@rm $(BIN_DIR)/ram_*.p.bin  
	
$(OTA_IMAGE): $(RAM2NS_IMAGE) $(RAM3_IMAGE) 
	@echo "==========================================================="
	@echo "Make OTA image ($(OTA_IMAGE))"
	@rm -f $(OTA_IMAGE) 
	@cat $(RAM2NS_IMAGE) > $(OTA_IMAGE)
	@cat $(RAM3P_IMAGE) >> $(OTA_IMAGE)
#	@chmod 777 $(OTA_IMAGE)
	@$(CHCKSUM) $(OTA_IMAGE) || true
#	@echo "==========================================================="

$(RAM1P_IMAGE): $(ELFFILE) $(NMAPFILE) 
	@echo "==========================================================="
	@echo "Create image1r ($(RAM1R_IMAGE))"
#	@echo "===========================================================" .bootloader
ifdef COMPILED_BOOT
	@mkdir -p $(BIN_DIR)
	@rm -f $(RAM1_IMAGE) $(RAM1R_IMAGE)
ifdef COMPILED_BOOT_BIN
	@$(eval RAM1_START_ADDR := $(shell grep _binary_build_bin_ram_1_r_bin_start $(NMAPFILE) | awk '{print $$1}'))
	@$(eval RAM1_END_ADDR := $(shell grep _binary_build_bin_ram_1_r_bin_end $(NMAPFILE) | awk '{print $$1}'))
else
	@$(eval RAM1_START_ADDR := $(shell grep __ram_image1_text_start__ $(NMAPFILE) | awk '{print $$1}'))
	@$(eval RAM1_END_ADDR := $(shell grep __ram_image1_text_end__ $(NMAPFILE) | awk '{print $$1}'))
endif
	$(if $(RAM1_START_ADDR),,$(error "Not found __ram_image1_text_start__!"))
	$(if $(RAM1_END_ADDR),,$(error "Not found __ram_image1_text_end__!"))
ifeq ($(RAM1_START_ADDR),$(RAM1_END_ADDR))
ifdef COMPILED_BOOT_BIN
	$(OBJCOPY) --change-section-address .boot.head=0x10000ba8 -j .boot.head -j .bootloader -Obinary $(ELFFILE) $(RAM1P_IMAGE)
else
#	$(OBJCOPY) -j .rom_ram -Obinary $(ELFFILE) $(RAM_IMAGE)
	$(OBJCOPY) -j .ram.start.table -j .ram_image1.text -Obinary $(ELFFILE) $(RAM1R_IMAGE)
	$(PICK) 0x$(RAM1_START_ADDR) 0x$(RAM1_END_ADDR) $(RAM1R_IMAGE) $(RAM1P_IMAGE) head+reset_offset 0x0B000
endif
else 
	$(error "BOOT-image size = 0")
#	$(error Flasher: COMPILE_BOOT = No)
endif	
else
	@if [ -s $(RAM1R_IMAGE) ]; then echo "Use external $(RAM1R_IMAGE)!"; fi 
endif

$(RAM2P_IMAGE): $(ELFFILE) $(NMAPFILE) 
	@echo "==========================================================="
	@echo "Create image2p ($(RAM2P_IMAGE))"
#	@echo "==========================================================="
	@mkdir -p $(BIN_DIR)
	@rm -f $(RAM2_IMAGE) $(RAM2P_IMAGE)
	@$(eval RAM2_START_ADDR = $(shell grep __ram_image2_text $(NMAPFILE) | grep _start__ | awk '{print $$1}'))
	@$(eval RAM2_END_ADDR = $(shell grep __ram_image2_text $(NMAPFILE) | grep _end__ | awk '{print $$1}'))
	$(if $(RAM2_START_ADDR),,$(error "Not found __ram_image2_text_start__!"))
	$(if $(RAM2_END_ADDR),,$(error "Not found __ram_image2_text_end__!"))
	@$(OBJCOPY) -j .image2.start.table -j .ram_image2.text -j .ram_image2.rodata -j .ram.data -Obinary $(ELFFILE) $(RAM2_IMAGE)
	@$(PICK) 0x$(RAM2_START_ADDR) 0x$(RAM2_END_ADDR) $(RAM2_IMAGE) $(RAM2P_IMAGE) body+reset_offset+sig

$(RAM2NS_IMAGE):$(ELFFILE) $(NMAPFILE) 
	@echo "==========================================================="
	@echo "Create image2ns ($(RAM2NS_IMAGE))"
#	@echo "==========================================================="
	mkdir -p $(BIN_DIR)
	rm -f $(RAM2_IMAGE) $(RAM2NS_IMAGE)
	$(eval RAM2_START_ADDR = $(shell grep __ram_image2_text $(NMAPFILE) | grep _start__ | awk '{print $$1}'))
	$(eval RAM2_END_ADDR = $(shell grep __ram_image2_text $(NMAPFILE) | grep _end__ | awk '{print $$1}'))
	$(if $(RAM2_START_ADDR),,$(error "Not found __ram_image2_text_start__!"))
	$(if $(RAM2_END_ADDR),,$(error "Not found __ram_image2_text_end__!"))
	$(OBJCOPY) -j .image2.start.table -j .ram_image2.text -j .ram_image2.rodata -j .ram.data -Obinary $(ELFFILE) $(RAM2_IMAGE)
	$(PICK) 0x$(RAM2_START_ADDR) 0x$(RAM2_END_ADDR) $(RAM2_IMAGE) $(RAM2NS_IMAGE) body+reset_offset

$(RAM3_IMAGE): $(ELFFILE) $(NMAPFILE) 
	@echo "==========================================================="
	@echo "Create image3 (SDRAM, $(RAM3P_IMAGE))"
#	@echo "==========================================================="
	@mkdir -p $(BIN_DIR)
	@rm -f $(RAM3_IMAGE) $(RAM3P_IMAGE)
	@$(eval RAM3_START_ADDR = $(shell grep __sdram_data_ $(NMAPFILE) | grep _start__ | awk '{print $$1}'))
	@$(eval RAM3_END_ADDR = $(shell grep __sdram_data_ $(NMAPFILE) | grep _end__ | awk '{print $$1}'))
	$(if $(RAM3_START_ADDR),,$(error "Not found __sdram_data_start__!"))
	$(if $(RAM3_END_ADDR),,$(error "Not found __sdram_data_end__!"))
#ifneq ($(RAM3_START_ADDR),$(RAM3_END_ADDR))
	@echo	$(RAM3_START_ADDR) $(RAM3_END_ADDR)
	@$(OBJCOPY) -j .image3 -j .sdr_text -j .sdr_rodata -j .sdr_data -Obinary $(ELFFILE) $(RAM3_IMAGE)
	$(PICK) 0x$(RAM3_START_ADDR) 0x$(RAM3_END_ADDR) $(RAM3_IMAGE) $(RAM3P_IMAGE) body+reset_offset
#else
#	@rm -f $(RAM3_IMAGE) $(RAM3P_IMAGE)
#	@echo "SDRAM not used (size = 0)"
#endif
	
$(ELFFILE):
	$(error Falsher: file $@ not found)

clean:
	@rm -f $(BIN_DIR)/*.bin
	
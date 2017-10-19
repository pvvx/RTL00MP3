include userset.mk

all: ram_all
mp: ram_all_mp

.PHONY: ram_all
ram_all:
	@$(MAKE) -f $(SDK_PATH)sdkbuild.mk
	@$(MAKE) -f $(SDK_PATH)flasher.mk genbin1 genbin23

.PHONY: ram_all_mp
ram_all_mp:
	@$(MAKE) -f $(SDK_PATH)sdkbuild.mk mp
	@$(MAKE) -f $(SDK_PATH)flasher.mk mp
	
.PHONY: clean  clean_all
clean:
	@$(MAKE) -f $(SDK_PATH)sdkbuild.mk clean

clean_all:
	@$(MAKE) -f $(SDK_PATH)sdkbuild.mk clean_all
	
.PHONY: flashburn runram reset test readfullflash flashwebfs flashboot
flashboot:
	@$(MAKE) -f $(SDK_PATH)flasher.mk flashboot

flashburn: 
	#JLinkGDB-WrFlash.bat
	@$(MAKE) -f $(SDK_PATH)flasher.mk flashburn

flash_OTA:
	@$(MAKE) -f $(SDK_PATH)flasher.mk flash_OTA
	
webfs:
	@$(MAKE) -f webfs.mk

flashwebfs:
	@$(MAKE) -f webfs.mk
	@$(MAKE) -f $(SDK_PATH)flasher.mk flashwebfs
	#JLinkGDB-WrWebFs.bat

runram: 
	#JLink-RunRAM.bat
	@$(MAKE) --f $(SDK_PATH)flasher.mk runram

runsdram: 
	#JLink-RunRAM.bat
	@$(MAKE) --f $(SDK_PATH)flasher.mk runsdram

reset: 
	#JLink-Reset.bat
	@$(MAKE) -f $(SDK_PATH)flasher.mk reset 

test: 
	JLink-RTL00ConsoleROM.bat
	#@make -f flasher.mk test 

readfullflash:
	#JLink-RdFullFlash.bat
	@$(MAKE) -f $(SDK_PATH)flasher.mk readfullflash 


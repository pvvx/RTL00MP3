
all: ram_all
mp: ram_all_mp

.PHONY: ram_all
ram_all:
	@$(MAKE) -f sdkbuild.mk
	@$(MAKE) -f flasher.mk genbin1 genbin23

webfs:
	@$(MAKE) -f webfs.mk webpages.espfs

.PHONY: ram_all_mp
ram_all_mp:
	@$(MAKE) -f sdkbuild.mk mp
	@$(MAKE) -f flasher.mk mp
	
.PHONY: clean  clean_all
clean:
	@$(MAKE) -f sdkbuild.mk clean
	@$(MAKE) -f webfs.mk clean

clean_all:
	@$(MAKE) -f sdkbuild.mk clean_all
	@$(MAKE) -f webfs.mk clean
	
.PHONY: flashburn runram reset test readfullflash flashwebfs
flashburn: 
	#JLinkGDB-WrFlash.bat
	@$(MAKE) -f flasher.mk flashburn

flash_OTA:
	@$(MAKE) -f flasher.mk flash_OTA
	
flashwebfs:
	@$(MAKE) -f webfs.mk webpages.espfs
	@$(MAKE) -f flasher.mk flashwebfs
	#JLinkGDB-WrWebFs.bat

runram: 
	#JLink-RunRAM.bat
	@$(MAKE) --f flasher.mk runram

reset: 
	#JLink-Reset.bat
	@$(MAKE) -f flasher.mk reset 

test: 
	JLink-RTL00ConsoleROM.bat
	#@make -f flasher.mk test 

readfullflash:
	JLink-RdFullFlash.bat
	#@make -f flasher.mk readfullflash 

.PHONY:	prerequirement
prerequirement:
	@$(file >DEPENDENCY_LIST.txt,$(DEPENDENCY_LIST))
	
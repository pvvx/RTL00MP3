
all: ram_all
mp: ram_all_mp

.PHONY: ram_all
ram_all:
	@$(MAKE) -f sdkbuild.mk
	@$(MAKE) -f flasher.mk genbin1 genbin23

.PHONY: ram_all_mp
ram_all_mp:
	@$(MAKE) -f sdkbuild.mk mp
	@$(MAKE) -f flasher.mk mp
	
.PHONY: clean  clean_all
clean:
	@$(MAKE) -f sdkbuild.mk clean

clean_all:
	@$(MAKE) -f sdkbuild.mk clean_all
	
.PHONY:	debug ramdebug
debug: 
	@$(MAKE) -f application.mk debug	

ramdebug: 
	@$(MAKE) -f application.mk ramdebug	

.PHONY: flashburn runram reset test readfullflash
flashburn: 
	JLinkGDB-WrFlash.bat
	#@$(MAKE) -f flasher.mk flashburn

runram: 
	JLink-RunRAM.bat
	#@$(MAKE) --f flasher.mk runram

reset: 
	JLink-Reset.bat
	#@make -f flasher.mk reset 

test: 
	JLink-RTL00ConsoleROM.bat
	#@make -f flasher.mk test 

readfullflash:
	JLink-RdFullFlash.bat
	#@make -f flasher.mk readfullflash 

.PHONY:	prerequirement
prerequirement:
#	@$(file >DEPENDENCY_LIST.txt,$(DEPENDENCY_LIST))
	
include userset.mk
include $(SDK_PATH)paths.mk
include project.mk

INCFLAGS = $(patsubst %,-I%,$(patsubst sdk/%,$(SDK_PATH)%,$(INCLUDES)))

LIBFLAGS = $(addprefix -L,$(patsubst sdk/%,$(SDK_PATH)%,$(PATHLIBS))) $(addprefix -l,$(LIBS))

LFLAGS += -Wl,-Map=$(OBJ_DIR)/$(TARGET).map 

CFLAGS += $(INCFLAGS)

SRC_O = $(patsubst %.c,%.o,$(patsubst sdk/%,$(SDK_PATH)%,$(ADD_SRC_C))) $(patsubst %.c,%.o,$(patsubst sdk/%,$(SDK_PATH)%,$(SRC_C)))
DRAM_O = $(patsubst %.c,%.o,$(patsubst sdk/%,$(SDK_PATH)%,$(DRAM_C)))
BOOT_O = $(patsubst %.c,%.o,$(patsubst sdk/%,$(SDK_PATH)%,$(BOOT_C)))

SRC_C_LIST = $(patsubst sdk/%,$(SDK_PATH)%,$(ADD_SRC_C)) $(patsubst sdk/%,$(SDK_PATH)%,$(SRC_C)) $(patsubst sdk/%,$(SDK_PATH)%,$(DRAM_C)) $(patsubst sdk/%,$(SDK_PATH)%,$(BOOT_C))
OBJ_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%.o,$(SRC_C_LIST)))
DEPENDENCY_LIST = $(patsubst %.c,$(OBJ_DIR)/%.d,$(SRC_C_LIST))

TARGET ?= build
OBJ_DIR ?= $(TARGET)/obj
BIN_DIR ?= $(TARGET)/bin
ELFFILE ?= $(OBJ_DIR)/$(TARGET).axf

all: prerequirement application
mp: prerequirement application

.PHONY: build_info
build_info:
	@echo \#define UTS_VERSION \"`date +%Y/%m/%d-%T`\" > .ver
	@echo \#define RTL8195AFW_COMPILE_TIME \"`date +%Y/%m/%d-%T`\" >> .ver
	@echo \#define RTL8195AFW_COMPILE_DATE \"`date +%Y%m%d`\" >> .ver
	@echo \#define RTL8195AFW_COMPILE_BY \"`id -u -n`\" >> .ver
	@echo \#define RTL8195AFW_COMPILE_HOST \"`$(HOSTNAME_APP)`\" >> .ver
	@if [ -x /bin/dnsdomainname ]; then \
		echo \#define RTL8195AFW_COMPILE_DOMAIN \"`dnsdomainname`\"; \
	elif [ -x /bin/domainname ]; then \
		echo \#define RTL8195AFW_COMPILE_DOMAIN \"`domainname`\"; \
	else \
		echo \#define RTL8195AFW_COMPILE_DOMAIN ; \
	fi >> .ver
	@echo \#define RTL195AFW_COMPILER \"gcc `$(CC) $(CFLAGS) -dumpversion | tr --delete '\r'`\" >> .ver
	@mv -f .ver project/inc/$@.h

.PHONY:	application 
application: build_info $(SRC_O) $(DRAM_O) $(BOOT_O)
	@echo "==========================================================="
	@echo "Link ($(TARGET))"
#	@echo "==========================================================="
	@mkdir -p $(BIN_DIR) $(OBJ_DIR)
	@$(file > $(OBJ_DIR)/obj_list.lst,$(OBJ_LIST))
	@$(LD) $(LFLAGS) -o $(ELFFILE) @$(OBJ_DIR)/obj_list.lst $(LIBFLAGS) -T$(LDFILE)
	@$(OBJDUMP) -d $(ELFFILE) > $(OBJ_DIR)/$(TARGET).asm

.PHONY:	prerequirement
#.NOTPARALLEL: prerequirement
prerequirement:
#	@$(file >DEPENDENCY_LIST.txt,$(DEPENDENCY_LIST))
	@echo "==========================================================="
	@echo "Compile ($(TARGET))"
#	@echo "==========================================================="
	@mkdir -p $(OBJ_DIR)

$(SRC_O): %.o : %.c
	@echo $<
	@mkdir -p $(OBJ_DIR)/$(dir $@)
	@$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $(OBJ_DIR)/$@
	@$(CC) -MM $(CFLAGS) $(INCFLAGS) $< -MT $@ -MF $(OBJ_DIR)/$(patsubst %.o,%.d,$@)

$(DRAM_O): %.o : %.c
	@echo $<
	@mkdir -p $(OBJ_DIR)/$(dir $@)
	@$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $(OBJ_DIR)/$@
	@$(OBJCOPY) --prefix-alloc-sections .sdram $(OBJ_DIR)/$@
	@$(CC) -MM $(CFLAGS) $(INCFLAGS) $< -MT $@ -MF $(OBJ_DIR)/$(patsubst %.o,%.d,$@)

$(BOOT_O): %.o : %.c
	@echo $<
	@mkdir -p $(OBJ_DIR)/$(dir $@)
	@$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $(OBJ_DIR)/$@
	@$(OBJCOPY) --prefix-alloc-sections .boot $(OBJ_DIR)/$@
	@$(CC) -MM $(CFLAGS) $(INCFLAGS) $< -MT $@ -MF $(OBJ_DIR)/$(patsubst %.o,%.d,$@)
	
-include $(DEPENDENCY_LIST)

VPATH:=$(OBJ_DIR) $(SDK_PATH)

#.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(OBJ_DIR)/$(SDK_PATH) 
	
	
#=============================================
# SDK CONFIG
#=============================================
#USE_SDRAM = 1
#USE_AT = 1
#USE_FATFS = 1
#USE_SDIOH = 1
#USE_POLARSSL = 1
#USE_P2P_WPS = 1
#USE_GCC_LIB = 1
USE_MBED = 1

ifndef USE_AT
USE_NEWCONSOLE = 1
USE_WIFI_API = 1
endif

#RTOSDIR=freertos_v8.1.2
RTOSDIR=freertos_v9.0.0
LWIPDIR=lwip_v1.4.1
include $(SDK_PATH)sdkset.mk
#compile
CFLAGS += -DLOGUART_STACK_SIZE=1024
#CFLAGS += -DDEFAULT_BAUDRATE=1562500
#=============================================
# User Files
#=============================================
#user main
ADD_SRC_C += project/src/user/main.c
# components
DRAM_C += project/src/user/wifi_console.c
DRAM_C += project/src/user/atcmd_user.c
ADD_SRC_C += project/src/user/spiram_fifo.c

#lib mad
ADD_SRC_C += project/src/mad/mad_version.c
ADD_SRC_C += project/src/mad/mpg12/layer12.c
ADD_SRC_C += project/src/mad/frame.c
ADD_SRC_C += project/src/mad/layer3.c
ADD_SRC_C += project/src/mad/align.c
ADD_SRC_C += project/src/mad/decoder.c
ADD_SRC_C += project/src/mad/huffman.c
ADD_SRC_C += project/src/mad/fixed.c
ADD_SRC_C += project/src/mad/bit.c
ADD_SRC_C += project/src/mad/synth.c
ADD_SRC_C += project/src/mad/timer.c
ADD_SRC_C += project/src/mad/stream.c

#driver
ADD_SRC_C += project/src/driver/i2s_freertos.c

#include
INCLUDES += project/inc/mad



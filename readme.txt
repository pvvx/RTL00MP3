MP3 stereo player V0004
PWM Out GC_2 and PE_2
RTL00 module (RTL8710AF)

Console:
# help
 
WLAN AT COMMAND SET:
==============================
 Set MP3 server
        # ATWS=URL,PATH,PORT
        Sample: ATWS=icecast.omroep.nl/3fm-sb-mp3,80
                ATWS=meuk.spritesserver.nl/Ii.Romanzeandante.mp3,80
                ATWS=?, ATWS=close, ATWS=save, ATWS=read
 Connect to an AES AP
        # ATW0=SSID
        # ATW1=PASSPHRASE
        # ATWC
 DisConnect AP
        # ATWD


# ATST

CLK CPU         166666666 Hz
RAM heap        16824 bytes
RAM free        5912 bytes
TCM heap        408 bytes
TCM ps_monitor  764 bytes
RAM Heap Memory List:
 [0]=0x0x10052278, 0
 [1]=0x0x10003010, 1984
 [2]=0x0x100597a8, 272
 [3]=0x0x1006aea0, 14568
TCM Free List:
 prev 10052468, chunk 1fff5454, size 408

CPU total run time is 1995102
TaskName        DeltaRunTime    percentage
log_servi               0               <1%
tskmad          		330             33%
IDLE            		660             66%
Tmr Svc         		0               <1%
TCP_IP          		0               <1%
cmd_threa               0               <1%
rtw_check               0               <1%
log_uart                0               <1%
rtw_littl               0               <1%
rtw_recv_               0               <1%
tskreader               0               <1%
rtw_inter               0               <1%
rtw_TDMA_               0               <1%
rtw_xmit_               0               <1%

[MEM] After do cmd, available heap 16824+408        


sdk-ameba-rtl8710af-v3.5a_without_NDA_GCC_V1.0.0.zip: readme.txt        
###########################################################################################
###########################################################################################
####################RTL8710_GCC_VERSION_V1.0.0#############################################
###########################################################################################
GCC SDK RTL8710 basic version (including the window platform cygwin installation and Ubuntu platform Linux Installation routines), 
including cross compilation of the installation, compile, link, run, debug, and so on.
SDK implementation of the function:
1, WiFi connection settings (including AP mode and STA mode).
2, peripheral resource control (including GPIO, SPI, UART, IIC, etc.).
3, the user uses the sample method.

SDK directory is as follows, please first read the document:
UM0096 Realtek Ameba-1 build environment setup - gcc.pdf
├── component 
│   ├── common
│   │   ├── api
│   │   ├── application
│   │   ├── drivers
│   │   ├── example
│   │   ├── file_system
│   │   ├── mbed
│   │   ├── network
│   │   ├── test
│   │   └── utilities
│   ├── os
│   │   ├── freertos
│   │   └── os_dep
│   └── soc
│       └── realtek
├── doc
│   ├── AN0004 Realtek low power wi-fi mp user guide.pdf
│   ├── AN0011 Realtek wlan simple configuration.pdf
│   ├── AN0012 Realtek secure socket layer(ssl).pdf
│   ├── AN0025 Realtek at command.pdf
│   ├── AN0033 Realtek Ameba-1 over the air firmware update.pdf
│   ├── AN0038 Realtek googlenest user guide.pdf
│   ├── AN0043 Realtek mdns user guide.pdf
│   ├── AN0045 Realtek Ameba-1 power modes.pdf
│   ├── AN0046 Realtek Ameba uart adapter.pdf
│   ├── AN0049 Realtek Ameba WiGadget iot demo kit application note.pdf
│   ├── AN0075 Realtek Ameba-1 at command v2.0.pdf
│   ├── UM0006 Realtek wificonf application programming interface.pdf
│   ├── UM0014 Realtek web server user guide.pdf
│   ├── UM0023 Realtek Ameba-1 build environment setup - iar.pdf
│   ├── UM0027 Realtek Ameba-1 crypto engine.pdf
│   ├── UM0034 Realtek Ameba-1 memory layout.pdf
│   ├── UM0039 Realtek Ameba-1 SDK quick start.pdf
│   └── UM0096 Realtek Ameba-1 build environment setup - gcc.pdf
├── project
│   └── realtek_ameba1_va0_example
│       ├── example_sources
│       ├── GCC-RELEASE
│       ├── inc
│       └── src
├── release_note.txt
└── tools
    ├── arm-none-eabi-gcc
    │   ├── 4.8.3-2014q1
    │   ├── 4.8.3-2014q1.tar.gz
    │   └── gcc-arm-none-eabi-4_8-2014q1-20140314-linux.tar.bz2
    ├── autopatch
    │   └── auto_patch.bat
    ├── DownloadServer
    │   ├── DownloadServer.exe
    │   ├── readme.txt
    │   └── start.bat
    ├── file_check_sum
    │   └── file_checksum.c
    ├── iperf.exe
    ├── serial_to_usb
    │   └── mbedWinSerial_16466.zip
    ├── simple_config_wizard
    │   ├── Android
    │   └── iOS
    ├── simple_config_wizard_3.4b
    │   ├── Android
    │   └── iOS
    ├── uart_adapter
    │   └── app
    └── wigadget
        ├── Android
        └── iOS

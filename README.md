# RTL00(RTL8710AF) Test MP3 AmebaV3.5a GCC
---

MP3 stereo player V0004<br>
RTL00 module (RTL8710AF)<br>

![SCH](https://github.com/pvvx/RTL00MP3/blob/master/RTL00_MP3_SCH.gif)

Толко Тест!<br>

PWM Out GC_2 and PE_2 (2 канала по 96Bit на один исходный sample 48 кГц)<br>

Console command (RX/TX GB1/GB0 38400 baud):<br>

ATW0=SSID - задать имя AP<br>
ATW1=PASSPHRASE - задать пароль AP<br>
ATWC - Connect to an AES AP<br>
ATWD - DisConnect AP<br>
ATWS=URL,PORT - задать канал web-radio или http файл<br> 
. Sample: ATWS=icecast.omroep.nl/3fm-sb-mp3,80<br>
. ATWS=meuk.spritesserver.nl/Ii.Romanzeandante.mp3,80<br>
. ATWS=?, ATWS=close, ATWS=save, ATWS=read<br>
. ATWS=x,0 и ATWS=save - отключить старт<br>
ATST - Mem/Task Info<br>
ATOF - Отключить MP3<br>

ATSD=hexaddr,count - Damp памяти/регистров с адреса...<br>
ATWD=hexaddr,hexdata - Записать dword по адресу<br>


По умолчанию, в качестве JTAG используется J-Link STLink V2.<br>

Используемое ПО<br>

JTAG/SWD Drivers:<br>
* [ST-Link](http://www.st.com/web/catalog/tools/FM146/CL1984/SC724/SS1677/PF251168?sc=internet/evalboard/product/251168.jsp)<br>
* [SEEGER J-Link Software and Documentation Pack](https://www.segger.com/downloads/jlink)<br>
* [Converting ST-LINK on-board into a J-Link](https://www.segger.com/jlink-st-link.html)<br>
System:<br>
* [MinGW](https://sourceforge.net/projects/mingw-w64/)<br>
* [GNU ARM Embedded Toolchain](https://launchpad.net/gcc-arm-embedded/+download)<br>
* [OpenOCD: Open On-Chip Debugger](https://sourceforge.net/projects/gnuarmeclipse/files/OpenOCD/)<br>
Eclipse:<br>
* [Eclipse IDE for C/C++ Developers](https://eclipse.org/downloads/packages/eclipse-ide-cc-developers/lunasr1a)<br>
* [GNU ARM Eclipse Plug-ins](http://gnuarmeclipse.github.io/downloads/)<br>
* [GNU ARM Eclipse OpenOCD](http://gnuarmeclipse.github.io/openocd/install/)<br>

Изначальные версии для дополнений исходников и SDK:<br> 
от [PADI IoT Stamp Resources – PINE64](https://www.pine64.org/?page_id=946):<br>
* [sdk-ameba-rtl8710af-v3.5a_without_NDA_GCC_V1.0.0.zip](https://yadi.sk/d/dfIwqNkZv6m63)<br>
от [Realtek IoT/Arduino Solution](http://www.amebaiot.com/en/):<br>
* [sdk-ameba1-v3.4b3_without_NDA.zip](https://yadi.sk/d/Yt7iS1KBvUAV9)<br>
из GitHub:<br>
* [Ameba8195/Arduino: This is Arduino SDK for Ameba Arduino board](https://github.com/Ameba8195/Arduino)<br>
+ [rtl_ameba_gcc_sample](https://github.com/eggman/rtl_ameba_gcc_sample)<br>
+ [rebane rtl8710_openocd](https://bitbucket.org/rebane/rtl8710_openocd/src)<br>
 
Доп.информация: [esp8266.ru/forum](https://esp8266.ru/forum/threads/rtl00-mp3-player.1697/) 
 
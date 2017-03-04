# RTL00(RTL8710AF) Test MP3 SDK V3.5.1 GCC
---

MP3 stereo player V0004<br>
Module RTL00(RTL8710AF), [F11AMIM13](http://fn-link.en.made-in-china.com/product/sSinPtAKZBke/China-RTL8711AM-Iot-Module.html) (RTL8711AM), [F11AFIM13-B1](http://fn-link.en.made-in-china.com/product/PSHnuEtJVXWh/China-RTL8711AF-IoT-Module-IEEE-802-11-B-G-N-2-4GHz-1T1R-WiFi-NFC-Module.html) (RTL8711AF)<br>
[PADI](https://www.pine64.org/?page_id=946) (RTL8710AF), [F10AFIM13-B1](http://en.ofeixin.com/products_detail/productId=65.html) (RTL8710AF), [TinyCon2005-A-BE](http://www.ralinwi.com/product.aspx?info_lb=54&flag=1) (RTL8711AF),<br>
[WFM-400](http://www.rayson.com/rayson/en/?pros=product&pros=product&b_cat_id=A03&m_cat_id=A0304&s_cat_id=A030401&prod_id=P0113&level=3) (RTL8711AM), [WFM-410](http://www.rayson.com/rayson/en/?pros=product&pros=product&b_cat_id=A03&m_cat_id=A0304&s_cat_id=A030401&prod_id=P0114&level=3) (RTL8711AF), [WFM-250](http://www.rayson.com/rayson/en/?pros=product&pros=product&b_cat_id=A03&m_cat_id=A0304&s_cat_id=A030401&prod_id=P0112&level=3) (RTL8195AM),<br>
[AW-CU238, AW-CU239](https://www.buyiot.net/pd-1) (RTL8711AM), [AW-CU245, AW-CU245, AW-CU245](https://www.buyiot.net/home-1) (RTL8711AM/RTL8195AM/RTL8711AF),<br>
[WG6611](http://www.jorjin.com/product.php?id=98) (RTL8711AM), [RAK473](http://www.rakwireless.com/en/download/RAK473/Firmware%20Upgrade) (RTL8711AM), [RAK474, RAK476](http://www.rakwireless.com/en/download/RAK473/Firmware%20Upgrade) (RTL8711AF), ...<br> 

RTL00 module (RTL8710AF)<br>

![SCH](https://github.com/pvvx/RTL00MP3/blob/master/RTL00_MP3_SCH.gif)

Толко Тест!<br>

PWM Out GC_2 and PE_2 (2 канала по 96Bit на один исходный sample 48 кГц)<br>

Console command (RX/TX GB1/GB0 38400 baud):<br>
```
CONSOLE COMMAND SET:<br>
==============================
ATPN=<SSID>[,<PASSPHRASE>[,WEPKEY]]: WIFI Connect to AP
ATWS=<URL,PORT>: MP3 Connect to URL
ATWS=<c>[lose]: Close MP3
ATWS=<r>[ead]: Read MP3 URL
ATWS=<s>[ave]: Save MP3 URL
ATWS=<?>: URL Info
ATWD: WIFI Disconnect
ATST: Memory info
ATLW: Lwip Info
ATSB=<ADDRES(hex)>[,COUNT(dec)]: Dump byte register
ATSD=<ADDRES(hex)>[,COUNT(dec)]: Dump dword register
ATSW=<ADDRES(hex)>,<DATA(hex)>: Set register
ATDS=[TIME(ms)]: Deep sleep
ATON: Open connections
ATOF: Close connections
?: This Help
==============================
```


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
 
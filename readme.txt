MP3 stereo player V0005
PWM Out GC_2 and PE_2
RTL00 module (RTL8710AF)

Console:
>?
CONSOLE COMMAND SET:
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
>atst

CLK CPU         83333333 Hz
RAM heap        22808 bytes
TCM heap        27928 bytes

CPU total run time is 3980699
TaskName        DeltaRunTime    percentage
loguart         3126            <1%
IDLE            809857          20%
Tmr Svc         322             <1%
TCP_IP          22486           <1%
rtw_xmit_       3               <1%
cmd_threa       3380            <1%
rtw_check       0               <1%
rtw_littl       11151           <1%
rtw_inter       8980            <1%
rtw_recv_       33723           <1%
tskmad          2988434         75%
tskreader       20133           <1%
rtw_TDMA_       0               <1%

Task List:
==============================
Name      Status Priority HighWaterMark TaskNumber
loguart         R       4       218     2
IDLE            R       0       36      3
Tmr Svc         B       5       438     4
TCP_IP          B       9       637     5
rtw_check       B       5       219     11
tskreader       B       8       130     54
rtw_TDMA_       B       7       219     12
rtw_littl       B       10      439     10
rtw_inter       B       6       209     8
rtw_recv_       B       5       891     6
tskmad          B       7       106     55
rtw_xmit_       B       5       183     7
cmd_threa       B       6       221     9

###########################################################################################
sdk-v3.5a+         

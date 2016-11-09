#ifndef __ATCMD_USR_H__
#define __ATCMD_USR_H__
#include <platform_opts.h>
#ifdef CONFIG_AT_USR
#include "main.h"
#include "lwip_netconf.h"

#ifndef WLAN0_NAME
  #define WLAN0_NAME		"wlan0"
#endif
#ifndef WLAN1_NAME
  #define WLAN1_NAME		"wlan1"
#endif
/* Give default value if not defined */
#ifndef NET_IF_NUM
  #ifdef CONFIG_CONCURRENT_MODE
    #define NET_IF_NUM ((CONFIG_ETHERNET) + (CONFIG_WLAN) + 1)
  #else
    #define NET_IF_NUM ((CONFIG_ETHERNET) + (CONFIG_WLAN))
  #endif  // end of CONFIG_CONCURRENT_MODE
#endif  // end of NET_IF_NUM

/*Static IP ADDRESS*/
#ifndef IP_ADDR0
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   3
#define IP_ADDR3   80
#endif

/*NETMASK*/
#ifndef NETMASK_ADDR0
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef GW_ADDR0
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   3
#define GW_ADDR3   1
#endif

/*Static IP ADDRESS*/
#ifndef AP_IP_ADDR0
#define AP_IP_ADDR0   192
#define AP_IP_ADDR1   168
#define AP_IP_ADDR2   43
#define AP_IP_ADDR3   1
#endif
   
/*NETMASK*/
#ifndef AP_NETMASK_ADDR0
#define AP_NETMASK_ADDR0   255
#define AP_NETMASK_ADDR1   255
#define AP_NETMASK_ADDR2   255
#define AP_NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef AP_GW_ADDR0
#define AP_GW_ADDR0   192
#define AP_GW_ADDR1   168
#define AP_GW_ADDR2   43
#define AP_GW_ADDR3   1  
#endif


#define at_printf(fmt, args...) do{printf(fmt, ##args);}while(0)
#define at_print_data(data, size) do{__rtl_memDump(data, size, NULL);}while(0)

typedef struct _USR_server_setings
{
	u16 port;
	u8 url[128];
} mp3_server_setings;

extern mp3_server_setings mp3_serv;
extern void at_USR_init(void);

#endif // CONFIG_AT_USR

#endif // __ATCMD_WIFI_H__

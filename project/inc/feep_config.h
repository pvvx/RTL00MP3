/*
 * feep_config.h
 *
 *  Created on: 06 нояб. 2016 г.
 *      Author: PVV
 */

#ifndef _INC_FEEP_CONFIG_H_
#define _INC_FEEP_CONFIG_H_

#define FEEP_ID_WIFI_CFG 0x5730 // id:'0W', type: struct wlan_fast_reconnect
#define FEEP_ID_WIFI_AP_CFG 0x5731 // id:'1W', type: struct rtw_wifi_config_t
#define FEEP_ID_UART_CFG 0x5530 // id:'0U', type: UART_LOG_CONF
#define FEEP_ID_LWIP_CFG 0x4C30 // id:'0L', type: struct atcmd_lwip_conf
#define FEEP_ID_DHCP_CFG 0x4430 // id:'0D', type: struct

typedef struct _sdhcp_cfg {
	  u8  mode; // =0 dhcp off, =1 - dhcp on, =2 Static ip, =3 - auto
	  u32 ip;
	  u32 mask;
	  u32 gw;
}dhcp_cfg;

/*
#define FEEP_WRITE_WIFI_CFG(x)  flash_write_cfg(x, FEEP_ID_WIFI_CFG, sizeof(struct wlan_fast_reconnect))
#define FEEP_READ_WIFI_CFG(x)  flash_read_cfg(x, FEEP_ID_WIFI_CFG, sizeof(struct wlan_fast_reconnect))
*/


#endif /* _INC_FEEP_CONFIG_H_ */

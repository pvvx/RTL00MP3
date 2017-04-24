/*
 * wifi_user_set.h
 *
 *  Created on: 01/04/2017
 *      Author: pvvx
 */

#ifndef _WIFI_USER_SET_H_
#define _WIFI_USER_SET_H_

//=========================================
//==== Wlan Config ========================
#define DEF_WIFI_MODE 		RTW_MODE_STA 		// Стартовый режим WiFi: RTW_MODE_STA_AP, RTW_MODE_AP, RTW_MODE_STA, RTW_MODE_NONE
#define DEF_WIFI_AP_STATIONS 3  				// Max number of STAs, should be 1..3, default is 3
#define DEF_WIFI_COUNTRY	RTW_COUNTRY_RU		// Страна для установки органичений каналов и прочего...
#define DEF_WIFI_TX_PWR		RTW_TX_PWR_PERCENTAGE_50 // RTW_TX_PWR_PERCENTAGE_75 // RTW_TX_PWR_PERCENTAGE_100
#define DEF_WIFI_BGN		RTW_NETWORK_BGN		// rtw_network_mode_t
#define DEF_WIFI_ST_SLEEP	0 					// 0 - none, 1 - on

#define DEF_LOAD_CFG ( 0 \
	| BID_WIFI_AP_CFG \
	| BID_WIFI_ST_CFG \
	| BID_AP_DHCP_CFG \
	| BID_ST_DHCP_CFG \
)//	| BID_WIFI_CFG \

#define DEF_SAVE_CFG ( 0 \
	| BID_WIFI_AP_CFG \
	| BID_WIFI_ST_CFG \
	| BID_AP_DHCP_CFG \
	| BID_ST_DHCP_CFG \
	| BID_WIFI_CFG \
)
//==== Interface 0 - wlan0 = AP ===========
#define DEF_AP_SSID			"RTL871X"
#define DEF_AP_PASSWORD		"0123456789"
#define DEF_AP_SECURITY		RTW_SECURITY_WPA2_AES_PSK // RTW_SECURITY_OPEN, RTW_SECURITY_WEP_PSK
#define DEF_AP_BEACON		100 // 100...6000 ms
#define DEF_AP_CHANNEL		1	// 1..14
#define DEF_AP_CHANNEL		1	// 1..14
#define DEF_AP_DHCP_MODE	1	// =0 dhcp off, =1 - dhcp on
#define DEF_AP_IP 			IP4ADDR(192,168,4,1)
#define DEF_AP_MSK			IP4ADDR(255,255,255,0)
#define DEF_AP_GW			IP4ADDR(192,168,4,1)
#define DEF_AP_DHCP_START	2
#define DEF_AP_DHCP_STOP	15
//==== Interface 1 - wlan1 = STA ==========
#define DEF_ST_SSID			"HOMEAP"
#define DEF_ST_PASSWORD		"0123456789"
#define DEF_ST_SECURITY		RTW_SECURITY_WPA_WPA2_MIXED
#define DEF_ST_BSSID		{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } // If bssid set is not ff.ff.ff.ff.ff.ff,
// station will connect to the router with both ssid[] and bssid[] matched.
#define DEF_ST_CHANNEL		1	// 1..14
#define DEF_ST_AUTORECONNECT   3 // 0 - none, 1..254 - count, 255 - all
#define DEF_ST_RECONNECT_PAUSE 1 // 1 sec
#define DEF_ST_DHCP_MODE	1 	// =0 dhcp off, =1 - dhcp on, =2 Static ip, =3 - auto
#define DEF_ST_IP			IP4ADDR(192,168,1,100)
#define DEF_ST_MSK			IP4ADDR(255,255,255,0)
#define DEF_ST_GW			IP4ADDR(192,168,1,1)
//==== Interface 2 - eth0 =================
#define DEF_EH_DHCP_MODE	1	// =0 dhcp off, =1 - dhcp on
#define DEF_EH_IP 			IP4ADDR(192,168,7,200)
#define DEF_EH_MSK			IP4ADDR(255,255,255,0)
#define DEF_EH_GW			IP4ADDR(192,168,7,1)
//=========================================

#endif /* _WIFI_USER_SET_H_ */

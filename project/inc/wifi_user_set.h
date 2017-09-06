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
#define DEF_WIFI_MODE 		RTW_MODE_STA_AP		// Стартовый режим WiFi: RTW_MODE_STA_AP, RTW_MODE_AP, RTW_MODE_STA, RTW_MODE_NONE
#define DEF_WIFI_AP_STATIONS 3					// Max number of STAs, should be 1..3, default is 3
#define DEF_WIFI_COUNTRY	RTW_COUNTRY_RU		// Регион использования WiFi...
#define DEF_WIFI_TX_PWR		RTW_TX_PWR_PERCENTAGE_25 // RTW_TX_PWR_PERCENTAGE_75 // RTW_TX_PWR_PERCENTAGE_100
#define DEF_WIFI_BGN		RTW_NETWORK_BGN		// rtw_network_mode_t
#define DEF_WIFI_ADAPTIVITY	RTW_ADAPTIVITY_DISABLE // RTW_ADAPTIVITY_NORMAL // RTW_ADAPTIVITY_DISABLE/RTW_ADAPTIVITY_NORMAL/RTW_ADAPTIVITY_CARRIER_SENSE

/* Опции загрузки конфигов по старту */
//#define DEF_LOAD_CFG 0								// старт в назначенном режиме по умолчанию
//#define DEF_LOAD_CFG BID_ALL_WIFI_CFG 				// старт в назначенном общем  режиме по умолчанию, с загрузками последних конфигураций ST и AP
#define DEF_LOAD_CFG (BID_ALL_WIFI_CFG | BID_WIFI_CFG)	// старт в записанном режиме

/* Опции разрешения записи конфигов по успешному соединению или выполнению операции установок режимов WiFi без ошибок  */
#define DEF_SAVE_CFG (BID_ALL_WIFI_CFG | BID_WIFI_CFG)	// сохранение по успешному соединению/выполненю для всех конфигов:
														// (для ST или AP пишутся раздельно) с проверкой на изменения
//==== Interface 0 - wlan0 = AP ===========
#define DEF_AP_SSID			"RTL871X"
#define DEF_AP_PASSWORD		"0123456789"
/*		 Варианты типов Security для AP:
  		 RTW_SECURITY_OPEN - Open Security
		 RTW_SECURITY_WPA_TKIP_PSK   - WPA Security
		 RTW_SECURITY_WPA2_AES_PSK   - WPA2 Security using AES cipher
		 RTW_SECURITY_WPA2_MIXED_PSK - WPA2 Security using AES and/or TKIP ciphers */
#define DEF_AP_SECURITY		RTW_ENCRYPTION_WPA2_MIXED // WEP security is NOT IMPLEMENTED. It is NOT SECURE!
#define DEF_AP_BEACON		100 	// 100...6000 ms
#define DEF_AP_CHANNEL		1		// 1..14
#define DEF_AP_CHANNEL		1		// 1..14
#define DEF_AP_DHCP_MODE	1		// =0 dhcp off, =1 - dhcp on
#define DEF_AP_IP 			IP4ADDR(192,168,4,1)
#define DEF_AP_MSK			IP4ADDR(255,255,255,0)
#define DEF_AP_GW			IP4ADDR(192,168,4,1)
// if not defined DHCP_START && DHCP_STOP -> 2..255
//#define DEF_AP_DHCP_START	2		// DHCP ip start xx.xx.xx.2
//#define DEF_AP_DHCP_STOP	15		// DHCP ip stop xx.xx.xx.15
//==== Interface 1 - wlan1 = STA ==========
#define DEF_ST_SSID			"HOMEAP"		// Имя SSID AP (роутера) для присоединения по умолчанию (первый старт)
#define DEF_ST_PASSWORD		"0123456789"	// Пароль AP (роутера) для присоединения по умолчанию (первый старт)
#define DEF_ST_SECURITY		RTW_ENCRYPTION_WPA2_MIXED // Тип Security
#define DEF_ST_BSSID		{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } // If bssid set is not ff.ff.ff.ff.ff.ff,
#define DEF_ST_USE_BSSID	0 			// station will connect to the router with both ssid[] and bssid[] matched.
#define DEF_ST_CHANNEL		1			// 1..14
#define DEF_ST_AUTORECONNECT   1 		// 0 - none, 1..254 - count, 255 - all
#define DEF_ST_RECONNECT_PAUSE 1 		// 5 sec
#define DEF_ST_SLEEP		3			// 0 - Off, 1 - IPS, 2 - LPS, 3 - IPS/LPS mode
#define DEF_ST_LPS_DTIM		0			// LPS DTIM
#define DEF_ST_DHCP_MODE	1 			// =0 dhcp off,
										// =1 - dhcp on, - динамический ip
										// =2 Static ip, - fixed ip
										// =3 - auto fix - старт в режиме "dhcp on", после получения ip - фиксация и переключение в режим рестартов со "Static ip"
#define DEF_ST_IP			IP4ADDR(192,168,1,100)
#define DEF_ST_MSK			IP4ADDR(255,255,255,0)
#define DEF_ST_GW			IP4ADDR(192,168,1,1)
//==== Interface 2 - eth0 =================
#define DEF_EH_DHCP_MODE	1		// =0 dhcp off, =1 - dhcp on
#define DEF_EH_IP 			IP4ADDR(192,168,7,200)
#define DEF_EH_MSK			IP4ADDR(255,255,255,0)
#define DEF_EH_GW			IP4ADDR(192,168,7,1)
//=========================================

#endif /* _WIFI_USER_SET_H_ */

/*
 * wifi_user_set.h
 *
 *  Created on: 01/04/2017
 *      Author: pvvx
 */

#ifndef _WIFI_API_H_
#define _WIFI_API_H_
#include "wifi_constants.h"

#define ip4_addr1(ipaddr) (((uint8_t*)(ipaddr))[0])
#define ip4_addr2(ipaddr) (((uint8_t*)(ipaddr))[1])
#define ip4_addr3(ipaddr) (((uint8_t*)(ipaddr))[2])
#define ip4_addr4(ipaddr) (((uint8_t*)(ipaddr))[3])

#define IPSTR "%d.%d.%d.%d"

#define IP2STR(ipaddr) \
	ip4_addr1(ipaddr), \
    ip4_addr2(ipaddr), \
    ip4_addr3(ipaddr), \
    ip4_addr4(ipaddr)

extern char str_rom_57ch3Dch0A[]; // "=========================================================\n" 57

#define BID_WIFI_AP_CFG		(1 << 0)
#define BID_WIFI_ST_CFG		(1 << 1)
#define BID_AP_DHCP_CFG		(1 << 2)
#define BID_ST_DHCP_CFG		(1 << 3)
#define BID_WIFI_CFG		(1 << 4)

#define WLAN_ST_NETIF_NUM  wlan_st_netifn
#define WLAN_AP_NETIF_NUM  wlan_ap_netifn
//==== FEEP_ID ===========================
#define FEEP_ID_WIFI_CFG	0x4347 // id:'GC'
#define FEEP_ID_WIFI_AP_CFG	0x5041 // id:'AP'
#define FEEP_ID_WIFI_ST_CFG	0x5453 // id:'ST'
#define FEEP_ID_AP_DHCP_CFG	0x4144 // id:'DA'
#define FEEP_ID_ST_DHCP_CFG	0x5344 // id:'DS'
#define FEEP_ID_UART_CFG	0x5530 // id:'0U', type: UART_LOG_CONF
#define FEEP_ID_LWIP_CFG	0x4C30 // id:'0L', type: struct atcmd_lwip_conf
#define FEEP_ID_DHCP_CFG	0x4430 // id:'0D', type: struct
//=========================================
#define IW_PASSPHRASE_MAX_SIZE		64
#define NDIS_802_11_LENGTH_SSID		32
#define IP4ADDR(a,b,c,d) (((unsigned int)((d) & 0xff) << 24) | \
                         ((unsigned int)((c) & 0xff) << 16) | \
                         ((unsigned int)((b) & 0xff) << 8)  | \
                          (unsigned int)((a) & 0xff))
//=========================================
//--- Wlan Config struct-------------------
typedef struct _wifi_config {
	unsigned char mode;			// rtw_mode_t
	unsigned char sleep;
	unsigned char country_code;	// rtw_country_code_t
	unsigned char tx_pwr;		// rtw_tx_pwr_percentage_t
	unsigned char bgn;			// 802.11 rtw_network_mode_t
	unsigned char load_flg;
	unsigned char save_flg;
} WIFI_CONFIG, *PWIFI_CONFIG;
//---- Interface 0 - wlan0 - AP - struct --
typedef struct _softap_config {
	unsigned char ssid[NDIS_802_11_LENGTH_SSID];
	unsigned char password[IW_PASSPHRASE_MAX_SIZE];
	rtw_security_t security_type; // RTW_SECURITY_OPEN, RTW_SECURITY_WEP_PSK
	uint16 beacon_interval;		// Note: support 100 ~ 60000 ms, default 100
	unsigned char channel;		// 1..14
	unsigned char ssid_hidden;	// Note: default 0
	unsigned char max_sta;		// 1..3
} SOFTAP_CONFIG, *PSOFTAP_CONFIG;
//---- Interface 1 - wlan1 - ST - struct -
typedef struct _station_config {
	unsigned char ssid[NDIS_802_11_LENGTH_SSID];
	unsigned char password[IW_PASSPHRASE_MAX_SIZE];
	rtw_security_t security_type;
	unsigned char bssid[6];			// Note: If bssid set is not ff.ff.ff.ff.ff.ff || 00:00:00:00:00:00
	unsigned char autoreconnect;	// 0 - none, 1..254 - count, 255 - all
	unsigned char reconnect_pause; 	// in sec
	unsigned char flg; 				// station will connect to the router with both ssid[], else if set flg - bssid[] matched.
// rtw_adaptivity_mode_t
} STATION_CONFIG, *PSTATION_CONFIG;
//--- LwIP Config -------------------------
struct lwip_conn_info {
	int32_t role; 				//client, server or seed
	unsigned int protocol; 		//tcp or udp
	unsigned int remote_addr; 	//remote ip
	unsigned int remote_port; 	//remote port
	unsigned int local_addr; 	//locale ip, not used yet
	unsigned int local_port; 	//locale port, not used yet
	unsigned int reserved; 		//reserve for further use
};
//--- DHCP Config -------------------------
typedef struct _dhcp_config {
	unsigned int ip;
	unsigned int mask;
	unsigned int gw;
	unsigned char mode; // =0 dhcp off, =1 - dhcp on, =2 Static ip, =3 - auto
} DHCP_CONFIG, *PDHCP_CONFIG;


extern WIFI_CONFIG wifi_cfg;
extern SOFTAP_CONFIG wifi_ap_cfg;
extern DHCP_CONFIG wifi_ap_dhcp;
extern STATION_CONFIG wifi_st_cfg;
extern DHCP_CONFIG wifi_st_dhcp;
extern rtw_mode_t wifi_run_mode;
extern rtw_mode_t wifi_mode;
extern char wlan_st_name[];
extern char wlan_ap_name[];
extern char wlan_st_netifn;
extern char wlan_ap_netifn;


void show_wifi_ap_ip(void);
void show_wifi_st_ip(void);
void show_wifi_cfg(void);
void show_wifi_st_cfg(void);
void show_wifi_ap_cfg(void);
uint32 read_wifi_cfg(uint32 flg);
uint32 write_wifi_cfg(uint32 flg);
int wifi_run(rtw_mode_t mode);
void wifi_init(void);

void _LwIP_Init(void);

#endif // _WIFI_API_H_

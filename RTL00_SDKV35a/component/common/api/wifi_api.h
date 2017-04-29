/*
 * wifi_user_set.h
 *
 *  Created on: 01/04/2017
 *      Author: pvvx
 */

#ifndef _WIFI_API_H_
#define _WIFI_API_H_
#include "wifi_constants.h"
#include "queue.h"

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
#define BID_AP_HOSTNAME		(1 << 5)
#define BID_ST_HOSTNAME		(1 << 6)

#define BID_ALL_WIFI_CFG	(BID_WIFI_AP_CFG|BID_WIFI_ST_CFG|BID_AP_DHCP_CFG|BID_ST_DHCP_CFG|BID_AP_HOSTNAME|BID_ST_HOSTNAME)

#define WLAN_ST_NETIF_NUM  wlan_st_netifn
#define WLAN_AP_NETIF_NUM  wlan_ap_netifn
//==== FEEP_ID ===========================
#define FEEP_ID_WIFI_CFG	0x4347 // id:'GC'
#define FEEP_ID_WIFI_AP_CFG	0x5041 // id:'AP'
#define FEEP_ID_WIFI_ST_CFG	0x5453 // id:'ST'
#define FEEP_ID_AP_DHCP_CFG	0x4144 // id:'DA'
#define FEEP_ID_ST_DHCP_CFG	0x5344 // id:'DS'
#define FEEP_ID_AP_HOSTNAME	0x4148 // id:'HA'
#define FEEP_ID_ST_HOSTNAME 0x5348 // id:'HP'
//#define FEEP_ID_UART_CFG	0x5530 // id:'0U', type: UART_LOG_CONF
//#define FEEP_ID_LWIP_CFG	0x4C30 // id:'0L', type: struct atcmd_lwip_conf
//#define FEEP_ID_DHCP_CFG	0x4430 // id:'0D', type: struct
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
	unsigned char adaptivity;	// rtw_adaptivity_mode_t
	unsigned char country_code;	// rtw_country_code_t
	unsigned char tx_pwr;		// rtw_tx_pwr_percentage_t
	unsigned char bgn;			// 802.11 rtw_network_mode_t
	unsigned char load_flg;		// см. BID_WIFI_CFG..
	unsigned char save_flg;
} WIFI_CONFIG, *PWIFI_CONFIG;
//---- Interface 0 - wlan0 - AP - struct --
typedef struct _softap_config {
	unsigned char ssid[NDIS_802_11_LENGTH_SSID+1];
	unsigned char password[IW_PASSPHRASE_MAX_SIZE+1];
	unsigned short beacon_interval;	// Note: support 100 ~ 60000 ms, default 100
	unsigned char ssid_hidden;	// Note: default 0
	unsigned char security; 	// 0 = RTW_SECURITY_OPEN, 1 = RTW_SECURITY_WPA2_AES_PSK
	unsigned char channel;		// 1..14
	unsigned char max_sta;		// 1..3
} SOFTAP_CONFIG, *PSOFTAP_CONFIG;
//---- Interface 1 - wlan1 - ST - struct -
typedef struct _station_config {
	unsigned char ssid[NDIS_802_11_LENGTH_SSID+1];
	unsigned char password[IW_PASSPHRASE_MAX_SIZE+1];
	unsigned char bssid[6];			// Note: If bssid set is not ff.ff.ff.ff.ff.ff || 00:00:00:00:00:00
	unsigned char flg; 				// station will connect to the router with both ssid[], else if set flg - bssid[] matched.
	unsigned char security;			// IDX_SECURITY
	unsigned char autoreconnect;	// 0 - none, 1..254 - count, 255 - all
	unsigned char reconnect_pause; 	// in sec
	unsigned char sleep;			// 0 - Off, 1 - IPS, 2 - LPS, 3 - IPS/LPS mode
	unsigned char dtim; 			// LPS DTIM (2..)
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
extern unsigned char wifi_run_mode; // rtw_mode_t
extern unsigned char wifi_mode; // rtw_mode_t
extern unsigned char wifi_st_status; // WIFI_STA_ENUM
extern char wlan_st_name[];
extern char wlan_ap_name[];
extern char wlan_st_netifn;
extern char wlan_ap_netifn;

/* WiFi Station & scan security */
typedef enum {
	IDX_SECURITY_OPEN = 0,			//0 Open security
	IDX_SECURITY_WEP_PSK,			//1 WEP Security with open authentication
	IDX_SECURITY_WEP_SHARED,		//2 WEP Security with shared authentication
	IDX_SECURITY_WPA_TKIP_PSK,		//3 WPA Security with TKIP
	IDX_SECURITY_WPA_AES_PSK,		//4 WPA Security with AES
	IDX_SECURITY_WPA2_TKIP_PSK,		//5 WPA2 Security with TKIP
	IDX_SECURITY_WPA2_AES_PSK,		//6 WPA2 Security with AES
	IDX_SECURITY_WPA2_MIXED_PSK,	//7 WPA2 Security with AES & TKIP
	IDX_SECURITY_WPA_WPA2_MIXED,	//8 WPA/WPA2 Security
	IDX_SECURITY_UNKNOWN			//9
} IDX_SECURITY;

/* wifi_st_status */
typedef enum {
	WIFI_STA_OFF,
	WIFI_STA_START,
	WIFI_STA_RECONNECT,
	WIFI_STA_CONNECTED
} WIFI_STA_ENUM;


uint8 * get_new_hostname(void);
uint32 get_new_ip(void);

void show_wifi_ap_ip(void);
void show_wifi_st_ip(void);
void show_wifi_cfg(void);
void show_wifi_st_cfg(void);
void show_wifi_ap_cfg(void);
uint32 read_wifi_cfg(uint32 flg);
uint32 write_wifi_cfg(uint32 flg);
int wifi_run(rtw_mode_t mode);
void wifi_init(void);

rtw_security_t idx_to_rtw_security(unsigned char idx);
unsigned char rtw_security_to_idx(rtw_security_t rtw_sec_type);
unsigned char * rtw_security_to_str(rtw_security_t rtw_sec_type);
unsigned char * idx_security_to_str(unsigned char idx);

/* -------- Api WiFi Scan ------------------------------- */
#include "wifi_conf.h"
#include "timers.h"

#define SCAN_CHANNELS 14
#define	MAX_AP_SIZE 32

extern QueueHandle_t xQueueWebSrv;

typedef struct web_scan_handler{
	TimerHandle_t timer;
	rtw_scan_result_t * ap_details;
	unsigned char ap_count;
	unsigned char start_show;
	unsigned char used_data;
	volatile unsigned char	flg;
} web_scan_handler_t;
extern web_scan_handler_t web_scan_handler_ptr;
typedef rtw_result_t (*api_scan_result_handler_t)(internal_scan_handler_t * ap_scan_result);
rtw_result_t api_wifi_scan(api_scan_result_handler_t scan_result_cb);
void wifi_close_scan(void);


#endif // _WIFI_API_H_

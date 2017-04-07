/*
 * wifi_console.c
 *
 *  Created on: 03/04/2017
 *      Author: pvvx
 */

#include <autoconf.h>
#include "FreeRTOS.h"
#include "diag.h"
#include "wifi_api.h"
#include "rtl8195a/rtl_libc.h"
#include "hal_platform.h"

#include "section_config.h"
#include "hal_diag.h"
#include "lwip/netif.h"


extern struct netif xnetif[NET_IF_NUM];

//==========================================================
//--- CONSOLE --------------------------

// ATPN=<SSID>[,password[,encryption[,auto reconnect[,reconnect pause]]]: WIFI Connect to AP
LOCAL void fATPN(int argc, char *argv[]){
	if(argc > 1) {
		if(argv[1][0] == '?') {
			show_wifi_st_cfg();
		}
		else {
			strncpy(wifi_st_cfg.ssid, argv[1], NDIS_802_11_LENGTH_SSID);
			if(argc > 2) {
				strncpy(wifi_st_cfg.password, argv[2], NDIS_802_11_LENGTH_SSID);
				int i = strlen(wifi_st_cfg.password);
				if(i > 7) {
					wifi_st_cfg.security_type = RTW_SECURITY_WPA2_AES_PSK;
				}
				else if(!i) {
					wifi_st_cfg.security_type = RTW_SECURITY_OPEN;
				}
				else {
					printf("password len < 8!\n");
					wifi_st_cfg.security_type = RTW_SECURITY_OPEN;
				}
			}
			else {
				wifi_st_cfg.password[0] = 0;
				wifi_st_cfg.security_type = RTW_SECURITY_OPEN;
			}
			if(argc > 3) {
				wifi_st_cfg.security_type = translate_rtw_security(atoi(argv[3]));
			}
			if(argc > 4) {
				wifi_st_cfg.autoreconnect = atoi(argv[3]);
			}
			else wifi_st_cfg.autoreconnect = 0;
			if(argc > 5) {
				wifi_st_cfg.reconnect_pause = atoi(argv[3]);
			}
			else wifi_st_cfg.reconnect_pause = 5;
			show_wifi_st_cfg();
			wifi_run(wifi_run_mode | RTW_MODE_STA);
		}
	}
}

// ATPA=<SSID>[,password[,encryption[,channel[,hidden[,max connections]]]]]: Start WIFI AP
LOCAL void fATPA(int argc, char *argv[]){
	if(argc > 1) {
		if(argv[1][0] == '?') {
			show_wifi_ap_cfg();
		}
		else {
			strncpy(wifi_ap_cfg.ssid, argv[1], NDIS_802_11_LENGTH_SSID);
			if(argc > 2) {
				strncpy(wifi_ap_cfg.password, argv[2], NDIS_802_11_LENGTH_SSID);
				int i = strlen(wifi_ap_cfg.password);
				if(i > 7) {
					wifi_ap_cfg.security_type = RTW_SECURITY_WPA2_AES_PSK;
				}
				else if(i == 0) {
					wifi_ap_cfg.security_type = RTW_SECURITY_OPEN;
				}
				else {
					printf("password len < 8!\n");
					wifi_ap_cfg.security_type = RTW_SECURITY_OPEN;
				}
			}
			else {
				wifi_ap_cfg.password[0] = 0;
				wifi_ap_cfg.security_type = RTW_SECURITY_OPEN;
			}
			if(argc > 3) {
				wifi_ap_cfg.security_type = translate_rtw_security(atoi(argv[3]));
			}
			if(argc > 4) {
				wifi_ap_cfg.channel = atoi(argv[4]);
			}
			else wifi_ap_cfg.channel = 1;
			if(argc > 5) {
				wifi_ap_cfg.ssid_hidden = atoi(argv[5]);
			}
			else wifi_ap_cfg.ssid_hidden = 0;

			if(argc > 6) {
				wifi_ap_cfg.max_sta = atoi(argv[6]);
			}
			else wifi_ap_cfg.max_sta = 3;

			show_wifi_ap_cfg();
			wifi_run(wifi_run_mode | RTW_MODE_AP);
		}
	}
}

// WIFI Connect, Disconnect
LOCAL void fATWR(int argc, char *argv[]){
	rtw_mode_t mode = RTW_MODE_NONE;
	if(argc > 1) mode = atoi(argv[1]);
	wifi_run(mode);
}

// Close connections
LOCAL void fATOF(int argc, char *argv[]){
	connect_close();
}

// Open connections
LOCAL void fATON(int argc, char *argv[]){
	connect_start();
}

LOCAL void fATWI(int argc, char *argv[]) {
	rtw_wifi_setting_t Setting;
	if((wifi_run_mode & RTW_MODE_AP)
		&& wifi_get_setting(wlan_ap_name, &Setting) == 0) {
		wifi_show_setting(wlan_ap_name, &Setting);
//		show_wifi_ap_ip();
		printf("\tIP: " IPSTR "\n", IP2STR(&xnetif[WLAN_AP_NETIF_NUM].ip_addr));
	}
	if((wifi_run_mode & RTW_MODE_STA)
		&& wifi_get_setting(wlan_st_name, &Setting) == 0) {
		wifi_show_setting(wlan_st_name, &Setting);
//		show_wifi_st_ip();
		printf("\tIP: " IPSTR "\n", IP2STR(&xnetif[WLAN_ST_NETIF_NUM].ip_addr));
	}
	printf("\nWIFI config:\n");
	printf(&str_rom_57ch3Dch0A[25]); // "================================\n"
	show_wifi_cfg();
	printf("\nWIFI AP config:\n");
	printf(&str_rom_57ch3Dch0A[25]); // "================================\n"
	show_wifi_ap_cfg();
	printf("\nWIFI ST config:\n");
	printf(&str_rom_57ch3Dch0A[25]); // "================================\n"
	show_wifi_st_cfg();
	printf("\n");
#if 1
	if(argc > 2) {
		uint8_t c = argv[1][0] | 0x20;
		if(c == 's') {
			int i = atoi(argv[2]);
			printf("Save configs(%d)..\n", i);
			write_wifi_cfg(atoi(argv[2]));
		}
		else if(c == 'l') {
			wifi_cfg.load_flg = atoi(argv[2]);
		}
		else if(c == 'm') {
			wifi_cfg.mode = atoi(argv[2]);
		}
	}
#endif
}

extern uint8_t rtw_power_percentage_idx;

LOCAL void fATWT(int argc, char *argv[]) {
	if(argc > 1) {
		int txpwr = atoi(argv[1]);
		debug_printf("set tx power (%d)...\n", txpwr);
		if(rltk_set_tx_power_percentage(txpwr) != RTW_SUCCESS) {
			error_printf("Error set tx power (%d)!", wifi_cfg.tx_pwr);
		}
	}
	printf("TX power = %d\n", rtw_power_percentage_idx);
}

//-- Test tsf (64-bits counts, 1 us step) ---

#include "hal_com_reg.h"

#define ReadTSF_Lo32() (*((volatile unsigned int *)(WIFI_REG_BASE + REG_TSFTR)))
#define ReadTSF_Hi32() (*((volatile unsigned int *)(WIFI_REG_BASE + REG_TSFTR1)))

LOCAL uint64_t get_tsf(void)
{
	return *((uint64_t *)(WIFI_REG_BASE + REG_TSFTR));
}

LOCAL void fATSF(int argc, char *argv[])
{
	uint64_t tsf = get_tsf();
	printf("\nTSF: %08x%08x\n", (uint32_t)(tsf>>32), (uint32_t)(tsf));
}

/* --------  WiFi Scan ------------------------------- */
unsigned char *tab_txt_rtw_secyrity[] = {
		"OPEN   ",
		"WEP    ",
		"WPA TKIP",
		"WPA AES",
		"WPA2 AES",
		"WPA2 TKIP",
		"WPA2 Mixed",
		"WPA/WPA2 AES",
		"Unknown"
};
unsigned int *tab_code_rtw_secyrity[] = {
		RTW_SECURITY_OPEN,
		RTW_SECURITY_WEP_PSK,
		RTW_SECURITY_WPA_TKIP_PSK,
		RTW_SECURITY_WPA_AES_PSK,
		RTW_SECURITY_WPA2_AES_PSK,
		RTW_SECURITY_WPA2_TKIP_PSK,
		RTW_SECURITY_WPA2_MIXED_PSK,
		RTW_SECURITY_WPA_WPA2_MIXED,
		RTW_SECURITY_UNKNOWN
};

volatile uint8_t scan_end;

/* --------  WiFi Scan ------------------------------- */
LOCAL rtw_result_t _scan_result_handler( rtw_scan_handler_result_t* malloced_scan_result )
{
	if (malloced_scan_result->scan_complete != RTW_TRUE) {
		rtw_scan_result_t* record = &malloced_scan_result->ap_details;
		record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */
		if(scan_end == 1) {
			printf("\nScan networks:\n\n");
			printf("N\tType\tMAC\t\t\tSignal\tCh\tWPS\tSecyrity\tSSID\n\n");
		};
		printf("%d\t", scan_end++);
	    printf("%s\t", (record->bss_type == RTW_BSS_TYPE_ADHOC)? "Adhoc": "Infra");
	    printf(MAC_FMT, MAC_ARG(record->BSSID.octet));
	    printf("\t%d\t", record->signal_strength);
	    printf("%d\t", record->channel);
	    printf("%d\t", record->wps_type);
	    int i = 0;
	    for(; record->security != tab_code_rtw_secyrity[i] && tab_code_rtw_secyrity[i] != RTW_SECURITY_UNKNOWN; i++);
	    printf("%s \t", tab_txt_rtw_secyrity[i]);
	    printf("%s\n", record->SSID.val);
	} else {
		scan_end = 0;
		printf("\n");
	}
	return RTW_SUCCESS;
}
/* --------  WiFi Scan ------------------------------- */
#define scan_channels 14
LOCAL void fATSN(int argc, char *argv[])
{
	int i;
	u8 *channel_list = (u8*)pvPortMalloc(scan_channels*2);
	if(channel_list) {
		scan_end = 1;
		u8 * pscan_config = &channel_list[scan_channels];
		//parse command channel list
		for(i = 1; i <= scan_channels; i++){
			*(channel_list + i - 1) = i;
			*(pscan_config + i - 1) = PSCAN_ENABLE;
		};
		if(wifi_set_pscan_chan(channel_list, pscan_config, scan_channels) < 0){
		    printf("ERROR: wifi set partial scan channel fail\n");
		} else if(wifi_scan_networks(_scan_result_handler, NULL ) != RTW_SUCCESS){
			printf("ERROR: wifi scan failed\n");
		} else {
			i = 300;
			while(i-- && scan_end) {
				vTaskDelay(10);
			};
		};
		vPortFree(channel_list);
	} else {
		printf("ERROR: Can't malloc memory for channel list\n");
	};
}

MON_RAM_TAB_SECTION COMMAND_TABLE console_cmd_wifi_api[] = {
		{"ATPN", 1, fATPN, "=<SSID>[,password[,encryption[,auto-reconnect[,reconnect pause]]]: WIFI Connect to AP"},
		{"ATPA", 1, fATPA, "=<SSID>[,password[,encryption[,channel[,hidden[,max connections]]]]]: Start WIFI AP"},
		{"ATWR", 0, fATWR, ": WIFI Connect, Disconnect"},
//		{"ATON", 0, fATON, ": Open connections"},
//		{"ATOF", 0, fATOF, ": Close connections"},
		{"ATWI", 0, fATWI, ": WiFi Info"},
#if CONFIG_DEBUG_LOG > 3
		{"ATWT", 1, fATWT, "=<tx_power>: WiFi tx power: 0 - 100%, 1 - 75%, 2 - 50%, 3 - 25%, 4 - 12.5%"},
		{"ATSF", 0, fATSF, ": Test TSF value"},
#endif
		{"ATSN", 0, fATSN, ": Scan networks"}
};



/*
 * wifi_api.c
 *
 *  Created on: 01/11/2017
 *      Author: pvvx
 */
#include "FreeRTOS.h"
#include <autoconf.h>
#include "main.h"

#include <platform_opts.h>

#if CONFIG_EXAMPLE_WLAN_FAST_CONNECT
#error "Udnef CONFIG_EXAMPLE_WLAN_FAST_CONNECT!"
#endif
#ifndef USE_FLASH_EEP
#error "Define USE_FLASH_EEP!"
#endif

#include "task.h"
#include <platform/platform_stdlib.h>
#include <wifi/wifi_conf.h>
#include "flash_api.h"
#include <lwip_netconf.h>
#include "dhcp/dhcps.h"
#include "ethernetif.h"
#if CONFIG_ETHERNET
#include "ethernet_mii/ethernet_mii.h"
#endif

#include "wlan_lib.h"

#include "flash_eep.h"
#include "feep_config.h"

#include "wifi_api.h"
#include "wifi_user_set.h"

//=========================================
//--- Wlan Config Init-------------------
WIFI_CONFIG wifi_cfg = {
		.mode = DEF_WIFI_MODE,		// rtw_mode_t
		.sleep = DEF_WIFI_ST_SLEEP,
		.country_code = DEF_WIFI_COUNTRY,// rtw_country_code_t
		.tx_pwr = DEF_WIFI_TX_PWR,	// rtw_tx_pwr_percentage_t
		.bgn = DEF_WIFI_BGN,		// rtw_network_mode_t
		.load_flg = DEF_LOAD_CFG,
		.save_flg = DEF_SAVE_CFG
};
//---- Interface 0 - wlan0 - AP - init ---
SOFTAP_CONFIG wifi_ap_cfg = {
		.ssid = DEF_AP_SSID,
		.password = DEF_AP_PASSWORD,
		.security_type = DEF_AP_SECURITY,
		.beacon_interval = DEF_AP_BEACON,
		.channel = DEF_AP_CHANNEL,
		.ssid_hidden = 0,
		.max_sta = DEF_WIFI_AP_STATIONS	// Max number of STAs, should be 1..3, default is 3
};
DHCP_CONFIG wifi_ap_dhcp = {
		.ip = DEF_AP_IP,
		.mask = DEF_AP_MSK,
		.gw = DEF_AP_GW,
		.mode = 2
};
//---- Interface 1 - wlan1 - ST - init ---
STATION_CONFIG wifi_st_cfg = {
		.ssid = DEF_ST_SSID,
		.password = DEF_ST_PASSWORD,
		.bssid = DEF_ST_BSSID,
		.security_type = DEF_ST_SECURITY,
		.autoreconnect = DEF_ST_AUTORECONNECT,
		.reconnect_pause =	DEF_ST_RECONNECT_PAUSE
};
DHCP_CONFIG wifi_st_dhcp = {
		.ip = DEF_ST_IP,
		.mask = DEF_ST_MSK,
		.gw = DEF_ST_GW,
		.mode = 1
};

rtw_mode_t wifi_run_mode = RTW_MODE_NONE;

typedef struct _feep_element {
	uint16	id;
	uint16	size;
	void * obj;
} FEEP_ELEMENT, *PFEEP_ELEMENT;

FEEP_ELEMENT feep_tab[] = {
		{ FEEP_ID_WIFI_AP_CFG,	sizeof(wifi_ap_cfg),	&wifi_ap_cfg },		// Bit0 BID_WIFI_AP_CFG
		{ FEEP_ID_WIFI_ST_CFG,	sizeof(wifi_st_cfg),	&wifi_st_cfg },		// Bit1 BID_WIFI_ST_CFG
		{ FEEP_ID_AP_DHCP_CFG,	sizeof(wifi_ap_dhcp),	&wifi_ap_dhcp },	// Bit2 BID_AP_DHCP_CFG
		{ FEEP_ID_ST_DHCP_CFG,	sizeof(wifi_st_dhcp),	&wifi_st_dhcp },	// Bit3 BID_ST_DHCP_CFG
		{ FEEP_ID_WIFI_CFG,		sizeof(wifi_cfg), 		&wifi_cfg },		// Bit4 BID_WIFI_CFG
//		{ FEEP_ID_LWIP_CFG,		sizeof(lwip_conn_info),	&lwip_conn_info },	// Bit5
		{ 0, 0, NULL }
};

uint32 read_wifi_cfg(uint32 flg)
{
	uint32 ret = 0;
	PFEEP_ELEMENT p = feep_tab;
	for(int m = 1; m && p->id != 0; m <<= 1, p++) {
		if((flg & m)
			&& flash_read_cfg(p->obj, p->id, p->size) < p->size) {
			ret |= m;
		};
	};
	return ret;
}

uint32 write_wifi_cfg(uint32 flg)
{
	uint32 ret = 0;
	PFEEP_ELEMENT p = feep_tab;
	for(int m = 1; m && p->id != 0; m <<= 1, p++) {
		if((flg & m)
			&& flash_write_cfg(p->obj, p->id, p->size) < p->size) {
			ret |= m;
		};
	};
	return ret;
}

typedef int (*wlan_init_done_ptr)(void);
typedef int (*write_reconnect_ptr)(uint8_t *data, uint32_t len);
//Function
#if CONFIG_AUTO_RECONNECT
extern void (*p_wlan_autoreconnect_hdl)(rtw_security_t, char*, int, char*, int,
		int);
#endif
extern wlan_init_done_ptr p_wlan_init_done_callback;
extern write_reconnect_ptr p_write_reconnect_ptr;
extern struct netif xnetif[NET_IF_NUM];

_WEAK void connect_start(void)
{
	info_printf("\%s: Time at start %d ms.\n", __func__, xTaskGetTickCount());
}

_WEAK void connect_close(void)
{
	info_printf("\%s: Time at start %d ms.\n", __func__, xTaskGetTickCount());
}

int wlan_init_done_callback(void) {
	info_printf("WiFi Init after %d ms\n", xTaskGetTickCount());
	return 0;
}

//char wlan_st_name[] = WLAN0_NAME;
char wlan_st_name[] = WLAN0_NAME;
char wlan_ap_name[] = WLAN1_NAME;
char wlan_st_netifn = 0;
char wlan_ap_netifn = 1;
extern rtw_mode_t wifi_mode;	// новый режим работы

uint8 chk_ap_netif_num(void)
{
	if (wifi_mode == RTW_MODE_AP) {
		wlan_st_name[4] = '1';
		wlan_ap_name[4] = '0';
		wlan_st_netifn = 1;
		wlan_ap_netifn = 0;
	}
	else { // if (wifi_mode == RTW_MODE_STA) {
		wlan_st_name[4] = '0';
		wlan_ap_name[4] = '1';
		wlan_st_netifn = 0;
		wlan_ap_netifn = 1;
	}
	return wlan_ap_netifn;
}

rtw_result_t wifi_run_ap(void) {
	chk_ap_netif_num();

	rtw_result_t ret = RTW_NOTAP;
	if( (wifi_mode == RTW_MODE_AP) || (wifi_mode == RTW_MODE_STA_AP) ){
		info_printf("Starting AP (%s, netif%d)...\n", wlan_ap_name, wlan_ap_netifn);
		netif_set_addr(&xnetif[WLAN_AP_NETIF_NUM], &wifi_ap_dhcp.ip,
			&wifi_ap_dhcp.mask, &wifi_ap_dhcp.gw);
		if(wext_set_sta_num(wifi_ap_cfg.max_sta) != 0) { // Max number of STAs, should be 1..3, default is 3
			error_printf("AP not set max connections %d!\n", wifi_ap_cfg.max_sta);
		};
		ret = wifi_start_ap(wifi_ap_cfg.ssid,			//char  *ssid,
				wifi_ap_cfg.security_type,	//rtw_security_t ecurity_type,
				wifi_ap_cfg.password, 		//char *password,
				strlen(wifi_ap_cfg.ssid),			//int ssid_len,
				strlen(wifi_ap_cfg.password), 	//int password_len,
				wifi_ap_cfg.channel);			//int channel
		wifi_run_mode |= RTW_MODE_AP;
		if (ret != RTW_SUCCESS) {
			error_printf("Error(%d): Start AP failed!\n\n", ret);;
		} else {
			int timeout = 10000 / 200;
			while (1) {
				char essid[33];
				if (wext_get_ssid(wlan_ap_name, (unsigned char *) essid) > 0) {
					if (strcmp((const char * ) essid,
							(const char * )wifi_ap_cfg.ssid)
							== 0) {
#ifdef CONFIG_DONT_CARE_TP
						pnetiff->flags |= NETIF_FLAG_IPSWITCH;
#endif
						dhcps_ip4addr_pool_start  = DEF_AP_DHCP_START;
						dhcps_ip4addr_pool_end = DEF_AP_DHCP_STOP;
						dhcps_init(&xnetif[WLAN_AP_NETIF_NUM]);
						info_printf("AP '%s' started after %d ms\n",
								wifi_ap_cfg.ssid, xTaskGetTickCount());
						show_wifi_ap_ip();
						if(wifi_cfg.save_flg & (BID_WIFI_AP_CFG | BID_AP_DHCP_CFG))
							write_wifi_cfg(wifi_cfg.save_flg & (BID_WIFI_AP_CFG | BID_AP_DHCP_CFG));
						ret = RTW_SUCCESS;
						break;
					}
				}
				if (timeout == 0) {
					error_printf("Start AP timeout!\n");
					ret = RTW_TIMEOUT;
					break;
				}
				vTaskDelay(200 / portTICK_RATE_MS);
				timeout--;
			}
		}
	}
	return ret;
}

rtw_result_t StartStDHCPClient(void)
{
	debug_printf("Start DHCPClient...\n");
	int ret = RTW_SUCCESS;
	struct netif * pnetif = &xnetif[WLAN_ST_NETIF_NUM];
	DHCP_CONFIG *p = (dhcp_cfg *)&wifi_st_dhcp;
	unsigned char mode = p->mode;
	if((mode == 3)
	&& (p->ip == IP4ADDR(255,255,255,255)
			|| p->ip == IP4ADDR(0,0,0,0))) {
		mode = 2;
	}
	else mode = 1;
	if(mode == 2) {
			netif_set_addr(pnetif, (ip_addr_t *)&p->ip, (ip_addr_t *)&p->mask, (ip_addr_t *)&p->gw);
//			dhcps_init(pnetif);
	}
	else if(mode) {
		UBaseType_t savePriority =  uxTaskPriorityGet(NULL);
		/* If not rise priority, LwIP DHCP may timeout */
		vTaskPrioritySet(NULL, tskIDLE_PRIORITY + 3);
		/* Start DHCP Client */
		ret = LwIP_DHCP(WLAN_ST_NETIF_NUM, DHCP_START);
		vTaskPrioritySet(NULL, savePriority);
		if (ret == DHCP_ADDRESS_ASSIGNED) {
			p->ip = pnetif->ip_addr.addr;
			p->gw = pnetif->gw.addr;
			p->mask = pnetif->netmask.addr;
			if(p->mode == 3) {
				p->mode = 2;
				write_wifi_cfg(BID_ST_DHCP_CFG);
			}
			ret = RTW_SUCCESS;
		}
		else {
			debug_printf("LwIP_DHCP ret=%d\n", ret);
			ret = RTW_ERROR;
		}
	}
	return ret;
}

rtw_result_t wifi_run_st(void) {
	rtw_result_t ret = RTW_SUCCESS;
	chk_ap_netif_num();
	if((wifi_mode == RTW_MODE_STA) || (wifi_mode == RTW_MODE_STA_AP)) {
#if CONFIG_AUTO_RECONNECT
//		p_wlan_autoreconnect_hdl = NULL;
		if (wifi_st_cfg.autoreconnect) {
			ret = wifi_config_autoreconnect(1, wifi_st_cfg.autoreconnect,
					wifi_st_cfg.reconnect_pause);
			if (ret != RTW_SUCCESS)
				warning_printf("ERROR: Operation failed! Error=%d\n", ret);
		}
#endif
		info_printf("Connected to AP (%s, netif%d)...\n", wlan_st_name, wlan_st_netifn);
		ret = wifi_connect(wifi_st_cfg.ssid, wifi_st_cfg.security_type,
				wifi_st_cfg.password, strlen(wifi_st_cfg.ssid),
				strlen(wifi_st_cfg.password), -1, NULL);
		wifi_run_mode |= RTW_MODE_STA;
		if (ret != RTW_SUCCESS) {
			error_printf("%s: Operation failed! Error(%d)\n", __func__, ret);
		} else {
			if(wifi_cfg.save_flg & BID_WIFI_ST_CFG)
				write_wifi_cfg(BID_WIFI_ST_CFG);
			// Start DHCPClient
			ret = StartStDHCPClient();
			if(ret == RTW_SUCCESS) {
				show_wifi_st_ip();
#if CONFIG_WLAN_CONNECT_CB
			//	extern void connect_start(void);
				connect_start();
#endif
			}
		}
	};
	return ret;
}

int _wifi_on(rtw_mode_t mode) {
	int ret = 0;
	uint32 timeout = xTaskGetTickCount();
	uint8 devnum;

	if (rltk_wlan_running(WLAN0_IDX)) {
		warning_printf("WIFI is already running\n");
		return 0;
	}

	static int event_init = 0;
	if (event_init == 0) {
		init_event_callback_list();
		event_init = 1;
	}
	wifi_mode = mode;
	info_printf("Initializing WIFI...\n");
	// set wifi mib
	// adaptivity
	wext_set_adaptivity(RTW_ADAPTIVITY_DISABLE);
//	wext_set_adaptivity(RTW_ADAPTIVITY_NORMAL);
//	wext_set_adaptivity(RTW_ADAPTIVITY_CARRIER_SENSE);

	devnum = (mode == RTW_MODE_STA_AP);

	ret = rltk_wlan_init(0, mode);
	if (ret < 0) return ret;
	if(devnum) {
		ret = rltk_wlan_init(1, mode);
		if (ret < 0) return ret;
	}
	rltk_wlan_start(0);
	if(devnum) rltk_wlan_start(1);
	while (1) {
		if (rltk_wlan_running(0) &&
			rltk_wlan_running(devnum) ) {
#if CONFIG_DEBUG_LOG > 2
			printf("WIFI initialized (%d ms)\n", xTaskGetTickCount() - timeout);
#endif
			break;
		}
		if(xTaskGetTickCount() - timeout > wifi_test_timeout_ms/portTICK_RATE_MS) {
			error_printf("WIFI init timeout!\n");
			break;
		}
		vTaskDelay(wifi_test_timeout_step_ms / portTICK_RATE_MS);
	}
	return ret;
}

extern int lwip_init_done;

void _LwIP_Init(void)
{
	if(!lwip_init_done) {
		int idx;
		debug_printf("LwIP Init (%d)\n", wifi_mode);
		/* Create tcp_ip stack thread */
		tcpip_init( NULL, NULL );

		chk_ap_netif_num(); // Исполняется после _wifi_on()
		for(idx = 0; idx < NET_IF_NUM; idx++) {
			xnetif[idx].name[0] = 'r';
			xnetif[idx].name[1] = '0' + idx;
		}
		netif_add(&xnetif[WLAN_ST_NETIF_NUM], (struct netif *)&wifi_st_dhcp.ip, (struct netif *)&wifi_st_dhcp.mask, (struct netif *)&wifi_st_dhcp.gw, NULL, &ethernetif_init, &tcpip_input);
		netif_add(&xnetif[WLAN_AP_NETIF_NUM], (struct netif *)&wifi_ap_dhcp.ip, (struct netif *)&wifi_ap_dhcp.mask, (struct netif *)&wifi_ap_dhcp.gw, NULL, &ethernetif_init, &tcpip_input);
	#if CONFIG_ETHERNET // && NET_IF_NUM > 2
		{
				struct ip_addr ipaddr;
				struct ip_addr netmask;
				struct ip_addr gw;
				ipaddr.addr = DEF_EH_IP;
				netmask.addr = DEF_EH_MSK;
				gw.addr = DEF_EH_GW;
				netif_add(&xnetif[2], &ipaddr, &netmask, &gw, NULL, &ethernetif_mii_init, &tcpip_input);
		}
	#endif
		/*  Registers the default network interface. */
		netif_set_default(&xnetif[0]);
		/*  When the netif is fully configured this function must be called.*/
		for(idx = 0; idx < NET_IF_NUM; idx++) {
			netif_set_up(&xnetif[idx]);
		}
		info_printf("interface %d is initialized\n", idx);
		wifi_mode = 0;
		lwip_init_done = 1;
	}
}

int wifi_run(rtw_mode_t mode) {
	int ret = 0;
#if CONFIG_DEBUG_LOG > 4
	debug_printf("\n%s(%d), %d\n", __func__, mode, wifi_run_mode);
#endif
	if(mode != RTW_MODE_NONE) {
		_LwIP_Init();
	};
	if(wifi_run_mode & RTW_MODE_AP) {
		info_printf("Deinit old AP...\n");
		LwIP_DHCP(WLAN_AP_NETIF_NUM, DHCP_STOP);
#if CONFIG_DEBUG_LOG > 4
		debug_printf("dhcps_deinit()\n");
#endif
		dhcps_deinit();
		wifi_run_mode &= ~RTW_MODE_AP;
	};
	if(wifi_run_mode & RTW_MODE_STA) {
		info_printf("Deinit old ST...\n");
		LwIP_DHCP(WLAN_ST_NETIF_NUM, DHCP_STOP);
		wifi_run_mode &= ~RTW_MODE_STA;
	};
//	if(mode != wifi_mode)
//	wifi_mode = mode;
//	chk_ap_netif_num();
	if (mode != RTW_MODE_NONE) {
		if(mode != (volatile)wifi_mode) {
			info_printf("Deinitializing WIFI ...\n");
			wifi_off();
			vTaskDelay(30);
			debug_printf("WiFi_on(%d)\n", mode);
			if (_wifi_on(mode) < 0) {
				error_printf("Wifi On failed!\n");
				goto error_end;
			};
			wifi_mode =	mode;
		};
		if(wifi_set_country(wifi_cfg.country_code) != RTW_SUCCESS) {
			error_printf("Error set tx country_code (%d)!", wifi_cfg.country_code);
		};
//	extern uint8_t rtw_power_percentage_idx;
		if(rtw_power_percentage_idx != wifi_cfg.tx_pwr) {
			if(rltk_set_tx_power_percentage(wifi_cfg.tx_pwr) != RTW_SUCCESS) {
				error_printf("Error set tx power (%d)!", wifi_cfg.tx_pwr);
			};
		}
		debug_printf("mode == wifi_mode? (%d == %d?)\n", mode, wifi_mode);
//		if(mode == wifi_mode)
		{
			if((wifi_mode == RTW_MODE_AP) || (wifi_mode == RTW_MODE_STA_AP)) {
				wifi_run_ap();
			};
			if((wifi_mode == RTW_MODE_STA) || (wifi_mode == RTW_MODE_STA_AP)) {
				wifi_run_st();
			};
#if CONFIG_INTERACTIVE_MODE
			/* Initial uart rx swmaphore*/
			vSemaphoreCreateBinary(uart_rx_interrupt_sema);
			xSemaphoreTake(uart_rx_interrupt_sema, 1/portTICK_RATE_MS);
			start_interactive_mode();
#endif
//			if(wifi_run_mode == wifi_cfg.mode)
			ret = 1;
		};
	} else {
		ret = 1;
error_end:
#if CONFIG_WLAN_CONNECT_CB
		connect_close();
#endif
		wifi_off();
	};
	chk_ap_netif_num();
	return ret;
}

void wifi_init(void) {
	debug_printf("\nLoad Config\n");
	read_wifi_cfg(wifi_cfg.load_flg); // DEF_LOAD_CFG
	// Call back from wlan driver after wlan init done
	p_wlan_init_done_callback = wlan_init_done_callback;
	// Call back from application layer after wifi_connection success
//		p_write_reconnect_ptr = wlan_write_reconnect_data_to_flash;
	p_wlan_autoreconnect_hdl = NULL;
	if (wifi_cfg.mode != RTW_MODE_NONE) {
		wifi_mode = wifi_cfg.mode;
#if CONFIG_WIFI_IND_USE_THREAD
		wifi_manager_init();
#endif
		wifi_run(wifi_cfg.mode);
	}
}

rtw_security_t translate_rtw_security(u8 security_type)
{
	rtw_security_t security_mode = RTW_SECURITY_OPEN;

	switch (security_type) {
//  	case RTW_ENCRYPTION_OPEN:
//    		security_mode = RTW_SECURITY_OPEN;
//    	break;
  	case RTW_ENCRYPTION_WEP40:
  	case RTW_ENCRYPTION_WEP104:
    		security_mode = RTW_SECURITY_WEP_PSK;
    	break;
  	case RTW_ENCRYPTION_WPA_TKIP:
  	case RTW_ENCRYPTION_WPA_AES:
  	case RTW_ENCRYPTION_WPA2_TKIP:
  	case RTW_ENCRYPTION_WPA2_AES:
  	case RTW_ENCRYPTION_WPA2_MIXED:
    		security_mode = RTW_SECURITY_WPA2_AES_PSK;
    	break;
//  	case RTW_ENCRYPTION_UNKNOWN:
//  	case RTW_ENCRYPTION_UNDEF:
//  	default:
  		//security_mode = RTW_SECURITY_OPEN;
	}
	return security_mode;
}

void show_wifi_ap_ip(void) {
	printf("SoftAP ip: " IPSTR "\n", IP2STR(&xnetif[WLAN_AP_NETIF_NUM].ip_addr));
}
void show_wifi_st_ip(void) {
	printf("Station ip: " IPSTR "\n", IP2STR(&xnetif[WLAN_ST_NETIF_NUM].ip_addr));
}

void show_wifi_st_cfg(void) {
	printf("\tSSID: '%s'\n", wifi_st_cfg.ssid);
	printf("\tPassword: '%s'\n", wifi_st_cfg.password);
	printf("\tSecurity type: %p\n", wifi_st_cfg.security_type);
	printf("\tAuto-reconnect: %d\n", wifi_st_cfg.autoreconnect);
	printf("\tReconnect pause: %d\n", wifi_st_cfg.reconnect_pause);
}

void show_wifi_ap_cfg(void) {
	printf("\tSSID: '%s'\n", wifi_ap_cfg.ssid);
	printf("\tSSID hidden: %d\n", wifi_ap_cfg.ssid_hidden);
	printf("\tPassword: '%s'\n", wifi_ap_cfg.password);
	printf("\tSecurity type: %p\n", wifi_ap_cfg.security_type);
	printf("\tChannel: %d\n", wifi_ap_cfg.channel);
	printf("\tBeacon interval: %d ms\n", wifi_ap_cfg.beacon_interval);
	printf("\tMax connections: %d\n", wifi_ap_cfg.max_sta);
}

void show_wifi_cfg(void) {
	printf("\tStart mode: %p\n", wifi_cfg.mode);
	printf("\tCountry code: %d\n", wifi_cfg.country_code);
	printf("\tSleep mode: %p\n", wifi_cfg.sleep);
	printf("\tNetwork mode: %d\n", wifi_cfg.bgn);
	printf("\tTx power: %d\n", wifi_cfg.tx_pwr);
	printf("\tLoad flags: %p\n", wifi_cfg.load_flg);
	printf("\tSave flags: %p\n", wifi_cfg.save_flg);
}


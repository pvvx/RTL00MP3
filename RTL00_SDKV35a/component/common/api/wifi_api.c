/*
 * wifi_api.c
 *
 *  Created on: 01/11/2017
 *      Author: pvvx
 */
#include <platform_opts.h>
#include "rtl8195a.h"
#include "drv_types.h"
#include <autoconf.h>
#include "FreeRTOS.h"
#if 1
#include "drv_types.h" // or #include "wlan_lib.h"
#else
#include "wifi_constants.h"
#include "wifi_structures.h"
//#include "wlan_lib.h" // or #include "drv_types.h"
#endif


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

#include "flash_eep.h"
#include "feep_config.h"

#include "wifi_api.h"
#include "main.h"
#include "wifi_user_set.h"

#if 0
#undef debug_printf
#define debug_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#undef info_printf
#define info_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#undef warning_printf
#define warning_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#undef error_printf
#define error_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#endif

#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
extern void cmd_ap_wps(int argc, char **argv);
extern int wpas_wps_dev_config(u8 *dev_addr, u8 bregistrar);
#endif //CONFIG_WPS_AP

//=========================================
//--- Wlan Config Init-------------------
WIFI_CONFIG wifi_cfg = {
		.mode = DEF_WIFI_MODE,		// rtw_mode_t
		.adaptivity = DEF_WIFI_ADAPTIVITY, // rtw_adaptivity_mode_t
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
		.security = DEF_AP_SECURITY, // RTW_SECURITY_WPA2_AES_PSK or RTW_SECURITY_OPEN
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
		.flg = DEF_ST_BSSID,
		.security = DEF_ST_SECURITY,
		.autoreconnect = DEF_ST_AUTORECONNECT,
		.reconnect_pause =	DEF_ST_RECONNECT_PAUSE,
		.sleep = DEF_ST_SLEEP,
		.dtim = DEF_ST_LPS_DTIM
};
DHCP_CONFIG wifi_st_dhcp = {
		.ip = DEF_ST_IP,
		.mask = DEF_ST_MSK,
		.gw = DEF_ST_GW,
		.mode = 1
};

unsigned char wifi_run_mode = RTW_MODE_NONE; // rtw_mode_t
unsigned char wifi_st_status;

typedef int (*wlan_init_done_ptr)(void);
typedef int (*write_reconnect_ptr)(uint8_t *data, uint32_t len);
//Function
#if CONFIG_AUTO_RECONNECT
extern void (*p_wlan_autoreconnect_hdl)(rtw_security_t, char*, int, char*, int,	int);
#endif
extern wlan_init_done_ptr p_wlan_init_done_callback;
//extern write_reconnect_ptr p_write_reconnect_ptr;
extern struct netif xnetif[NET_IF_NUM];
extern uint8_t rtw_power_percentage_idx;
extern Rltk_wlan_t rltk_wlan_info[2]; // in wrapper.h

#define PARM_RECONNECT 1

struct wifi_autoreconnect_param {
#if	PARM_RECONNECT
	rtw_security_t security_type;
	int key_id;
#else
	rtw_security_t security_type;
	char *ssid;
	int ssid_len;
	char *password;
	int password_len;
	int key_id;
#endif
};

struct wifi_autoreconnect_param wifi_autoreconnect;

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
		{ FEEP_ID_AP_HOSTNAME,	LWIP_NETIF_HOSTNAME_SIZE,	&lwip_host_name[1] },	// Bit5 BID_AP_HOSTNAME
		{ FEEP_ID_ST_HOSTNAME,	LWIP_NETIF_HOSTNAME_SIZE,	&lwip_host_name[0] },	// Bit5 BID_ST_HOSTNAME
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
		if(flg & m) {
			if(!flash_write_cfg(p->obj, p->id, p->size)) {
				ret |= m;
			};
		};
	};
	return ret;
}

#if CONFIG_WLAN_CONNECT_CB
_WEAK void connect_start(void)
{
	info_printf("\%s: Time at start %d ms.\n", __func__, xTaskGetTickCount());
}

_WEAK void connect_close(void)
{
	info_printf("\%s: Time at start %d ms.\n", __func__, xTaskGetTickCount());
}
#endif

#ifdef NOT_USE_CALLS
LOCAL int wlan_init_done_callback(void) {
	printf("WiFi Init after %d ms\n", xTaskGetTickCount());
	return 0;
}
#endif // #ifdef NOT_USE_CALLS

//char wlan_st_name[] = WLAN0_NAME;
char wlan_st_name[] = WLAN0_NAME;
char wlan_ap_name[] = WLAN1_NAME;
char wlan_st_netifn = 0;
char wlan_ap_netifn = 1;


uint32 get_new_ip(void)
{
	if(!(wifi_cfg.mode ^ wifi_run_mode)) {
		return current_netif->ip_addr.addr;
	} else if(wifi_cfg.mode == RTW_MODE_AP) {
		return wifi_ap_dhcp.ip;
	} else if(wifi_st_dhcp.mode == 2) {
		return wifi_st_dhcp.ip;
	}
	return 0;
}
uint8 * get_new_hostname(void)
{
	if(!(wifi_cfg.mode ^ wifi_run_mode)) {
		return current_netif->hostname;
	} else if(wifi_cfg.mode == RTW_MODE_AP) {
		return lwip_host_name[1];
	};
	return lwip_host_name[0];
}

LOCAL uint8 chk_ap_netif_num(void)
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

extern Rltk_wlan_t rltk_wlan_info[2]; // in wrapper.h

/*LOCAL _adapter * get_padapter(int num) {
	if(rltk_wlan_info[num].enable) {
		return *(_adapter **)((rltk_wlan_info[0].dev)->priv);
	}
	return NULL;
};*/
#define get_padapter(num) (*(_adapter **)((rltk_wlan_info[num].dev)->priv));

LOCAL rtw_result_t _wext_set_lps_dtim(int adapter_num, uint8 lps_dtim ) {
	_adapter * pad =	get_padapter(adapter_num);
	rtw_result_t ret = RTW_ERROR;
	if(pad) {
		ret = rtw_pm_set_lps_dtim(pad, lps_dtim);
	}
	return ret;
}

LOCAL rtw_result_t _wext_enable_powersave(int adapter_num, uint8 ips_mode, uint8 lps_mode) {
	_adapter * pad =	get_padapter(adapter_num);
	rtw_result_t ret = RTW_ERROR;
	if(pad) {
		ret = rtw_pm_set_ips(pad, ips_mode); // 2 режима 1,2 !
		if(ret == RTW_SUCCESS) {
			LeaveAllPowerSaveMode(pad);
			ret = rtw_pm_set_lps(pad, lps_mode);
		}
	}
	return ret;
}

LOCAL int _wext_cmp_ssid(int adapter_num, uint8 *ssid)
{
	_adapter * pad = get_padapter(adapter_num);
	int ret = 0;
	if((pad != NULL) && (pad->mlmepriv.fw_state & 0x41) != 0) {
		int len = pad->mlmepriv.cur_network.network.Ssid.SsidLength;
		if(len < 32) len++;
		else len = 32;
		ret = (rtl_memcmp(ssid, &pad->mlmepriv.cur_network.network.Ssid.Ssid, len) == 0);
		debug_printf("%d s[%d]'%s'\n", pad->mlmepriv.fw_state, len, ssid);
	}
	return ret;
}

#ifdef NOT_USE_CALLS

LOCAL rtw_result_t _wext_get_mode(int adapter_num, int *mode) {
	_adapter * pad =	get_padapter(adapter_num);
	rtw_result_t ret = RTW_ERROR;
	if(pad) {
		uint16 f = pad->mlmepriv.fw_state;
		if(f & 8) *mode = 2;
		else if(f & 0x60) *mode = 1;
		else if(!(f & 0x10)) *mode = 0;
		else *mode = 3;
		ret = RTW_SUCCESS;
	}
	return ret;
}

LOCAL rtw_result_t _wext_get_channel(int adapter_num, uint8 *ch)
{
	_adapter * pad = get_padapter(adapter_num);
	rtw_result_t ret = RTW_ERROR;
	if(pad) {
		if(pad->mlmepriv.fw_state & 1) {
			*ch = pad->mlmepriv.htpriv.ch_offset;
		}
		else {
			*ch = pad->mlmeextpriv.cur_channel;
		}
		ret = RTW_SUCCESS;
	}
	return ret;
}

#endif // #ifdef NOT_USE_CALLS


LOCAL rtw_result_t wifi_run_ap(void) {
	rtw_result_t ret = RTW_NOTAP;
	if( (wifi_mode == RTW_MODE_AP) || (wifi_mode == RTW_MODE_STA_AP) ){
		info_printf("Starting AP (%s, netif%d)...\n", wlan_ap_name, wlan_ap_netifn);
/*
		netif_set_addr(&xnetif[WLAN_AP_NETIF_NUM], &wifi_ap_dhcp.ip,
			&wifi_ap_dhcp.mask, &wifi_ap_dhcp.gw);
*/
		if(wext_set_sta_num(wifi_ap_cfg.max_sta) != 0) { // Max number of STAs, should be 1..3, default is 3
			error_printf("AP not set max connections %d!\n", wifi_ap_cfg.max_sta);
		};
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
		// todo: ветка не доделана
		wpas_wps_dev_config(xnetif[WLAN_AP_NETIF_NUM].hwaddr, 1);
#endif //CONFIG_WPS_AP
		if(wifi_ap_cfg.channel == 0 || wifi_ap_cfg.channel > 14) {
			wifi_ap_cfg.channel = 1;
		}
		ret = wifi_start_ap(wifi_ap_cfg.ssid,	//char  *ssid,
				wifi_ap_cfg.security,			//rtw_security_t ecurity_type,
				wifi_ap_cfg.password, 			//char *password,
				wifi_ap_cfg.channel,			//int channel
				wifi_ap_cfg.ssid_hidden);		//
//		wifi_run_mode |= RTW_MODE_AP;
		if (ret != RTW_SUCCESS) {
			error_printf("Error(%d): Start AP failed!\n\n", ret);;
		} else {
			int timeout = wifi_test_timeout_ms / wifi_test_timeout_step_ms;
			while (1) {
#if 1
				if (_wext_cmp_ssid(WLAN_AP_NETIF_NUM, &wifi_ap_cfg.ssid )) {
#else
				char essid[33];
				if ((wext_get_ssid(wlan_ap_name, (unsigned char *) essid) > 0)
					&&(strcmp((const char * ) essid, (const char * )wifi_ap_cfg.ssid) == 0)) {
#endif
#ifdef CONFIG_DONT_CARE_TP
						pnetiff->flags |= NETIF_FLAG_IPSWITCH;
#endif
						if(wifi_ap_dhcp.mode) {
#if defined(DEF_AP_DHCP_START) && defined(DEF_AP_DHCP_STOP)
							dhcps_ip4addr_pool_start  = DEF_AP_DHCP_START;
							dhcps_ip4addr_pool_end = DEF_AP_DHCP_STOP;
#endif
							dhcps_init(&xnetif[WLAN_AP_NETIF_NUM]);
						};
						info_printf("AP '%s' started after %d ms\n",
								wifi_ap_cfg.ssid, xTaskGetTickCount());
						show_wifi_ap_ip();
						if(wifi_cfg.save_flg & (BID_WIFI_AP_CFG | BID_AP_DHCP_CFG))
							write_wifi_cfg(wifi_cfg.save_flg & (BID_WIFI_AP_CFG | BID_AP_DHCP_CFG));
						ret = RTW_SUCCESS;
#if CONFIG_WLAN_CONNECT_CB
						//	extern void connect_start(void);
						connect_start();
#endif
						break;
				}
				if (timeout == 0) {
					error_printf("Start AP timeout!\n");
					ret = RTW_TIMEOUT;
					break;
				}
				vTaskDelay(wifi_test_timeout_step_ms / portTICK_RATE_MS);
				timeout--;
			}
		}
	}
	return ret;

}

LOCAL rtw_result_t StartStDHCPClient(void)
{
	debug_printf("Start DHCPClient...\n");
	int ret = RTW_SUCCESS;
	struct netif * pnetif = &xnetif[WLAN_ST_NETIF_NUM];
	DHCP_CONFIG *p = (dhcp_cfg *)&wifi_st_dhcp;
	unsigned char mode = p->mode;
	if((mode == 3) // Auto fix
	&& p->ip != IP4ADDR(255,255,255,255)
	&& p->ip != IP4ADDR(0,0,0,0)) {
	}
	else mode = 1; // DHCP On
	if(mode == 2) { // fixed ip
			netif_set_addr(pnetif, (ip_addr_t *)&p->ip, (ip_addr_t *)&p->mask, (ip_addr_t *)&p->gw);
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
			if(p->mode == 3) { // Auto fix
				p->mode = 2; // fixed ip
				write_wifi_cfg(BID_ST_DHCP_CFG);
			}
			ret = RTW_SUCCESS;
		}
		else {
			debug_printf("LwIP_DHCP ret=%d\n", ret);
			ret = RTW_ERROR;
		}
	}
	if(ret == RTW_SUCCESS) {
		show_wifi_st_ip();
		wifi_st_status = WIFI_STA_CONNECTED;
#if CONFIG_WLAN_CONNECT_CB
	//	extern void connect_start(void);
		connect_start();
#endif
	}
	return ret;
}

LOCAL void wifi_autoreconnect_thread_(void *param) {
	int ret = RTW_ERROR;
	struct wifi_autoreconnect_param *reconnect_param =
			(struct wifi_autoreconnect_param *) param;
	printf("auto reconnect ...\n");
	wifi_st_status = WIFI_STA_RECONNECT;
	ret = wifi_connect(
			wifi_st_cfg.bssid,
			wifi_st_cfg.flg,
#if PARM_RECONNECT
			wifi_st_cfg.ssid,
			reconnect_param->security_type,
			wifi_st_cfg.password,
#else
			reconnect_param->ssid,
			reconnect_param->security_type,
			reconnect_param->password,
#endif
			reconnect_param->key_id,
			NULL);
	if (ret == RTW_SUCCESS) {
		if(wifi_cfg.save_flg & BID_WIFI_ST_CFG)
			write_wifi_cfg(BID_WIFI_ST_CFG);
		// Start DHCPClient
		StartStDHCPClient();
	}
	vTaskDelete(NULL);
}

LOCAL void wifi_autoreconnect_hdl_(rtw_security_t security_type, char *ssid,
		int ssid_len, char *password, int password_len, int key_id) {
	wifi_autoreconnect.security_type = security_type;
	wifi_autoreconnect.key_id = key_id;
#if PARM_RECONNECT==0
	wifi_autoreconnect.ssid = ssid;
	wifi_autoreconnect.password = password;
#endif
	_adapter * ad = *(_adapter **)((rltk_wlan_info[0].dev)->priv);
	if(ad->mlmeextpriv.reconnect_cnt == 255) {
		ad->mlmeextpriv.reconnect_cnt = 0;
	};
	xTaskCreate(wifi_autoreconnect_thread_, (const char * )"st_recon", 400,
			&wifi_autoreconnect, tskIDLE_PRIORITY + 1, NULL);
}

LOCAL void st_set_autoreconnect(uint8 mode, uint8 count, uint16 timeout) {
	p_wlan_autoreconnect_hdl = wifi_autoreconnect_hdl_;
	_adapter * ad = *(_adapter **)((rltk_wlan_info[0].dev)->priv);
	ad->mlmeextpriv.reconnect_times = count;
	ad->mlmeextpriv.reconnect_timeout =	timeout;
	ad->mlmeextpriv.reconnect_cnt = 0;
	ad->mlmeextpriv.auto_reconnect = (mode != 0);
}

LOCAL rtw_result_t wifi_run_st(void) {
	rtw_result_t ret = RTW_SUCCESS;
//	chk_ap_netif_num();
	if((wifi_mode == RTW_MODE_STA) || (wifi_mode == RTW_MODE_STA_AP)) {
#if CONFIG_AUTO_RECONNECT
//		p_wlan_autoreconnect_hdl = NULL;
		if (wifi_st_cfg.autoreconnect) {
			st_set_autoreconnect(1, wifi_st_cfg.autoreconnect, wifi_st_cfg.reconnect_pause);
			ret = wext_set_autoreconnect(WLAN0_NAME, 1, wifi_st_cfg.autoreconnect, wifi_st_cfg.reconnect_pause);
			if (ret != RTW_SUCCESS)
				warning_printf("ERROR: Operation failed! Error=%d\n", ret);
		}
#endif
		info_printf("Connected to AP (%s, netif%d)...\n", wlan_st_name, wlan_st_netifn);
		ret = wifi_connect(
				wifi_st_cfg.bssid,
				wifi_st_cfg.flg,
				wifi_st_cfg.ssid,
				idx_to_rtw_security(wifi_st_cfg.security),
				wifi_st_cfg.password,
				-1, 
				NULL);
		wifi_st_status = WIFI_STA_START;
//		wifi_run_mode |= RTW_MODE_STA;
		if (ret != RTW_SUCCESS) {
			error_printf("%s: Operation failed! Error(%d)\n", __func__, ret);
		} else {
			if(wifi_cfg.save_flg & BID_WIFI_ST_CFG)
				write_wifi_cfg(BID_WIFI_ST_CFG);
			// Start DHCPClient
			StartStDHCPClient();
		}
	};
	return ret;
}

LOCAL int _wifi_on(rtw_mode_t mode) {
	int ret = 0;
/*
	if (!((rltk_wlan_running(WLAN0_IDX) == 0) && (rltk_wlan_running(WLAN1_IDX) == 0))) {
		warning_printf("WIFI is already running\n");
		return 0;
	}
*/
	info_printf("Initializing WIFI...\n");

	uint8 devnum = (mode == RTW_MODE_STA_AP); // flag = 1 -> 2 netif
	wifi_mode = mode;
	chk_ap_netif_num();

	// set wifi mib
	wext_set_adaptivity(wifi_cfg.adaptivity & 3); // rtw_adaptivity_mode_t

	ret = rltk_wlan_init(WLAN0_IDX, mode); // rtw_mode_t

	netif_set_up(&xnetif[0]);
	if (ret < 0) return ret;
	if(devnum) {
		ret = rltk_wlan_init(WLAN1_IDX, mode);
		if (ret < 0) return ret;
		netif_set_up(&xnetif[1]);
	}
	else {
		netif_set_down(&xnetif[1]);
	}

	uint32 timeout = xTaskGetTickCount();
	rltk_wlan_start(WLAN0_IDX);
	if(devnum) rltk_wlan_start(WLAN1_IDX);
	while (1) {
		if (rltk_wlan_running(WLAN0_IDX)
			&& rltk_wlan_running(devnum) ) {
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

LOCAL void _LwIP_Init(void)
{
	if(!lwip_init_done) {
		int idx;
		debug_printf("LwIP Init (%d)\n", wifi_mode);
		/* Create tcp_ip stack thread */
		tcpip_init( NULL, NULL );

//		chk_ap_netif_num(); // Исполняется после _wifi_on()
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
		lwip_init_done = 1;

		init_event_callback_list();
	}
}

int wifi_run(rtw_mode_t mode) {
	int ret = 0;
#if CONFIG_DEBUG_LOG > 4
	debug_printf("\n%s(%d), %d\n", __func__, mode, wifi_run_mode);
	debug_printf("old mode = %d, new mode = %d\n", wifi_run_mode, mode);
#endif
	if(wifi_mode) { // != mode) {
		info_printf("Deinitializing WIFI ...\n");
		wifi_off();
		wifi_st_status = WIFI_STA_OFF;
//		wifi_run_mode = RTW_MODE_NONE;
		vTaskDelay(30);
	}
	if (mode != RTW_MODE_NONE) {
		if (_wifi_on(mode) < 0) {
			error_printf("Wifi On failed!\n");
			goto error_end;
		};
		if(wifi_set_country(wifi_cfg.country_code) != RTW_SUCCESS) {
			error_printf("WiFi: Error set tx country_code (%d)!", wifi_cfg.country_code);
		};
		if(rtw_power_percentage_idx != wifi_cfg.tx_pwr) {
			if(rltk_set_tx_power_percentage(wifi_cfg.tx_pwr) != RTW_SUCCESS) {
				error_printf("WiFi: Error set tx power (%d)!", wifi_cfg.tx_pwr);
			};
		}
		if(wifi_set_network_mode(wifi_cfg.bgn) != RTW_SUCCESS) {
			error_printf("WiFi: Error set network mode (%d)!", wifi_cfg.bgn);
		}
		debug_printf("mode=%d, wifi_mode=%d, old_mоde=%d\n", mode, wifi_mode, wifi_run_mode);

		if(mode <= RTW_MODE_STA_AP) {
			struct netif * pnif = &xnetif[WLAN_ST_NETIF_NUM];
#if LWIP_NETIF_HOSTNAME
			// @todo ethernetif_init()...
			pnif->hostname = lwip_host_name[0];
#ifdef USE_NETBIOS
    		netbios_set_name(WLAN_ST_NETIF_NUM, lwip_host_name[0]);
#endif
#endif
			netif_set_addr(&xnetif[WLAN_ST_NETIF_NUM], &wifi_st_dhcp.ip,
				&wifi_st_dhcp.mask, &wifi_st_dhcp.gw);
			pnif = &xnetif[WLAN_AP_NETIF_NUM];
#if LWIP_NETIF_HOSTNAME
			// @todo ethernetif_init()...
			pnif->hostname = lwip_host_name[1];
#ifdef USE_NETBIOS
    		netbios_set_name(WLAN_AP_NETIF_NUM, lwip_host_name[1]);
#endif
#endif
			netif_set_addr(&xnetif[WLAN_AP_NETIF_NUM], &wifi_ap_dhcp.ip,
				&wifi_ap_dhcp.mask, &wifi_ap_dhcp.gw);

		}
		switch(mode) {
			 case RTW_MODE_STA_AP:
				 ret = wifi_run_ap() | wifi_run_st();
//				 _wext_enable_powersave(0, 0, 0);
//				 _wext_set_lps_dtim(0, 0);
		 		 break;
			 case RTW_MODE_STA:
				 ret = wifi_run_st();
		 		 if(_wext_set_lps_dtim(0, wifi_st_cfg.dtim)!= RTW_SUCCESS) {
					error_printf("WiFi: Error set DTIM(%d)!", wifi_st_cfg.dtim);
		 		 };
		 		 if(_wext_enable_powersave(0, wifi_st_cfg.sleep & 1, (wifi_st_cfg.sleep >> 1) & 1) != RTW_SUCCESS) {
					error_printf("WiFi: Error set powersave mode!");
		 		 };
		 		 break;
			 case RTW_MODE_AP:
				 ret = wifi_run_ap();
//				 _wext_enable_powersave(WLAN0_NAME, 0, 0);
				 break;
#if 0// CONFIG_ENABLE_??
			 case RTW_MODE_PROMISC:
				 // @todo
				 break;
#endif
#if CONFIG_ENABLE_P2P
			 case RTW_MODE_P2P:
				 // @todo
				 break;
#endif
			 default:
				ret = 1;
				error_printf("WiFi: Error mode(%d)\n", wifi_mode);
		};
		wifi_run_mode = mode;
		if(ret == 0 && (wifi_cfg.save_flg & BID_WIFI_CFG)) {
			wifi_cfg.mode = mode;
			write_wifi_cfg(BID_WIFI_CFG);
		};
	} else {
		ret = 0;
error_end:
#if CONFIG_WLAN_CONNECT_CB
		connect_close();
#endif
		if(wifi_run_mode) {
			wifi_disconnect();
		};
		wifi_off();
		wifi_st_status = WIFI_STA_OFF;
		wifi_run_mode = RTW_MODE_NONE;
		chk_ap_netif_num();
	};
	return ret;
}

/* Load cfg, init WiFi + LwIP init, WiFi start if wifi_cfg.mode !=  RTW_MODE_NONE */
void wifi_init(void) {
	debug_printf("\nLoad Config\n");
	read_wifi_cfg(wifi_cfg.load_flg); // DEF_LOAD_CFG
	// Call back from wlan driver after wlan init done
//	p_wlan_init_done_callback = wlan_init_done_callback;
	// Call back from application layer after wifi_connection success
//		p_write_reconnect_ptr = wlan_write_reconnect_data_to_flash;
	p_wlan_autoreconnect_hdl = NULL;
	_LwIP_Init();
#if CONFIG_WIFI_IND_USE_THREAD
	wifi_manager_init();
#endif
//	wifi_cfg.mode = RTW_MODE_PROMISC; //RTW_MODE_P2P;
	wifi_run(wifi_cfg.mode);
}

unsigned char *tab_txt_rtw_secyrity[] = {
		"OPEN",			//0 Open security
		"WEP",			//1 WEP Security with open authentication
		"WEP SHARED",	//2 WEP Security with shared authentication
		"WPA TKIP",		//3 WPA Security with TKIP
		"WPA AES",		//4 WPA Security with AES
		"WPA2 TKIP",	//5 WPA2 Security with TKIP
		"WPA2 AES",		//6 WPA2 Security with AES
		"WPA2 Mixed",	//7 WPA2 Security with AES & TKIP
		"WPA/WPA2 AES",	//8 WPA/WPA2 Security
		"Unknown"		//9
};

unsigned int tab_code_rtw_secyrity[] = {
		RTW_SECURITY_OPEN,				//0 Open security
		RTW_SECURITY_WEP_PSK,			//1 WEP Security with open authentication
		RTW_SECURITY_WEP_SHARED,		//2 WEP Security with shared authentication
		RTW_SECURITY_WPA_TKIP_PSK,		//3 WPA Security with TKIP
		RTW_SECURITY_WPA_AES_PSK,		//4 WPA Security with AES
		RTW_SECURITY_WPA2_TKIP_PSK,		//5 WPA2 Security with TKIP
		RTW_SECURITY_WPA2_AES_PSK,		//6 WPA2 Security with AES
		RTW_SECURITY_WPA2_MIXED_PSK,	//7 WPA2 Security with AES & TKIP
		RTW_SECURITY_WPA_WPA2_MIXED,	//8 WPA/WPA2 Security
		RTW_SECURITY_UNKNOWN			//9
};

/*
unsigned char *tab_txt_rtw_eccryption[] = {
    "Unknown",
    "OPEN",
    "WEP40",
    "WPA_TKIP",
    "WPA_AES",
    "WPA2_TKIP",
    "WPA2_AES",
    "WPA2_MIXED",
	"???",
    "WEP104",
    "Udef" // 0xff
};
*/

rtw_security_t idx_to_rtw_security(unsigned char idx)
{
	if(idx > IDX_SECURITY_UNKNOWN - 1) idx = IDX_SECURITY_WPA2_AES_PSK;
	return (rtw_security_t)tab_code_rtw_secyrity[idx];
}

unsigned char rtw_security_to_idx(rtw_security_t rtw_sec_type)
{
	unsigned char i = 0;
	while(rtw_sec_type != tab_code_rtw_secyrity[i] && tab_code_rtw_secyrity[i] != RTW_SECURITY_UNKNOWN) i++;
	return i;
}

unsigned char * idx_security_to_str(unsigned char idx)
{
	if(idx > IDX_SECURITY_UNKNOWN) idx = IDX_SECURITY_UNKNOWN;
	return tab_txt_rtw_secyrity[idx];
}

unsigned char * rtw_security_to_str(rtw_security_t rtw_sec_type)
{
	return tab_txt_rtw_secyrity[rtw_security_to_idx(rtw_sec_type)];
}

void show_wifi_ap_ip(void) {
	printf("SoftAP ip: " IPSTR "\n", IP2STR(&xnetif[WLAN_AP_NETIF_NUM].ip_addr));
}
void show_wifi_st_ip(void) {
	printf("Station ip: " IPSTR "\n", IP2STR(&xnetif[WLAN_ST_NETIF_NUM].ip_addr));
}

void show_wifi_MAC(void) {
	printf("MAC: " IPSTR "\n", IP2STR(&xnetif[WLAN_ST_NETIF_NUM].ip_addr));
}

void show_wifi_st_cfg(void) {
	printf("\tSSID: '%s'\n", wifi_st_cfg.ssid);
	printf("\tPassword: '%s'\n", wifi_st_cfg.password);
	printf("\tSecurity type: %s\n", idx_security_to_str(wifi_st_cfg.security));
	printf("\tAuto-reconnect: %d\n", wifi_st_cfg.autoreconnect);
	printf("\tReconnect pause: %d\n", wifi_st_cfg.reconnect_pause);
	printf("\tSleep mode: %p\n", wifi_st_cfg.sleep);
	printf("\tDTIM: %d\n", wifi_st_cfg.dtim);
}

void show_wifi_ap_cfg(void) {
	printf("\tSSID: '%s'\n", wifi_ap_cfg.ssid);
	printf("\tSSID hidden: %d\n", wifi_ap_cfg.ssid_hidden);
	printf("\tPassword: '%s'\n", wifi_ap_cfg.password);
	printf("\tSecurity type: %s\n", (wifi_ap_cfg.security)? tab_txt_rtw_secyrity[IDX_SECURITY_WPA2_AES_PSK] : tab_txt_rtw_secyrity[IDX_SECURITY_OPEN]);
	printf("\tChannel: %d\n", wifi_ap_cfg.channel);
	printf("\tBeacon interval: %d ms\n", wifi_ap_cfg.beacon_interval);
	printf("\tMax connections: %d\n", wifi_ap_cfg.max_sta);
}

void show_wifi_cfg(void) {
	printf("\tStart mode: %p\n", wifi_cfg.mode);
	printf("\tCountry code: %d\n", wifi_cfg.country_code);
	printf("\tNetwork mode: %d\n", wifi_cfg.bgn);
	printf("\tTx power: %d\n", wifi_cfg.tx_pwr);
	printf("\tAdaptivity: %d\n", wifi_cfg.adaptivity);
	printf("\tLoad flags: %p\n", wifi_cfg.load_flg);
	printf("\tSave flags: %p\n", wifi_cfg.save_flg);
}


/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/

 /** @file

	This example demonstrate how to implement wifi fast reconnection
**/
#include <platform_opts.h>
#if defined(CONFIG_EXAMPLE_WLAN_FAST_CONNECT) && CONFIG_EXAMPLE_WLAN_FAST_CONNECT
#include <wlan_fast_connect/example_wlan_fast_connect.h>


#include "task.h"
#include <platform/platform_stdlib.h>
#include <wifi/wifi_conf.h>
#include "flash_api.h"
#include <lwip_netconf.h>
#ifdef USE_FLASH_EEP
#include "flash_eep.h"
#include "feep_config.h"
#endif

write_reconnect_ptr p_write_reconnect_ptr;

extern void fATW0(void *arg);
extern void fATW1(void *arg);
extern void fATW2(void *arg);
extern void fATWC(void *arg);

/*
* Usage:
*       wifi connection indication trigger this function to save current
*       wifi profile in flash
*
* Condition: 
*       CONFIG_EXAMPLE_WLAN_FAST_CONNECT flag is set
*/

int wlan_write_reconnect_data_to_flash(u8 *data, uint32_t len)
{
	if(data == NULL || len < sizeof(struct wlan_fast_reconnect)) return -1;
	DBG_8195A("WiFi connected at start %d ms\n", xTaskGetTickCount());
#ifdef USE_FLASH_EEP
	flash_write_cfg(data, FEEP_ID_WIFI_CFG, sizeof(struct wlan_fast_reconnect));
#else
	struct wlan_fast_reconnect read_data = {0};
	flash_t flash;
	if(!data) return -1;
	flash_stream_read(&flash, FAST_RECONNECT_DATA, sizeof(struct wlan_fast_reconnect), (u8 *) &read_data);

	//wirte it to flash if different content: SSID, Passphrase, Channel, Security type
	if(memcmp(data, (u8 *) &read_data, sizeof(struct wlan_fast_reconnect)) != 0) {
		DBG_8195A(" %s():not the same ssid/passphrase/channel, write new profile to flash\n", __func__);
	    flash_erase_sector(&flash, FAST_RECONNECT_DATA);
	    flash_stream_write(&flash, FAST_RECONNECT_DATA, len, (uint8_t *) data);
	}
#endif
	return 0;
}

/*
* Usage:
*       After wifi init done, waln driver call this function to check whether
*       auto-connect is required.
*
*       This function read previous saved wlan profile in flash and execute connection.
*
* Condition: 
*       CONFIG_EXAMPLE_WLAN_FAST_CONNECT flag is set
*/
//scan_buf_arg scan_buf;

int wlan_init_done_callback()
{
	flash_t		flash;
	struct wlan_fast_reconnect *data;
	uint32_t	channel;
	uint8_t     pscan_config;
	char key_id[2] = {0};
//	wifi_disable_powersave();
#if CONFIG_AUTO_RECONNECT
	//setup reconnection flag
//	u8 mode;
//	if(wifi_get_autoreconnect(&mode) > 0 && mode != 1)
	wifi_set_autoreconnect(1); // (not work if lib_wlan_mp.a ?)
#endif
	DBG_8195A("WiFi Init after %d ms\n", xTaskGetTickCount());
	data = (struct wlan_fast_reconnect *)rtw_zmalloc(sizeof(struct wlan_fast_reconnect));
	if(data) {
#ifdef USE_FLASH_EEP
		if (flash_read_cfg(data, FEEP_ID_WIFI_CFG, sizeof(struct wlan_fast_reconnect)) == sizeof(struct wlan_fast_reconnect)) {
#else
	    flash_stream_read(&flash, FAST_RECONNECT_DATA, sizeof(struct wlan_fast_reconnect), (uint8_t *)data);
	    if(*((uint32_t *) data) != ~0x0) {
#endif
#if 1 // not use AT
#if 0
	    	//set partial scan for entering to listen beacon quickly
	    	channel = data->channel & 0xFF;
		    pscan_config = PSCAN_ENABLE |PSCAN_FAST_SURVEY; //  PSCAN_SIMPLE_CONFIG |
			if(wifi_set_pscan_chan((uint8_t *)&channel, &pscan_config, 1) < 0)
				printf("Wifi set partial scan channel fail!\n");
//		    rtw_network_info_t *wifi;
//		    SC_connect_to_candidate_AP
//		    SC_parse_scan_result_and_connect();
			if (wifi_connect(
					NULL,
					0,
					data->psk_essid, 
					data->security_type, 
					data->psk_passphrase, 
					RTW_SECURITY_OPEN, 
					NULL) == RTW_SUCCESS) {
#else
	    	//set partial scan for entering to listen beacon quickly
		    	channel = data->channel & 0xFF;
			    pscan_config = PSCAN_ENABLE |PSCAN_FAST_SURVEY; //  PSCAN_SIMPLE_CONFIG |
				if(wifi_set_pscan_chan((uint8_t *)&channel, &pscan_config, 1) < 0)
					printf("Wifi set partial scan channel fail!\n");
//	    	channel = data->channel & 0xFF;
		    wifi_set_channel(1); //channel);
//		    wifi_set_channel_plan(channel);
//		    pscan_config = PSCAN_ENABLE | PSCAN_FAST_SURVEY; //  PSCAN_SIMPLE_CONFIG |
//		    wifi_set_pscan_chan((uint8_t *)&channel, &pscan_config, 1);
//		    DiagPrintf("\nScan end at start %d ms.\n", xTaskGetTickCount());
//		    u8 bssid[ETH_ALEN] = { 0x1a,0xfe,0x34,0x99,0xad,0x1d };
		    u8 bssid[ETH_ALEN] = { 0xbc,0xae,0xc5,0xeb,0x09,0x90 };

//			if (wifi_connect(data->psk_essid, data->security_type, data->psk_passphrase, strlen(data->psk_essid), strlen(data->psk_passphrase), RTW_SECURITY_OPEN, NULL) == RTW_SUCCESS) {
	        if (wifi_connect(
	        			bssid, 
	        			1,
	        			data->psk_essid, 
	        			data->security_type, 
	        			data->psk_passphrase, 
	        			data->channel>>28, 
	        			NULL) == RTW_SUCCESS) {
#endif
//		    	DBG_8195A("WiFi connected at start %dms\n", xTaskGetTickCount());
#if CONFIG_LWIP_LAYER
#ifdef USE_FLASH_EEP
	        	dhcp_cfg *p = (dhcp_cfg *)&data;
	        	p->mode = 3;
	        	extern struct netif xnetif[NET_IF_NUM];
        		struct netif * pnetif = &xnetif[0];
	        	if(flash_read_cfg(p, FEEP_ID_DHCP_CFG, sizeof(dhcp_cfg)) > 1) {
	        		rtl_printf("Read dhcp_config: mode = %d, ip:%p, msk:%p, gw:%p\n", p->mode, p->ip, p->mask, p->gw);
	        		if(p->mode == 2) {
	        			netif_set_addr(pnetif, (ip_addr_t *)&p->ip, (ip_addr_t *)&p->mask, (ip_addr_t *)&p->gw);
	        			dhcps_init(pnetif);
	        		}
	        		else if(p->mode) LwIP_DHCP(0, DHCP_START);
	        	}
	        	else LwIP_DHCP(0, DHCP_START);
        		if(p->mode == 3 && pnetif->ip_addr.addr != 0) {
        			p->mode = 2;
        			p->ip = pnetif->ip_addr.addr;
        			p->gw = pnetif->gw.addr;
        			p->mask = pnetif->netmask.addr;
        			flash_write_cfg(p, FEEP_ID_DHCP_CFG, sizeof(dhcp_cfg));
	        		rtl_printf("Write dhcp_config: mode = %d, ip:%p, msk:%p, gw:%p\n", p->mode, p->ip, p->mask, p->gw);
        		}
#else
	        	LwIP_DHCP(0, DHCP_START);
#endif //#if USE_FLASH_EEP
#endif //#if CONFIG_LWIP_LAYER
#if CONFIG_WLAN_CONNECT_CB
		    	connect_start();
#endif
		    }
#else
//	    	flash_read_cfg(psk_essid, 0x5731, sizeof(psk_essid));
		    memcpy(psk_essid, data->psk_essid, sizeof(data->psk_essid));
//	    	flash_read_cfg(psk_passphrase, 0x5732, sizeof(psk_passphrase));
		    memcpy(psk_passphrase, data->psk_passphrase, sizeof(data->psk_passphrase));
//	    	flash_read_cfg(wpa_global_PSK, 0x5733, sizeof(wpa_global_PSK));
		    memcpy(wpa_global_PSK, data->wpa_global_PSK, sizeof(data->wpa_global_PSK));
		    //set partial scan for entering to listen beacon quickly
		    pscan_config = PSCAN_ENABLE | PSCAN_FAST_SURVEY;
		    channel = data->channel & 0xFF;
		    wifi_set_pscan_chan((uint8_t *)&channel, &pscan_config, 1);
		    //set wifi connect
		    switch(data->security_type){
	            case RTW_SECURITY_OPEN:
	                fATW0((char*)psk_essid);
	                break;
	            case RTW_SECURITY_WEP_PSK:
	                fATW0((char*)psk_essid);
	                fATW1((char*)psk_passphrase);
	    		    sprintf(key_id,"%d",(char) (data->channel>>28));
	                fATW2(key_id);
	                break;
	            case RTW_SECURITY_WPA_TKIP_PSK:
	            case RTW_SECURITY_WPA2_AES_PSK:
	                fATW0((char*)psk_essid);
	                fATW1((char*)psk_passphrase);
	                break;
	            default:
	                break;
		    }
		    fATWC(NULL);
#endif // use AT
	    }
	    rtw_mfree(data);
	}

	return 0;
}


void example_wlan_fast_connect()
{
	// Call back from wlan driver after wlan init done
	p_wlan_init_done_callback = wlan_init_done_callback;

	// Call back from application layer after wifi_connection success
	p_write_reconnect_ptr = wlan_write_reconnect_data_to_flash;

}

#endif // defined(CONFIG_EXAMPLE_WLAN_FAST_CONNECT) && CONFIG_EXAMPLE_WLAN_FAST_CONNECT


/*
 * wifi_api_scan.c
 *
 *  Created on: 23/04/2017
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

#if 1
#include "drv_types.h" // or #include "wlan_lib.h"
#else
#include "wifi_constants.h"
#include "wifi_structures.h"
#include "wlan_lib.h" // or #include "drv_types.h"
#endif

#include "flash_eep.h"
#include "feep_config.h"

#include "wifi_api.h"
#include "main.h"
#include "wifi_user_set.h"

/******************************************************
 *
 ******************************************************/

web_scan_handler_t web_scan_handler_ptr;

/* --------  WiFi Scan ------------------------------- */
volatile uint8_t scan_end;

extern internal_scan_handler_t scan_result_handler_ptr;
void wifi_scan_each_report_hdl(char* buf, int buf_len, int flags, void* userdata);

LOCAL void _wifi_scan_done_hdl(char* buf, int buf_len, int flags, void* userdata);

void wifi_set_timer_scan(int ms) {
	if(web_scan_handler_ptr.flg) {
		if(xTimerChangePeriod(web_scan_handler_ptr.timer, ms, portMAX_DELAY) != pdPASS) {
			error_printf("Error xTimerChangePeriod\n");
		}
	}
}
/* --------  WiFi Scan Close ------------------------- */
void wifi_close_scan(void)
{
	internal_scan_handler_t * pscan_rec = &scan_result_handler_ptr;
	web_scan_handler_t * pwscn_rec = &web_scan_handler_ptr;
	printf("Close scan rec\n");
	if(pscan_rec->scan_running) {
		wifi_unreg_event_handler(WIFI_EVENT_SCAN_RESULT_REPORT,	wifi_scan_each_report_hdl);
		wifi_unreg_event_handler(WIFI_EVENT_SCAN_DONE, _wifi_scan_done_hdl);
		if(pscan_rec->ap_details) rtw_free(pscan_rec->ap_details);
		rtw_memset((void *) pscan_rec, 0, sizeof(internal_scan_handler_t));
	}
	if(pwscn_rec->flg) {
		if(pwscn_rec->timer) xTimerDelete(pwscn_rec->timer, portMAX_DELAY);
		if(pwscn_rec->ap_details) rtw_free(pwscn_rec->ap_details);
		rtw_memset(pwscn_rec, 0, sizeof(web_scan_handler_t));
	}
//	pscan_rec->scan_complete = 1;
}
/* --------  WiFi Scan Done ------------------------- */
LOCAL void _wifi_scan_done_hdl(char* buf, int buf_len, int flags, void* userdata) {
	internal_scan_handler_t * pscan_rec = &scan_result_handler_ptr;
	web_scan_handler_t * pwscn_rec = &web_scan_handler_ptr;
	if(pscan_rec->gscan_result_handler) {
		// сторонний вывод
		(*pscan_rec->gscan_result_handler)(pscan_rec);
	}
	else {
		// оставить структуру pscan_rec->pap_details[i] для вывода в web scan на 5 сек
		if(pwscn_rec->flg && pscan_rec->scan_cnt) {
			debug_printf("\nScan done, wait read rec\n");
			if(xTimerChangePeriod(pwscn_rec->timer, 5000, portMAX_DELAY) != pdPASS) {
//				error_printf("Error xTimerChangePeriod\n");
			} else {
				if(pwscn_rec->ap_details) rtw_free(pwscn_rec->ap_details);
				pwscn_rec->ap_details = pscan_rec->ap_details;
				pwscn_rec->ap_count = pscan_rec->scan_cnt;
				wifi_unreg_event_handler(WIFI_EVENT_SCAN_RESULT_REPORT,	wifi_scan_each_report_hdl);
				wifi_unreg_event_handler(WIFI_EVENT_SCAN_DONE, _wifi_scan_done_hdl);
				rtw_memset((void *) pscan_rec, 0, sizeof(internal_scan_handler_t));
				pwscn_rec->flg = 2;
			}
			return;
		};
	}
	wifi_close_scan();
	return;
}

/* --------  WiFi Scan Start ------------------------- */
LOCAL int _wifi_scan_networks(rtw_scan_result_handler_t results_handler) {
	internal_scan_handler_t * pscan_rec = &scan_result_handler_ptr;
	pscan_rec->gscan_result_handler = results_handler;
	pscan_rec->max_ap_size = MAX_AP_SIZE;
	pscan_rec->ap_details = (rtw_scan_result_t*) rtw_zmalloc(MAX_AP_SIZE * sizeof(rtw_scan_result_t) + MAX_AP_SIZE * sizeof(rtw_scan_result_t*));
	if (pscan_rec->ap_details != NULL) {
		pscan_rec->pap_details = (rtw_scan_result_t**) (&pscan_rec->ap_details[MAX_AP_SIZE]);
		pscan_rec->scan_cnt = 0;
		pscan_rec->scan_complete = RTW_FALSE;
		pscan_rec->user_data = NULL;
		wifi_reg_event_handler(WIFI_EVENT_SCAN_RESULT_REPORT, wifi_scan_each_report_hdl, NULL);
		wifi_reg_event_handler(WIFI_EVENT_SCAN_DONE, _wifi_scan_done_hdl, NULL);
		if(wext_set_scan(WLAN0_NAME, NULL, 0, RTW_SCAN_TYPE_ACTIVE | (RTW_SCAN_COMMAMD << 4) | (RTW_BSS_TYPE_ANY << 8)) == RTW_SUCCESS) {
			return RTW_SUCCESS;
		}
	};
	wifi_close_scan();
	return RTW_ERROR;
}

/* --------  wext_set_pscan_channels ----------------- */

LOCAL int wext_set_pscan_channels(void) {
	struct iwreq iwr;
	int ret = -1;
	__u8 *para;
	int i = 0;
	rtw_memset(&iwr, 0, sizeof(iwr));
	//Format of para:function_name num_channel chan1... pscan_config1 ...
	iwr.u.data.length = (SCAN_CHANNELS + SCAN_CHANNELS + 1) + 12;
	para = rtw_malloc(iwr.u.data.length); //size:num_chan + num_time + length + function_name
	iwr.u.data.pointer = para;
	if (para != NULL) {
		rtw_memcpy((char*) para, "PartialScan", 12); //Cmd
		*(para + 12) = SCAN_CHANNELS; // length
		for (i = 0; i < SCAN_CHANNELS; i++) {
			*(para + 13 + i) = i + 1;
			*((__u16 *) (para + 13 + SCAN_CHANNELS + i)) = PSCAN_ENABLE;
		}
		ret = iw_ioctl(WLAN0_NAME, SIOCDEVPRIVATE, &iwr);
		rtw_free(para);
	}
#if	CONFIG_DEBUG_LOG > 3
	else {
		error_printf("%s: Can't malloc memory!\n", __func__);
	}
#endif
	return ret;
}

/* --------  WiFi Scan ------------------------------- */
rtw_result_t api_wifi_scan(api_scan_result_handler_t scan_result_cb)
{
	internal_scan_handler_t * pscan_rec = &scan_result_handler_ptr;
	web_scan_handler_t * pwscn_rec = &web_scan_handler_ptr;
	if ((!pscan_rec->scan_running)
		&& (!pwscn_rec->flg)) {
		pscan_rec->scan_running = 1;
		rtw_memset(pwscn_rec, 0, sizeof(web_scan_handler_t));
		pwscn_rec->flg = 1;
		debug_printf("\nStart scan...\n");
		pwscn_rec->timer = xTimerCreate("webscan", 2500, pdFALSE, NULL, (TimerCallbackFunction_t)wifi_close_scan);
		if(!pwscn_rec->timer) {
//			error_printf("Error xTimerCreate\n");
		} else if(xTimerStart(pwscn_rec->timer, portMAX_DELAY) != pdPASS) {
//			error_printf("Error xTimerStart\n");
		} else if(wext_set_pscan_channels() < 0) {
//			error_printf("ERROR: wifi set partial scan channel fail\n");
		} else if(_wifi_scan_networks(scan_result_cb) != RTW_SUCCESS) {
//			error_printf("ERROR: wifi scan failed\n");
		} else if(scan_result_cb) {
			int i = 300;
			while(i-- && pscan_rec->scan_running) {
				vTaskDelay(10);
			};
			return RTW_SUCCESS;
		} else
			return RTW_SUCCESS;
		wifi_close_scan();
		return RTW_ERROR;
	};
	return RTW_TIMEOUT;
}


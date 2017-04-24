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

#include "wlan_lib.h"

#include "flash_eep.h"
#include "feep_config.h"

#include "wifi_api.h"
#include "main.h"
#include "wifi_user_set.h"

/******************************************************
 *                    Constants
 ******************************************************/
#define SCAN_USE_SEMAPHORE	0
#define	MAX_AP_SIZE 64

extern internal_scan_handler_t scan_result_handler_ptr;
void wifi_scan_each_report_hdl(char* buf, int buf_len, int flags, void* userdata);
/*
	xQueueWiFiScan = xQueueCreate(25, sizeof(rtw_scan_result_t)); // Create a queue...
	if(xQueueWebSrv) {
*/

LOCAL void _wifi_scan_done_hdl(char* buf, int buf_len, int flags, void* userdata) {
	int i = 0;
	internal_scan_handler_t * pscan_rec = &scan_result_handler_ptr;
	rtw_scan_handler_result_t scan_result_report;

	for (i = 0; i < pscan_rec->scan_cnt; i++) {
		rtw_memcpy(&scan_result_report.ap_details,
				pscan_rec->pap_details[i],
				sizeof(rtw_scan_result_t));
		scan_result_report.scan_complete =	pscan_rec->scan_complete;
		scan_result_report.user_data = pscan_rec->user_data;
		(*pscan_rec->gscan_result_handler)(&scan_result_report);
	}

	pscan_rec->scan_complete = RTW_TRUE;
	scan_result_report.scan_complete = RTW_TRUE;
	(*pscan_rec->gscan_result_handler)(&scan_result_report);

	rtw_free(pscan_rec->ap_details);
	rtw_free(pscan_rec->pap_details);
#if SCAN_USE_SEMAPHORE
	rtw_up_sema(&pscan_rec->scan_semaphore);
#else
	pscan_rec->scan_running = 0;
#endif
	wifi_unreg_event_handler(WIFI_EVENT_SCAN_RESULT_REPORT,	wifi_scan_each_report_hdl);
	wifi_unreg_event_handler(WIFI_EVENT_SCAN_DONE, _wifi_scan_done_hdl);
	return;
}

/* --------  WiFi Scan ------------------------------- */
LOCAL int _wifi_scan(rtw_scan_type_t scan_type, rtw_bss_type_t bss_type) {
	int ret;
	scan_buf_arg * pscan_buf;
	u16 flags = scan_type | (bss_type << 8);
	wifi_reg_event_handler(WIFI_EVENT_SCAN_RESULT_REPORT, wifi_scan_each_report_hdl, NULL);
	wifi_reg_event_handler(WIFI_EVENT_SCAN_DONE, _wifi_scan_done_hdl, NULL);
	return wext_set_scan(WLAN0_NAME, NULL, 0, flags);
}

/* --------  WiFi Scan ------------------------------- */
LOCAL int _wifi_scan_networks(rtw_scan_result_handler_t results_handler) {
	internal_scan_handler_t * pscan_rec = &scan_result_handler_ptr;
#if SCAN_USE_SEMAPHORE
	rtw_bool_t result;
	if(NULL == pscan_rec->scan_semaphore)
	rtw_init_sema(&pscan_rec->scan_semaphore, 1);

	pscan_rec->scan_start_time = rtw_get_current_time();
	/* Initialise the semaphore that will prevent simultaneous access - cannot be a mutex, since
	 * we don't want to allow the same thread to start a new scan */
	result = (rtw_bool_t)rtw_down_timeout_sema(&pscan_rec->scan_semaphore, SCAN_LONGEST_WAIT_TIME);
	if ( result != RTW_TRUE )
	{
		/* Return error result, but set the semaphore to work the next time */
		rtw_up_sema(&pscan_rec->scan_semaphore);
		return RTW_TIMEOUT;
	}
#else
	if (pscan_rec->scan_running) {
		int count = 100;
		while (pscan_rec->scan_running && count > 0) {
			rtw_msleep_os(20);
			count--;
		}
		if (count == 0) {
			printf("[%d]WiFi: Scan is running. Wait 2s timeout.\n",
					rtw_get_current_time());
			return RTW_TIMEOUT;
		}
	}
	pscan_rec->scan_start_time = rtw_get_current_time();
	pscan_rec->scan_running = 1;
#endif

	pscan_rec->gscan_result_handler = results_handler;
	pscan_rec->max_ap_size = MAX_AP_SIZE;
	pscan_rec->ap_details = (rtw_scan_result_t*) rtw_zmalloc(MAX_AP_SIZE * sizeof(rtw_scan_result_t) + MAX_AP_SIZE * sizeof(rtw_scan_result_t*));
	if (pscan_rec->ap_details != NULL) {
		pscan_rec->pap_details = (rtw_scan_result_t**) (&pscan_rec->ap_details[MAX_AP_SIZE]);
		pscan_rec->scan_cnt = 0;
		pscan_rec->scan_complete = RTW_FALSE;
		pscan_rec->user_data = NULL; // ?????????

		if (_wifi_scan(RTW_SCAN_COMMAMD << 4 | RTW_SCAN_TYPE_ACTIVE,	RTW_BSS_TYPE_ANY) == RTW_SUCCESS) {
			return RTW_SUCCESS;
		};
		rtw_free((u8*)pscan_rec->ap_details);
	};
	rtw_memset((void *) pscan_rec, 0, sizeof(internal_scan_handler_t));
	return RTW_ERROR;
}
/* --------  WiFi Scan ------------------------------- */
#define scan_channels 14
/* --------  WiFi Scan ------------------------------- */
LOCAL int wext_set_pscan_channels(void) {
	struct iwreq iwr;
	int ret = -1;
	__u8 *para;
	int i = 0;
	rtw_memset(&iwr, 0, sizeof(iwr));
	//Format of para:function_name num_channel chan1... pscan_config1 ...
	iwr.u.data.length = (scan_channels + scan_channels + 1) + 12;
	para = pvPortMalloc((scan_channels + scan_channels + 1) + 12); //size:num_chan + num_time + length + function_name
	iwr.u.data.pointer = para;
	if (para != NULL) {
		//Cmd
		rtw_memcpy((char*) para, "PartialScan", 12);
		//length
		*(para + 12) = scan_channels;
		for (i = 0; i < scan_channels; i++) {
			*(para + 13 + i) = i + 1;
			*((__u16 *) (para + 13 + scan_channels + i)) = PSCAN_ENABLE;
		}

		ret = iw_ioctl(WLAN0_NAME, SIOCDEVPRIVATE, &iwr);
		vPortFree(para);
	}
#if	CONFIG_DEBUG_LOG > 3
	else {
		error_printf("%s: Can't malloc memory!\n", __func__);
	}
#endif
	return ret;
}
/* --------  WiFi Scan ------------------------------- */
LOCAL volatile uint8_t scan_end;
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
	    {
	    	uint8 * s = rtw_security_to_str(record->security);
	    	printf("%s\t", s);
	    	if(strlen(s) < 8) printf("\t");
	    }
	    printf("%s\n", record->SSID.val);
	} else {
		scan_end = 0;
		printf("\n");
	}
	return RTW_SUCCESS;
}
/* --------  WiFi Scan ------------------------------- */
void api_wifi_scan(void)
{
		scan_end = 1;
		if(wext_set_pscan_channels() < 0){
		    error_printf("ERROR: wifi set partial scan channel fail\n");
		} else if(_wifi_scan_networks(_scan_result_handler) != RTW_SUCCESS){
			error_printf("ERROR: wifi scan failed\n");
		} else {
			int i = 300;
			while(i-- && scan_end) {
				vTaskDelay(10);
			};
		};
}


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "queue.h"
#include "utils/os.h"
#include <lwip_netconf.h>
#include <lwip/netif.h>
#include "wifi/wifi_conf.h"
#include "wps/wps_defs.h"
#include <platform/platform_stdlib.h>

/*
 * @brief  struct wps_credential - WPS Credential
 */
struct dev_credential {
	u8 ssid[32]; 			/**< SSID */
	size_t ssid_len;		/**< Length of SSID */
	u16 auth_type;			/**< Authentication Type (WPS_AUTH_OPEN, .. flags) */
	u16 encr_type;			/**< Encryption Type (WPS_ENCR_NONE, .. flags) */
	u8 key_idx;				/**< Key index */
	u8 key[65];				/**< Key */
	size_t key_len;			/**< Key length in octets */
	u8 mac_addr[6];			/**< MAC address of the Credential receiver */
	const u8 *cred_attr;	/**< Unparsed Credential attribute data (used only in cred_cb()).
							This may be NULL, if not used. */
	size_t cred_attr_len;	/**< Length of cred_attr in octets */
	u16 ap_channel;			/**< AP channel */
};

typedef struct {
	char *target_ssid;
	u16 config_method;
	_sema scan_sema;
	int isoverlap;
} internal_wps_scan_handler_arg_t;

#define WLAN0_NAME "wlan0"
#ifndef ENABLE
#define ENABLE	(1)
#endif
#ifndef DISABLE
#define	DISABLE (0)
#endif
#define STACKSIZE     512


//static xSemaphoreHandle wps_reconnect_semaphore;
//static struct _WIFI_NETWORK wifi_get_from_certificate = {0};

#define WPS_AUTH_TYPE_OPEN              (0x0001)
#define WPS_AUTH_TYPE_WPA_PERSONAL      (0x0002)
#define WPS_AUTH_TYPE_SHARED            (0x0004)
#define WPS_AUTH_TYPE_WPA_ENTERPRISE    (0x0008)
#define WPS_AUTH_TYPE_WPA2_PERSONAL     (0x0010)
#define WPS_AUTH_TYPE_WPA2_ENTERPRISE   (0x0020)

#define WPS_ENCR_TYPE_NONE              (0x0001)
#define WPS_ENCR_TYPE_WEP               (0x0002)
#define WPS_ENCR_TYPE_TKIP              (0x0004)
#define WPS_ENCR_TYPE_AES               (0x0008)

#define 	SCAN_BUFFER_LENGTH	(4096)

#if CONFIG_ENABLE_P2P
extern void _wifi_p2p_wps_success(const u8 *peer_addr, int registrar);
extern void _wifi_p2p_wps_failed();
#endif
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
extern u32 _wps_registrar_process_msg(void *priv, u32 op_code, const void *pmsg);
extern void * _wps_registrar_get_msg(void *priv, u32 *op_code);
extern void * _wps_registrar_init(void *priv, const void* pcfg);
extern void _wps_registrar_deinit(void *priv);
extern void *_wps_registrar_alloc();
extern int _wps_registrar_add_pin(void *priv, const u8 *addr,
			  const u8 *uuid, const u8 *pin, size_t pin_len,
			  int timeout);
extern int _wps_registrar_button_pushed(void *priv,
				const u8 *p2p_dev_addr);
extern int _wps_registrar_wps_cancel(void *priv);
extern void _wpas_wsc_ap_send_eap_reqidentity(void *priv, u8 *rx_buf);
extern void _wpas_wsc_ap_check_eap_rspidentity(void *priv, u8 *rx_buf);
extern void _wpas_wsc_registrar_send_eap_fail(void *priv);
extern void _wpas_wsc_registrar_handle_recvd(void *priv, u8 *rx_buf);
extern void * _eap_wsc_server_process_hdl(void *priv, void* req, u8 id);
extern void *_eap_wsc_server_reset(void *priv);
#endif
extern void wpas_wsc_sta_wps_start_hdl(char *buf, int buf_len, int flags, void *userdata);
extern void wpas_wsc_wps_finish_hdl(char *buf, int buf_len, int flags, void *userdata);
extern void wpas_wsc_eapol_recvd_hdl(char *buf, int buf_len, int flags, void *userdata);

void wifi_p2p_wps_success(const u8 *peer_addr, int registrar)
{
#if CONFIG_ENABLE_P2P
	_wifi_p2p_wps_success(peer_addr, registrar);
#endif
}

void wifi_p2p_wps_failed()
{
#if CONFIG_ENABLE_P2P
	_wifi_p2p_wps_failed();
#endif
}

void * wps_registrar_init(void *priv, void *pcfg)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	return _wps_registrar_init(priv, pcfg);
#else
	return NULL;
#endif
}

void wps_registrar_deinit(void *priv)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	_wps_registrar_deinit(priv);
#endif
}

void *wps_registrar_alloc()
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	return _wps_registrar_alloc();
#else
	return NULL;
#endif
}

u32 wps_registrar_process_msg(void *priv, u32 op_code, const void *pmsg)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	return _wps_registrar_process_msg(priv, op_code, pmsg);
#else
	return 0;
#endif
}

void * wps_registrar_get_msg(void *priv, u32 *op_code)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	return _wps_registrar_get_msg(priv, op_code);
#else
	return NULL;
#endif
}


int wps_registrar_add_pin(void *priv, const u8 *addr,
			  const u8 *uuid, const u8 *pin, size_t pin_len,
			  int timeout)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	return _wps_registrar_add_pin(priv, NULL,NULL,pin,pin_len,0);
#else
	return 0;
#endif
}

int wps_registrar_button_pushed(void *priv,
				const u8 *p2p_dev_addr)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	return _wps_registrar_button_pushed(priv, p2p_dev_addr);
#else
	return 0;
#endif
}

int wps_registrar_wps_cancel(void *priv)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	return _wps_registrar_wps_cancel(priv);
#else
	return 0;
#endif

}

void wpas_wsc_ap_send_eap_reqidentity(void *priv, u8 *rx_buf)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	_wpas_wsc_ap_send_eap_reqidentity(priv, rx_buf);
#endif
}

void wpas_wsc_ap_check_eap_rspidentity(void *priv, u8 *rx_buf)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	_wpas_wsc_ap_check_eap_rspidentity(priv, rx_buf);
#endif
}

void wpas_wsc_registrar_send_eap_fail(void *priv)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	_wpas_wsc_registrar_send_eap_fail(priv);
#endif
}

void wpas_wsc_registrar_handle_recvd(void *priv, u8 *rx_buf)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	_wpas_wsc_registrar_handle_recvd(priv, rx_buf);
#endif
}

void * eap_wsc_server_process_hdl(void *priv, void* req, u8 id)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	return _eap_wsc_server_process_hdl(priv, req, id);
#else
	return NULL;
#endif
}

void eap_wsc_server_reset(void *priv)
{
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	_eap_wsc_server_reset(priv);
#endif
}

#if CONFIG_ENABLE_WPS
xqueue_handle_t queue_for_credential;
char wps_pin_code[32];
u16 config_method;
u8 wps_password_id;
static TaskHandle_t ap_wps_task = NULL;

void wps_check_and_show_connection_info(void)
{
	rtw_wifi_setting_t setting;	
#if CONFIG_LWIP_LAYER 
	/* Start DHCP Client */
	LwIP_DHCP(0, DHCP_START);		
#endif	
	wifi_get_setting(WLAN0_NAME, &setting);
	wifi_show_setting(WLAN0_NAME, &setting);

#if CONFIG_INIC_CMD_RSP
	inic_c2h_wifi_info("ATWW", RTW_SUCCESS);
#endif
}

static void wps_config_wifi_setting(rtw_network_info_t *wifi, struct dev_credential *dev_cred)
{
	printf("wps_config_wifi_setting\n");
	//memcpy((void *)wifi->ssid, (void *)dev_cred->ssid, dev_cred->ssid_len); 
	strcpy((char*)wifi->ssid.val, (char*)&dev_cred->ssid[0]);
	printf("wps_wifi.ssid = %s\n", wifi->ssid.val);
	wifi->ssid.len = dev_cred->ssid_len;
	printf("wps_wifi.ssid_len = %d\n", wifi->ssid.len);

	switch(dev_cred->auth_type) {
	case WPS_AUTH_TYPE_OPEN :
	case WPS_AUTH_TYPE_SHARED :
		if(dev_cred->encr_type == WPS_ENCR_TYPE_WEP) {
			printf("security_type = RTW_SECURITY_WEP_PSK\n");
			wifi->security_type = RTW_SECURITY_WEP_PSK;
			wifi->key_id = dev_cred->key_idx - 1;
		}
		else {
			printf("security_type = RTW_SECURITY_OPEN\n");
			wifi->security_type = RTW_SECURITY_OPEN;
		}
		break;
	case WPS_AUTH_TYPE_WPA_PERSONAL : 
	case WPS_AUTH_TYPE_WPA_ENTERPRISE : 
		printf("security_type = RTW_SECURITY_WPA_AES_PSK\n");
		wifi->security_type = RTW_SECURITY_WPA_AES_PSK;
		break;
	case WPS_AUTH_TYPE_WPA2_PERSONAL : 
	case WPS_AUTH_TYPE_WPA2_ENTERPRISE : 
		printf("security_type = RTW_SECURITY_WPA2_AES_PSK\n");
		wifi->security_type = RTW_SECURITY_WPA2_AES_PSK;
		break;
	}

	printf("wps_wifi.security_type = %d\n", wifi->security_type);

	//memcpy(wifi->password, dev_cred->key, dev_cred->key_len);
	wifi->password = dev_cred->key;
	printf("wps_wifi.password = %s\n", wifi->password);
	wifi->password_len = dev_cred->key_len;
	printf("wps_wifi.password_len = %d", wifi->password_len);
	//xSemaphoreGive(wps_reconnect_semaphore);
	//printf("\r\nrelease wps_reconnect_semaphore");			
}

static int wps_connect_to_AP_by_certificate(rtw_network_info_t *wifi)
{
#define RETRY_COUNT		3
	int retry_count = RETRY_COUNT, ret;

	printf("=============== wifi_certificate_info ===============\n");
	printf("wps_wifi.ssid = %s\n", wifi->ssid.val);
	printf("security_type = %d\n", wifi->security_type);
	printf("wps_wifi.password = %s\n", wifi->password);
	printf("ssid_len = %d\n", wifi->ssid.len);
	printf("password_len = %d\n", wifi->password_len);
	while (1) {
		ret = wifi_connect(
						NULL,
						0,
						(char*)wifi->ssid.val,
						 wifi->security_type,
						 (char*)wifi->password,
						 wifi->key_id,
						 NULL);
		if (ret == RTW_SUCCESS) {
			if(retry_count == RETRY_COUNT)
				rtw_msleep_os(1000);  //When start wps with OPEN AP, AP will send a disassociate frame after STA connected, need reconnect here.
			if(RTW_SUCCESS == wifi_is_connected_to_ap( )){
				//printf("\r\n[WPS]Ready to tranceive!\n");
				wps_check_and_show_connection_info();
				break;
			}
		}
		if (retry_count == 0) {
			printf("[WPS]Join bss failed\n");
			ret = -1;
			break;
		}
		retry_count --;
	}
	return ret;
}

static int wps_connect_to_AP_by_open_system(char *target_ssid)
{
	int retry_count = 3, ret;
	
	if (target_ssid != NULL) {
		rtw_msleep_os(500);	//wait scan complete.
		while (1) {
			ret = wifi_connect(
							 NULL,
							 0,
							 target_ssid,
							 RTW_SECURITY_OPEN,
							 NULL,
							 0,
							 NULL);
			if (ret == RTW_SUCCESS) {
			  	//wps_check_and_show_connection_info();
				break;
			}
			if (retry_count == 0) {
				printf("[WPS]Join bss failed\n");
				return -1;
			}
			retry_count --;
		}
		//
	} else {
		printf("[WPS]Target SSID is NULL\n");
	}

	return 0;
}

static void process_wps_scan_result( rtw_scan_result_t* record, void * user_data )
{
	internal_wps_scan_handler_arg_t *wps_arg = (internal_wps_scan_handler_arg_t *)user_data;

	if (record->wps_type != 0xff) {
		if (wps_arg->config_method == WPS_CONFIG_PUSHBUTTON) {
			if (record->wps_type == 0x04) {
				wps_password_id = record->wps_type;
				if (++wps_arg->isoverlap == 0) {
					memcpy(&wps_arg->target_ssid[0], record->SSID.val, record->SSID.len);
					wps_arg->target_ssid[record->SSID.len] = '\0';
					printf("[pbc]Record first triger wps AP = %s\n", wps_arg->target_ssid);
				}
			}
		} else if (wps_arg->config_method == WPS_CONFIG_DISPLAY || wps_arg->config_method == WPS_CONFIG_KEYPAD) {
			if (record->wps_type == 0x00) {
				wps_arg->isoverlap = 0;
				wps_password_id = record->wps_type;
				memcpy(&wps_arg->target_ssid[0], record->SSID.val, record->SSID.len);
				wps_arg->target_ssid[record->SSID.len] = '\0';
				printf("[pin]find out first triger wps AP = %s\n", wps_arg->target_ssid);
			}
		}
	}
}

static rtw_result_t wps_scan_result_handler( rtw_scan_handler_result_t* malloced_scan_result )
{
	internal_wps_scan_handler_arg_t *wps_arg = (internal_wps_scan_handler_arg_t *)malloced_scan_result->user_data;
	if (malloced_scan_result->scan_complete != RTW_TRUE)
	{
		rtw_scan_result_t* record = &malloced_scan_result->ap_details;
		record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */

		process_wps_scan_result(record, malloced_scan_result->user_data);
	}
	else
	{
		printf("WPS scan done!\n");
		rtw_up_sema(&wps_arg->scan_sema);
	}
	return RTW_SUCCESS;
}

extern void wifi_scan_each_report_hdl( char* buf, int buf_len, int flags, void* userdata);
extern void wifi_scan_done_hdl( char* buf, int buf_len, int flags, void* userdata);

static int wps_find_out_triger_wps_AP(char *target_ssid, u16 config_method)
{
	internal_wps_scan_handler_arg_t wps_arg = {0};

	wps_password_id = 0xFF;

	wps_arg.isoverlap = -1;
	wps_arg.config_method = config_method;
	wps_arg.target_ssid = target_ssid;
	rtw_init_sema(&wps_arg.scan_sema, 0);
	if(wps_arg.scan_sema == NULL) return RTW_ERROR;

	if(wifi_scan_networks(wps_scan_result_handler, &wps_arg ) != RTW_SUCCESS){
		printf("ERROR: wifi scan failed\n");
		goto exit;
	}
	if(rtw_down_timeout_sema(&wps_arg.scan_sema, SCAN_LONGEST_WAIT_TIME) == RTW_FALSE){
		printf("WPS scan done early!\n");
	}
	wifi_unreg_event_handler(WIFI_EVENT_SCAN_RESULT_REPORT, wifi_scan_each_report_hdl);
	wifi_unreg_event_handler(WIFI_EVENT_SCAN_DONE, wifi_scan_done_hdl);

exit:
	rtw_free_sema(&wps_arg.scan_sema);
	
	return wps_arg.isoverlap;
}

int wps_start(u16 wps_config, char *pin, u8 channel, char *ssid)
{
	struct dev_credential dev_cred;
	rtw_network_info_t wifi = {0}; 
	char target_ssid[64];
	int is_overlap = -1;
	u32 start_time = rtw_get_current_time();
	int ret = 0;

	memset(&dev_cred, 0, sizeof(struct dev_credential));
	memset(target_ssid, 0, 64);
	if((wps_config != WPS_CONFIG_PUSHBUTTON) 
		&& (wps_config != WPS_CONFIG_DISPLAY)
		&& (wps_config != WPS_CONFIG_KEYPAD)){
		printf("WPS: Wps method(%d) is wrong. Not triger WPS.\n", wps_config);
		return -1;
	}
	config_method = wps_config;
	
	if(wps_config == WPS_CONFIG_DISPLAY
		|| wps_config == WPS_CONFIG_KEYPAD) {
		if(pin)
			strcpy(wps_pin_code, pin);
		else{
			printf("WPS: PIN is NULL. Not triger WPS.\n");
			return -1;
		}
	}
	
	if(!ssid)	{	
		while (1) {
			unsigned int current_time = rtw_get_current_time();
			if (rtw_systime_to_sec(current_time - start_time) < 120) { 	
				is_overlap = wps_find_out_triger_wps_AP(&target_ssid[0], wps_config);
				if ((is_overlap == 0) || (is_overlap > 0)) 
					break;
			} else {
				printf("WPS: WPS Walking Time Out\n");
				return -2;
			}
		}

		if (is_overlap > 0) {
			printf("WPS: WPS session overlap. Not triger WPS.\n");
			return -2;
		}
	}else{
		rtw_memcpy(target_ssid, ssid, strlen(ssid));
	}
	
	if (queue_for_credential != NULL) {
		os_xqueue_delete(queue_for_credential);
		queue_for_credential = NULL;
	}
	queue_for_credential = os_xqueue_create(1, sizeof(struct dev_credential));
	if(!queue_for_credential)
		return -1;

	wifi_reg_event_handler(WIFI_EVENT_STA_WPS_START, wpas_wsc_sta_wps_start_hdl, NULL);
	wifi_reg_event_handler(WIFI_EVENT_WPS_FINISH, wpas_wsc_wps_finish_hdl, NULL);
	wifi_reg_event_handler(WIFI_EVENT_EAPOL_RECVD, wpas_wsc_eapol_recvd_hdl, NULL);

	wifi_set_wps_phase(ENABLE);
	ret = wps_connect_to_AP_by_open_system(target_ssid);
	if(ret < 0){
		printf("WPS: WPS Fail!\n");
		goto exit;
	}
	os_xqueue_receive(queue_for_credential, &dev_cred, 120);
	if (dev_cred.ssid[0] != 0 && dev_cred.ssid_len <= 32) {
		wps_config_wifi_setting(&wifi, &dev_cred);
		wifi_set_wps_phase(DISABLE);
		ret = wps_connect_to_AP_by_certificate(&wifi);
		goto exit1;
	} else {
		printf("WPS: WPS FAIL!\n");
//		printf("\n\rWPS: WPS FAIL!\n");
//		printf("\n\rWPS: WPS FAIL!\n");
		ret = -1;
	}
exit:
	wifi_set_wps_phase(DISABLE);
exit1:
	if (queue_for_credential != NULL) {
		os_xqueue_delete(queue_for_credential);
		queue_for_credential = NULL;
	}

	wifi_unreg_event_handler(WIFI_EVENT_STA_WPS_START, wpas_wsc_sta_wps_start_hdl);
	wifi_unreg_event_handler(WIFI_EVENT_WPS_FINISH, wpas_wsc_wps_finish_hdl);
	wifi_unreg_event_handler(WIFI_EVENT_EAPOL_RECVD, wpas_wsc_eapol_recvd_hdl);

	wpas_wps_deinit();
	return ret;
}

#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
static int ap_wps_start(u16 wps_config, char *pin)
{
	u8 authorized_mac[ETH_ALEN];
	int ret = 0;
	u32 pin_val = 0;

	if (queue_for_credential != NULL) {
		os_xqueue_delete(queue_for_credential);
		queue_for_credential = NULL;
	}

	queue_for_credential = os_xqueue_create(1, sizeof(authorized_mac));	
	if(!queue_for_credential)
		return -1;
	
	wifi_reg_event_handler(WIFI_EVENT_STA_WPS_START, wpas_wsc_sta_wps_start_hdl, NULL);
	wifi_reg_event_handler(WIFI_EVENT_WPS_FINISH, wpas_wsc_wps_finish_hdl, NULL);
	wifi_reg_event_handler(WIFI_EVENT_EAPOL_RECVD, wpas_wsc_eapol_recvd_hdl, NULL);

	wifi_set_wps_phase(ENABLE);
	
	if(wps_config == WPS_CONFIG_KEYPAD)
	{
		pin_val = atoi(pin);
		if (!wps_pin_valid(pin_val)) {
			printf("WPS-AP: Enter pin code is unvalid.\n");
			goto exit;
		}
		ret = wpas_wps_registrar_add_pin((unsigned char const*)pin, strlen(pin));
	}
	else if(wps_config == WPS_CONFIG_DISPLAY)
		ret = wpas_wps_registrar_add_pin((unsigned char const*)pin, strlen(pin));
	else
		ret = wpas_wps_registrar_button_pushed();

	if(ret<0)
		goto exit;

	printf("WPS-AP: wait for STA connect!\n");
	os_xqueue_receive(queue_for_credential, authorized_mac, 120); //max wait 2min

	if(!wpas_wps_registrar_check_done())
	{
		ret = -1;
		wpas_wps_registrar_wps_cancel();
	}

exit:
	wifi_set_wps_phase(0);
	os_xqueue_delete(queue_for_credential);
	queue_for_credential = NULL;	
	printf("WPS-AP: Finished!\n");
	
	wifi_unreg_event_handler(WIFI_EVENT_STA_WPS_START, wpas_wsc_sta_wps_start_hdl);
	wifi_unreg_event_handler(WIFI_EVENT_WPS_FINISH, wpas_wsc_wps_finish_hdl);
	wifi_unreg_event_handler(WIFI_EVENT_EAPOL_RECVD, wpas_wsc_eapol_recvd_hdl);
	
	return ret;
}

static void wifi_start_ap_wps_thread_hdl( void *param)
{
	ap_wps_start(config_method, wps_pin_code);  //Not support WPS_CONFIG_KEYPAD

	ap_wps_task = NULL;
	vTaskDelete(NULL);
}

void wifi_start_ap_wps_thread(u16 config_methods, char *pin)
{
	if((config_methods != WPS_CONFIG_PUSHBUTTON) 
		&& (config_methods != WPS_CONFIG_DISPLAY)
		&& (config_methods != WPS_CONFIG_KEYPAD)){
		printf("WPS-AP: Wps method(%d) is wrong. Not triger WPS.\n", config_methods);
		return;
	}
	config_method = config_methods;
	if(config_methods == WPS_CONFIG_DISPLAY
		|| config_methods == WPS_CONFIG_KEYPAD) {
		if(pin)
			strcpy(wps_pin_code, pin);
		else{
			printf("WPS-AP: PIN is NULL. Not triger WPS.\n");
			return;
		}
	}
	if(ap_wps_task != NULL){ //push item to wait queue to finish last ap_wps task 
		printf("WPS-AP: Wait for last ap_wps task exiting...\n");
		if(queue_for_credential)
			os_xqueue_send(queue_for_credential, NULL, 0);
		while(ap_wps_task != NULL)
			vTaskDelay(1);
		vTaskDelay(20);
		printf("Last ap_wps task completed.\n");
	}
	if(xTaskCreate(wifi_start_ap_wps_thread_hdl, ((const char*)"ap_wps"), 256, NULL, tskIDLE_PRIORITY + 3, &ap_wps_task) != pdPASS)
		printf("%s xTaskCreate(ap_wps thread) failed\n", __FUNCTION__);
}

#endif //CONFIG_ENABLE_WPS_AP

void wps_judge_staion_disconnect(void) 
{
	int mode = 0;
	unsigned char ssid[33];

	wext_get_mode(WLAN0_NAME, &mode);

	switch(mode) {
	case IW_MODE_MASTER:		//In AP mode
//		rltk_wlan_deinit();
//		rltk_wlan_init(0,RTW_MODE_STA);
//		rltk_wlan_start(0);
		//modified by Chris Yang for iNIC
		wifi_off();
		vTaskDelay(20);
		wifi_on(RTW_MODE_STA);
		break;
	case IW_MODE_INFRA:		//In STA mode
		if(wext_get_ssid(WLAN0_NAME, ssid) > 0)
			wifi_disconnect();
	}	
}

void cmd_wps(int argc, char **argv)
{
	int ret = -1;
	wps_judge_staion_disconnect();
	
	if((argc == 2 || argc == 3 ) && (argv[1] != NULL)){
		if(strcmp(argv[1],"pin") == 0){
			unsigned int pin_val = 0;
			/* start pin */
			if(argc == 2){
				char device_pin[10];
				pin_val = wps_generate_pin();
				sprintf(device_pin, "%08d", pin_val);
				/* Display PIN 3 times to prevent to be overwritten by logs from other tasks */
				printf("WPS: Start WPS PIN Display. PIN: [%s]\n", device_pin);
				printf("WPS: Start WPS PIN Display. PIN: [%s]\n", device_pin);
				printf("WPS: Start WPS PIN Display. PIN: [%s]\n", device_pin);
				ret = wps_start(WPS_CONFIG_DISPLAY, (char*)device_pin, 0, NULL);
			}else{
	 			pin_val = atoi(argv[2]);
				if (!wps_pin_valid(pin_val)) {
					printf("WPS: Device pin code is invalid. Not triger WPS.\n");
					goto exit;
				}
				printf("WPS: Start WPS PIN Keypad.\n");
				ret = wps_start(WPS_CONFIG_KEYPAD, argv[2], 0, NULL);
			}
		}else if(strcmp(argv[1],"pbc") == 0){
			/* start pbc */
			printf("WPS: Start WPS PBC.\n");
			ret = wps_start(WPS_CONFIG_PUSHBUTTON, NULL, 0, NULL);
		}else{
			printf("WPS: Wps Method is wrong. Not triger WPS.\n");
			goto exit;
		}
	}
exit:
#if CONFIG_INIC_CMD_RSP
	if(ret != 0)
		inic_c2h_msg("ATWW", ret, NULL, 0);
#endif
	return;
}

#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
/*
cmd_ap_wps for AP WSC setting. command style:
cmd_ap_wps pbc or cmd_ap_wps pin 12345678
*/
void cmd_ap_wps(int argc, char **argv)
{
	int mode = 0;
	if(rltk_wlan_running(WLAN1_IDX)){
		printf("Not support con-current softAP WSC!\n");
		return;
	}
	wext_get_mode(WLAN0_NAME, &mode);
	if(mode != IW_MODE_MASTER){
		printf("Only valid for IW_MODE_MASTER!\n");
		return;
	}
		
	if((argc == 2 || argc == 3) && (argv[1] != NULL)) {
		if (strcmp(argv[1],"pin") == 0 ) {
			unsigned int pin_val = 0;
			if(argc == 3){
				pin_val = atoi(argv[2]);
				if (!wps_pin_valid(pin_val)) {
					printf("WPS-AP: Device pin code is invalid. Not trigger WPS.\n");
					return;
				}
				printf("WPS-AP: Start AP WPS PIN Keypad.\n");
				wifi_start_ap_wps_thread(WPS_CONFIG_KEYPAD, argv[2]);
			}else{
				char device_pin[10];
				pin_val = wps_generate_pin();
				sprintf(device_pin, "%08d", pin_val);
				printf("WPS: Start WPS PIN Display. PIN: %s\n", device_pin);
				wifi_start_ap_wps_thread(WPS_CONFIG_DISPLAY, (char*)device_pin);
			}
		}else if (strcmp(argv[1],"pbc") == 0) {
			printf("WPS-AP: Start AP WPS PBC\n");
			wifi_start_ap_wps_thread(WPS_CONFIG_PUSHBUTTON, NULL);
		}else{
			printf("WPS-AP Usage:\"wifi_ap_wps pin [pin_code]\" or \"wifi_ap_wps pbc\"\n");
			return;
		}
	} else {
		printf("WPS-AP Usage:\"wifi_ap_wps pin [pin_code]\" or \"wifi_ap_wps pbc\"\n");
	}
	return;
}
#endif //CONFIG_ENABLE_P2P
#endif //CONFIG_ENABLE_WPS

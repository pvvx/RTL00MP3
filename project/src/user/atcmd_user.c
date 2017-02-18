#include <platform_opts.h>

#ifdef CONFIG_AT_USR

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "at_cmd/log_service.h"
#include "at_cmd/atcmd_wifi.h"
#include <lwip_netconf.h>
#include "tcpip.h"
#include <dhcp/dhcps.h>
#include <wifi/wifi_conf.h>
#include <wifi/wifi_util.h>
#include "tcm_heap.h"
#include "user/atcmd_user.h"
#include "user/playerconfig.h"

rtw_mode_t wifi_mode = RTW_MODE_STA;
mp3_server_setings mp3_serv = {0,{0}}; //{ PLAY_PORT, { PLAY_SERVER }};

#define DEBUG_AT_USER_LEVEL 1

/******************************************************************************/
/*
#define	_AT_WLAN_SET_SSID_          "ATW0"
#define	_AT_WLAN_SET_PASSPHRASE_    "ATW1"
#define	_AT_WLAN_SET_KEY_ID_        "ATW2"
#define	_AT_WLAN_JOIN_NET_          "ATWC"
#define	_AT_WLAN_SET_MP3_URL_       "ATWS"
*/
//extern struct netif xnetif[NET_IF_NUM];

/* fastconnect use wifi AT command. Not init_wifi_struct when log service disabled
 * static initialize all values for using fastconnect when log service disabled
 */
static rtw_network_info_t wifi = {
	{0},    // ssid
	{0},    // bssid
	0,      // security
	NULL,   // password
	0,      // password len
	-1      // key id
};

static rtw_ap_info_t ap = {0};
static unsigned char password[65] = {0};

_WEAK void connect_start(void)
{
}

_WEAK void connect_close(void)
{
}

static void init_wifi_struct(void)
{
	memset(wifi.ssid.val, 0, sizeof(wifi.ssid.val));
	memset(wifi.bssid.octet, 0, ETH_ALEN);	
	memset(password, 0, sizeof(password));
	wifi.ssid.len = 0;
	wifi.password = NULL;
	wifi.password_len = 0;
	wifi.key_id = -1;
	memset(ap.ssid.val, 0, sizeof(ap.ssid.val));
	ap.ssid.len = 0;
	ap.password = NULL;
	ap.password_len = 0;
	ap.channel = 1;
}

void fATW0(void *arg){
	if(!arg){
		printf("ATW0: Usage: ATW0=SSID\n");
		goto exit;
	}
#if	DEBUG_AT_USER_LEVEL > 1
	printf("ATW0: %s\n", (char*)arg);
#endif
	strcpy((char *)wifi.ssid.val, (char*)arg);
	wifi.ssid.len = strlen((char*)arg);
exit:
	return;
}

void fATW1(void *arg){
#if	DEBUG_AT_USER_LEVEL > 1
    printf("ATW1: %s\n", (char*)arg);
#endif
	strcpy((char *)password, (char*)arg);
	wifi.password = password;
	wifi.password_len = strlen((char*)arg);
	return;	
}

void fATW2(void *arg){
#if	DEBUG_AT_USER_LEVEL > 1
	printf("ATW2: %s\n", (char*)arg);
#endif
	if((strlen((const char *)arg) != 1 ) || (*(char*)arg <'0' ||*(char*)arg >'3')) {
		printf("ATW2: Wrong WEP key id. Must be one of 0,1,2, or 3.\n");
		return;
	}
	wifi.key_id = atoi((const char *)(arg));
	return;
}

// Test
void fATST(void *arg){
	AT_PRINTK("[ATS#]: _AT_SYSTEM_TEST_");
	DBG_8195A("\nCLK CPU\t\t%d Hz\nRAM heap\t%d bytes\nTCM heap\t%d bytes\n",
			HalGetCpuClk(), xPortGetFreeHeapSize(), tcm_heap_freeSpace());
	dump_mem_block_list();
	tcm_heap_dump();
	DBG_8195A("\n");
}

int mp3_cfg_read(void)
{
	bzero(&mp3_serv, sizeof(mp3_serv));
	if(flash_read_cfg(mp3_serv, 0x5000, sizeof(mp3_serv.port) + 2) >= sizeof(mp3_serv.port) + 2) {
		mp3_serv.port = PLAY_PORT;
		strcpy(mp3_serv.url, PLAY_SERVER);
	}
	return mp3_serv.port;
}


// MP3 Set server, Close connect
void fATWS(void *arg){
	int   argc           = 0;
	char *argv[MAX_ARGC] = {0};
	if(arg) {
       	argc = parse_param(arg, argv);
    	if (argc == 2) {
    		if(argv[1][0] == '?') {
			    printf("ATWS: %s,%d\n", mp3_serv.url, mp3_serv.port);
    		    return;
    		}
    		else if(strcmp(argv[1], "open") == 0) {
    		    printf("ATWS: open %s:%d\n", mp3_serv.url, mp3_serv.port);
    			connect_close();
    		    return;
    		}
    		else if(strcmp(argv[1], "close") == 0) {
    		    printf("ATWS: close\n");
    			connect_close();
    		    return;
    		}
    		else if(strcmp(argv[1], "read") == 0) {
    			mp3_cfg_read();
    			connect_start();
    		    return;
    		}
    		else if(strcmp(argv[1], "save") == 0) {
			    printf("ATWS: %s,%d\n", mp3_serv.url, mp3_serv.port);
    			if(flash_write_cfg(&mp3_serv, 0x5000, strlen(mp3_serv.port) + strlen(mp3_serv.url)))
    			    printf("ATWS: saved\n", mp3_serv.url, mp3_serv.port);
    		    return;
    		}
    	}
    	else if (argc >= 3 ) {
    		strcpy((char *)mp3_serv.url, (char*)argv[1]);
        	mp3_serv.port = atoi((char*)argv[2]);
        	printf("ATWS: %s,%d\r\n", mp3_serv.url, mp3_serv.port);
        	connect_start();
        	return;
    	}
	}	
	printf("ATWS: Usage: ATWS=URL,PORT or ATWS=close, ATWS=read, ATWS=save\n");
}


void fATWC(void *arg){
	int mode, ret;
	unsigned long tick1 = xTaskGetTickCount();
	unsigned long tick2, tick3;
	char empty_bssid[6] = {0}, assoc_by_bssid = 0;
	
	connect_close();
#if	DEBUG_AT_USER_LEVEL > 1
	printf("ATWC: Connect to AP...\n");
#endif
	if(memcmp (wifi.bssid.octet, empty_bssid, 6))
		assoc_by_bssid = 1;
	else if(wifi.ssid.val[0] == 0){
		printf("ATWC: Error: SSID can't be empty\n");
		ret = RTW_BADARG;
		goto EXIT;
	}
	if(wifi.password != NULL){
		if((wifi.key_id >= 0)&&(wifi.key_id <= 3)) {
			wifi.security_type = RTW_SECURITY_WEP_PSK;
		}
		else{
			wifi.security_type = RTW_SECURITY_WPA2_AES_PSK;
		}
	}
	else{
		wifi.security_type = RTW_SECURITY_OPEN;
	}
	//Check if in AP mode
	wext_get_mode(WLAN0_NAME, &mode);
	if(mode == IW_MODE_MASTER) {
		dhcps_deinit();
		wifi_off();
		vTaskDelay(20);
		if (wifi_on(RTW_MODE_STA) < 0){
			printf("ERROR: Wifi on failed!\n");
                        ret = RTW_ERROR;
			goto EXIT;
		}
	}

	if(assoc_by_bssid){
		printf("Joining BSS by BSSID "MAC_FMT" ...\n", MAC_ARG(wifi.bssid.octet));
		ret = wifi_connect_bssid(wifi.bssid.octet, (char*)wifi.ssid.val, wifi.security_type, (char*)wifi.password, 
						ETH_ALEN, wifi.ssid.len, wifi.password_len, wifi.key_id, NULL);		
	} else {
		printf("Joining BSS by SSID %s...\n", (char*)wifi.ssid.val);
		ret = wifi_connect((char*)wifi.ssid.val, wifi.security_type, (char*)wifi.password, wifi.ssid.len,
						wifi.password_len, wifi.key_id, NULL);
	}
	
	if(ret!= RTW_SUCCESS){
		printf("ERROR: Can't connect to AP\n");
		goto EXIT;
	}
	tick2 = xTaskGetTickCount();
	printf("Connected after %dms\n", (tick2-tick1));
	/* Start DHCPClient */
	LwIP_DHCP(0, DHCP_START);
	tick3 = xTaskGetTickCount();
	printf("Got IP after %dms\n", (tick3-tick1));
	printf("\n\r");
	connect_start();
EXIT:
	init_wifi_struct( );
}

void fATWD(void *arg){
	int timeout = 20;
	char essid[33];
	int ret = RTW_SUCCESS;

	connect_close();
#if	DEBUG_AT_USER_LEVEL > 1
	printf("ATWD: Disconnect...\n");
#endif
	printf("Dissociating AP ...\n");
	if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) < 0) {
		printf("WIFI disconnected\n");
		goto exit;
	}

	if((ret = wifi_disconnect()) < 0) {
		printf("ERROR: Operation failed!\n");
		goto exit;
	}

	while(1) {
		if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) < 0) {
			printf("WIFI disconnected\n");
			break;
		}

		if(timeout == 0) {
			printf("ERROR: Deassoc timeout!\n");
			ret = RTW_TIMEOUT;
			break;
		}

		vTaskDelay(1 * configTICK_RATE_HZ);
		timeout --;
	}
    printf("\n\r");
exit:
    init_wifi_struct( );
	return;
}

// Dump register
void fATSD(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};

#if	DEBUG_AT_USER_LEVEL > 1
	printf("ATSD: dump registers\n");
#endif
	if(!arg){
		printf("ATSD: Usage: ATSD=REGISTER");
		return;
	}
	argc = parse_param(arg, argv);
	if(argc == 2 || argc == 3)
		CmdDumpWord(argc-1, (unsigned char**)(argv+1));
}

void fATSW(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};

#if	DEBUG_AT_USER_LEVEL > 1
	printf("ATSW: write register\n");
#endif
	if(!arg){
		printf("ATSW: Usage: ATSW=REGISTER,DATA");
		return;
	}
	argc = parse_param(arg, argv);
	if(argc == 2 || argc == 3)
		CmdWriteWord(argc-1, (unsigned char**)(argv+1));
}

///// MP3 Set Mode
// MP3 Off
void fATOF(void *arg)
{
#if	DEBUG_AT_USER_LEVEL > 1
	printf("ATOF: MP3 off...\n");
#endif
	connect_close();
}


void print_wlan_help(void *arg){
		printf("WLAN AT COMMAND SET:\n");
		printf("==============================\n");
        printf(" Set MP3 server\n");
        printf("\t# ATWS=URL,PORT\n");
        printf("\tSample:\tATWS=icecast.omroep.nl/3fm-sb-mp3,80\n");
        printf("\t\tATWS=meuk.spritesserver.nl/Ii.Romanzeandante.mp3,80\n");
        printf("\t\tATWS=?, ATWS=close, ATWS=save, ATWS=read\n");
        printf(" Connect to an AES AP\n");
        printf("\t# ATW0=SSID\n");
        printf("\t# ATW1=PASSPHRASE\n");
        printf("\t# ATWC\n");
        printf(" DisConnect AP\n");
        printf("\t# ATWD\n");
}

log_item_t at_user_items[ ] = {
	{"ATW0", fATW0,},
	{"ATW1", fATW1,},
	{"ATW2", fATW2,},
	{"ATWC", fATWC,},
	{"ATST", fATST,},
	{"ATSD", fATSD,},	// Dump register
	{"ATSW", fATSW,},	// Set register
	{"ATWD", fATWD,},	//
	{"ATWS", fATWS,},	// MP3 Set server, Close connect
	{"ATOF", fATOF,},	// MP3 Set Mode
};


void at_user_init(void)
{
	init_wifi_struct();
	mp3_cfg_read();
	log_service_add_table(at_user_items, sizeof(at_user_items)/sizeof(at_user_items[0]));
}

log_module_init(at_user_init);

#endif //#ifdef CONFIG_AT_USR

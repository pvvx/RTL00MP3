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

#include "sleep_ex_api.h"

#include "lwip/tcp_impl.h"

rtw_mode_t wifi_mode = RTW_MODE_STA;

mp3_server_setings mp3_serv = {0,{0}}; //{ PLAY_PORT, { PLAY_SERVER }};

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
#ifdef CONFIG_DEBUG_LOG
	printf("Time at start %d ms.\n", xTaskGetTickCount());
#endif
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

static int mp3_cfg_read(void)
{
	bzero(&mp3_serv, sizeof(mp3_serv));
	if(flash_read_cfg(mp3_serv, 0x5000, sizeof(mp3_serv.port) + 2) >= sizeof(mp3_serv.port) + 2) {
		mp3_serv.port = PLAY_PORT;
		strcpy(mp3_serv.url, PLAY_SERVER);
	}
	return mp3_serv.port;
}

void start_init(void)
{
	init_wifi_struct();
	mp3_cfg_read();
}

// MP3 Set server, Close/Open connect
void fATWS(int argc, char *argv[]){
    	if (argc == 2) {
    		StrUpr(argv[1]);
    		if(argv[1][0] == '?') {
			    printf("ATWS: %s,%d\n", mp3_serv.url, mp3_serv.port);
    		    return;
    		}
    		else if(argv[1][0] == 'O') { // strcmp(argv[1], "open") == 0
    		    printf("ATWS: open %s:%d\n", mp3_serv.url, mp3_serv.port);
    			connect_close();
    		    return;
    		}
    		else if(argv[1][0] == 'C') { // strcmp(argv[1], "close") == 0
    		    printf("ATWS: close\n");
    			connect_close();
    		    return;
    		}
    		else if(argv[1][0] == 'R') { // strcmp(argv[1], "read") == 0
    			mp3_cfg_read();
    			connect_start();
    		    return;
    		}
    		else if(argv[1][0] == 'S') { // strcmp(argv[1], "save") == 0
			    printf("%s: %s,%d\n", argv[0], mp3_serv.url, mp3_serv.port);
    			if(flash_write_cfg(&mp3_serv, 0x5000, strlen(mp3_serv.port) + strlen(mp3_serv.url)))
    			    printf("ATWS: saved\n", mp3_serv.url, mp3_serv.port);
    		    return;
    		}
    	}
    	else if (argc >= 3 ) {
    		strcpy((char *)mp3_serv.url, (char*)argv[1]);
        	mp3_serv.port = atoi((char*)argv[2]);
        	printf("%s: %s,%d\r\n", argv[0], mp3_serv.url, mp3_serv.port);
        	connect_start();
        	return;
    	}
}

// Mem info
void fATST(void){
	extern u8 __HeapLimit, __StackTop;
	extern struct Heap g_tcm_heap;
		printf("\nCLK CPU\t\t%d Hz\nRAM heap\t%d bytes\nTCM heap\t%d bytes\n",
				HalGetCpuClk(), xPortGetFreeHeapSize(), tcm_heap_freeSpace());
		dump_mem_block_list();
		u32 saved = ConfigDebugInfo;
		DBG_INFO_MSG_ON(_DBG_TCM_HEAP_); // On Debug TCM MEM
		tcm_heap_dump();
		ConfigDebugInfo = saved;
		printf("\n");
#if (configGENERATE_RUN_TIME_STATS == 1)
		char *cBuffer = pvPortMalloc(512);
		if(cBuffer != NULL) {
			vTaskGetRunTimeStats((char *)cBuffer);
			printf("%s", cBuffer);
		}
		vPortFree(cBuffer);
#endif
}

void fATWC(int argc, char *argv[]){
	int mode, ret;
	unsigned long tick1 = xTaskGetTickCount();
	unsigned long tick2, tick3;
	char empty_bssid[6] = {0}, assoc_by_bssid = 0;

	if(argc > 1) {
		if(argv[1][0] == '?') {
			printf("Not released!\n");
			return;
		}
		strcpy((char *)wifi.ssid.val, argv[1]);
		wifi.ssid.len = strlen((char*)wifi.ssid.val);
	}
	if(argc > 2) {
		strcpy((char *)password, argv[2]);
		wifi.password = password;
		wifi.password_len = strlen(password);
	}
	if(argc > 3) {
		if((strlen(argv[3]) != 1 ) || (argv[3][0] <'0' || argv[3][0] >'3')) {
			printf("%s: Wrong WEP key id. Must be one of 0,1,2, or 3.\n", argv[0]);
			return;
		}
		wifi.key_id = atoi(argv[1]);
	}
	if(memcmp (wifi.bssid.octet, empty_bssid, 6))
		assoc_by_bssid = 1;
	else if(wifi.ssid.val[0] == 0){
		printf("%s: Error: SSID can't be empty\n", argv[0]);
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
	connect_close();
	//Check if in AP mode
	wext_get_mode(WLAN0_NAME, &mode);
	if(mode == IW_MODE_MASTER) {
		dhcps_deinit();
		wifi_off();
		vTaskDelay(wifi_test_timeout_step_ms/portTICK_RATE_MS);
		if (wifi_on(RTW_MODE_STA) < 0){
			printf("ERROR: Wifi on failed!\n");
                        ret = RTW_ERROR;
			goto EXIT;
		}
	}

	///wifi_set_channel(1);

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
	printf("Connected after %d ms\n", (tick2-tick1));
	/* Start DHCPClient */
	LwIP_DHCP(0, DHCP_START);
	tick3 = xTaskGetTickCount();
	printf("Got IP after %d ms\n\n", (tick3-tick1));
	connect_start();
EXIT:
	init_wifi_struct( );
}

// WIFI Disconnect
void fATWD(int argc, char *argv[]){
	int timeout = wifi_test_timeout_ms/wifi_test_timeout_step_ms;;
	char essid[33];
	int ret = RTW_SUCCESS;

	connect_close();
	printf("Deassociating AP ...\n");
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

		vTaskDelay(wifi_test_timeout_step_ms/portTICK_RATE_MS);
		timeout --;
	}
    printf("\n\r");
exit:
    init_wifi_struct( );
	return;
}

/*-------------------------------------------------------------------------------------
 Копирует данные из области align(4) (flash, registers, ...) в область align(1) (ram)
--------------------------------------------------------------------------------------*/
extern void copy_align4_to_align1(unsigned char * pd, void * ps, unsigned int len);
/*
static void copy_align4_to_align1(unsigned char * pd, void * ps, unsigned int len)
{
	union {
		unsigned char uc[4];
		unsigned int ud;
	}tmp;
	unsigned int *p = (unsigned int *)((unsigned int)ps & (~3));
	unsigned int xlen = (unsigned int)ps & 3;
	//	unsigned int size = len;

	if(xlen) {
		tmp.ud = *p++;
		while (len)  {
			len--;
			*pd++ = tmp.uc[xlen++];
			if(xlen & 4) break;
		}
	}
	xlen = len >> 2;
	while(xlen) {
		tmp.ud = *p++;
		*pd++ = tmp.uc[0];
		*pd++ = tmp.uc[1];
		*pd++ = tmp.uc[2];
		*pd++ = tmp.uc[3];
		xlen--;
	}
	if(len & 3) {
		tmp.ud = *p;
		pd[0] = tmp.uc[0];
		if(len & 2) {
			pd[1] = tmp.uc[1];
			if(len & 1) {
				pd[2] = tmp.uc[2];
			}
		}
	}
	//	return size;
}
*/
int print_hex_dump(uint8_t *buf, int len, unsigned char k) {
	uint32_t ss[2];
	ss[0] = 0x78323025; // "%02x"
	ss[1] = k;	// ","...'\0'
	uint8_t * ptr = buf;
	int result = 0;
	while (len--) {
		if (len == 0)
			ss[1] = 0;
		result += printf((uint8_t *) &ss, *ptr++);
	}
	return result;
}

extern char str_rom_hex_addr[]; // in *.ld "[Addr]   .0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .A .B .C .D .E .F\n"
// Dump byte register
void fATSB(int argc, char *argv[])
{
	uint8 buf[17];
	int size = 0;
	int addr = Strtoul(argv[1],0,16);
	if (argc > 2)
		size = Strtoul(argv[2],0,10);
	if (size <= 0 || size > 16384)
		size = 1;
	u32 symbs_line = sizeof(buf)-1;
	printf(str_rom_hex_addr);
	while (size) {
		if (symbs_line > size) symbs_line = size;
		printf("%08X ", addr);
		copy_align4_to_align1(buf, addr, symbs_line);
		print_hex_dump(buf, symbs_line, ' ');
		int i;
		for(i = 0 ; i < symbs_line ; i++) {
			if(buf[i] < 0x20 || buf[i] > 0x7E) {
				buf[i] = '.';
			}
		}
		buf[symbs_line] = 0;
		i = (sizeof(buf)-1) - symbs_line;
		while(i--) printf("   ");
		printf(" %s\r\n", buf);
		addr += symbs_line;
		size -= symbs_line;
	}
}

// Dump dword register
void fATSD(int argc, char *argv[])
{
	CmdDumpWord(argc-1, (unsigned char**)(argv+1));
}

void fATSW(int argc, char *argv[])
{
	CmdWriteWord(argc-1, (unsigned char**)(argv+1));
}

// Close connections
void fATOF(int argc, char *argv[])
{
	connect_close();
}

// Open connections
void fATON(int argc, char *argv[])
{
	connect_start();
}

/* Get one byte from the 4-byte address */
#define ip4_addr1(ipaddr) (((u8_t*)(ipaddr))[0])
#define ip4_addr2(ipaddr) (((u8_t*)(ipaddr))[1])
#define ip4_addr3(ipaddr) (((u8_t*)(ipaddr))[2])
#define ip4_addr4(ipaddr) (((u8_t*)(ipaddr))[3])
/* These are cast to u16_t, with the intent that they are often arguments
 * to printf using the U16_F format from cc.h. */
#define ip4_addr1_16(ipaddr) ((u16_t)ip4_addr1(ipaddr))
#define ip4_addr2_16(ipaddr) ((u16_t)ip4_addr2(ipaddr))
#define ip4_addr3_16(ipaddr) ((u16_t)ip4_addr3(ipaddr))
#define ip4_addr4_16(ipaddr) ((u16_t)ip4_addr4(ipaddr))

#define IP2STR(ipaddr) ip4_addr1_16(ipaddr), \
    ip4_addr2_16(ipaddr), \
    ip4_addr3_16(ipaddr), \
    ip4_addr4_16(ipaddr)

#define IPSTR "%d.%d.%d.%d"

extern const char * const tcp_state_str[];
/*
static const char * const tcp_state_str[] = {
  "CLOSED",
  "LISTEN",
  "SYN_SENT",
  "SYN_RCVD",
  "ESTABLISHED",
  "FIN_WAIT_1",
  "FIN_WAIT_2",
  "CLOSE_WAIT",
  "CLOSING",
  "LAST_ACK",
  "TIME_WAIT"
};
*/
/******************************************************************************
 * FunctionName : debug
 * Parameters   :
 * Returns      :
*******************************************************************************/
void print_udp_pcb(void)
{
  struct udp_pcb *pcb;
  bool prt_none = true;
  rtl_printf("UDP pcbs:\n");
  for(pcb = udp_pcbs; pcb != NULL; pcb = pcb->next) {
	  rtl_printf("flg:%02x\t" IPSTR ":%d\t" IPSTR ":%d\trecv:%p\n", pcb->flags, IP2STR(&pcb->local_ip), pcb->local_port, IP2STR(&pcb->remote_ip), pcb->remote_port, pcb->recv );
	  prt_none = false;
  }
  if(prt_none) rtl_printf("none\n");
}
/******************************************************************************
 * FunctionName : debug
 * Parameters   :
 * Returns      :
*******************************************************************************/
void print_tcp_pcb(void)
{
  struct tcp_pcb *pcb;
  rtl_printf("Active PCB states:\n");
  bool prt_none = true;
  for(pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
     rtl_printf("Port %d|%d\tflg:%02x\ttmr:%p\t%s\n", pcb->local_port, pcb->remote_port, pcb->flags, pcb->tmr, tcp_state_str[pcb->state]);
     prt_none = false;
  }
  if(prt_none) rtl_printf("none\n");
  rtl_printf("Listen PCB states:\n");
  prt_none = true;
  for(pcb = (struct tcp_pcb *)tcp_listen_pcbs.pcbs; pcb != NULL; pcb = pcb->next) {
    rtl_printf("Port %d|%d\tflg:%02x\ttmr:%p\t%s\n", pcb->local_port, pcb->remote_port, pcb->flags, pcb->tmr, tcp_state_str[pcb->state]);
    prt_none = false;
  }
  if(prt_none) rtl_printf("none\n");
  rtl_printf("TIME-WAIT PCB states:\n");
  prt_none = true;
  for(pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next) {
    rtl_printf("Port %d|%d\tflg:%02x\ttmr:%p\t%s\n", pcb->local_port, pcb->remote_port, pcb->flags, pcb->tmr, tcp_state_str[pcb->state]);
    prt_none = false;
  }
  if(prt_none) rtl_printf("none\n");
}
/******************************************************************************
 * FunctionName : debug
 * Parameters   :
 * Returns      :
*******************************************************************************/
//------------------------------------------------------------------------------
void fATLW(int argc, char *argv[]) 	// Info Lwip
{
	print_udp_pcb();
	print_tcp_pcb();
}

void fATDS(int argc, char *argv[]) 	// Deep sleep
{
	uint32 sleep_ms = 10000;
	if(argc > 2) sleep_ms = atoi(argv[1]);
#if 0
	// turn off log uart
    sys_log_uart_off();
    // initialize wakeup pin at PB_1
    gpio_t gpio_wake;
    gpio_init(&gpio_wake, PB_1);
    gpio_dir(&gpio_wake, PIN_INPUT);
    gpio_mode(&gpio_wake, PullDown);

    // enter deep sleep
    deepsleep_ex(DSLEEP_WAKEUP_BY_GPIO | DSLEEP_WAKEUP_BY_TIMER, 10000); */
	//	standby_wakeup_event_add(STANDBY_WAKEUP_BY_STIMER, 10000, 0);
//  deepstandby_ex();
//  sleep_ex(SLEEP_WAKEUP_BY_STIMER, 8000); // sleep_ex can't be put in irq handler
//	release_wakelock(WAKELOCK_OS);
#else
    deepsleep_ex(DSLEEP_WAKEUP_BY_TIMER, sleep_ms);
#endif
}

MON_RAM_TAB_SECTION COMMAND_TABLE console_commands1[] = {
		{"ATPN", 1, fATWC, "=<SSID>[,<PASSPHRASE>[,WEPKEY]]: WIFI Connect to AP"},
		{"ATWS", 1, fATWS, "=<URL,PORT>: MP3 Connect to URL\nATWS=<c>[lose]: Close MP3\nATWS=<r>[ead]: Read MP3 URL\nATWS=<s>[ave]: Save MP3 URL\nATWS=<?>: URL Info"},
		{"ATWD", 0, fATWD, ": WIFI Disconnect"},
		{"ATST", 0, fATST, ": Memory info"},
		{"ATLW", 0, fATLW, ": Lwip Info"},
		{"ATSB", 1, fATSB, "=<ADDRES(hex)>[,COUNT(dec)]: Dump byte register"},
		{"ATSD", 1, fATSD, "=<ADDRES(hex)>[,COUNT(dec)]: Dump dword register"},
		{"ATSW", 2, fATSW, "=<ADDRES(hex)>,<DATA(hex)>: Set register"},
		{"ATDS", 0, fATDS, "=[TIME(ms)]: Deep sleep"},
		{"ATON", 0, fATON, ": Open connections"},
		{"ATOF", 0, fATOF, ": Close connections"}
};

#endif //#ifdef CONFIG_AT_USR

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

#include "sleep_ex_api.h"

#include "lwip/tcp_impl.h"

extern void wifi_run(void);

#define printf rtl_printf // DiagPrintf

/* RAM/TCM/Heaps info */
extern void ShowMemInfo(void);
/*
void ShowMemInfo(void)
{
	printf("\nCLK CPU\t\t%d Hz\nRAM heap\t%d bytes\nTCM heap\t%d bytes\n",
			HalGetCpuClk(), xPortGetFreeHeapSize(), tcm_heap_freeSpace());
}
 */
// Mem info
void fATST(int argc, char *argv[]) {
		ShowMemInfo();
#if 0 //CONFIG_DEBUG_LOG > 1
		dump_mem_block_list();
		tcm_heap_dump();
#endif;
		printf("\n");
#if (configGENERATE_RUN_TIME_STATS == 1)
		char *cBuffer = pvPortMalloc(512);
		if(cBuffer != NULL) {
			vTaskGetRunTimeStats((char *)cBuffer);
			printf("%s", cBuffer);
		}
		vPortFree(cBuffer);
#endif
#if defined(configUSE_TRACE_FACILITY) && (configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS == 1)
	{
		char * pcWriteBuffer = malloc(1024);
		if(pcWriteBuffer) {
			vTaskList((char*)pcWriteBuffer);
			printf("\nTask List:\n");
	        printf("==============================\n");
	        printf("Name\t  Status Priority HighWaterMark TaskNumber\n%s\n", pcWriteBuffer);
			free(pcWriteBuffer);
		}
	}
#endif
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
void dump_bytes(uint32 addr, int size)
{
	uint8 buf[17];
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
// Dump byte register
void fATSB(int argc, char *argv[])
{
	int size = 16;
	uint32 addr = Strtoul(argv[1],0,16);
	if (argc > 2) {
		size = Strtoul(argv[2],0,10);
		if (size <= 0 || size > 16384)
			size = 16;
	}
	dump_bytes(addr, size);
}

// Dump dword register
void fATSD(int argc, char *argv[])
{
/*
	if (argc > 2) {
		int size = Strtoul(argv[2],0,10);
		if (size <= 0 || size > 16384)
			argv[2] = "16";
	}
*/
	CmdDumpWord(argc-1, (unsigned char**)(argv+1));
}

void fATSW(int argc, char *argv[])
{
	CmdWriteWord(argc-1, (unsigned char**)(argv+1));
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
		{"ATST", 0, fATST, ": Memory info"},
		{"ATLW", 0, fATLW, ": LwIP Info"},
		{"ATSB", 1, fATSB, "=<ADDRES(hex)>[,COUNT(dec)]: Dump byte register"},
		{"ATSD", 1, fATSD, "=<ADDRES(hex)>[,COUNT(dec)]: Dump dword register"},
		{"ATSW", 2, fATSW, "=<ADDRES(hex)>,<DATA(hex)>: Set register"},
		{"ATDS", 0, fATDS, "=[TIME(ms)]: Deep sleep"},
};

#endif //#ifdef CONFIG_AT_USR

/* Includes ------------------------------------------------------------------*/
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "ethernetif.h"
#include "main.h"
#include "lwip_netconf.h"
#if CONFIG_WLAN
#include "wifi_ind.h"
#endif
#if defined(STM32F2XX)
#include "stm322xg_eval_lcd.h"
#elif defined(STM32F4XX)
#include "stm324xg_eval_lcd.h"
#endif
#include <platform/platform_stdlib.h>


/*Static IP ADDRESS*/
#ifndef IP_ADDR0
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   80
#endif

/*NETMASK*/
#ifndef NETMASK_ADDR0
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef GW_ADDR0
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1
#endif

/*Static IP ADDRESS*/
#ifndef AP_IP_ADDR0
#define AP_IP_ADDR0   192
#define AP_IP_ADDR1   168
#define AP_IP_ADDR2   43
#define AP_IP_ADDR3   1
#endif
   
/*NETMASK*/
#ifndef AP_NETMASK_ADDR0
#define AP_NETMASK_ADDR0   255
#define AP_NETMASK_ADDR1   255
#define AP_NETMASK_ADDR2   255
#define AP_NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef AP_GW_ADDR0
#define AP_GW_ADDR0   192
#define AP_GW_ADDR1   168
#define AP_GW_ADDR2   43
#define AP_GW_ADDR3   1  
#endif

/*Static IP ADDRESS FOR ETHERNET*/
#ifndef ETH_IP_ADDR0
#define ETH_IP_ADDR0 192
#define ETH_IP_ADDR1 168
#define ETH_IP_ADDR2 0
#define ETH_IP_ADDR3 80
#endif

/*NETMASK FOR ETHERNET*/
#ifndef ETH_NETMASK_ADDR0
#define ETH_NETMASK_ADDR0 255
#define ETH_NETMASK_ADDR1 255
#define ETH_NETMASK_ADDR2 255
#define ETH_NETMASK_ADDR3 0
#endif

/*Gateway address for ethernet*/
#ifndef ETH_GW_ADDR0
#define ETH_GW_ADDR0 192
#define ETH_GW_ADDR1 168
#define ETH_GW_ADDR2 0
#define ETH_GW_ADDR3 1
#endif

/* Private define ------------------------------------------------------------*/
#define MAX_DHCP_TRIES 5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

struct netif xnetif[NET_IF_NUM]; /* network interface structure */
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
#if CONFIG_WLAN
extern  char error_flag;
extern 	unsigned char wifi_mode; // rtw_mode_t
#endif

int lwip_init_done = 0;

void LwIP_Init(void)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	int8_t idx = 0;
	/* Create tcp_ip stack thread */
	tcpip_init( NULL, NULL );	

	/* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
	        struct ip_addr *netmask, struct ip_addr *gw,
	        void *state, err_t (* init)(struct netif *netif),
	        err_t (* input)(struct pbuf *p, struct netif *netif))

	Adds your network interface to the netif_list. Allocate a struct
	netif and pass a pointer to this structure as the first argument.
	Give pointers to cleared ip_addr structures when using DHCP,
	or fill them with sane numbers otherwise. The state pointer may be NULL.

	The init function pointer must point to a initialization function for
	your ethernet netif interface. The following code illustrates it's use.*/
	//printf("NET_IF_NUM:%d\n\r",NET_IF_NUM);
	for(idx = NET_IF_NUM - 1; idx >= 0 ; idx--) {
		if(idx == 0){
			IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
			IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
			IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
		}
#if CONFIG_ETHERNET
		else if(idx == NET_IF_NUM - 1)
		{
			IP4_ADDR(&ipaddr, ETH_IP_ADDR0, ETH_IP_ADDR1, ETH_IP_ADDR2, ETH_IP_ADDR3);
			IP4_ADDR(&netmask, ETH_NETMASK_ADDR0, ETH_NETMASK_ADDR1 , ETH_NETMASK_ADDR2, ETH_NETMASK_ADDR3);
			IP4_ADDR(&gw, ETH_GW_ADDR0, ETH_GW_ADDR1, ETH_GW_ADDR2, ETH_GW_ADDR3);
		}
#endif
		else {
			IP4_ADDR(&ipaddr, AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
			IP4_ADDR(&netmask, AP_NETMASK_ADDR0, AP_NETMASK_ADDR1 , AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
			IP4_ADDR(&gw, AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
		}
		xnetif[idx].name[0] = 'r';
		xnetif[idx].name[1] = '0'+idx;
		
#if CONFIG_ETHERNET
    if(idx == NET_IF_NUM - 1)
      netif_add(&xnetif[idx], &ipaddr, &netmask, &gw, NULL, &ethernetif_mii_init, &tcpip_input);
    else
      netif_add(&xnetif[idx], &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
#else
    	netif_add(&xnetif[idx], &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
#endif
    	info_printf("interface %d is initialized\n", idx);
	}
	
	/*  Registers the default network interface. */
	netif_set_default(&xnetif[0]);
	
	/*  When the netif is fully configured this function must be called.*/
	for(idx = 0; idx < NET_IF_NUM; idx++)
		netif_set_up(&xnetif[idx]); 
	
	lwip_init_done = 1;	 
}

/**
  * @brief  LwIP_DHCP_Process_Handle
  * @param  None
  * @retval None
  */
uint8_t LwIP_DHCP(uint8_t idx, uint8_t dhcp_state) {
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	uint32_t IPaddress;
#if CONFIG_DEBUG_LOG > 2
	uint8_t iptab[4];
#endif
	uint8_t DHCP_state;
	int mscnt = 0;
	struct netif *pnetif = NULL;

	DHCP_state = dhcp_state;

#if !CONFIG_ETHERNET
	if (idx > 1)
		idx = 1;
#endif

	pnetif = &xnetif[idx];
	if (DHCP_state == 0) {
		pnetif->ip_addr.addr = 0;
		pnetif->netmask.addr = 0;
		pnetif->gw.addr = 0;
	}

	for (;;) {
		//info_printf("\n\r ========DHCP_state:%d============\n\r",DHCP_state);
		switch (DHCP_state) {
		case DHCP_START: {
#if CONFIG_WLAN
			wifi_unreg_event_handler(WIFI_EVENT_BEACON_AFTER_DHCP, wifi_rx_beacon_hdl);
#endif
			dhcp_start(pnetif);
			IPaddress = 0;
			DHCP_state = DHCP_WAIT_ADDRESS;
		}
			break;

		case DHCP_WAIT_ADDRESS: {
			/* Read the new IP address */
			IPaddress = pnetif->ip_addr.addr;

			if (IPaddress != 0) {
				DHCP_state = DHCP_ADDRESS_ASSIGNED;
#if CONFIG_WLAN
				wifi_reg_event_handler(WIFI_EVENT_BEACON_AFTER_DHCP, wifi_rx_beacon_hdl, NULL);
#endif

				/* Stop DHCP */
				// dhcp_stop(pnetif);  /* can not stop, need to renew, Robbie*/
#if CONFIG_DEBUG_LOG > 2
				iptab[0] = (uint8_t) (IPaddress >> 24);
				iptab[1] = (uint8_t) (IPaddress >> 16);
				iptab[2] = (uint8_t) (IPaddress >> 8);
				iptab[3] = (uint8_t) (IPaddress);
				info_printf("Interface %d IP address: %d.%d.%d.%d\n", idx, iptab[3],
						iptab[2], iptab[1], iptab[0]);
#endif
#if CONFIG_WLAN
				error_flag = RTW_NO_ERROR;
#endif
				return DHCP_ADDRESS_ASSIGNED;
			} else {
				/* DHCP timeout */
				if (pnetif->dhcp->tries > MAX_DHCP_TRIES) {
					DHCP_state = DHCP_TIMEOUT;

					/* Stop DHCP */
					dhcp_stop(pnetif);

					/* Static address used */
					IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
					IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
					IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
					netif_set_addr(pnetif, &ipaddr, &netmask, &gw);
#if CONFIG_DEBUG_LOG > 2

					iptab[0] = IP_ADDR3;
					iptab[1] = IP_ADDR2;
					iptab[2] = IP_ADDR1;
					iptab[3] = IP_ADDR0;
					info_printf("Interface %d DHCP timeout\n", idx);
					info_printf("Static IP address: %d.%d.%d.%d\n", iptab[3], iptab[2], iptab[1], iptab[0]);
#endif
#if CONFIG_WLAN
					error_flag = RTW_DHCP_FAIL;
#endif
					return DHCP_TIMEOUT;
				} else {
					//sys_msleep(DHCP_FINE_TIMER_MSECS);
					vTaskDelay(DHCP_FINE_TIMER_MSECS);
					dhcp_fine_tmr();
					mscnt += DHCP_FINE_TIMER_MSECS;
					if (mscnt >= DHCP_COARSE_TIMER_SECS * 1000) {
						dhcp_coarse_tmr();
						mscnt = 0;
					}
				}
			}
		}
			break;
		case DHCP_RELEASE_IP:
#if CONFIG_WLAN
			wifi_unreg_event_handler(WIFI_EVENT_BEACON_AFTER_DHCP, wifi_rx_beacon_hdl);
#endif
			info_printf("LwIP_DHCP(%d): Release ip\n", idx);
			dhcp_release_unicast(pnetif);
			return DHCP_RELEASE_IP;
		case DHCP_STOP:
#if CONFIG_WLAN
			wifi_unreg_event_handler(WIFI_EVENT_BEACON_AFTER_DHCP, wifi_rx_beacon_hdl);
#endif
			info_printf("LwIP_DHCP(%d): dhcp stop.\n", idx);
			dhcp_stop(pnetif);
			return DHCP_STOP;
		default:
			break;
		}
	}
}

uint8_t* LwIP_GetMAC(struct netif *pnetif)
{
	return (uint8_t *) (pnetif->hwaddr);
}

uint8_t* LwIP_GetIP(struct netif *pnetif)
{
	return (uint8_t *) &(pnetif->ip_addr);
}

uint8_t* LwIP_GetGW(struct netif *pnetif)
{
	return (uint8_t *) &(pnetif->gw);
}

uint8_t* LwIP_GetMASK(struct netif *pnetif)
{
	return (uint8_t *) &(pnetif->netmask);
}

uint8_t* LwIP_GetBC(struct netif *pnetif)
{
	return (uint8_t *) &(pnetif->dhcp->offered_bc_addr);
}

#if LWIP_DNS
void LwIP_GetDNS(struct ip_addr* dns)
{
	*dns = dns_getserver(0);
}

void LwIP_SetDNS(struct ip_addr* dns)
{
	dns_setserver(0, dns);
}
#endif
void LwIP_UseStaticIP(struct netif *pnetif)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;

	/* Static address used */
	if(pnetif->name[1] == '0'){
#if CONFIG_WLAN
		if(wifi_mode == RTW_MODE_STA){
		IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
		IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
		IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
		}
		else if(wifi_mode == RTW_MODE_AP){
			IP4_ADDR(&ipaddr, AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
			IP4_ADDR(&netmask, AP_NETMASK_ADDR0, AP_NETMASK_ADDR1 , AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
			IP4_ADDR(&gw, AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
		}
#endif
	}else{
		IP4_ADDR(&ipaddr, AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
		IP4_ADDR(&netmask, AP_NETMASK_ADDR0, AP_NETMASK_ADDR1 , AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
		IP4_ADDR(&gw, AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
	}
	
	netif_set_addr(pnetif, &ipaddr , &netmask, &gw);
}
#if LWIP_AUTOIP
#include <lwip/autoip.h>

void LwIP_AUTOIP(struct netif *pnetif)
{
	uint8_t *ip = LwIP_GetIP(pnetif);

	autoip_start(pnetif);

	while((pnetif->autoip->state == AUTOIP_STATE_PROBING) || (pnetif->autoip->state == AUTOIP_STATE_ANNOUNCING)) {
		vTaskDelay(1000);
	}

	if(*((uint32_t *) ip) == 0) {
		struct ip_addr ipaddr;
		struct ip_addr netmask;
		struct ip_addr gw;

		printf("AUTOIP timeout\n");

		/* Static address used */
		IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
		IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
		IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
		netif_set_addr(pnetif, &ipaddr , &netmask, &gw);
		printf("Static IP address : %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
	}
	else {
		printf("Link-local address: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
	}
}
#endif
#if LWIP_IPV6
/* Get IPv6 address with lwip 1.5.0 */
void LwIP_AUTOIP_IPv6(struct netif *pnetif)
{
	uint8_t *ipv6 = (uint8_t *) &(pnetif->ip6_addr[0].addr[0]);

	netif_create_ip6_linklocal_address(pnetif, 1);
	printf("IPv6 link-local address: %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
	       ipv6[0], ipv6[1],  ipv6[2],  ipv6[3],  ipv6[4],  ipv6[5],  ipv6[6], ipv6[7],
	       ipv6[8], ipv6[9], ipv6[10], ipv6[11], ipv6[12], ipv6[13], ipv6[14], ipv6[15]);
}
#endif

/**
 * @file
 * NetBIOS name service sample
 * https://tools.ietf.org/html/rfc1002 
 * Modified for RTL871x pvvx
 */

#if 1 // def USE_NETBIOS

/*
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 */
#include "rtl8195a/rtl_common.h"
#include "rtl8195a.h"

//#include "lwip/opt.h"
#include "netbios/netbios.h"

#if LWIP_UDP  /* don't build if not configured for use in lwipopts.h */

#include <string.h>

#include "ipv4/lwip/ip.h"
#include "lwip/udp.h"
#include "lwip/netif.h"
#include "lwip_netconf.h"
#include "platform/esp_comp.h"

#define NETBIOS_CODE_ATTR
#define NETBIOS_DATA_ATTR

extern struct netif xnetif[NET_IF_NUM];

#define NBS_DEF_NAME	DEF_HOSTNAME

/** This is an example implementation of a NetBIOS name server.
 * It responds to name queries for a configurable name.
 * Name resolving is not supported.
 *
 * Note that the device doesn't broadcast it's own name so can't
 * detect duplicate names!
 */

/** Since there's no standard function for case-insensitive string comparision,
 * we need another define here:
 * define this to stricmp() for windows or strcasecmp() for linux.
 * If not defined, comparision is case sensitive and NETBIOS_LWIP_NAME must be
 * uppercase
 */
#ifndef NETBIOS_STRCMP
#define NETBIOS_STRCMP(str1, str2) os_strcmp(str1, str2)
#endif

/** default port number for "NetBIOS Name service */
//#define NETBIOS_PORT     137
/** NetBIOS name of LWIP device
 * This must be uppercase until NETBIOS_STRCMP() is defined to a string
 * comparision function that is case insensitive.
 * If you want to use the netif's hostname, use this (with LWIP_NETIF_HOSTNAME):
 * (ip_current_netif() != NULL ? ip_current_netif()->hostname != NULL ? ip_current_netif()->hostname : "" : "")
 */

/** size of a NetBIOS name */
//#define NETBIOS_NAME_LEN 16 // in netbios.h
char netbios_name[NET_IF_NUM][NETBIOS_NAME_LEN + 1]; // default netifs: 0 - SoftAP, 1 - Station, 2 - Ethernet

/** The Time-To-Live for NetBIOS name responds (in seconds)
 * Default is 300000 seconds (3 days, 11 hours, 20 minutes) */
#define NETBIOS_NAME_TTL 300000

/** NetBIOS header flags */
#define NETB_HFLAG_RESPONSE           0x8000U
#define NETB_HFLAG_OPCODE             0x7800U
#define NETB_HFLAG_OPCODE_NAME_QUERY  0x0000U
#define NETB_HFLAG_AUTHORATIVE        0x0400U
#define NETB_HFLAG_TRUNCATED          0x0200U
#define NETB_HFLAG_RECURS_DESIRED     0x0100U
#define NETB_HFLAG_RECURS_AVAILABLE   0x0080U
#define NETB_HFLAG_BROADCAST          0x0010U
#define NETB_HFLAG_REPLYCODE          0x0008U
#define NETB_HFLAG_REPLYCODE_NOERROR  0x0000U

/** NetBIOS name flags */
#define NETB_NFLAG_UNIQUE             0x8000U
#define NETB_NFLAG_NODETYPE           0x6000U
#define NETB_NFLAG_NODETYPE_HNODE     0x6000U
#define NETB_NFLAG_NODETYPE_MNODE     0x4000U
#define NETB_NFLAG_NODETYPE_PNODE     0x2000U
#define NETB_NFLAG_NODETYPE_BNODE     0x0000U

/** NetBIOS message header */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct netbios_hdr {
	PACK_STRUCT_FIELD(u16_t trans_id);
	PACK_STRUCT_FIELD(u16_t flags);
	PACK_STRUCT_FIELD(u16_t questions);
	PACK_STRUCT_FIELD(u16_t answerRRs);
	PACK_STRUCT_FIELD(u16_t authorityRRs);
	PACK_STRUCT_FIELD(u16_t additionalRRs);
}PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/** NetBIOS message name part */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct netbios_name_hdr {
	PACK_STRUCT_FIELD(u8_t nametype);
	PACK_STRUCT_FIELD(u8_t encname[(NETBIOS_NAME_LEN*2)+1]);
	PACK_STRUCT_FIELD(u16_t type);
	PACK_STRUCT_FIELD(u16_t cls);
	PACK_STRUCT_FIELD(u32_t ttl);
	PACK_STRUCT_FIELD(u16_t datalen);
	PACK_STRUCT_FIELD(u16_t flags);
	PACK_STRUCT_FIELD(ip_addr_p_t addr);
}PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/** NetBIOS message */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct netbios_resp {
	struct netbios_hdr resp_hdr;
	struct netbios_name_hdr resp_name;
}PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

//#define toupper(CH) \
//  (((CH) >= 'a' && (CH) <= 'z') ? ((CH) - 'a' + 'A') : (CH))


/** NetBIOS decoding name */
static int8_t NETBIOS_CODE_ATTR NBNS_decode(char *dst, char *src)
{
  uint8_t i, j;
  char c;

  for(i=0, j=0; i<15; i++)
  {
    c  = (src[j++]-'A')<<4;
    c |= (src[j++]-'A')<<0;
    if(c == ' ')
    {
      break;
    }
    dst[i] = toupper(c);
  }
  dst[i] = 0;

  return (((src[30]-'A')<<4)|(src[31]-'A')); // 0x00 = Workstation
}

#if 0
/** NetBIOS encoding name */
static void NBNS_encode(char *dst, char *src, uint8_t type)
{
  uint8_t i, j;
  char c;

  //encode name
  for(i=0, j=0; (i<15) && src[i]; i++)
  {
    c = toupper(src[i]);
    dst[j++] = 'A'+((c>>4)&0x0f);
    dst[j++] = 'A'+((c>>0)&0x0f);
  }

  //add spaces
  for(; i<15; i++)
  {
    dst[j++] = 'A'+((' '>>4)&0x0f);
    dst[j++] = 'A'+((' '>>0)&0x0f);
  }

  //set type (0x00 = Workstation)
  dst[j++] = 'A'+((type>>4)&0x0f);
  dst[j++] = 'A'+((type>>0)&0x0f);
}
#endif

/** NetBIOS Name service recv callback */
static void NETBIOS_CODE_ATTR
netbios_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, ip_addr_t *addr,
		u16_t port) {
	LWIP_UNUSED_ARG(arg);
	/* if packet is valid */
	if (p != NULL && p->len >= sizeof(struct netbios_hdr) + sizeof(struct netbios_name_hdr) - 12) {
		if (current_netif != NULL && current_netif->num < NET_IF_NUM) {
			uint32 ip = current_netif->ip_addr.addr;
			char *curbiosname = netbios_name[current_netif->num];
			if (curbiosname[0] != '\0' && ip != NULL
			/* we only answer if we got a default interface */
			&& (((ip ^ addr->addr) & current_netif->netmask.addr) == 0)) { // запрет ответа другой подсети
#if DEBUGSOO > 3
				os_printf("nbns: " IPSTR ",\t'%s'\n", IP2STR(&ip), curbiosname);
#endif
				char netbiosname[NETBIOS_NAME_LEN + 1];
				os_memset(netbiosname, 0, sizeof(netbiosname));
				struct netbios_hdr* netbios_hdr =
						(struct netbios_hdr*) p->payload;
				struct netbios_name_hdr* netbios_name_hdr =
						(struct netbios_name_hdr*) (netbios_hdr + 1);
				/* @todo: do we need to check answerRRs/authorityRRs/additionalRRs? */
				/* if the packet is a NetBIOS name query question */
				if (((netbios_hdr->flags & PP_NTOHS(NETB_HFLAG_OPCODE))
						== PP_NTOHS(NETB_HFLAG_OPCODE_NAME_QUERY))
						&& ((netbios_hdr->flags & PP_NTOHS(NETB_HFLAG_RESPONSE))
								== 0)
						&& (netbios_hdr->questions == PP_NTOHS(1))
						&& (netbios_name_hdr->cls == PP_NTOHS(1))
						&& (netbios_name_hdr->type == PP_NTOHS(0x20))) {
					/* decode the NetBIOS name */
					int8_t ret = NBNS_decode(netbiosname,
							(char*) (netbios_name_hdr->encname));
					/* if the packet is for us */
#if DEBUGSOO > 2
					if (ret == 0) os_printf("nbns: get " IPSTR ", '%s'\n", IP2STR(addr),
							netbiosname);
#endif
					if (ret == 0 && NETBIOS_STRCMP(curbiosname, netbiosname) == 0) { // netbiosname[0] == '*'
#if DEBUGSOO > 1
						os_printf("nbns: out " IPSTR ", '%s'\n", IP2STR(&ip),
								curbiosname);
#endif
						struct pbuf *q;
						struct netbios_resp *resp;

						q = pbuf_alloc(PBUF_TRANSPORT,
								sizeof(struct netbios_resp), PBUF_RAM);
						if (q != NULL) {
							resp = (struct netbios_resp*) q->payload;

							/* prepare NetBIOS header response */
							resp->resp_hdr.trans_id = netbios_hdr->trans_id;
							resp->resp_hdr.flags =
									PP_HTONS(
											NETB_HFLAG_RESPONSE | NETB_HFLAG_OPCODE_NAME_QUERY | NETB_HFLAG_AUTHORATIVE | NETB_HFLAG_RECURS_DESIRED);
							resp->resp_hdr.questions = 0;
							resp->resp_hdr.answerRRs = PP_HTONS(1);
							resp->resp_hdr.authorityRRs = 0;
							resp->resp_hdr.additionalRRs = 0;

							/* prepare NetBIOS header datas */
							MEMCPY(resp->resp_name.encname,
									netbios_name_hdr->encname,
									sizeof(netbios_name_hdr->encname));
							resp->resp_name.nametype =
									netbios_name_hdr->nametype;
							resp->resp_name.type = netbios_name_hdr->type;
							resp->resp_name.cls = netbios_name_hdr->cls;
							resp->resp_name.ttl = PP_HTONL(NETBIOS_NAME_TTL);
							resp->resp_name.datalen = PP_HTONS(
									sizeof(resp->resp_name.flags)
											+ sizeof(resp->resp_name.addr));
							resp->resp_name.flags = PP_HTONS(
									NETB_NFLAG_NODETYPE_BNODE);

							resp->resp_name.addr.addr = ip; // netif_default->ip_addr.addr;
							/* send the NetBIOS response */
							udp_sendto(upcb, q, addr, port);
							/* free the "reference" pbuf */
							pbuf_free(q);
						}
					}
				}
			}
		}
		/* free the pbuf */
		pbuf_free(p);
	}
}

struct udp_pcb * NETBIOS_CODE_ATTR netbios_pcb(void) {
	struct udp_pcb *pcb;
	for (pcb = udp_pcbs; pcb != NULL; pcb = pcb->next) {
		if (pcb->local_port == NETBIOS_PORT)
			return pcb;
	}
	return NULL;
}

/* default netifs/interfacenum: 0 - SoftAP, 1 - Station, 2 - Ethernet */

bool NETBIOS_CODE_ATTR netbios_set_name(unsigned char interfacenum, char * name) {
	if (interfacenum >= NET_IF_NUM)
		return false;
	int i;
	uint8 * pnbn = netbios_name[interfacenum];
	uint8 * pmane = name;
	if (name != NULL) {
		for (i = 0; i < NETBIOS_NAME_LEN; i++) {
			if (*pmane < ' ')
				break;
			else if (*pmane == ' ')
				*pnbn++ = '_';
			else
				*pnbn++ = toupper(*pmane);
			pmane++;
		};
	};
	*pnbn = '\0';
	return true;
}

bool NETBIOS_CODE_ATTR netbios_off(void) {
	struct udp_pcb *pcb = netbios_pcb();
	if (pcb == NULL)
		return false;
	udp_remove(pcb);
	return true;
}

void NETBIOS_CODE_ATTR netbios_init(void) {
	struct udp_pcb *pcb;
	char buf[NETBIOS_NAME_LEN];
	if (netbios_pcb() != NULL)
		return;

	for(int i = 0; i < NET_IF_NUM; i++) {
		if (netbios_name[i][0] == 0) {
#if LWIP_NETIF_HOSTNAME
			if(xnetif[i].hostname != 0) {
				netbios_set_name(i, xnetif[i].hostname);
			}
			else 
#endif
			{
				sprintf(buf, NBS_DEF_NAME "%d", i);
				netbios_set_name(i, buf);
			};
		};
	};

#if DEBUGSOO > 1
	os_printf("NetBIOS init, interface ");
	for(int i = 0; i < NET_IF_NUM; i++) {
		os_printf("%d: '%s' ", i, netbios_name[i]);
	}
	os_printf("\n");
#endif
	pcb = udp_new();
	if (pcb != NULL) {
		/* we have to be allowed to send broadcast packets! */
		pcb->so_options |= SOF_BROADCAST;
		udp_bind(pcb, IP_ADDR_ANY, NETBIOS_PORT);
		udp_recv(pcb, netbios_recv, pcb);
	}
}
#endif /* LWIP_UDP */

#endif // USE_NETBIOS

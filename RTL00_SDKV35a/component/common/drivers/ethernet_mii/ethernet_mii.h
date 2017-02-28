#ifndef __MII_ETHERNETIF_H__
#define __MII_ETHERNETIF_H__

#include "lwip_netconf.h"

#define MII_TX_DESC_CNT		4
#define MII_RX_DESC_CNT		10
#if CONFIG_ETHERNET
extern s8 rltk_mii_send(struct eth_drv_sg *sg_list, int sg_len, int total_len);
extern void rltk_mii_recv(struct eth_drv_sg *sg_list, int sg_len);
#endif
#endif // __MII_ETHERNETIF_H__

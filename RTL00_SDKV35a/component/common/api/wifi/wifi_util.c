#include <wireless.h>
#include <wlan_intf.h>
#include "FreeRTOS.h"
#include <platform/platform_stdlib.h>
#include <wifi/wifi_conf.h>
#include <wifi/wifi_ind.h>
#if 1
#include "drv_types.h" // or #include "wlan_lib.h"
#else
#include "wifi_constants.h"
#include "wifi_structures.h"
#include "wlan_lib.h" // or #include "drv_types.h"
#endif
#include <osdep_service.h>

#define USE_WIFI_ADAPTER 1 // использовать прямое обращение в тело драйвера WiFi

int iw_ioctl(const char * ifname, unsigned long request, struct iwreq * pwrq) {
	memcpy(pwrq->ifr_name, ifname, 5);
	int ret = rltk_wlan_control(request, (void *) pwrq);
#if	CONFIG_DEBUG_LOG > 3
	debug_printf("ioctl[%p, '%s', %02x %02x %02x %02x ...] = %d\n",
				request, ifname, pwrq->u.name[0], pwrq->u.name[1],
				pwrq->u.name[2], pwrq->u.name[3], ret);
#endif
#if	CONFIG_DEBUG_LOG > 4
		if (pwrq->u.data.length) {
			extern void dump_bytes(uint32 addr, int size);
			dump_bytes(pwrq->u.data.pointer, pwrq->u.data.length);
		}
#endif
	return ret;
}

#ifdef USE_WIFI_ADAPTER
extern Rltk_wlan_t rltk_wlan_info[2]; // in wrapper.h
LOCAL _adapter * get_padapter(const char *ifname) {
	if(ifname[4] == '0') {
		return *(_adapter **)((rltk_wlan_info[0].dev)->priv);
	} else {
		return *(_adapter **)((rltk_wlan_info[1].dev)->priv);
	}
	return NULL;
};
#endif

/* ssid = NULL -> not connected */
int wext_get_ssid(const char *ifname, __u8 *ssid) {
#ifdef USE_WIFI_ADAPTER
	_adapter * pad = get_padapter(ifname);
	rtw_result_t ret = RTW_ERROR;
	if(pad != NULL && (pad->mlmepriv.fw_state & 0x41) != 0) {
		int len = pad->mlmepriv.cur_network.network.Ssid.SsidLength;
		if(len > 32) rtw_memcpy(ssid, pad->mlmepriv.cur_network.network.Ssid.Ssid, 32);
		else {
			rtw_memcpy(ssid, &pad->mlmepriv.cur_network.network.Ssid.Ssid, len);
			ssid[len] = '\0';
			debug_printf("s=[%s]\n", ssid);
		}
		ret = RTW_SUCCESS;
	}
	return ret;
#else
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.essid.pointer = ssid;
	iwr.u.essid.length = 32;
	int ret = iw_ioctl(ifname, SIOCGIWESSID, &iwr);
	if (ret >= 0) {
		ret = iwr.u.essid.length;
		if (ret > 32)
			ret = 32;
		/* Some drivers include nul termination in the SSID, so let's
		 * remove it here before further processing. WE-21 changes this
		 * to explicitly require the length _not_ to include nul
		 * termination. */
		if (ret > 0 && ssid[ret - 1] == '\0')
			ret--;
		ssid[ret] = '\0';
	}
	return ret;
#endif
}

int wext_set_ssid(const char *ifname, const __u8 *ssid, __u16 ssid_len) {
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.essid.pointer = (void *) ssid;
	iwr.u.essid.length = ssid_len;
	iwr.u.essid.flags = (ssid_len != 0);
	return iw_ioctl(ifname, SIOCSIWESSID, &iwr);
}

int wext_set_bssid(const char *ifname, const __u8 *bssid) {
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.ap_addr.sa_family = ARPHRD_ETHER;
	memcpy(iwr.u.ap_addr.sa_data, bssid, ETH_ALEN);
	if (bssid[ETH_ALEN] == '#' && bssid[ETH_ALEN + 1] == '@') {
		memcpy(iwr.u.ap_addr.sa_data + ETH_ALEN, bssid + ETH_ALEN, 6);
	}
	return iw_ioctl(ifname, SIOCSIWAP, &iwr);
}

int is_broadcast_ether_addr(const unsigned char *addr) {
	return (addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5]) == 0xff;
}

int wext_set_auth_param(const char *ifname, __u16 idx, __u32 value) {
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.param.flags = idx & IW_AUTH_INDEX;
	iwr.u.param.value = value;
	return iw_ioctl(ifname, SIOCSIWAUTH, &iwr);
}

int wext_set_key_ext(const char *ifname, __u16 alg, const __u8 *addr,
		int key_idx, int set_tx, const __u8 *seq, __u16 seq_len, __u8 *key,
		__u16 key_len) {
	struct iwreq iwr;
	int ret = -1;
	struct iw_encode_ext *ext;

	ext = (struct iw_encode_ext *) pvPortMalloc(
			sizeof(struct iw_encode_ext) + key_len);
	if (ext != NULL) {
		memset(ext, 0, sizeof(struct iw_encode_ext) + key_len);
		memset(&iwr, 0, sizeof(iwr));
		iwr.u.encoding.flags = key_idx + 1;
		iwr.u.encoding.flags |= IW_ENCODE_TEMP;
		iwr.u.encoding.pointer = ext;
		iwr.u.encoding.length = sizeof(struct iw_encode_ext) + key_len;
		if (alg == IW_ENCODE_DISABLED)
			iwr.u.encoding.flags |= IW_ENCODE_DISABLED;
		if (addr == NULL || is_broadcast_ether_addr(addr))
			ext->ext_flags |= IW_ENCODE_EXT_GROUP_KEY;
		if (set_tx)
			ext->ext_flags |= IW_ENCODE_EXT_SET_TX_KEY;
		ext->addr.sa_family = ARPHRD_ETHER;
		if (addr)
			memcpy(ext->addr.sa_data, addr, ETH_ALEN);
		else
			memset(ext->addr.sa_data, 0xff, ETH_ALEN);
		if (key && key_len) {
			memcpy(ext->key, key, key_len);
			ext->key_len = key_len;
		}
		ext->alg = alg;
		if (seq && seq_len) {
			ext->ext_flags |= IW_ENCODE_EXT_RX_SEQ_VALID;
			memcpy(ext->rx_seq, seq, seq_len);
		}
		ret = iw_ioctl(ifname, SIOCSIWENCODEEXT, &iwr);
		vPortFree(ext);
	}
#if	CONFIG_DEBUG_LOG > 3
	else {
		error_printf("%s: Can't malloc memory!\n", __func__);
	}
#endif
	return ret;
}

int wext_get_enc_ext(const char *ifname, __u16 *alg, __u8 *key_idx,
		__u8 *passphrase) {
	struct iwreq iwr;
	int ret = -1;
	struct iw_encode_ext *ext;

	ext = (struct iw_encode_ext *) pvPortMalloc(
			sizeof(struct iw_encode_ext) + 16);
	if (ext != NULL) {
		memset(ext, 0, sizeof(struct iw_encode_ext) + 16);
		iwr.u.encoding.pointer = ext;
		ret = iw_ioctl(ifname, SIOCGIWENCODEEXT, &iwr);
		if (ret >= 0) {
			*alg = ext->alg;
			if (key_idx)
				*key_idx = (__u8 ) iwr.u.encoding.flags;
			if (passphrase)
				memcpy(passphrase, ext->key, ext->key_len);
		}
		vPortFree(ext);
	}
#if	CONFIG_DEBUG_LOG > 3
	else {
		error_printf("%s: Can't malloc memory!\n", __func__);
	}	
#endif
	return ret;
}

int wext_set_passphrase(const char *ifname, const __u8 *passphrase,
		__u16 passphrase_len) {
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.passphrase.pointer = (void *) passphrase;
	iwr.u.passphrase.length = passphrase_len;
	iwr.u.passphrase.flags = (passphrase_len != 0);
	return iw_ioctl(ifname, SIOCSIWPRIVPASSPHRASE, &iwr);
}

int wext_get_passphrase(const char *ifname, __u8 *passphrase) {
#if USE_WIFI_ADAPTER
	extern int rtw_wx_get_passphrase(struct net_device *dev, struct iw_request_info *a, union iwreq_data *wrqu, char *extra);
	struct net_device * pdev = rltk_wlan_info[0].dev;
	if(ifname[4] != '0')
		pdev = rltk_wlan_info[1].dev;
	rtw_result_t ret = RTW_ERROR;
	if(pdev) {
		uint16 len[4];
		ret = rtw_wx_get_passphrase(pdev, 0, &len, passphrase);
		if(ret == RTW_SUCCESS) passphrase[len[2]] = '\0';
		debug_printf("pas[%d]-<%s>\n", len[2], passphrase);
	}
	return ret;
#else
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.passphrase.pointer = (void *) passphrase;
	int ret = iw_ioctl(ifname, SIOCGIWPRIVPASSPHRASE, &iwr);
	if (ret >= 0) {
		ret = iwr.u.passphrase.length;
		passphrase[ret] = '\0';
	}
	return ret;
#endif
}

#if 0
int wext_set_mac_address(const char *ifname, char * mac)
{
	char buf[13+17+1];
	memset(buf, 0, sizeof(buf));
	snprintf(buf, 13+17, "write_mac %s", mac);
	return wext_private_command(ifname, buf, 0);
}

int wext_get_mac_address(const char *ifname, char * mac)
{
	int ret = 0;
	char buf[32];

	memset(buf, 0, sizeof(buf));
	memcpy(buf, "read_mac", 8);
	ret = wext_private_command_with_retval(ifname, buf, buf, 32);
	strcpy(mac, buf);
	return ret;
}
#endif

int wext_enable_powersave(const char *ifname, __u8 ips_mode, __u8 lps_mode) {
#ifdef USE_WIFI_ADAPTER
	_adapter * pad = get_padapter(ifname);
	rtw_result_t ret = RTW_ERROR;
	if(pad) {
		ret = rtw_pm_set_ips(pad, ips_mode); // 2 режима 1,2 !
		if(ret == RTW_SUCCESS) {
			LeaveAllPowerSaveMode(pad);
			ret = rtw_pm_set_lps(pad, lps_mode);
		}
	}
	return ret;
#else
	struct iwreq iwr;
	__u16 pindex = 7;
	__u8 para[16]; // 7 + (1+1+1) + (1+1+1)
	int cmd_len = sizeof("pm_set");
	memset(&iwr, 0, sizeof(iwr));
	// Encode parameters as TLV (type, length, value) format
	snprintf((char*) para, cmd_len, "pm_set");
	para[pindex++] = 0; // type 0 for ips
	para[pindex++] = 1;
	para[pindex++] = ips_mode;
	para[pindex++] = 1; // type 1 for lps
	para[pindex++] = 1;
	para[pindex++] = lps_mode;
	iwr.u.data.pointer = &para[0];
	iwr.u.data.length = pindex;
	return iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
#endif
}
/*
int wext_disable_powersave(const char *ifname) {
	struct iwreq iwr;
	__u16 pindex = 7;
	__u8 para[16]; // 7 + (1+1+1) + (1+1+1)
	int cmd_len = sizeof("pm_set");
	memset(&iwr, 0, sizeof(iwr));
	// Encode parameters as TLV (type, length, value) format
	snprintf((char*) para, cmd_len, "pm_set");
	para[pindex++] = 0; // type 0 for ips
	para[pindex++] = 1;
	para[pindex++] = 0; // ips = 0
	para[pindex++] = 1; // type 1 for lps
	para[pindex++] = 1;
	para[pindex++] = 0; // lps = 0
	iwr.u.data.pointer = &para[0];
	iwr.u.data.length = pindex;
	return iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
}
*/

int wext_set_tdma_param(const char *ifname, __u8 slot_period,
		__u8 rfon_period_len_1, __u8 rfon_period_len_2, __u8 rfon_period_len_3) {
#ifdef USE_WIFI_ADAPTER
	_adapter * pad = get_padapter(ifname);
	rtw_result_t ret = RTW_ERROR;
	if(pad) {
		ret = rtw_pm_set_tdma_param(pad,
				slot_period,
				rfon_period_len_1,
				rfon_period_len_2,
				rfon_period_len_3);
	}
	return ret;
#else
	struct iwreq iwr;
	int ret = -1;
	__u16 pindex = 7;
	__u8 para[16]; // 7+(1+1+4)
	int cmd_len = sizeof("pm_set");
	memset(&iwr, 0, sizeof(iwr));
	// Encode parameters as TLV (type, length, value) format
	snprintf((char*) para, cmd_len, "pm_set");
	para[pindex++] = 2; // type 2 tdma param
	para[pindex++] = 4;
	para[pindex++] = slot_period;
	para[pindex++] = rfon_period_len_1;
	para[pindex++] = rfon_period_len_2;
	para[pindex++] = rfon_period_len_3;
	iwr.u.data.pointer = para;
	iwr.u.data.length = pindex;
	return iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
#endif
}

int wext_set_lps_dtim(const char *ifname, __u8 lps_dtim) {
#ifdef USE_WIFI_ADAPTER
	_adapter * pad =	get_padapter(ifname);
	rtw_result_t ret = RTW_ERROR;
	if(pad) {
		ret = rtw_pm_set_lps_dtim(pad, lps_dtim);
	}
	return ret;
#else
	struct iwreq iwr;
	int ret = -1;
	__u16 pindex = 7;
	__u8 para[16]; // 7+(1+1+1)
	int cmd_len = 0;

	memset(&iwr, 0, sizeof(iwr));
	cmd_len = sizeof("pm_set");
	// Encode parameters as TLV (type, length, value) format
	snprintf((char*) para, cmd_len, "pm_set");
	para[pindex++] = 3; // type 3 lps dtim
	para[pindex++] = 1;
	para[pindex++] = lps_dtim;
	iwr.u.data.pointer = &para[0];
	iwr.u.data.length = pindex;
	ret = iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
	return ret;
#endif
}

int wext_get_lps_dtim(const char *ifname, __u8 *lps_dtim) {
#ifdef USE_WIFI_ADAPTER
	_adapter * pad = get_padapter(ifname);
	rtw_result_t ret = RTW_ERROR;
	if(pad) {
		*lps_dtim = rtw_pm_get_lps_dtim(pad);
		ret = RTW_SUCCESS;
	}
	return ret;
#else
	struct iwreq iwr;
	int ret = -1;
	__u16 pindex = 7;
	int cmd_len = 0;
	__u8 para[16]; // 7+(1+1+1)
	memset(&iwr, 0, sizeof(iwr));
	cmd_len = sizeof("pm_get");
	// Encode parameters as TLV (type, length, value) format
	snprintf((char*) para, cmd_len, "pm_get");
	para[pindex++] = 3; // type 3 for lps dtim
	para[pindex++] = 1;
	para[pindex++] = 0;
	iwr.u.data.pointer = &para[0];
	iwr.u.data.length = pindex;
	ret = iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
	//get result at the beginning of iwr.u.data.pointer
	if ((para[0] == 3) && (para[1] == 1))
		*lps_dtim = para[2];
	else {
#if	CONFIG_DEBUG_LOG > 3
		error_printf("%s: error!\n", __func__);
#endif
		ret = -1;
	}
	return ret;
#endif
}

int wext_set_tos_value(const char *ifname, __u8 *tos_value) {
	struct iwreq iwr;
	int ret = -1;
	__u8 para[sizeof("set_tos_value") + 4];
	int cmd_len = sizeof("set_tos_value");
	memset(&iwr, 0, sizeof(iwr));
	snprintf((char*) para, cmd_len, "set_tos_value");
	if (*tos_value >= 0 && *tos_value <= 32) {
		*(para + cmd_len) = 0x4f;
		*(para + cmd_len + 1) = 0xa4;
		*(para + cmd_len + 2) = 0;
		*(para + cmd_len + 3) = 0;
	} else if (*tos_value > 32 && *tos_value <= 96) {
		*(para + cmd_len) = 0x2b;
		*(para + cmd_len + 1) = 0xa4;
		*(para + cmd_len + 2) = 0;
		*(para + cmd_len + 3) = 0;
	} else if (*tos_value > 96 && *tos_value <= 160) {
		*(para + cmd_len) = 0x22;
		*(para + cmd_len + 1) = 0x43;
		*(para + cmd_len + 2) = 0x5e;
		*(para + cmd_len + 3) = 0;
	} else if (*tos_value > 160) {
		*(para + cmd_len) = 0x22;
		*(para + cmd_len + 1) = 0x32;
		*(para + cmd_len + 2) = 0x2f;
		*(para + cmd_len + 3) = 0;
	}
	iwr.u.data.pointer = &para[0];
	iwr.u.data.length = cmd_len + 4;
	return iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
}

int wext_get_tx_power(const char *ifname, __u8 *poweridx) {
	struct iwreq iwr;
	int ret = -1;
	__u8 *para;
	int cmd_len = sizeof("get_tx_power");
	memset(&iwr, 0, sizeof(iwr));
	//Tx power size : 20 Bytes
	//CCK 1M,2M,5.5M,11M : 4 Bytes
	//OFDM 6M, 9M, 12M, 18M, 24M, 36M 48M, 54M : 8 Bytes
	//MCS 0~7 : 8 Bytes
	para = pvPortMalloc(cmd_len + 20);
	if (para != NULL) {
		snprintf((char*) para, cmd_len, "get_tx_power");
		iwr.u.data.pointer = para;
		iwr.u.data.length = cmd_len + 20;
		ret = iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
		if (ret >= 0)
			memcpy(poweridx, (__u8 *) (iwr.u.data.pointer), 20);
		vPortFree(para);
	}
#if	CONFIG_DEBUG_LOG > 3
	else {
		error_printf("%s: Can't malloc memory!\n", __func__);
	}
#endif
	return ret;
}

#if 0 // work ?
int wext_set_txpower(const char *ifname, int poweridx) {
	int ret;
	char buf[32];

	memset(buf, 0, sizeof(buf));
//	snprintf(buf, 24, "txpower patha=%d", poweridx);
	snprintf(buf, sizeof(buf), "txpower patha=%d,pathb=%d", poweridx, poweridx);
	ret = wext_private_command(ifname, buf, 0);

	return ret;
}

int wext_get_associated_client_list(const char *ifname,
		void * client_list_buffer, uint16_t buffer_length) {
	int ret;
	char buf[25];

	memset(buf, 0, sizeof(buf));
	snprintf(buf, 25, "get_client_list %x", client_list_buffer);
	ret = wext_private_command(ifname, buf, 0);

	return ret;
}

int wext_get_ap_info(const char *ifname, rtw_bss_info_t * ap_info,
		rtw_security_t* security) {
	int ret = 0;
	char buf[24];

	memset(buf, 0, sizeof(buf));
	snprintf(buf, 24, "get_ap_info %x", ap_info);
	ret = wext_private_command(ifname, buf, 0);

	snprintf(buf, 24, "get_security");
	ret = wext_private_command_with_retval(ifname, buf, buf, 24);
	sscanf(buf, "%d", security);

	return ret;
}
#endif

int wext_set_mode(const char *ifname, int mode) {
#ifdef USE_WIFI_ADAPTER
	_adapter * pad =	get_padapter(ifname);
	int nwm;
	if(rtw_pwr_wakeup(pad) && pad->hw_init_completed) {
		switch(mode) {
		case IW_MODE_AUTO:
			nwm = Ndis802_11AutoUnknown;
			break;
		case IW_MODE_MASTER:
			nwm = Ndis802_11APMode;
			break;
		case IW_MODE_INFRA:
			nwm = Ndis802_11Infrastructure;
			break;
		case IW_MODE_ADHOC:
			nwm = Ndis802_11IBSS;
			break;
        default:
            return RTW_NORESOURCE;
		}
        if(rtw_set_802_11_infrastructure_mode(pad, nwm)) {
          return set_opmode(pad, nwm);
        }
	}
    return RTW_ERROR;
#else
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.mode = mode;
	return iw_ioctl(ifname, SIOCSIWMODE, &iwr);
#endif
}

int wext_get_mode(const char *ifname, int *mode) {
#ifdef USE_WIFI_ADAPTER
	_adapter * pad =	get_padapter(ifname);
	rtw_result_t ret = RTW_ERROR;
	if(pad) {
		uint16 f = pad->mlmepriv.fw_state;
		if(f & 8) *mode = 2;
		else if(f & 0x60) *mode = 1;
		else if(!(f & 0x10)) *mode = 0;
		else *mode = 3;
		ret = RTW_SUCCESS;
	}
	return ret;
#else
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	int ret = iw_ioctl(ifname, SIOCGIWMODE, &iwr);
	if (ret >= 0)
		*mode = iwr.u.mode;
	return ret;
#endif
}

int wext_set_ap_ssid(const char *ifname, const __u8 *ssid, __u16 ssid_len) {
#ifdef USE_WIFI_ADAPTER
	struct net_device * pdev = rltk_wlan_info[0].dev;
	if(ifname[4] != '0')
		pdev = rltk_wlan_info[1].dev;
	rtw_result_t ret = RTW_ERROR;
	if(pdev) {
		uint16 len[2];
		len[0] = ssid_len;
		len[1] = (ssid_len != 0);
		ret = rtw_wx_set_ap_essid(pdev, 0, &len, ssid);
	}
	return ret;
#else
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.essid.pointer = (void *) ssid;
	iwr.u.essid.length = ssid_len;
	iwr.u.essid.flags = (ssid_len != 0);
	return iw_ioctl(ifname, SIOCSIWPRIVAPESSID, &iwr);
#endif
}

int wext_set_country(const char *ifname, rtw_country_code_t country_code) {
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.param.value = country_code;
	return iw_ioctl(ifname, SIOCSIWPRIVCOUNTRY, &iwr);
}

int wext_get_rssi(const char *ifname, int *rssi) {
#ifdef USE_WIFI_ADAPTER
	_adapter * pad =	get_padapter(ifname);
	rtw_result_t ret = RTW_ERROR;
	if(pad) {
		if(pad->mlmepriv.fw_state & 1) {
			*rssi = pad->recvpriv.rssi; // +2932
		}
		else *rssi = 0;
		ret = RTW_SUCCESS;
	}
	return ret;
#else
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	int ret = iw_ioctl(ifname, SIOCGIWSENS, &iwr);
	if (ret >= 0)
		*rssi = 0 - iwr.u.sens.value;
	return ret;
#endif
}

int wext_set_pscan_channel(const char *ifname,
						__u8 *ch,
						__u8 *pscan_config,
						__u8 length) {
	struct iwreq iwr;
	int ret = -1;
	__u8 *para;
	int i = 0;
	memset(&iwr, 0, sizeof(iwr));
	//Format of para:function_name num_channel chan1... pscan_config1 ...
	para = pvPortMalloc((length + length + 1) + 12); //size:num_chan + num_time + length + function_name
	if (para != NULL) {
		//Cmd
		snprintf((char*) para, 12, "PartialScan");
		//length
		*(para + 12) = length;
		for (i = 0; i < length; i++) {
			*(para + 13 + i) = *(ch + i);
			*((__u16 *) (para + 13 + length + i)) = *(pscan_config + i);
		}

		iwr.u.data.pointer = para;
		iwr.u.data.length = (length + length + 1) + 12;
		ret = iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
		vPortFree(para);
	}
#if	CONFIG_DEBUG_LOG > 3
	else {
		error_printf("%s: Can't malloc memory!\n", __func__);
	}
#endif
	return ret;
}

//extern int rtw_wx_set_freq(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);

int wext_set_channel(const char *ifname, __u8 ch) {
#if 0 //def USE_WIFI_ADAPTER // link: undefined reference to `rtw_wx_set_freq' -> rtw_wx_set_freq.isra.10
	struct net_device * pdev = rltk_wlan_info[0].dev;
	if(ifname[4] != '0')
		pdev = rltk_wlan_info[1].dev;
	rtw_result_t ret = RTW_ERROR;
	if(pdev) {
		ret = rtw_wx_set_freq(pdev, ch, NULL, NULL);
	}
	return ret;
#else
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.freq.m = 0;
	iwr.u.freq.e = 0;
	iwr.u.freq.i = ch;
	return iw_ioctl(ifname, SIOCSIWFREQ, &iwr);
#endif
}

int wext_get_channel(const char *ifname, __u8 *ch) {
#ifdef USE_WIFI_ADAPTER
		_adapter * pad = get_padapter(ifname);
		rtw_result_t ret = RTW_ERROR;
		if(pad) {
			if(pad->mlmepriv.fw_state & 1) {
				*ch = pad->mlmepriv.cur_network.network.Configuration.DSConfig;
			}
			else {
				*ch = pad->mlmeextpriv.cur_channel;
			}
			ret = RTW_SUCCESS;
		}
		return ret;
#else
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	int ret = iw_ioctl(ifname, SIOCGIWFREQ, &iwr);
	if (ret >= 0)
		*ch = iwr.u.freq.i;
	return ret;
#endif
}

int wext_register_multicast_address(const char *ifname, rtw_mac_t *mac) {
	char buf[32];
	memset(buf, 0, sizeof(buf));
	snprintf(buf, 32, "reg_multicast "MAC_FMT, MAC_ARG(mac->octet));
	return wext_private_command(ifname, buf, 0);
}

int wext_unregister_multicast_address(const char *ifname, rtw_mac_t *mac) {
	char buf[36];
	memset(buf, 0, sizeof(buf));
	snprintf(buf, 35, "reg_multicast -d "MAC_FMT, MAC_ARG(mac->octet));
	return wext_private_command(ifname, buf, 0);
}

int wext_set_scan(const char *ifname, char *buf, __u16 buf_len, __u16 flags) {
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
#if 0 //for scan_with_ssid	
	if(buf)
		memset(buf, 0, buf_len);
#endif
	iwr.u.data.pointer = buf;
	iwr.u.data.flags = flags;
	iwr.u.data.length = buf_len;
	return iw_ioctl(ifname, SIOCSIWSCAN, &iwr);
}

int wext_get_scan(const char *ifname, char *buf, __u16 buf_len) {
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.data.pointer = buf;
	iwr.u.data.length = buf_len;
	int ret = iw_ioctl(ifname, SIOCGIWSCAN, &iwr);
	if (ret >= 0)
		ret = iwr.u.data.flags;
	return ret;
}

int wext_private_command_with_retval(const char *ifname, char *cmd,
		char *ret_buf, int ret_len) {
	struct iwreq iwr;
	int ret = -1, buf_size;
	char *buf;

	buf_size = 128;

	if (strlen(cmd) >= buf_size)
		buf_size = strlen(cmd) + 1;	// 1 : '\0'
	buf = (char*) pvPortMalloc(buf_size);
	if (buf != NULL) {
		memset(buf, 0, buf_size);
		strcpy(buf, cmd);
		memset(&iwr, 0, sizeof(iwr));
		iwr.u.data.pointer = buf;
		iwr.u.data.length = buf_size;
		iwr.u.data.flags = 0;
		ret = iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
		if (ret >= 0 & ret_buf != NULL) {
			if (ret_len > iwr.u.data.length)
				ret_len = iwr.u.data.length;
			memcpy(ret_buf, (char *) iwr.u.data.pointer, ret_len);
		}
		vPortFree(buf);
	}
#if	CONFIG_DEBUG_LOG > 3
	else {
		error_printf("%s: Can't malloc memory!\n", __func__);
	}
#endif
	return ret;
}

int wext_private_command(const char *ifname, char *cmd, int show_msg) {
	struct iwreq iwr;
	int ret = -1, buf_size;
	char *buf;

	u8 cmdname[17] = { 0 }; // IFNAMSIZ+1

	sscanf(cmd, "%16s", cmdname);
	if ((strcmp((const char *)cmdname, "config_get") == 0)
			|| (strcmp((const char *)cmdname, "config_set") == 0)
			|| (strcmp((const char *)cmdname, "efuse_get") == 0)
			|| (strcmp((const char *)cmdname, "efuse_set") == 0)
			|| (strcmp((const char *)cmdname, "mp_psd") == 0))
		buf_size = 2600;	//2600 for config_get rmap,0,512 (or realmap)
	else
		buf_size = 512;

	if (strlen(cmd) >= buf_size)
		buf_size = strlen(cmd) + 1;	// 1 : '\0'
	buf = (char*) pvPortMalloc(buf_size);
	if (buf != NULL) {
		memset(buf, 0, buf_size);
		strcpy(buf, cmd);
		memset(&iwr, 0, sizeof(iwr));
		iwr.u.data.pointer = buf;
		iwr.u.data.length = buf_size;
		iwr.u.data.flags = 0;

		ret = iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
		if (ret >= 0 && show_msg && iwr.u.data.length) {
#if	CONFIG_DEBUG_LOG > 3
			if (iwr.u.data.length > buf_size) {
				error_printf("%s: Can't malloc memory!\n", __func__);
			}
			info_printf("Private Message: %s\n", (char * ) iwr.u.data.pointer);
#endif
		}
		vPortFree(buf);
	}
#if	CONFIG_DEBUG_LOG > 3
	else {
		error_printf("%s: Can't malloc memory!\n", __func__);
	}
#endif
	return ret;
}

void wext_wlan_indicate(unsigned int cmd, union iwreq_data *wrqu, char *extra) {
	unsigned char null_mac[6] = { 0 };

	switch (cmd) {
	case SIOCGIWAP:
		if (wrqu->ap_addr.sa_family == ARPHRD_ETHER) {
			if (!memcmp(wrqu->ap_addr.sa_data, null_mac, sizeof(null_mac)))
				wifi_indication(WIFI_EVENT_DISCONNECT, NULL, 0, 0);
			else
				wifi_indication(WIFI_EVENT_CONNECT, wrqu->ap_addr.sa_data,
						sizeof(null_mac), 0);
		}
		break;

	case IWEVCUSTOM:
		if (extra) {
#if CONFIG_DEBUG_LOG > 4
			info_printf("IWEVCUSTOM '%s'\n", extra);
#endif
			if (!memcmp(IW_EXT_STR_FOURWAY_DONE, extra,
					strlen(IW_EXT_STR_FOURWAY_DONE)))
				wifi_indication(WIFI_EVENT_FOURWAY_HANDSHAKE_DONE, extra,
						strlen(IW_EXT_STR_FOURWAY_DONE), 0);
			else if (!memcmp(IW_EXT_STR_RECONNECTION_FAIL, extra,
					strlen(IW_EXT_STR_RECONNECTION_FAIL)))
				wifi_indication(WIFI_EVENT_RECONNECTION_FAIL, extra,
						strlen(IW_EXT_STR_RECONNECTION_FAIL), 0);
			else if (!memcmp(IW_EVT_STR_NO_NETWORK, extra,
					strlen(IW_EVT_STR_NO_NETWORK)))
				wifi_indication(WIFI_EVENT_NO_NETWORK, extra,
						strlen(IW_EVT_STR_NO_NETWORK), 0);
#if CONFIG_ENABLE_P2P || defined(CONFIG_AP_MODE)
			else if (!memcmp(IW_EVT_STR_STA_ASSOC, extra,
					strlen(IW_EVT_STR_STA_ASSOC)))
				wifi_indication(WIFI_EVENT_STA_ASSOC, wrqu->data.pointer,
						wrqu->data.length, 0);
			else if (!memcmp(IW_EVT_STR_STA_DISASSOC, extra,
					strlen(IW_EVT_STR_STA_DISASSOC)))
				wifi_indication(WIFI_EVENT_STA_DISASSOC, wrqu->addr.sa_data,
						sizeof(null_mac), 0);
			else if (!memcmp(IW_EVT_STR_SEND_ACTION_DONE, extra,
					strlen(IW_EVT_STR_SEND_ACTION_DONE)))
				wifi_indication(WIFI_EVENT_SEND_ACTION_DONE, NULL, 0,
						wrqu->data.flags);
#endif
		}
		break;
	case SIOCGIWSCAN:
		if (wrqu->data.pointer == NULL)
			wifi_indication(WIFI_EVENT_SCAN_DONE, NULL, 0, 0);
		else
			wifi_indication(WIFI_EVENT_SCAN_RESULT_REPORT, wrqu->data.pointer,
					wrqu->data.length, 0);
		break;
#if CONFIG_ENABLE_P2P
	case IWEVMGNTRECV:
		wifi_indication(WIFI_EVENT_RX_MGNT, wrqu->data.pointer,
				wrqu->data.length, wrqu->data.flags);
		break;
#endif
#ifdef REPORT_STA_EVENT
		case IWEVREGISTERED:
		if(wrqu->addr.sa_family == ARPHRD_ETHER)
		wifi_indication(WIFI_EVENT_STA_ASSOC, wrqu->addr.sa_data, sizeof(null_mac), 0);
		break;
		case IWEVEXPIRED:
		if(wrqu->addr.sa_family == ARPHRD_ETHER)
		wifi_indication(WIFI_EVENT_STA_DISASSOC, wrqu->addr.sa_data, sizeof(null_mac), 0);
		break;
#endif
	default:
#if CONFIG_DEBUG_LOG > 4
			info_printf("\nwlan_indicate: %p!\n", cmd);
#endif
		break;
	}
}

int wext_send_eapol(const char *ifname, char *buf, __u16 buf_len, __u16 flags) {
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.data.pointer = buf;
	iwr.u.data.length = buf_len;
	iwr.u.data.flags = flags;
	return iw_ioctl(ifname, SIOCSIWEAPOLSEND, &iwr);
}

#if CONFIG_ENABLE_P2P
int wext_send_mgnt(const char *ifname, char *buf, __u16 buf_len, __u16 flags) {
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.data.pointer = buf;
	iwr.u.data.length = buf_len;
	iwr.u.data.flags = flags;
	return iw_ioctl(ifname, SIOCSIWMGNTSEND, &iwr);
}
#endif

int wext_set_gen_ie(const char *ifname, char *buf, __u16 buf_len, __u16 flags) {
	struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));
	iwr.u.data.pointer = buf;
	iwr.u.data.length = buf_len;
	iwr.u.data.flags = flags;
	return iw_ioctl(ifname, SIOCSIWGENIE, &iwr);
}

int wext_set_autoreconnect(const char *ifname, __u8 mode, __u8 retyr_times,
		__u16 timeout) {
	struct iwreq iwr;
	int ret = 0;
	__u8 para[sizeof("SetAutoRecnt") + 4];
	int cmd_len = sizeof("SetAutoRecnt");
	memset(&iwr, 0, sizeof(iwr));
	//Cmd
	snprintf((char*) para, cmd_len, "SetAutoRecnt");
	//length
	*(para + cmd_len) = mode;	//para1
	*(para + cmd_len + 1) = retyr_times; //para2
	*(para + cmd_len + 2) = timeout; //para3
	iwr.u.data.pointer = &para[0];
	iwr.u.data.length = sizeof(para);
	return iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
}

int wext_get_autoreconnect(const char *ifname, __u8 *mode) {
	struct iwreq iwr;
	__u8 para[sizeof("GetAutoRecnt") + 1];
	int cmd_len = sizeof("GetAutoRecnt");
	memset(&iwr, 0, sizeof(iwr));
	//Cmd
	snprintf((char*) para, cmd_len, "GetAutoRecnt");
	//length
	iwr.u.data.pointer = &para[0];
	iwr.u.data.length = cmd_len;
	int ret = iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
	if (ret >= 0)
		*mode = *(__u8 *) (iwr.u.data.pointer);
	return ret;
}

int wext_get_drv_ability(const char *ifname, __u32 *ability) {
	int ret = -1;
	char * buf = (char *) pvPortMalloc(33);
	if (buf != NULL) {
		memset(buf, 0, 33);
		snprintf(buf, 33, "get_drv_ability %x", ability);
		ret = wext_private_command(ifname, buf, 0);
		vPortFree(buf);
	}
#if	CONFIG_DEBUG_LOG > 3
	else {
		error_printf("%s: Can't malloc memory!\n", __func__);
	}
#endif
	return ret;
}

#ifdef CONFIG_CUSTOM_IE
int wext_add_custom_ie(const char *ifname, void *cus_ie, int ie_num) {
	struct iwreq iwr;
	int ret = -1;
	__u8 *para;
	int cmd_len = sizeof("SetCusIE");
	if (ie_num <= 0 || !cus_ie) {
#if	CONFIG_DEBUG_LOG > 3
		error_printf("%s: wrong parameter!\n", __func__);
#endif
	} else {
		memset(&iwr, 0, sizeof(iwr));
		para = pvPortMalloc((4) * 2 + cmd_len); //size:addr len+cmd_len
		if (para != NULL) {
			//Cmd
			snprintf(para, cmd_len, "SetCusIE");
			//addr length
			*(__u32 *) (para + cmd_len) = (__u32 ) cus_ie; //ie addr
			//ie_num
			*(__u32 *) (para + cmd_len + 4) = ie_num; //num of ie

			iwr.u.data.pointer = para;
			iwr.u.data.length = (4) * 2 + cmd_len; // 2 input
			ret = iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
			vPortFree(para);
		}
#if	CONFIG_DEBUG_LOG > 3
		else {
			error_printf("%s: Can't malloc memory!\n", __func__);
		}
#endif
	}
	return ret;
}

int wext_update_custom_ie(const char *ifname, void * cus_ie, int ie_index) {
	struct iwreq iwr;
	int ret = -1;
	__u8 *para = NULL;
	int cmd_len = sizeof("UpdateIE");
	if (ie_index <= 0 || !cus_ie) {
#if	CONFIG_DEBUG_LOG > 3
		error_printf("%s: wrong parameter!\n", __func__);
#endif
	} else {
		memset(&iwr, 0, sizeof(iwr));
		cmd_len = para = pvPortMalloc((4) * 2 + cmd_len); //size:addr len+cmd_len
		if (para != NULL) {
			//Cmd
			snprintf(para, cmd_len, "UpdateIE");
			//addr length
			*(__u32 *) (para + cmd_len) = (__u32 ) cus_ie; //ie addr
			//ie_index
			*(__u32 *) (para + cmd_len + 4) = ie_index; //num of ie

			iwr.u.data.pointer = para;
			iwr.u.data.length = (4) * 2 + cmd_len; // 2 input
			ret = iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
			vPortFree(para);
		}
#if	CONFIG_DEBUG_LOG > 3
		else {
			error_printf("%s: Can't malloc memory!\n", __func__);
		}
#endif
	}
	return ret;
}

int wext_del_custom_ie(const char *ifname) {
	struct iwreq iwr;
	__u8 para[sizeof("DelIE")];
	memset(&iwr, 0, sizeof(iwr));
	//Cmd
	snprintf(para, sizeof("DelIE"), "DelIE");
	iwr.u.data.pointer = &para[0];
	iwr.u.data.length = sizeof("DelIE");
	return iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
}

#endif

#ifdef CONFIG_AP_MODE
int wext_set_forwarding(const char *ifname, char flg) {
	struct iwreq iwr;
	__u8 para[sizeof("forwarding_set") + 1];
	int cmd_len = sizeof("forwarding_set");
	memset(&iwr, 0, sizeof(iwr));
	// forwarding_set 1
	snprintf((char *) para, cmd_len, "forwarding_set");
	*(para + cmd_len) = flg;
	iwr.u.essid.pointer = para;
	iwr.u.essid.length = cmd_len + 1;
	return iw_ioctl(ifname, SIOCDEVPRIVATE, &iwr);
}

int wext_enable_forwarding(const char *ifname) {
	return wext_set_forwarding(ifname, '1');
}

int wext_disable_forwarding(const char *ifname) {
	return wext_set_forwarding(ifname, '0');
}
#endif

#ifdef CONFIG_CONCURRENT_MODE
int wext_set_ch_deauth(const char *ifname, __u8 enable) {
	char buf[16];
	memset(buf, 0, 16);
	snprintf(buf, 16, "SetChDeauth %d", enable);
	return wext_private_command(ifname, buf, 0);
}
#endif

int wext_set_adaptivity(rtw_adaptivity_mode_t adaptivity_mode) {
	extern u8 rtw_adaptivity_en;
	extern u8 rtw_adaptivity_mode;

	switch (adaptivity_mode) {
	case RTW_ADAPTIVITY_NORMAL:
		rtw_adaptivity_en = 1; // enable adaptivity
		rtw_adaptivity_mode = RTW_ADAPTIVITY_MODE_NORMAL;
		break;
	case RTW_ADAPTIVITY_CARRIER_SENSE:
		rtw_adaptivity_en = 1; // enable adaptivity
		rtw_adaptivity_mode = RTW_ADAPTIVITY_MODE_CARRIER_SENSE;
		break;
	case RTW_ADAPTIVITY_DISABLE:
	default:
		rtw_adaptivity_en = 0; //disable adaptivity
		break;
	}
	return 0;
}

int wext_set_adaptivity_th_l2h_ini(__u8 l2h_threshold) {
	extern s8 rtw_adaptivity_th_l2h_ini;
	rtw_adaptivity_th_l2h_ini = (__s8 ) l2h_threshold;
	return 0;
}

extern int rltk_get_auto_chl(const char *ifname, unsigned char *channel_set,
		int channel_num);

int wext_get_auto_chl(const char *ifname, unsigned char *channel_set,
		unsigned char channel_num) {
	int ret = -1;
	int channel = 0;
	wext_disable_powersave(ifname);
	if ((channel = rltk_get_auto_chl(ifname, channel_set, channel_num)) != 0)
		ret = channel;
	wext_enable_powersave(ifname, 1, 1);
	return ret;
}

extern int rltk_set_sta_num(unsigned char ap_sta_num);

int wext_set_sta_num(unsigned char ap_sta_num) {
	return rltk_set_sta_num(ap_sta_num);
}

extern int rltk_del_station(const char *ifname, unsigned char *hwaddr);

int wext_del_station(const char *ifname, unsigned char* hwaddr) {
	return rltk_del_station(ifname, hwaddr);
}

extern struct list_head *mf_list_head;
int wext_init_mac_filter(void) {
	int ret = -1;
	if (mf_list_head == NULL) {
		mf_list_head = malloc(sizeof(struct list_head));
		if (mf_list_head != NULL) {
			INIT_LIST_HEAD(mf_list_head);
			ret = 0;
		}
#if	CONFIG_DEBUG_LOG > 3
		else { 
			error_printf("%s: Can't malloc memory!\n", __func__);
		}
#endif
	}
	return ret;
}

int wext_deinit_mac_filter(void) {
	int ret = -1;
	if (mf_list_head != NULL) {
		struct list_head *iterator;
		rtw_mac_filter_list_t *item;
		list_for_each(iterator, mf_list_head)
		{
			item = list_entry(iterator, rtw_mac_filter_list_t, node);
			list_del(iterator);
			free(item);
			item = NULL;
			iterator = mf_list_head;
		}
		free(mf_list_head);
		mf_list_head = NULL;
		ret = 0;
	}
	return ret;
}

int wext_add_mac_filter(unsigned char* hwaddr) {
	int ret = -1;
	if (mf_list_head != NULL) {
		rtw_mac_filter_list_t *mf_list_new;
		mf_list_new = malloc(sizeof(rtw_mac_filter_list_t));
		if (mf_list_new != NULL) {
			memcpy(mf_list_new->mac_addr, hwaddr, 6);
			list_add(&(mf_list_new->node), mf_list_head);
			ret = 0;
		}
#if	CONFIG_DEBUG_LOG > 3
		else {
			error_printf("%s: Can't malloc memory!\n", __func__);
		}
#endif
	}
	return ret;
}

int wext_del_mac_filter(unsigned char* hwaddr) {
	int ret = -1;
	if (mf_list_head != NULL) {
		struct list_head *iterator;
		rtw_mac_filter_list_t *item;
		list_for_each(iterator, mf_list_head)
		{
			item = list_entry(iterator, rtw_mac_filter_list_t, node);
			if (memcmp(item->mac_addr, hwaddr, 6) == 0) {
				list_del(iterator);
				free(item);
				item = NULL;
				ret = 0;
			}
		}
	}
	return ret;
}

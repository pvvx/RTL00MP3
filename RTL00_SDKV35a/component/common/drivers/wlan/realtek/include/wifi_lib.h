/*
 wifi_lib.h
 RTL871x pvvx
 */
#ifndef _WLAN_LIB_H
#define _WLAN_LIB_H

#include "osdep_service.h"
#include "freertos/wrapper.h"
#include "rtl_bios_data.h"

#define _atr_aligned2_ __attribute__((aligned(2)))
#define _atr_aligned4_ __attribute__((aligned(4)))
#define _atr_aligned8_ __attribute__((aligned(4)))

#define sint8_t char
#define sint16_t short
#define sint32_t int
//#define sint64_t long long // warning align(8)!
/*
 struct _ADAPTER;
 struct dvobj_priv;
 union recv_frame;
 struct sk_buff;
 struct sk_buff_head;
 struct sta_info;
 struct _cus_ie;
 struct fifo_more_data;
 struct hw_xmit;
 struct tx_buf_desc;
 struct recv_buf_stat;
 struct _wpa_sta_info;
 struct xmit_frame;
 struct xmit_buf;
 struct submit_ctx;
 struct net_device;
 struct iwreq;
 struct co_data_priv;
 */
//typedef int sint32_t_t;
//typedef char sint8_t_t;
//typedef sint8_t_t int8_t;
//typedef uint32_t __uint32_t;
//typedef sint32_t_t int32_t;
//typedef __uint32_t uint32_t;
//typedef uint32_t dma_addr_t; // basic_types.h
//typedef uint8_t uint8_t;
//typedef uint16_t uint16_t;
//typedef unsigned sint64_t u8Byte;
typedef int sint;
typedef uint8_t BOOL;
typedef uint8_t bool;
typedef uint8_t BOOLEAN;

typedef uint8_t u1Byte;
typedef uint16_t u2Byte;
typedef uint32_t u4Byte;
typedef uint64_t u8Byte;
typedef uint64_t __attribute__((aligned(4))) _u8Byte;
typedef sint8_t s1Byte;
typedef sint16_t s2Byte;
typedef sint32_t s4Byte;
typedef sint8_t *ps1Byte;
typedef uint8_t *pu1Byte;
typedef uint16_t *pu2Byte;
typedef uint32_t *pu4Byte;

typedef uint32_t sizetype;
typedef struct _ADAPTER _adapter;
typedef void *_lock;
typedef struct list_head _list;
typedef void *PVOID;
typedef struct __queue _queue;
typedef void *_mutex;

typedef int (*init_done_ptr)(void);

enum _EFUSE_DEF_TYPE // : sint32_t
{
	TYPE_EFUSE_MAX_SECTION = 0x0,
	TYPE_EFUSE_REAL_CONTENT_LEN = 0x1,
	TYPE_AVAILABLE_EFUSE_BYTES_BANK = 0x2,
	TYPE_AVAILABLE_EFUSE_BYTES_TOTAL = 0x3,
	TYPE_EFUSE_MAP_LEN = 0x4,
	TYPE_EFUSE_PROTECT_BYTES_BANK = 0x5,
	TYPE_EFUSE_CONTENT_LEN_BANK = 0x6,
};

enum _IFACE_TYPE //: sint32_t
{
	IFACE_PORT0 = 0x0, IFACE_PORT1 = 0x1, MAX_IFACE_PORT = 0x2,
};

enum _FW_ERR0_STATUS_ //: sint32_t
{
	FES0_H2C_CMDID = 0x1,
	FES0_H2C_PTR = 0x2,
	FES0_BB_RW = 0x4,
	FES0_TXPKT_TXPAUSE = 0x8,
	FES0_TSF_STABLE = 0x10,
	FES0_TXSM_STABLE = 0x20,
	FES0_RPWM_STABLE = 0x40,
	FES0_C2H_TIMEOUT_ERR = 0x80,
};

enum _TRPC_ //: sint32_t
{
	TPRC_ISSUENULLDATA_1 = 0x26,
	TPRC_ISSUENULLDATA_2 = 0x27,
	TPRC_PSS2TS3 = 0x2B,
	TPRC_PSS0TS1 = 0x2C,
	TPRC_PSS2TS4 = 0x2D,
	TPRC_PSS2TS5 = 0x2E,
	TPRC_PSS0TS6 = 0x2F,
};

enum _PS_MODE_SETTING_SELECTION_ // : sint32_t
{
	MODE_SETTING_ACTIVE = 0x0,
	MODE_SETTING_LEGACY = 0x1,
	MODE_SETTING_WMMPS = 0x2,
	MODE_SETTING_TDMA = 0x3,
};

enum _RxListenBeaconMode_ // : sint32_t
{
	RLBM_MIN = 0x0, RLBM_MAX = 0x1, RLBM_SELF_DEFINED = 0x2,
};

enum _SMART_PS_MODE_FOR_LEGACY_ // : sint32_t
{
	SMART_PS_MODE_LEGACY_PWR1 = 0x0,
	SMART_PS_MODE_TX_PWR0 = 0x1,
	SMART_PS_MODE_TRX_PWR0 = 0x2,
};

enum $BFA04BDFA6C0C275C890D1E658AFCEEF // : sint32_t
{
	ROM_E_RTW_MSGP_PWR_INDEX_1 = 0x0,
	ROM_E_RTW_MSGP_PWR_INDEX_2 = 0x1,
	ROM_E_RTW_MSGP_RX_INFO_1 = 0x2,
	ROM_E_RTW_MSGP_RX_INFO_2 = 0x3,
	ROM_E_RTW_MSGP_RX_INFO_3 = 0x4,
	ROM_E_RTW_MSGP_RX_INFO_4 = 0x5,
	ROM_E_RTW_MSGP_TX_RATE_1 = 0x6,
	ROM_E_RTW_MSGP_TX_RATE_2 = 0x7,
	ROM_E_RTW_MSGP_DM_RA_1 = 0x8,
	ROM_E_RTW_MSGP_DM_RA_2 = 0x9,
	ROM_E_RTW_MSGP_DM_RA_3 = 0xA,
	ROM_E_RTW_MSGP_DM_RA_4 = 0xB,
	ROM_E_RTW_MSGP_DM_DIG_1 = 0xC,
	ROM_E_RTW_MSGP_PWR_TRACKING_1 = 0xD,
	ROM_E_RTW_MSGP_PWR_TRACKING_2 = 0xE,
	ROM_E_RTW_MSGP_PWR_TRACKING_3 = 0xF,
	ROM_E_RTW_MSGP_PWR_TRACKING_4 = 0x10,
	ROM_E_RTW_MSGP_PWR_TRACKING_5 = 0x11,
	ROM_E_RTW_MSGP_PWR_TRACKING_6 = 0x12,
	ROM_E_RTW_MSGP_PWR_TRACKING_7 = 0x13,
	ROM_E_RTW_MSGP_RF_IQK_1 = 0x14,
	ROM_E_RTW_MSGP_RF_IQK_2 = 0x15,
	ROM_E_RTW_MSGP_RF_IQK_3 = 0x16,
	ROM_E_RTW_MSGP_RF_IQK_4 = 0x17,
	ROM_E_RTW_MSGP_RF_IQK_5 = 0x18,
	ROM_E_RTW_MSGP_DM_ADAPTIVITY_1 = 0x19,
	ROM_E_RTW_MSGP_DM_ADAPTIVITY_2 = 0x1A,
	ROM_E_RTW_MSGP_DM_ADAPTIVITY_3 = 0x1B,
	ROM_E_RTW_MSGP_DM_ANT_DIV_1 = 0x1C,
	ROM_E_RTW_MSGP_DM_ANT_DIV_2 = 0x1D,
	ROM_E_RTW_MSGP_DM_ANT_DIV_3 = 0x1E,
	ROM_E_RTW_MSGP_DM_ANT_DIV_4 = 0x1F,
	ROM_E_RTW_MSGP_DM_ANT_DIV_5 = 0x20,
	ROM_E_RTW_MSGP_MAC_REG_DUMP_1 = 0x21,
	ROM_E_RTW_MSGP_BB_REG_DUMP_1 = 0x22,
	ROM_E_RTW_MSGP_RF_REG_DUMP_1 = 0x23,
	ROM_E_RTW_MSGP_RF_REG_DUMP_2 = 0x24,
	ROM_E_RTW_MSGP_REG_DUMP_1 = 0x25,
	ROM_E_RTW_MSGP_REG_DUMP_2 = 0x26,
	ROM_E_RTW_MSGP_REG_DUMP_3 = 0x27,
	ROM_E_RTW_MSGP_READ_REG_1 = 0x28,
	ROM_E_RTW_MSGP_READ_REG_2 = 0x29,
	ROM_E_RTW_MSGP_READ_REG_3 = 0x2A,
	ROM_E_RTW_MSGP_WRITE_REG_1 = 0x2B,
	ROM_E_RTW_MSGP_WRITE_REG_2 = 0x2C,
	ROM_E_RTW_MSGP_WRITE_REG_3 = 0x2D,
	ROM_E_RTW_MSGP_READ_BB_1 = 0x2E,
	ROM_E_RTW_MSGP_WRITE_BB_1 = 0x2F,
	ROM_E_RTW_MSGP_READ_RF_1 = 0x30,
	ROM_E_RTW_MSGP_WRITE_RF_1 = 0x31,
	ROM_E_RTW_MSGP_READ_SYS_1 = 0x32,
	ROM_E_RTW_MSGP_WRITE_SYS_1 = 0x33,
	ROM_E_RTW_MSGP_FIX_CHANNEL_1 = 0x34,
	ROM_E_RTW_MSGP_FIX_CHANNEL_2 = 0x35,
	ROM_E_RTW_MSGP_PWR_SAVE_MODE_1 = 0x36,
	ROM_E_RTW_MSGP_FIX_RATE_1 = 0x37,
	ROM_E_RTW_MSGP_GET_ODM_DBG_FLAG_1 = 0x38,
	ROM_E_RTW_MSGP_SET_ODM_DBG_FLAG_1 = 0x39,
	ROM_E_RTW_MSGP_DUMP_PWR_IDX_1 = 0x3A,
	ROM_E_RTW_MSGP_DUMP_INFO_1 = 0x3B,
	ROM_E_RTW_MSGP_DUMP_INFO_2 = 0x3C,
	ROM_E_RTW_MSGP_DUMP_INFO_3 = 0x3D,
	ROM_E_RTW_MSGP_DUMP_INFO_4 = 0x3E,
	ROM_E_RTW_MSGP_DUMP_INFO_5 = 0x3F,
	ROM_E_RTW_MSGP_DUMP_INFO_6 = 0x40,
	ROM_E_RTW_MSGP_DUMP_INFO_7 = 0x41,
	ROM_E_RTW_MSGP_DUMP_INFO_8 = 0x42,
	ROM_E_RTW_MSGP_DUMP_INFO_9 = 0x43,
	ROM_E_RTW_MSGP_DM_FUNC_FLAG_1 = 0x44,
	ROM_E_RTW_MSGP_DM_FUNC_FLAG_2 = 0x45,
	ROM_E_RTW_MSGP_DM_FUNC_FLAG_3 = 0x46,
	ROM_E_RTW_MSGP_DM_FUNC_FLAG_4 = 0x47,
	ROM_E_RTW_MSGP_DM_FUNC_FLAG_5 = 0x48,
	ROM_E_RTW_MSGP_DM_FUNC_FLAG_6 = 0x49,
	ROM_E_RTW_MSGP_DM_FUNC_FLAG_7 = 0x4A,
	ROM_E_RTW_MSGP_DM_FUNC_FLAG_8 = 0x4B,
	ROM_E_RTW_MSGP_DM_FUNC_FLAG_9 = 0x4C,
	ROM_E_RTW_MSGP_RX_MPDU_1 = 0x4D,
	ROM_E_RTW_MSGP_AP_TIMEOUT_CHK_1 = 0x4E,
	ROM_E_RTW_MSGP_INIT_DRV_SW_1 = 0x4F,
	ROM_E_RTW_MSGP_SET_BSSID_1 = 0x50,
	ROM_E_RTW_MSGP_SET_SSID_1 = 0x51,
	ROM_E_RTW_MSGP_ON_BEACON_1 = 0x52,
	ROM_E_RTW_MSGP_ON_AUTH_1 = 0x53,
	ROM_E_RTW_MSGP_ON_AUTH_2 = 0x54,
	ROM_E_RTW_MSGP_ON_AUTH_CLIENT_1 = 0x55,
	ROM_E_RTW_MSGP_ON_ASSOC_REQ_1 = 0x56,
	ROM_E_RTW_MSGP_ON_ASSOC_RSP_1 = 0x57,
	ROM_E_RTW_MSGP_ON_DE_AUTH_1 = 0x58,
	ROM_E_RTW_MSGP_ON_DE_AUTH_2 = 0x59,
	ROM_E_RTW_MSGP_ON_DISASSOC_1 = 0x5A,
	ROM_E_RTW_MSGP_ON_DISASSOC_2 = 0x5B,
	ROM_E_RTW_MSGP_ISSUE_BEACON_1 = 0x5C,
	ROM_E_RTW_MSGP_ISSUE_PROBERSP_1 = 0x5D,
	ROM_E_RTW_MSGP_ISSUE_PROBEREQ_1 = 0x5E,
	ROM_E_RTW_MSGP_ISSUE_AUTH_1 = 0x5F,
	ROM_E_RTW_MSGP_ISSUE_ASSOCRSP_1 = 0x60,
	ROM_E_RTW_MSGP_ISSUE_ASSOCREQ_1 = 0x61,
	ROM_E_RTW_MSGP_ISSUE_NULLDATA_1 = 0x62,
	ROM_E_RTW_MSGP_ISSUE_QOS_NULLDATA_1 = 0x63,
	ROM_E_RTW_MSGP_ISSUE_DEAUTH_1 = 0x64,
	ROM_E_RTW_MSGP_ISSUE_ACTION_BA_1 = 0x65,
	ROM_E_RTW_MSGP_ISSUE_BSS_COEXIST_1 = 0x66,
	ROM_E_RTW_MSGP_START_CLNT_AUTH_1 = 0x67,
	ROM_E_RTW_MSGP_LINKED_STATUS_CHK_1 = 0x68,
	ROM_E_RTW_MSGP_SETKEY_HDL_1 = 0x69,
	ROM_E_RTW_MSGP_SET_STAKEY_HDL_1 = 0x6A,
	ROM_E_RTW_MSGP_SET_STAKEY_HDL_2 = 0x6B,
	ROM_E_RTW_MSGP_P2P_BUILD_MGNT_FRAME_1 = 0x6C,
	ROM_E_RTW_MSGP_SEND_EAPOL_1 = 0x6D,
	ROM_E_RTW_MSGP_SEND_EAPOL_2 = 0x6E,
	ROM_E_RTW_MSGP_SEND_EAPOL_3 = 0x6F,
	ROM_E_RTW_MSGP_EAPOL_KEY_RECVD_1 = 0x70,
	ROM_E_RTW_MSGP_EAPOL_KEY_RECVD_2 = 0x71,
	ROM_E_RTW_MSGP_EAPOL_KEY_RECVD_3 = 0x72,
	ROM_E_RTW_MSGP_FREE_RECVFRAME_1 = 0x73,
	ROM_E_RTW_MSGP_VAR_PORT_SWITCH_1 = 0x74,
	ROM_E_RTW_MSGP_VAR_PORT_SWITCH_2 = 0x75,
	ROM_E_RTW_MSGP_DOWN_SEMA_1 = 0x76,
	ROM_E_RTW_MSGP_MAX = 0x77
};

struct _atr_aligned4_ _PS_PARM_ {
	uint8_t Enter32KHzPermission;
	uint8_t bAllQueueUAPSD;
	uint8_t ps_dtim_flag;
	uint8_t pstrx_rxcnt_period;
	uint8_t NoConnect32k;
	uint8_t ack_last_rpwm;
	uint8_t TxNull0;
	uint8_t TxNull1;
	uint8_t TxNull0ok;
	uint8_t TxNull1ok;
	uint8_t RfOffLicenseForBCNRx;
	uint8_t BCNAggEn;
	uint8_t IsGoingTo32K;
	uint8_t bMaxTrackingBcnMode;
	uint8_t BcnTraceDone;
	uint8_t smart_ps :4;
	uint8_t RLBM :4;
	uint8_t AwakeInterval;
	uint8_t ps_mode;
	uint8_t ClkRequestEnable;
	uint8_t last_rpwm;
	uint8_t current_ps_state;
	uint8_t ps_data_open;
	uint8_t ps_bcn_pass_time;
	uint8_t ps_dtim_period;
	uint8_t ps_dtim_cnt;
	uint8_t ps_bcn_to;
	uint8_t bcn_to_cnt;
	uint8_t bcn_to_times_cnt;
	uint8_t min_rate_in_rrsr;
	uint16_t ps_drv_early_itv;
	uint32_t null1_ok_cnt;
	uint8_t SlotPeriod;
	uint8_t FirstOnPeriod;
	uint8_t SecondOnPeriod;
	uint8_t ThirdOnPeriod;
	uint8_t CurrentSlot;
	BOOLEAN TDMAOnPeriod;
};
typedef struct _PS_PARM_ PS_PARM;
typedef struct _PS_PARM_ *PPS_PARM;

struct _LEGACY_PS_PPARM_ {
	uint8_t ps_mode :7;
	uint8_t ClkRequestEnable :1;
	uint8_t RLBM :4;
	uint8_t smart_ps :4;
	uint8_t AwakeInterval;
	uint8_t bAllQueueUAPSD :1;
	uint8_t bMaxTrackingBcnMode :1;
	uint8_t rsvd :6;
	uint8_t PwrState;
	uint8_t LowPwrRxBCN :1;
	uint8_t AntAutoSwitch :1;
	uint8_t PSAllowBTHighPri :1;
	uint8_t ProtectBCN :1;
	uint8_t SilencePeriod :1;
	uint8_t FastBTConnect :1;
	uint8_t TwoAntennaEn :1;
	uint8_t rsvd2 :1;
	uint8_t AdoptUserSetting :1;
	uint8_t DrvBcnEarlyShift :3;
	uint8_t DrvBcnTimeOut :4;
	uint8_t SlotPeriod;
	uint8_t FirstOnPeriod;
	uint8_t SecondOnPeriod;
	uint8_t ThirdOnPeriod;
};
typedef struct _LEGACY_PS_PPARM_ LEGACY_PS_PARM;
typedef struct _LEGACY_PS_PPARM_ *PLEGACY_PS_PARM;

struct _H2CParam_SetPwrMode_parm_ {
	LEGACY_PS_PARM PwrModeParm;
};
typedef struct _H2CParam_SetPwrMode_parm_ *PH2CParam_PwrMode;

struct atomic_t {
	volatile int counter;
};

/*
 // dlist.h
 struct list_head
 {
 _list *next;
 _list *prev;
 };

 // freertos_service.h
 struct __queue
 {
 _list queue;
 _lock lock;
 };
 */

struct iw_request_info {
	uint16_t cmd;
	uint16_t flags;
};

typedef int (*iw_handler)(struct net_device *, struct iw_request_info *,
		union iwreq_data *, char *);

struct _NDIS_802_11_SSID {
	uint32_t SsidLength;	//+164
	uint8_t Ssid[36];
};
typedef struct _NDIS_802_11_SSID NDIS_802_11_SSID;
typedef uint8_t NDIS_802_11_MAC_ADDRESS[6];
typedef int NDIS_802_11_RSSI;

enum _NDIS_802_11_NETWORK_TYPE //sint32_t
{
	Ndis802_11FH = 0x0,
	Ndis802_11DS = 0x1,
	Ndis802_11OFDM5 = 0x2,
	Ndis802_11OFDM24 = 0x3,
	Ndis802_11NetworkTypeMax = 0x4,
};
typedef enum _NDIS_802_11_NETWORK_TYPE NDIS_802_11_NETWORK_TYPE;

struct _NDIS_802_11_CONFIGURATION_FH {
	uint32_t Length;
	uint32_t HopPattern;
	uint32_t HopSet;
	uint32_t DwellTime;
};
typedef struct _NDIS_802_11_CONFIGURATION_FH NDIS_802_11_CONFIGURATION_FH;

struct _NDIS_802_11_CONFIGURATION {
	uint32_t Length;
	uint32_t BeaconPeriod;
	uint32_t ATIMWindow;
	uint32_t DSConfig;
	NDIS_802_11_CONFIGURATION_FH FHConfig;
};
typedef struct _NDIS_802_11_CONFIGURATION NDIS_802_11_CONFIGURATION;

enum _NDIS_802_11_NETWORK_INFRASTRUCTURE // : sint32_t
{
	Ndis802_11IBSS = 0x0,
	Ndis802_11Infrastructure = 0x1,
	Ndis802_11AutoUnknown = 0x2,
	Ndis802_11InfrastructureMax = 0x3,
	Ndis802_11APMode = 0x4,
};
typedef enum _NDIS_802_11_NETWORK_INFRASTRUCTURE NDIS_802_11_NETWORK_INFRASTRUCTURE;

typedef uint8_t NDIS_802_11_RATES_EX[16];

struct _WLAN_PHY_INFO {
	uint8_t SignalStrength;
	uint8_t SignalQuality;
	uint8_t Optimum_antenna;
	uint8_t Reserved_0;
};
typedef struct _WLAN_PHY_INFO WLAN_PHY_INFO;

struct _WLAN_BSSID_EX {
	uint32_t Length;
	NDIS_802_11_MAC_ADDRESS MacAddress;
	uint8_t Reserved[2];
	NDIS_802_11_SSID Ssid; // +164
	uint32_t Privacy;
	NDIS_802_11_RSSI Rssi;
	NDIS_802_11_NETWORK_TYPE NetworkTypeInUse;
	NDIS_802_11_CONFIGURATION Configuration;
	NDIS_802_11_NETWORK_INFRASTRUCTURE InfrastructureMode;
	NDIS_802_11_RATES_EX SupportedRates;
	WLAN_PHY_INFO PhyInfo;
	uint32_t IELength;
	uint8_t IEs[768];
};
typedef struct _WLAN_BSSID_EX WLAN_BSSID_EX;

struct _atr_aligned2_ _WLAN_BCN_INFO { // __attribute__((packed))!?
	uint8_t encryp_protocol;
	int group_cipher;
	int pairwise_cipher;
	int is_8021x;
	uint16_t ht_cap_info;
	uint8_t ht_info_infos_0;
};
typedef struct _WLAN_BCN_INFO WLAN_BCN_INFO;

struct wlan_network {
	_list list;
	int network_type;
	int fixed;
	uint32_t last_scanned;
	int aid;
	int join_res;
	WLAN_BSSID_EX network;
	WLAN_BCN_INFO BcnInfo;
};

/* osdep_service.h
 typedef void *_timerHandle;

 struct timer_list
 {
 _timerHandle timer_hdl;
 uint32_t data;
 void (*function)(void *);
 };

 typedef struct timer_list _timer;
 */

struct qos_priv {
	uint32_t qos_option;
};

struct __attribute__((packed)) _atr_aligned2_ rtw_ieee80211_ht_cap {
	uint16_t cap_info;
	uint8_t ampdu_params_info;
	uint8_t supp_mcs_set[16];
	uint16_t extended_ht_cap_info;
	uint32_t tx_BF_cap_info;
	uint8_t antenna_selection_info;
};

struct ht_priv {
	uint32_t ht_option;
	uint32_t ampdu_enable;
	uint8_t bwmode;
	uint8_t ch_offset;
	uint8_t sgi;
	uint8_t agg_enable_bitmap;
	uint8_t candidate_tid_bitmap;
	uint8_t stbc_cap;
	struct rtw_ieee80211_ht_cap ht_cap;
};

struct _atr_aligned4_ _RT_LINK_DETECT_T {
	uint32_t NumTxOkInPeriod;
	uint32_t NumRxOkInPeriod;
	uint32_t NumRxUnicastOkInPeriod;
	BOOLEAN bBusyTraffic;
	BOOLEAN bTxBusyTraffic;
	BOOLEAN bRxBusyTraffic;
	BOOLEAN bHigherBusyTraffic;
	BOOLEAN bHigherBusyRxTraffic;
	BOOLEAN bHigherBusyTxTraffic;
};
typedef struct _RT_LINK_DETECT_T RT_LINK_DETECT_T;

enum _RT_SCAN_TYPE //: sint32_t
{
	SCAN_PASSIVE = 0x0, SCAN_ACTIVE = 0x1, SCAN_MIX = 0x2
};
typedef enum _RT_SCAN_TYPE RT_SCAN_TYPE;

struct mlme_priv {
	_lock lock;					//+12
	sint fw_state;				//+16
	uint8_t bScanInProcess;		//+20
	uint8_t to_join;			//+21
	uint8_t *nic_hdl;			//+24
	_list *pscanned;			//+28
	_queue free_bss_pool;		//+32
	_queue scanned_queue;		//+44
	uint8_t *free_bss_buf;		//+56
	uint16_t num_of_scanned;	//+60
	uint8_t *scan_buf;			//+64
	uint32_t scan_buf_len;		//+68
	uint16_t scan_cnt;			//+72
	uint16_t scan_type;			//+74
	NDIS_802_11_SSID assoc_ssid;	//+76
	uint8_t assoc_bssid[6];
	struct wlan_network cur_network;
	uint32_t scan_interval;
	_timer assoc_timer;
	uint8_t assoc_by_bssid;
	uint8_t assoc_by_rssi;
	_timer scan_to_timer;
	uint32_t scan_start_time;
	struct qos_priv qospriv;
	uint16_t num_sta_no_ht;
	uint16_t num_FortyMHzIntolerant;
	struct ht_priv htpriv;
	RT_LINK_DETECT_T LinkDetectInfo;
	_timer dynamic_chk_timer;
	uint8_t key_mask;
	uint8_t acm_mask;
	uint8_t ChannelPlan;
	uint8_t scan_mode; // RT_SCAN_TYPE scan_mode; byte/dword ??
	uint8_t *wps_probe_req_ie;
	uint32_t wps_probe_req_ie_len;
	uint8_t *wps_assoc_req_ie;
	uint32_t wps_assoc_req_ie_len;
	uint16_t num_sta_non_erp;
	uint16_t num_sta_no_short_slot_time;
	uint16_t num_sta_no_short_preamble;
	uint16_t num_sta_ht_no_gf;
	uint16_t num_sta_ht_20mhz;
	uint8_t olbc_ht;
	uint16_t ht_op_mode;
	uint8_t *wps_beacon_ie;
	uint8_t *wps_probe_resp_ie;
	uint8_t *wps_assoc_resp_ie;
	uint32_t wps_beacon_ie_len;
	uint32_t wps_probe_resp_ie_len;
	uint32_t wps_assoc_resp_ie_len;
	_lock bcn_update_lock;
	uint8_t update_bcn;
	uint8_t scanning_via_buddy_intf;
	struct recv_frame *p_copy_recv_frame;
};

struct _atr_aligned4_ _RT_CHANNEL_INFO {
	uint8_t ChannelNum;
	RT_SCAN_TYPE ScanType; // uint8_t ScanType; //  byte/dword?
	uint8_t pscan_config;
};
typedef struct _RT_CHANNEL_INFO RT_CHANNEL_INFO;

struct ss_res {
	int state;
	int bss_cnt;
	int channel_idx;
	int scan_mode;
	NDIS_802_11_SSID ssid[1];
};

struct __attribute__((packed)) __attribute__((aligned(1))) ADDBA_request {
	uint8_t dialog_token;
	uint16_t BA_para_set;
	uint16_t BA_timeout_value;
	uint16_t BA_starting_seqctrl;
};

struct AC_param {
	uint8_t ACI_AIFSN;
	uint8_t CW;
	uint16_t TXOP_limit;
};

struct WMM_para_element {
	uint8_t QoS_info;
	uint8_t reserved;
	struct AC_param ac_param[4];
};

struct HT_caps_element {
	union {
		struct  _atr_aligned4_ {
			uint16_t HT_caps_info;
			uint8_t AMPDU_para;
			uint8_t MCS_rate[16];
			uint16_t HT_ext_caps;
			uint32_t Beamforming_caps;
			uint8_t ASEL_caps;
		} HT_cap_element;
		uint8_t HT_cap[26];
	}u;
};

struct HT_info_element {
	uint8_t primary_channel;
	uint8_t infos[5];
	uint8_t MCS_rate[16];
};

struct FW_Sta_Info {
	struct sta_info *psta;
	uint32_t status;
	uint32_t rx_pkt;
	uint32_t retry;
	NDIS_802_11_RATES_EX SupportedRates;
};

struct mlme_ext_info {
	uint32_t state;
	uint32_t reauth_count;
	uint32_t reassoc_count;
	uint32_t link_count;
	uint32_t auth_seq;
	uint32_t auth_algo;
	uint32_t authModeToggle;
	uint32_t enc_algo;
	uint32_t key_index;
	uint32_t iv;
	uint8_t chg_txt[128];
	uint16_t aid;
	uint16_t bcn_interval;
	uint16_t capability;
	uint8_t assoc_AP_vendor;
	uint8_t slotTime;
	uint8_t preamble_mode;
	uint8_t WMM_enable;
	uint8_t ERP_enable;
	uint8_t ERP_IE;
	uint8_t HT_enable;
	uint8_t HT_caps_enable;
	uint8_t HT_info_enable;
	uint8_t HT_protection;
	uint8_t turboMode_cts2self;
	uint8_t turboMode_rtsen;
	uint8_t SM_PS;
	uint8_t agg_enable_bitmap;
	uint8_t ADDBA_retry_count;
	uint8_t candidate_tid_bitmap;
	uint8_t dialogToken;
	uint8_t bwmode_updated;
	uint8_t hidden_ssid_mode;
	struct ADDBA_request ADDBA_req;
	struct WMM_para_element WMM_param;
	struct __attribute__((packed)) __attribute__((aligned(1))) HT_caps_element HT_caps;
	struct HT_info_element HT_info;
	struct FW_Sta_Info FW_sta_info[5];
};

#ifndef _CUS_IE_
#define _CUS_IE_
typedef struct _cus_ie{
	__u8 *ie;
	__u8 type;
} cus_ie, *p_cus_ie;
#endif /* _CUS_IE_ */
// typedef struct _cus_ie *p_cus_ie;

struct mlme_ext_priv { //__attribute__((packed))?
	_adapter *padapter; //+0 padapter+1256 [912]
	uint8_t mlmeext_init;
	struct atomic_t event_seq;
	uint16_t mgnt_seq;
	uint8_t cur_channel;	//	padapter+1270
	uint8_t cur_bwmode;
	uint8_t cur_ch_offset;
	uint8_t cur_wireless_mode;
	uint8_t max_chan_nums;
	RT_CHANNEL_INFO channel_set[14];
	uint8_t basicrate[13];
	uint8_t datarate[13];
	struct ss_res sitesurvey_res; //padapter+1472
	struct mlme_ext_info mlmext_info; //padapter+1528
	_timer survey_timer;
	_timer link_timer;
	uint16_t chan_scan_time;	//padapter+1984
	uint8_t scan_abort;			//padapter+1986
	uint8_t tx_rate;			//padapter+1987
	uint8_t retry;				//padapter+1988
	_u8Byte TSFValue;		//+740?  padapter+1992
	uint8_t bstart_bss;
	uint16_t action_public_rxseq;
	_timer reconnect_timer;
	uint8_t reconnect_deauth_filtered;
	uint8_t reconnect_times;	//+2017
	uint8_t reconnect_cnt;
	uint16_t reconnect_timeout;	//+ 2020
	uint8_t saved_alg;
	uint8_t saved_essid[33];
	uint8_t saved_key[32];
	uint16_t saved_key_len;
	uint8_t saved_key_idx;
	uint8_t saved_wpa_passphrase[65];
	uint8_t saved_eap_method;
	uint8_t auto_reconnect;		// + 2157 ?
	uint8_t partial_scan;
	p_cus_ie cus_ven_ie;
	uint8_t ie_num;
	uint8_t bChDeauthDisabled;
	uint8_t bConcurrentFlushingSTA;
};

struct cmd_priv {
	_queue cmd_queue;
	uint8_t cmdthd_running;
	_adapter *padapter;
};

struct evt_priv {
	struct atomic_t event_seq;
	uint8_t *evt_buf;
	uint8_t *evt_allocated_buf;
	uint32_t evt_done_cnt;
};

struct _io_ops {
	int (*init_io_priv)(struct dvobj_priv *);
	int (*write8_endian)(struct dvobj_priv *, uint32_t, uint32_t, uint32_t);
	uint8_t (*_read8)(struct dvobj_priv *, uint32_t, int32_t *);
	uint16_t (*_read16)(struct dvobj_priv *, uint32_t, int32_t *);
	uint32_t (*_read32)(struct dvobj_priv *, uint32_t, int32_t *);
	int32_t (*_write8)(struct dvobj_priv *, uint32_t, uint8_t, int32_t *);
	int32_t (*_write16)(struct dvobj_priv *, uint32_t, uint16_t, int32_t *);
	int32_t (*_write32)(struct dvobj_priv *, uint32_t, uint32_t, int32_t *);
	int (*read_rx_fifo)(struct dvobj_priv *, uint32_t, uint8_t *, uint32_t,
			struct fifo_more_data *);
	int (*write_tx_fifo)(struct dvobj_priv *, uint32_t, uint8_t *, uint32_t);
};

struct io_priv {
	struct _io_ops io_ops;
};

struct rtw_tx_ring {
	struct tx_buf_desc *desc;
	dma_addr_t dma;
	uint32_t idx;
	uint32_t entries;
	_queue queue;
	uint32_t qlen;
};

struct _atr_aligned8_ xmit_priv {
	_lock lock;
	_queue be_pending;
	_queue bk_pending;
	_queue vi_pending;
	_queue vo_pending;
	_queue bm_pending;
	uint8_t *pallocated_frame_buf;
	uint8_t *pxmit_frame_buf;
	uint32_t free_xmitframe_cnt;
	_queue free_xmit_queue;
	_adapter *adapter;
	uint8_t vcs_setting;
	uint8_t vcs;
	uint8_t vcs_type;
	u8Byte tx_bytes;
	u8Byte tx_pkts;
	u8Byte tx_drop;
	u8Byte last_tx_bytes;
	u8Byte last_tx_pkts;
	struct hw_xmit *hwxmits;
	uint8_t hwxmit_entry;
	struct rtw_tx_ring tx_ring[8];
	int txringcount[8];
	uint8_t beaconDMAing;
	_queue free_xmitbuf_queue;
	_queue pending_xmitbuf_queue;
	uint8_t *pallocated_xmitbuf;
	uint8_t *pxmitbuf;
	uint32_t free_xmitbuf_cnt;
	_queue free_xmit_extbuf_queue;
	uint8_t *pallocated_xmit_extbuf;
	uint8_t *pxmit_extbuf;
	uint32_t free_xmit_extbuf_cnt;
	uint16_t nqos_ssn;
};

struct rtw_rx_ring {
	struct recv_buf_stat *desc;
	dma_addr_t dma;
	uint32_t idx;
	struct sk_buff *rx_buf[4];
};

struct signal_stat {
	uint8_t update_req;
	uint8_t avg_val;
	uint32_t total_num;
	uint32_t total_val;
};

struct _atr_aligned8_ recv_priv {
	_lock lock;
	_queue free_recv_queue;
	_queue recv_pending_queue;
	_queue uc_swdec_pending_queue;
	uint8_t *pallocated_frame_buf;
	uint8_t *precv_frame_buf;
	uint32_t free_recvframe_cnt;
	_adapter *adapter;
	uint32_t bIsAnyNonBEPkts;
	u8Byte rx_bytes;
	u8Byte rx_pkts;
	u8Byte rx_drop;
	u8Byte rx_overflow;
	u8Byte last_rx_bytes;
	uint32_t rx_icv_err;
	uint32_t rx_largepacket_crcerr;
	uint32_t rx_smallpacket_crcerr;
	uint32_t rx_middlepacket_crcerr;
	uint8_t *pallocated_recv_buf;
	uint8_t *precv_buf;
	_queue free_recv_buf_queue;
	uint32_t free_recv_buf_queue_cnt;
	struct rtw_rx_ring rx_ring[1];
	int rxringcount;
	uint16_t rxbuffersize;
	uint8_t is_signal_dbg;
	uint8_t signal_strength_dbg;
	int8_t rssi;					// +2932
	int8_t rxpwdb;
	uint8_t signal_strength;
	uint8_t signal_qual;
	uint8_t noise;
	int RxSNRdB[2];
	int8_t RxRssi[2];
	int FalseAlmCnt_all;
	_timer signal_stat_timer;
	uint32_t signal_stat_sampling_interval;
	struct signal_stat signal_qual_data;
	struct signal_stat signal_strength_data;
	uint8_t promisc_enabled;
	uint8_t promisc_len_used;
	_list promisc_list;
	_lock promisc_lock;
	uint32_t promisc_bk_rcr;
	uint16_t promisc_bk_rxfltmap2;
	uint8_t promisc_mgntframe_enabled;
};

struct _atr_aligned4_ sta_priv {
	uint8_t *pallocated_stainfo_buf;
	uint32_t allocated_stainfo_size;
	uint8_t *pstainfo_buf;
	_queue free_sta_queue;
	_lock sta_hash_lock;
	_list sta_hash[5];
	int asoc_sta_count;
	_queue sleep_q;
	_queue wakeup_q;
	_adapter *padapter;
	_list asoc_list;
	_list auth_list;
	_lock asoc_list_lock;
	_lock auth_list_lock;
	uint32_t auth_to;
	uint32_t assoc_to;
	uint32_t expire_to;
	struct sta_info *sta_aid[5];
	uint16_t sta_dz_bitmap;
	uint16_t tim_bitmap;
	uint16_t max_num_sta;
};

union Keytype {
	uint8_t skey[16];
	uint32_t lkey[4];
};



union pn48 {
	u8Byte val;
	struct {
		uint8_t TSC0;
		uint8_t TSC1;
		uint8_t TSC2;
		uint8_t TSC3;
		uint8_t TSC4;
		uint8_t TSC5;
		uint8_t TSC6;
		uint8_t TSC7;
	}_byte_;
};

struct _NDIS_802_11_WEP {
	uint32_t Length;
	uint32_t KeyIndex;
	uint32_t KeyLength;
	uint8_t KeyMaterial[16];
};
typedef struct _NDIS_802_11_WEP NDIS_802_11_WEP;

struct $D75518714447A990003EBC933C23F70E {
	uint32_t HighPart;
	uint32_t LowPart;
};

union _LARGE_INTEGER {
	uint8_t charData[8];
	struct $D75518714447A990003EBC933C23F70E field;
};
typedef union _LARGE_INTEGER LARGE_INTEGER;

struct $121C25F90E4E195D1524BBC5399ADEBE {
	LARGE_INTEGER HighPart;
	LARGE_INTEGER LowPart;
};

union _OCTET16_INTEGER {
	uint8_t charData[16];
	struct $121C25F90E4E195D1524BBC5399ADEBE field;
};
typedef union _OCTET16_INTEGER OCTET16_INTEGER;

struct $BB6DA6E37D48DEE353E02A8C8F92DDF7 {
	OCTET16_INTEGER HighPart;
	OCTET16_INTEGER LowPart;
};

union _OCTET32_INTEGER {
	uint8_t charData[32];
	struct $BB6DA6E37D48DEE353E02A8C8F92DDF7 field;
};
typedef union _OCTET32_INTEGER OCTET32_INTEGER;

struct _OCTET_STRING {
	uint8_t *Octet;
	int Length;
};
typedef struct _OCTET_STRING OCTET_STRING;

struct _wpa_global_info {
	OCTET32_INTEGER Counter;
	int GTKAuthenticator;
	int GKeyDoneStations;
	int GInitAKeys;
	int GUpdateStationKeys;
	int GkeyReady;
	OCTET_STRING AuthInfoElement;
	uint8_t AuthInfoBuf[128];
	uint8_t MulticastCipher;
	OCTET_STRING GNonce;
	uint8_t GNonceBuf[32];
	uint8_t GTK[4][32];
	uint8_t GMK[32];
	int GN;
	int GM;
	int GTKRekey;
};
typedef struct _wpa_global_info WPA_GLOBAL_INFO;

typedef struct _wpa_sta_info WPA_STA_INFO;

struct _atr_aligned4_ security_priv {
	uint32_t dot11AuthAlgrthm;
	uint32_t dot11PrivacyAlgrthm;
	uint32_t dot11PrivacyKeyIndex;
	union Keytype dot11DefKey[4];
	uint32_t dot11DefKeylen[4];
	uint32_t dot118021XGrpPrivacy;
	uint32_t dot118021XGrpKeyid;
	union Keytype dot118021XGrpKey[4];
	union Keytype dot118021XGrptxmickey[4];
	union Keytype dot118021XGrprxmickey[4];
	union pn48 dot11Grptxpn;
	union pn48 dot11Grprxpn;
	uint32_t dot8021xalg;
	uint32_t wpa_psk;
	uint32_t wpa_group_cipher;
	uint32_t wpa2_group_cipher;
	uint32_t wpa_pairwise_cipher;
	uint32_t wpa2_pairwise_cipher;
	uint8_t wps_ie[512];
	int wps_ie_len;
	uint8_t binstallGrpkey;
	uint8_t busetkipkey;
	uint8_t bcheck_grpkey;
	uint8_t bgrpkey_handshake;
	int32_t sw_encrypt;
	int32_t sw_decrypt;
	int32_t hw_decrypted;
	uint32_t ndisauthtype;
	uint32_t ndisencryptstatus;
	NDIS_802_11_WEP ndiswep;
	uint8_t supplicant_ie[256];
	uint32_t last_mic_err_time;
	uint8_t btkip_countermeasure;
	uint8_t btkip_wait_report;
	uint32_t btkip_countermeasure_time;
	WPA_GLOBAL_INFO wpa_global_info;
	uint8_t *palloc_wpastainfo_buf;
	uint32_t alloc_wpastainfo_size;
	WPA_STA_INFO *wpa_sta_info[3];
	uint8_t wpa_passphrase[65];
	uint8_t wps_phase;
};

struct _atr_aligned4_ registry_priv {
	uint8_t chip_version;
	uint8_t hci;
	NDIS_802_11_SSID ssid;
	uint8_t channel;
	uint8_t wireless_mode;
	uint8_t scan_mode;
	uint8_t vrtl_carrier_sense;
	uint8_t vcs_type;
	uint16_t rts_thresh;
	uint8_t soft_ap;
	uint8_t power_mgnt;
	uint8_t ps_enable;
	uint8_t ips_mode;
	uint8_t smart_ps;
	uint8_t mp_mode;
	uint8_t software_encrypt;
	uint8_t software_decrypt;
	uint8_t acm_method;
	uint8_t wmm_enable;
	uint8_t uapsd_enable;
	uint32_t beacon_period;
	uint8_t ht_enable;
	uint8_t ampdu_enable;
	uint8_t rx_stbc;
	uint8_t ampdu_amsdu;
	uint8_t rf_config;
	uint8_t power_percentage_idx;
	uint8_t wifi_spec;
	uint8_t channel_plan;
	uint8_t ifname[16];
	uint8_t if2name[16];
	uint8_t RegEnableTxPowerLimit;
	uint8_t RegEnableTxPowerByRate;
	uint8_t RegEnableKFree;
	uint8_t RegPowerBase;
	uint8_t RegPwrTblSel;
	uint8_t adaptivity_en;
	uint8_t adaptivity_mode;
	uint8_t adaptivity_dml;
	uint8_t adaptivity_dc_backoff;
	int8_t adaptivity_th_l2h_ini;
};

typedef void *_sema;

typedef _sema _pwrlock;

enum _rt_rf_power_state //: sint32_t
{
	rf_on = 0x0, rf_sleep = 0x1, rf_off = 0x2, rf_max = 0x3,
};
typedef enum _rt_rf_power_state rt_rf_power_state;

struct _atr_aligned4_ pwrctrl_priv {
	_pwrlock lock;
	volatile uint8_t rpwm;
	volatile uint8_t cpwm;
	volatile uint8_t tog;
	volatile uint8_t cpwm_tog;
	uint8_t pwr_mode;
	uint8_t smart_ps;
	uint8_t bcn_ant_mode;
	uint32_t alives;
	u8Byte wowlan_fw_iv;
	uint8_t bpower_saving;
	uint8_t b_hw_radio_off;
	uint8_t reg_rfoff;
	uint8_t reg_pdnmode;
	uint32_t rfoff_reason;
	uint32_t cur_ps_level;
	uint32_t reg_rfps_level;
	uint8_t b_support_aspm;
	uint8_t b_support_backdoor;
	uint8_t const_amdpci_aspm;
	uint32_t ips_enter_cnts;
	uint32_t ips_leave_cnts;
	uint8_t ps_enable;
	uint8_t ips_mode;
	uint8_t ips_org_mode;
	uint8_t ips_mode_req;
	uint32_t bips_processing;
	uint32_t ips_deny_time;
	uint8_t ps_processing;
	uint8_t bLeisurePs;
	uint8_t LpsIdleCount;
	uint8_t power_mgnt;
	uint8_t org_power_mgnt;
	uint8_t bFwCurrentInPSMode;
	uint32_t DelayLPSLastTimeStamp;
	uint8_t btcoex_rfon;
	int32_t pnp_current_pwr_state;
	uint8_t pnp_bstop_trx;
	uint8_t bInternalAutoSuspend;
	uint8_t bInSuspend;
	uint8_t bSupportRemoteWakeup;
	_timer pwr_state_check_timer;
	int pwr_state_check_interval;
	uint8_t pwr_state_check_cnts;
	int ps_flag;
	rt_rf_power_state rf_pwrstate;
	rt_rf_power_state change_rfpwrstate;
	uint8_t wepkeymask;
	uint8_t bHWPowerdown;
	uint8_t bHWPwrPindetect;
	uint8_t bkeepfwalive;
	uint8_t brfoffbyhw;
	uint32_t PS_BBRegBackup[4];
	uint8_t tdma_slot_period;
	uint8_t tdma_rfon_period_len_1;
	uint8_t tdma_rfon_period_len_2;
	uint8_t tdma_rfon_period_len_3;
	uint8_t lps_dtim;
};

struct _atr_aligned2_ eeprom_priv { // __attribute__((packed))!?
	uint8_t bautoload_fail_flag;
	uint8_t mac_addr[6];
	uint16_t CustomerID;
	uint8_t EepromOrEfuse;
	uint8_t efuse_eeprom_data[512];
	uint8_t EEPROMRFGainOffset;
	uint8_t EEPROMRFGainVal;
};

enum _CHANNEL_WIDTH // : sint32_t
{
	CHANNEL_WIDTH_20 = 0x0,
	CHANNEL_WIDTH_40 = 0x1,
	CHANNEL_WIDTH_80 = 0x2,
	CHANNEL_WIDTH_160 = 0x3,
	CHANNEL_WIDTH_80_80 = 0x4,
	CHANNEL_WIDTH_MAX = 0x5,
};
typedef enum _CHANNEL_WIDTH CHANNEL_WIDTH;

enum _HAL_DEF_VARIABLE // : sint32_t
{
	HAL_DEF_UNDERCORATEDSMOOTHEDPWDB = 0x0,
	HAL_DEF_IS_SUPPORT_ANT_DIV = 0x1,
	HAL_DEF_CURRENT_ANTENNA = 0x2,
	HAL_DEF_DRVINFO_SZ = 0x3,
	HAL_DEF_MAX_RECVBUF_SZ = 0x4,
	HAL_DEF_RX_PACKET_OFFSET = 0x5,
	HAL_DEF_RX_DMA_SZ_WOW = 0x6,
	HAL_DEF_RX_DMA_SZ = 0x7,
	HAL_DEF_RX_PAGE_SIZE = 0x8,
	HAL_DEF_DBG_DM_FUNC = 0x9,
	HAL_DEF_RA_DECISION_RATE = 0xA,
	HAL_DEF_RA_SGI = 0xB,
	HAL_DEF_PT_PWR_STATUS = 0xC,
	HW_VAR_MAX_RX_AMPDU_FACTOR = 0xD,
	HW_DEF_RA_INFO_DUMP = 0xE,
	HAL_DEF_DBG_DUMP_TXPKT = 0xF,
	HW_DEF_ODM_DBG_FLAG = 0x10,
	HW_DEF_ODM_DBG_LEVEL = 0x11,
	HAL_DEF_TX_PAGE_SIZE = 0x12,
	HAL_DEF_TX_PAGE_BOUNDARY = 0x13,
	HAL_DEF_MACID_SLEEP = 0x14,
	HAL_DEF_DBG_RX_INFO_DUMP = 0x15,
};
typedef enum _HAL_DEF_VARIABLE HAL_DEF_VARIABLE;

enum _HAL_ODM_VARIABLE // : sint32_t
{
	HAL_ODM_STA_INFO = 0x0,
	HAL_ODM_DBG_FLAG = 0x1,
	HAL_ODM_RX_INFO_DUMP = 0x2,
	HAL_ODM_NOISE_MONITOR = 0x3,
	HAL_ODM_REGULATION = 0x4,
};
typedef enum _HAL_ODM_VARIABLE HAL_ODM_VARIABLE;

typedef void *_thread_hdl_;

/*
 // osdep_service.h
 struct task_struct
 {
 const char *task_name;
 _thread_hdl_ task;
 _sema wakeup_sema;
 _sema terminate_sema;
 uint32_t blocked;
 uint32_t callback_running;
 };
 */

typedef struct net_device *_nic_hdl;
/*
 // wrapper.h
 struct net_device_stats
 {
 uint32_t rx_packets;
 uint32_t tx_packets;
 uint32_t rx_dropped;
 uint32_t tx_dropped;
 uint32_t rx_bytes;
 uint32_t tx_bytes;
 uint32_t rx_overflow;
 };
 */

struct dvobj_priv {
	void *if1;
	void *if2;
	void *padapters[2];
	uint8_t iface_nums;
	uint8_t RtOutPipe[3];
	uint8_t Queue2Pipe[8];
	uint8_t irq_alloc;
	uint8_t irq_enabled;
	_lock irq_th_lock;
};

struct phy_info {
	uint8_t RxPWDBAll;
	uint8_t SignalQuality;
	uint8_t RxMIMOSignalStrength[1];
	int8_t RecvSignalPower;
	uint8_t SignalStrength;
};

struct _atr_aligned4_ rx_pkt_attrib {
	uint16_t pkt_len;
	uint8_t physt;
	uint8_t drvinfo_sz;
	uint8_t shift_sz;
	uint8_t hdrlen;
	uint8_t to_fr_ds;
	uint8_t amsdu;
	uint8_t qos;
	uint8_t priority;
	uint8_t pw_save;
	uint8_t mdata;
	uint16_t seq_num;
	uint8_t frag_num;
	uint8_t mfrag;
	uint8_t order;
	uint8_t privacy;
	uint8_t bdecrypted;
	uint8_t encrypt;
	uint8_t iv_len;
	uint8_t icv_len;
	uint8_t crc_err;
	uint8_t icv_err;
	uint16_t eth_type;
	uint8_t dst[6];
	uint8_t src[6];
	uint8_t ta[6];
	uint8_t ra[6];
	uint8_t bssid[6];
	uint8_t ack_policy;
	uint8_t tcpchk_valid;
	uint8_t ip_chkrpt;
	uint8_t tcp_chkrpt;
	uint8_t key_index;
	uint8_t mcs_rate;
	uint8_t rxht;
	uint8_t sgi;
	uint8_t pkt_rpt_type;
	uint32_t MacIDValidEntry[2];
	uint8_t data_rate;
	struct phy_info phy_info;
};

struct recv_frame_hdr {
	_list list;
	struct sk_buff *pkt;
	struct sk_buff *pkt_newalloc;
	_adapter *adapter;
	uint8_t fragcnt;
	int frame_tag;
	struct rx_pkt_attrib attrib;
	uint32_t len;
	uint8_t *rx_head;
	uint8_t *rx_data;
	uint8_t *rx_tail;
	uint8_t *rx_end;
	void *precvbuf;
	struct sta_info *psta;
};

struct recv_frame {
union {
	_list list;
	struct recv_frame_hdr hdr;
	uint32_t mem[32];
	};
};
/*
union $AB04817EA6EB89125E28056B7464A4D7 {
	_list list;
	struct recv_frame_hdr hdr;
	uint32_t mem[32];
};

union recv_frame {
	union $AB04817EA6EB89125E28056B7464A4D7 u;
};
*/
/*
 // skbuff.h
 struct sk_buff
 {
 struct sk_buff *next;
 struct sk_buff *prev;
 struct sk_buff_head *list;
 uint8_t *head;
 uint8_t *data;
 uint8_t *tail;
 uint8_t *end;
 void *dev;
 uint32_t len;
 };

 struct sk_buff_head
 {
 struct list_head *next;
 struct list_head *prev;
 uint32_t qlen;
 };
 */

struct tx_servq {
	_list tx_pending;
	_queue sta_pending;
	int qcnt;
};

struct sta_xmit_priv {
	_lock lock;
	sint option;
	sint apsd_setting;
	struct tx_servq be_q;
	struct tx_servq bk_q;
	struct tx_servq vi_q;
	struct tx_servq vo_q;
	_list legacy_dz;
	_list apsd;
	uint16_t txseq_tid[16];
};

struct stainfo_rxcache {
	uint16_t tid_rxseq[16];
};

struct sta_recv_priv {
	_lock lock;
	sint option;
	_queue defrag_q;
	struct stainfo_rxcache rxcache;
};

struct stainfo_stats {
	u8Byte rx_mgnt_pkts;
	u8Byte rx_ctrl_pkts;
	u8Byte rx_data_pkts;
	u8Byte last_rx_mgnt_pkts;
	u8Byte last_rx_ctrl_pkts;
	u8Byte last_rx_data_pkts;
	u8Byte rx_bytes;
	u8Byte tx_pkts;
	u8Byte tx_bytes;
};

struct _RSSI_STA {
	int32_t UndecoratedSmoothedPWDB;
	int32_t UndecoratedSmoothedCCK;
	int32_t UndecoratedSmoothedOFDM;
	u8Byte PacketMap;
	uint8_t ValidBit;
	uint32_t OFDM_pkt;
};
typedef struct _RSSI_STA RSSI_STA;

struct sta_info {
	_lock lock;
	_list list;
	_list hash_list;
	_adapter *padapter;
	struct sta_xmit_priv sta_xmitpriv;
	struct sta_recv_priv sta_recvpriv;
	_queue sleep_q;
	uint32_t sleepq_len;
	uint32_t state;
	uint32_t aid;
	uint32_t mac_id;
	uint32_t qos_option;
	uint8_t hwaddr[6];
	uint32_t ieee8021x_blocked;
	uint32_t dot118021XPrivacy;
	union Keytype dot11tkiptxmickey;
	union Keytype dot11tkiprxmickey;
	union Keytype dot118021x_UncstKey;
	union pn48 dot11txpn;
	union pn48 dot11rxpn;
	uint8_t bssrateset[16];
	uint32_t bssratelen;
	int32_t rssi;
	int32_t signal_quality;
	uint8_t cts2self;
	uint8_t rtsen;
	uint8_t raid;
	uint8_t init_rate;
	uint32_t ra_mask;
	uint8_t wireless_mode;
	struct stainfo_stats sta_stats;
	_timer addba_retry_timer;
	uint16_t BA_starting_seqctrl[16];
	struct ht_priv htpriv;
	_list asoc_list;
	_list auth_list;
	uint32_t expire_to;
	uint32_t auth_seq;
	uint32_t authalg;
	uint8_t chg_txt[128];
	uint16_t capability;
	uint32_t flags;
	int dot8021xalg;
	int wpa_psk;
	int wpa_group_cipher;
	int wpa2_group_cipher;
	int wpa_pairwise_cipher;
	int wpa2_pairwise_cipher;
	uint8_t bpairwise_key_installed;
	uint8_t wpa_ie[32];
	uint8_t nonerp_set;
	uint8_t no_short_slot_time_set;
	uint8_t no_short_preamble_set;
	uint8_t no_ht_gf_set;
	uint8_t no_ht_set;
	uint8_t ht_20mhz_set;
	uint32_t tx_ra_bitmap;
	uint8_t qos_info;
	uint8_t max_sp_len;
	uint8_t uapsd_bk;
	uint8_t uapsd_be;
	uint8_t uapsd_vi;
	uint8_t uapsd_vo;
	uint8_t has_legacy_ac;
	uint32_t sleepq_ac_len;
	RSSI_STA rssi_stat;
	uint8_t bValid;
	uint8_t IOTPeer;
	uint8_t rssi_level;
	uint8_t RSSI_Path[4];
	uint8_t RSSI_Ave;
	uint8_t RXEVM[4];
	uint8_t RXSNR[4];
};
/*
 // wifi_conf.h
 struct _atr_aligned4_ _cus_ie
 {
 uint8_t *ie;
 uint8_t type;
 };
 */

struct fifo_more_data {
	uint32_t more_data;
	uint32_t len;
};

struct hw_xmit {
	_queue *sta_queue;
	int accnt;
};

struct tx_buf_desc {
	uint32_t txdw0;
	uint32_t txdw1;
	uint32_t txdw2;
	uint32_t txdw3;
	uint32_t txdw4;
	uint32_t txdw5;
	uint32_t txdw6;
	uint32_t txdw7;
};

struct recv_buf_stat {
	uint32_t rxdw0;
	uint32_t rxdw1;
};

struct _wpa_sta_info {
	int state;
	int gstate;
	int RSNEnabled;
	int PInitAKeys;
	uint8_t UnicastCipher;
	LARGE_INTEGER CurrentReplayCounter;
	LARGE_INTEGER ReplayCounterStarted;
	OCTET_STRING ANonce;
	OCTET_STRING SNonce;
	uint8_t AnonceBuf[32];
	uint8_t SnonceBuf[32];
	uint8_t PMK[32];
	uint8_t PTK[64];
	OCTET_STRING EAPOLMsgRecvd;
	OCTET_STRING EAPOLMsgSend;
	OCTET_STRING EapolKeyMsgRecvd;
	OCTET_STRING EapolKeyMsgSend;
	uint8_t eapSendBuf[512];
	struct timer_list resendTimer;
	int resendCnt;
	int clientHndshkProcessing;
	int clientHndshkDone;
	int clientGkeyUpdate;
	LARGE_INTEGER clientMICReportReplayCounter;
};

struct pkt_attrib {
	uint8_t type;
	uint8_t subtype;
	uint8_t bswenc;
	uint8_t dhcp_pkt;
	uint16_t ether_type;
	uint16_t seqnum;
	uint16_t pkt_hdrlen;
	uint16_t hdrlen;
	uint32_t pktlen;
	uint32_t last_txcmdsz;
	uint8_t encrypt;
	uint8_t iv_len;
	uint8_t icv_len;
	uint8_t iv[18];
	uint8_t icv[16];
	uint8_t priority;
	uint8_t ack_policy;
	uint8_t mac_id;
	uint8_t vcs_mode;
	uint8_t dst[6];
	uint8_t src[6];
	uint8_t ta[6];
	uint8_t ra[6];
	uint8_t key_idx;
	uint8_t qos_en;
	uint8_t ht_en;
	uint8_t raid;
	uint8_t bwmode;
	uint8_t ch_offset;
	uint8_t sgi;
	uint8_t ampdu_en;
	uint8_t mdata;
	uint8_t pctrl;
	uint8_t triggered;
	uint8_t qsel;
	uint8_t eosp;
	uint8_t rate;
	uint8_t intel_proxim;
	uint8_t retry_ctrl;
	struct sta_info *psta;
};

typedef struct sk_buff _pkt;

struct _XIMT_BUF_ {
	uint32_t AllocatBufAddr;
	uint32_t BufAddr;
	uint32_t BufLen;
};
typedef struct _XIMT_BUF_ XIMT_BUF;

struct _atr_aligned4_ xmit_frame {
	_list list;
	struct pkt_attrib attrib;
	_pkt *pkt;
	int frame_tag;
	_adapter *padapter;
	uint8_t *buf_addr;
	struct xmit_buf *pxmitbuf;
	uint32_t TxDexAddr;
	uint32_t HdrLen;
	uint32_t PayLoadAddr;
	uint32_t PayLoadLen;
	uint32_t TotalLen;
	uint32_t BlockNum;
	XIMT_BUF BufInfo[4];
	BOOLEAN NoCoalesce;
};

struct xmit_buf {
	_list list;
	_adapter *padapter;
	_pkt *pkt;
	uint8_t *pbuf;
	void *priv_data;
	uint16_t buf_tag;
	uint16_t flags;
	uint32_t alloc_sz;
	uint32_t len;
	struct submit_ctx *sctx;
	XIMT_BUF BufInfo[4];
	uint32_t BlockNum;
};

struct submit_ctx {
	uint32_t submit_time;
	uint32_t timeout_ms;
	int status;
};
/*
 // wrapper.h
 struct net_device
 {
 char name[16];
 void *priv;
 uint8_t dev_addr[6];
 int (*init)(void);
 int (*open)(struct net_device *);
 int (*stop)(struct net_device *);
 int (*hard_start_xmit)(struct sk_buff *, struct net_device *);
 int (*do_ioctl)(struct net_device *, struct iwreq *, int);
 struct net_device_stats *(*get_stats)(struct net_device *);
 };
 */
/*
 // wireless.h
 struct iw_point
 {
 void *pointer;
 uint16_t length;
 uint16_t flags;
 };

 struct iw_param
 {
 sint32_t value;
 uint8_t fixed;
 uint8_t disabled;
 uint16_t flags;
 };


 struct iw_freq
 {
 sint32_t m;
 sint16_t e;
 uint8_t i;
 uint8_t flags;
 };

 struct iw_quality
 {
 uint8_t qual;
 uint8_t level;
 uint8_t noise;
 uint8_t updated;
 };

 struct sockaddr_t
 {
 uint8_t sa_len;
 uint8_t sa_family;
 char sa_data[14];
 };

 union iwreq_data
 {
 char name[16];
 struct iw_point essid;
 struct iw_param nwid;
 struct iw_freq freq;
 struct iw_param sens;
 struct iw_param bitrate;
 struct iw_param txpower;
 struct iw_param rts;
 struct iw_param frag;
 uint32_t mode;
 struct iw_param retry;
 struct iw_point encoding;
 struct iw_param power;
 struct iw_quality qual;
 struct sockaddr_t ap_addr;
 struct sockaddr_t addr;
 struct iw_param param;
 struct iw_point data;
 struct iw_point passphrase;
 };

 struct iwreq
 {
 char ifr_name[16];
 union iwreq_data u;
 };
 */
struct co_data_priv {
	uint8_t co_ch;
	uint8_t co_bw;
	uint8_t co_ch_offset;
	uint8_t rsvd;
};

enum _HARDWARE_TYPE // : sint32_t
{
	HARDWARE_TYPE_RTL8180 = 0x0,
	HARDWARE_TYPE_RTL8185 = 0x1,
	HARDWARE_TYPE_RTL8187 = 0x2,
	HARDWARE_TYPE_RTL8188 = 0x3,
	HARDWARE_TYPE_RTL8190P = 0x4,
	HARDWARE_TYPE_RTL8192E = 0x5,
	HARDWARE_TYPE_RTL819xU = 0x6,
	HARDWARE_TYPE_RTL8192SE = 0x7,
	HARDWARE_TYPE_RTL8192SU = 0x8,
	HARDWARE_TYPE_RTL8192CE = 0x9,
	HARDWARE_TYPE_RTL8192CU = 0xA,
	HARDWARE_TYPE_RTL8192DE = 0xB,
	HARDWARE_TYPE_RTL8192DU = 0xC,
	HARDWARE_TYPE_RTL8723AE = 0xD,
	HARDWARE_TYPE_RTL8723AU = 0xE,
	HARDWARE_TYPE_RTL8723AS = 0xF,
	HARDWARE_TYPE_RTL8188EE = 0x10,
	HARDWARE_TYPE_RTL8188EU = 0x11,
	HARDWARE_TYPE_RTL8188ES = 0x12,
	HARDWARE_TYPE_RTL8192EE = 0x13,
	HARDWARE_TYPE_RTL8192EU = 0x14,
	HARDWARE_TYPE_RTL8192ES = 0x15,
	HARDWARE_TYPE_RTL8812E = 0x16,
	HARDWARE_TYPE_RTL8812AU = 0x17,
	HARDWARE_TYPE_RTL8811AU = 0x18,
	HARDWARE_TYPE_RTL8821E = 0x19,
	HARDWARE_TYPE_RTL8821U = 0x1A,
	HARDWARE_TYPE_RTL8821S = 0x1B,
	HARDWARE_TYPE_RTL8723BE = 0x1C,
	HARDWARE_TYPE_RTL8723BU = 0x1D,
	HARDWARE_TYPE_RTL8723BS = 0x1E,
	HARDWARE_TYPE_RTL8195A = 0x1F,
	HARDWARE_TYPE_RTL8711B = 0x20,
	HARDWARE_TYPE_RTL8188FE = 0x21,
	HARDWARE_TYPE_RTL8188FU = 0x22,
	HARDWARE_TYPE_RTL8188FS = 0x23,
	HARDWARE_TYPE_MAX = 0x24,
};

struct RF_Shadow_Compare_Map {
	uint32_t Value;
	uint8_t Compare;
	uint8_t ErrorOrNot;
	uint8_t Recorver;
	uint8_t Driver_Write;
};
typedef struct RF_Shadow_Compare_Map RF_SHADOW_T;

enum _PS_BBRegBackup_ // : sint32_t
{
	PSBBREG_RF0 = 0x0,
	PSBBREG_RF1 = 0x1,
	PSBBREG_RF2 = 0x2,
	PSBBREG_AFE0 = 0x3,
	PSBBREG_TOTALCNT = 0x4,
};

/*
 // hal_irqn.h
 enum _IRQn_Type_ // : sint32_t
 {
 NonMaskableInt_IRQn = 0xFFFFFFF2,
 HardFault_IRQn = 0xFFFFFFF3,
 MemoryManagement_IRQn = 0xFFFFFFF4,
 BusFault_IRQn = 0xFFFFFFF5,
 UsageFault_IRQn = 0xFFFFFFF6,
 SVCall_IRQn = 0xFFFFFFFB,
 DebugMonitor_IRQn = 0xFFFFFFFC,
 PendSV_IRQn = 0xFFFFFFFE,
 SysTick_IRQn = 0xFFFFFFFF,
 SYSTEM_ON_IRQ = 0x0,
 WDG_IRQ = 0x1,
 TIMER0_IRQ = 0x2,
 TIMER1_IRQ = 0x3,
 I2C3_IRQ = 0x4,
 TIMER2_7_IRQ = 0x5,
 SPI0_IRQ = 0x6,
 GPIO_IRQ = 0x7,
 UART0_IRQ = 0x8,
 SPI_FLASH_IRQ = 0x9,
 USB_OTG_IRQ = 0xA,
 SDIO_HOST_IRQ = 0xB,
 SDIO_DEVICE_IRQ = 0xC,
 I2S0_PCM0_IRQ = 0xD,
 I2S1_PCM1_IRQ = 0xE,
 WL_DMA_IRQ = 0xF,
 WL_PROTOCOL_IRQ = 0x10,
 CRYPTO_IRQ = 0x11,
 GMAC_IRQ = 0x12,
 PERIPHERAL_IRQ = 0x13,
 GDMA0_CHANNEL0_IRQ = 0x14,
 GDMA0_CHANNEL1_IRQ = 0x15,
 GDMA0_CHANNEL2_IRQ = 0x16,
 GDMA0_CHANNEL3_IRQ = 0x17,
 GDMA0_CHANNEL4_IRQ = 0x18,
 GDMA0_CHANNEL5_IRQ = 0x19,
 GDMA1_CHANNEL0_IRQ = 0x1A,
 GDMA1_CHANNEL1_IRQ = 0x1B,
 GDMA1_CHANNEL2_IRQ = 0x1C,
 GDMA1_CHANNEL3_IRQ = 0x1D,
 GDMA1_CHANNEL4_IRQ = 0x1E,
 GDMA1_CHANNEL5_IRQ = 0x1F,
 I2C0_IRQ = 0x40,
 I2C1_IRQ = 0x41,
 I2C2_IRQ = 0x42,
 SPI1_IRQ = 0x48,
 SPI2_IRQ = 0x49,
 UART1_IRQ = 0x50,
 UART2_IRQ = 0x51,
 UART_LOG_IRQ = 0x58,
 ADC_IRQ = 0x59,
 DAC0_IRQ = 0x5B,
 DAC1_IRQ = 0x5C,
 LP_EXTENSION_IRQ = 0x5D,
 PTA_TRX_IRQ = 0x5F,
 RXI300_IRQ = 0x60,
 NFC_IRQ = 0x61,
 };

 typedef enum _IRQn_Type_ IRQn_Type;

 typedef void (*IRQ_FUN)(void *);

 struct _IRQ_HANDLE_
 {
 IRQ_FUN IrqFun;
 IRQn_Type IrqNum;
 uint32_t Data;
 uint32_t Priority;
 };
 typedef struct _IRQ_HANDLE_ IRQ_HANDLE;
 */
/*
 // hal_soc_ps_monitor.h
 struct _power_state_
 {
 uint8_t FuncIdx;
 uint8_t PowerState;
 };

 typedef struct _power_state_ POWER_STATE;

 struct _atr_aligned4_ _power_mgn_
 {
 uint8_t ActFuncCount;
 POWER_STATE PwrState[10];
 uint8_t CurrentState;
 uint8_t SDREn;
 uint32_t MSPbackup[129];
 uint32_t CPURegbackup[25];
 uint32_t CPUPSP;
 uint32_t WakeEventFlag;
 BOOL SleepFlag;
 };
 typedef struct _power_mgn_ Power_Mgn;

 /*
 // hal_gpio.h
 enum $E1AD70AB12E7AA6E98B8D89D9B965EB5 //: sint32_t
 {
 _PORT_A = 0x0,
 _PORT_B = 0x1,
 _PORT_C = 0x2,
 _PORT_D = 0x3,
 _PORT_E = 0x4,
 _PORT_F = 0x5,
 _PORT_G = 0x6,
 _PORT_H = 0x7,
 _PORT_I = 0x8,
 _PORT_J = 0x9,
 _PORT_K = 0xA,
 _PORT_MAX = 0xB,
 };

 typedef void (*GPIO_IRQ_FUN)(void *, uint32_t);

 typedef void (*GPIO_USER_IRQ_FUN)(uint32_t);

 struct _atr_aligned4_ _HAL_GPIO_ADAPTER_
 {
 IRQ_HANDLE IrqHandle;
 GPIO_USER_IRQ_FUN UserIrqHandler;
 GPIO_IRQ_FUN PortA_IrqHandler[32];
 void *PortA_IrqData[32];
 void (*EnterCritical)(void);
 void (*ExitCritical)(void);
 uint32_t Local_Gpio_Dir[3];
 uint8_t Gpio_Func_En;
 uint8_t Locked;
 };
 typedef struct _HAL_GPIO_ADAPTER_ *PHAL_GPIO_ADAPTER;
 */

struct hal_ops {
	uint32_t (*hal_power_on)(_adapter *);
	uint32_t (*hal_init)(_adapter *);
	uint32_t (*hal_deinit)(_adapter *);
	void (*free_hal_data)(_adapter *);
	uint32_t (*inirp_init)(_adapter *);
	uint32_t (*inirp_deinit)(_adapter *);
	void (*irp_reset)(_adapter *);
	int32_t (*init_xmit_priv)(_adapter *);
	void (*free_xmit_priv)(_adapter *);
	int32_t (*init_recv_priv)(_adapter *);
	void (*free_recv_priv)(_adapter *);
	void (*update_txdesc)(struct xmit_frame *, uint8_t *);
	void (*InitSwLeds)(_adapter *);
	void (*DeInitSwLeds)(_adapter *);
	void (*dm_init)(_adapter *);
	void (*dm_deinit)(_adapter *);
	void (*read_chip_version)(_adapter *);
	void (*init_default_value)(_adapter *);
	void (*intf_chip_configure)(_adapter *);
	void (*read_adapter_info)(_adapter *);
	void (*enable_interrupt)(_adapter *);
	void (*disable_interrupt)(_adapter *);
	int32_t (*interrupt_handler)(_adapter *);
	void (*set_bwmode_handler)(_adapter *, CHANNEL_WIDTH, uint8_t);
	void (*set_channel_handler)(_adapter *, uint8_t);
	void (*set_chnl_bw_handler)(_adapter *, uint8_t, CHANNEL_WIDTH, uint8_t,
			uint8_t);
	void (*hal_dm_watchdog)(_adapter *);
	void (*SetHwRegHandler)(_adapter *, uint8_t, uint8_t *);
	void (*GetHwRegHandler)(_adapter *, uint8_t, uint8_t *);
	uint8_t (*GetHalDefVarHandler)(_adapter *, HAL_DEF_VARIABLE, PVOID);
	uint8_t (*SetHalDefVarHandler)(_adapter *, HAL_DEF_VARIABLE, PVOID);
	void (*GetHalODMVarHandler)(_adapter *, HAL_ODM_VARIABLE, PVOID, BOOLEAN);
	void (*SetHalODMVarHandler)(_adapter *, HAL_ODM_VARIABLE, PVOID, BOOLEAN);
	void (*UpdateRAMaskHandler)(_adapter *, uint32_t, uint8_t);
	void (*Add_RateATid)(_adapter *, uint32_t, uint8_t *, uint8_t);
	void (*clone_haldata)(_adapter *, _adapter *);
	void (*run_thread)(_adapter *);
	void (*cancel_thread)(_adapter *);
	int32_t (*hal_xmit)(_adapter *, struct xmit_frame *);
	int32_t (*mgnt_xmit)(_adapter *, struct xmit_frame *);
	uint32_t (*read_bbreg)(_adapter *, uint32_t, uint32_t);
	void (*write_bbreg)(_adapter *, uint32_t, uint32_t, uint32_t);
	uint32_t (*read_rfreg)(_adapter *, uint32_t, uint32_t, uint32_t);
	void (*write_rfreg)(_adapter *, uint32_t, uint32_t, uint32_t, uint32_t);
	void (*EfusePowerSwitch)(_adapter *, uint8_t, uint8_t);
	void (*ReadEFuse)(_adapter *, uint8_t, uint16_t, uint16_t, uint8_t *,
			BOOLEAN);
	void (*EFUSEGetEfuseDefinition)(_adapter *, uint8_t, uint8_t, void *,
			BOOLEAN);
	uint16_t (*EfuseGetCurrentSize)(_adapter *, uint8_t, BOOLEAN);
	int (*Efuse_PgPacketWrite)(_adapter *, uint8_t, uint8_t, uint8_t *,
			BOOLEAN);
	uint8_t (*Efuse_WordEnableDataWrite)(_adapter *, uint16_t, uint8_t,
			uint8_t *, BOOLEAN);
	void (*recv_tasklet)(void *);
	int32_t (*fill_h2c_cmd)(_adapter *, uint8_t, uint32_t, uint8_t *);
	void (*fill_fake_txdesc)(_adapter *, uint8_t *, uint32_t, uint8_t, uint8_t,
			uint8_t);
	uint8_t (*hal_get_tx_buff_rsvd_page_num)(_adapter *, bool);
};

struct _atr_aligned4_ _ADAPTER {
	uint16_t HardwareType;
	uint16_t interface_type;	//+2
	uint32_t work_mode;			//+4
	struct dvobj_priv *dvobj;	//+8
	struct mlme_priv mlmepriv; //+12 [1244]
	struct mlme_ext_priv mlmeextpriv; //+1256 [912]
	struct cmd_priv cmdpriv; //+2168
	struct evt_priv evtpriv; //+
	struct io_priv iopriv;
	struct xmit_priv xmitpriv; //+2248
	struct recv_priv recvpriv; //+2752
	struct sta_priv stapriv; //+3024 [164]
	struct security_priv securitypriv;
	struct registry_priv registrypriv;
	struct pwrctrl_priv pwrctrlpriv; // pwrctrlpriv.bInternalAutoSuspend //+5061
	struct eeprom_priv eeprompriv;
	PVOID HalData;
	uint32_t hal_data_sz;
	struct hal_ops HalFunc;
	int32_t bDriverStopped;		//+5880
	int32_t bSurpriseRemoved;	//+5884
	int32_t bCardDisableWOHSM;	//+5888
	uint8_t RxStop;				//+5892
	uint32_t IsrContent;
	uint32_t ImrContent;
	uint8_t EepromAddressSize;
	uint8_t hw_init_completed;	//+5905
	uint8_t bDriverIsGoingToUnload;
	uint8_t init_adpt_in_progress;
	uint8_t bMpDriver;
	uint8_t bForwardingDisabled;
	struct task_struct isrThread;	//+5888
	struct task_struct cmdThread;   //+5920
	struct task_struct recvtasklet_thread; //+5952
	struct task_struct xmittasklet_thread; //+5984
	void (*intf_start)(_adapter *); //+6008
	void (*intf_stop)(_adapter *); //+6012
	_nic_hdl pnetdev;	//+6016
	int bup;			//+6020
	struct net_device_stats stats;
	uint8_t net_closed; //+6052
	uint8_t bFWReady;
	uint8_t bLinkInfoDump;
	uint8_t bRxRSSIDisplay;
	_adapter *pbuddy_adapter; //+6056
	_mutex *hw_init_mutex;	//+6060
	uint8_t isprimary;		//+6064
	uint8_t adapter_type; 	//+6065
	uint8_t iface_type;		//+6056
	_mutex *ph2c_fwcmd_mutex; //+6068
	_mutex *psetch_mutex;	//+6072
	_mutex *psetbw_mutex;	//+6076
	struct co_data_priv *pcodatapriv; //+6080
	uint8_t fix_rate; //+6084
}; // [6088] (!)
typedef struct _ADAPTER *PADAPTER;
// if sizeof(struct _ADAPTER) != 6088 #error "Check aligned struct!" !

enum tag_HAL_IC_Type_Definition // : sint32_t
{
	CHIP_8192S = 0x0,
	CHIP_8188C = 0x1,
	CHIP_8192C = 0x2,
	CHIP_8192D = 0x3,
	CHIP_8723A = 0x4,
	CHIP_8188E = 0x5,
	CHIP_8812 = 0x6,
	CHIP_8821 = 0x7,
	CHIP_8723B = 0x8,
	CHIP_8192E = 0x9,
	CHIP_8195A = 0xA,
	CHIP_8711B = 0xB,
	CHIP_8188F = 0xC,
};
typedef enum tag_HAL_IC_Type_Definition HAL_IC_TYPE_E;

enum tag_HAL_CHIP_Type_Definition // : sint32_t
{
	TEST_CHIP = 0x0, NORMAL_CHIP = 0x1, FPGA = 0x2,
};
typedef enum tag_HAL_CHIP_Type_Definition HAL_CHIP_TYPE_E;

enum tag_HAL_Cut_Version_Definition // : sint32_t
{
	A_CUT_VERSION = 0x0,
	B_CUT_VERSION = 0x1,
	C_CUT_VERSION = 0x2,
	D_CUT_VERSION = 0x3,
	E_CUT_VERSION = 0x4,
	F_CUT_VERSION = 0x5,
	G_CUT_VERSION = 0x6,
	H_CUT_VERSION = 0x7,
	I_CUT_VERSION = 0x8,
	J_CUT_VERSION = 0x9,
	K_CUT_VERSION = 0xA,
};
typedef enum tag_HAL_Cut_Version_Definition HAL_CUT_VERSION_E;

enum tag_HAL_Manufacturer_Version_Definition //: sint32_t
{
	CHIP_VENDOR_TSMC = 0x0, CHIP_VENDOR_UMC = 0x1, CHIP_VENDOR_SMIC = 0x2,
};
typedef enum tag_HAL_Manufacturer_Version_Definition HAL_VENDOR_E;

enum tag_HAL_RF_Type_Definition //: sint32_t
{
	RF_TYPE_1T1R = 0x0,
	RF_TYPE_1T2R = 0x1,
	RF_TYPE_2T2R = 0x2,
	RF_TYPE_2T3R = 0x3,
	RF_TYPE_2T4R = 0x4,
	RF_TYPE_3T3R = 0x5,
	RF_TYPE_3T4R = 0x6,
	RF_TYPE_4T4R = 0x7,
};
typedef enum tag_HAL_RF_Type_Definition HAL_RF_TYPE_E;

struct _atr_aligned4_ tag_HAL_VERSION {
	HAL_IC_TYPE_E ICType;
	HAL_CHIP_TYPE_E ChipType;
	HAL_CUT_VERSION_E CUTVersion;
	HAL_VENDOR_E VendorType;
	HAL_RF_TYPE_E RFType;
	uint8_t ROMVer;
};
typedef struct tag_HAL_VERSION HAL_VERSION;

enum _HW_VARIABLES //: sint32_t
{
	HW_VAR_MEDIA_STATUS = 0x0,
	HW_VAR_MEDIA_STATUS1 = 0x1,
	HW_VAR_SET_OPMODE = 0x2,
	HW_VAR_MAC_ADDR = 0x3,
	HW_VAR_BSSID = 0x4,
	HW_VAR_INIT_RTS_RATE = 0x5,
	HW_VAR_BASIC_RATE = 0x6,
	HW_VAR_TXPAUSE = 0x7,
	HW_VAR_BCN_FUNC = 0x8,
	HW_VAR_CORRECT_TSF = 0x9,
	HW_VAR_CHECK_BSSID = 0xA,
	HW_VAR_MLME_DISCONNECT = 0xB,
	HW_VAR_MLME_SITESURVEY = 0xC,
	HW_VAR_MLME_JOIN = 0xD,
	HW_VAR_ON_RCR_AM = 0xE,
	HW_VAR_OFF_RCR_AM = 0xF,
	HW_VAR_BEACON_INTERVAL = 0x10,
	HW_VAR_SLOT_TIME = 0x11,
	HW_VAR_RESP_SIFS = 0x12,
	HW_VAR_ACK_PREAMBLE = 0x13,
	HW_VAR_SEC_CFG = 0x14,
	HW_VAR_SEC_DK_CFG = 0x15,
	HW_VAR_RF_TYPE = 0x16,
	HW_VAR_DM_FLAG = 0x17,
	HW_VAR_DM_FUNC_OP = 0x18,
	HW_VAR_DM_FUNC_SET = 0x19,
	HW_VAR_DM_FUNC_CLR = 0x1A,
	HW_VAR_CAM_EMPTY_ENTRY = 0x1B,
	HW_VAR_CAM_INVALID_ALL = 0x1C,
	HW_VAR_CAM_WRITE = 0x1D,
	HW_VAR_CAM_READ = 0x1E,
	HW_VAR_AC_PARAM_VO = 0x1F,
	HW_VAR_AC_PARAM_VI = 0x20,
	HW_VAR_AC_PARAM_BE = 0x21,
	HW_VAR_AC_PARAM_BK = 0x22,
	HW_VAR_ACM_CTRL = 0x23,
	HW_VAR_AMPDU_MIN_SPACE = 0x24,
	HW_VAR_AMPDU_FACTOR = 0x25,
	HW_VAR_RXDMA_AGG_PG_TH = 0x26,
	HW_VAR_SET_RPWM = 0x27,
	HW_VAR_GET_RPWM = 0x28,
	HW_VAR_CPWM = 0x29,
	HW_VAR_H2C_FW_PWRMODE = 0x2A,
	HW_VAR_H2C_PS_TUNE_PARAM = 0x2B,
	HW_VAR_H2C_FW_JOINBSSRPT = 0x2C,
	HW_VAR_FWLPS_RF_ON = 0x2D,
	HW_VAR_H2C_FW_P2P_PS_OFFLOAD = 0x2E,
	HW_VAR_TDLS_WRCR = 0x2F,
	HW_VAR_TDLS_INIT_CH_SEN = 0x30,
	HW_VAR_TDLS_RS_RCR = 0x31,
	HW_VAR_TDLS_DONE_CH_SEN = 0x32,
	HW_VAR_INITIAL_GAIN = 0x33,
	HW_VAR_TRIGGER_GPIO_0 = 0x34,
	HW_VAR_CURRENT_ANTENNA = 0x35,
	HW_VAR_ANTENNA_DIVERSITY_LINK = 0x36,
	HW_VAR_ANTENNA_DIVERSITY_SELECT = 0x37,
	HW_VAR_SWITCH_EPHY_WoWLAN = 0x38,
	HW_VAR_EFUSE_USAGE = 0x39,
	HW_VAR_EFUSE_BYTES = 0x3A,
	HW_VAR_FIFO_CLEARN_UP = 0x3B,
	HW_VAR_RESTORE_HW_SEQ = 0x3C,
	HW_VAR_HCI_SUS_STATE = 0x3D,
	HW_VAR_CHECK_TXBUF = 0x3E,
	HW_VAR_APFM_ON_MAC = 0x3F,
	HW_VAR_NAV_UPPER = 0x40,
	HW_VAR_C2H_HANDLE = 0x41,
	HW_VAR_RPT_TIMER_SETTING = 0x42,
	HW_VAR_TX_RPT_MAX_MACID = 0x43,
	HW_VAR_H2C_MEDIA_STATUS_RPT = 0x44,
	HW_VAR_CHK_HI_QUEUE_EMPTY = 0x45,
	HW_VAR_DL_BCN_SEL = 0x46,
	HW_VAR_PORT_SWITCH = 0x47,
	HW_VAR_DM_IN_LPS = 0x48,
	HW_VAR_SET_REQ_FW_PS = 0x49,
	HW_VAR_FW_PS_STATE = 0x4A,
	HW_VAR_DL_RSVD_PAGE = 0x4B,
	HW_VAR_MACID_SLEEP = 0x4C,
	HW_VAR_MACID_WAKEUP = 0x4D,
	HW_VAR_DUMP_MAC_QUEUE_INFO = 0x4E,
	HW_VAR_ASIX_IOT = 0x4F,
	HW_VAR_PROMISC = 0x50,
};

enum _BAND_TYPE // : sint32_t
{
	BAND_ON_2_4G = 0x0, BAND_ON_5G = 0x1, BAND_ON_BOTH = 0x2, BANDMAX = 0x3,
};
typedef enum _BAND_TYPE BAND_TYPE;

struct _BB_REGISTER_DEFINITION {
	uint32_t rfintfs;
	uint32_t rfintfo;
	uint32_t rfintfe;
	uint32_t rf3wireOffset;
	uint32_t rfHSSIPara2;
	uint32_t rfLSSIReadBack;
	uint32_t rfLSSIReadBackPi;
};
typedef struct _BB_REGISTER_DEFINITION BB_REGISTER_DEFINITION_T;

enum dot11AuthAlgrthmNum //: sint32_t
{
	dot11AuthAlgrthm_Open = 0x0,
	dot11AuthAlgrthm_Shared = 0x1,
	dot11AuthAlgrthm_8021X = 0x2,
	dot11AuthAlgrthm_Auto = 0x3,
	dot11AuthAlgrthm_WAPI = 0x4,
	dot11AuthAlgrthm_MaxNum = 0x5,
};

enum _RT_CHANNEL_DOMAIN //: sint32_t
{
	RT_CHANNEL_DOMAIN_FCC = 0x0,
	RT_CHANNEL_DOMAIN_IC = 0x1,
	RT_CHANNEL_DOMAIN_ETSI = 0x2,
	RT_CHANNEL_DOMAIN_SPAIN = 0x3,
	RT_CHANNEL_DOMAIN_FRANCE = 0x4,
	RT_CHANNEL_DOMAIN_MKK = 0x5,
	RT_CHANNEL_DOMAIN_MKK1 = 0x6,
	RT_CHANNEL_DOMAIN_ISRAEL = 0x7,
	RT_CHANNEL_DOMAIN_TELEC = 0x8,
	RT_CHANNEL_DOMAIN_GLOBAL_DOAMIN = 0x9,
	RT_CHANNEL_DOMAIN_WORLD_WIDE_13 = 0xA,
	RT_CHANNEL_DOMAIN_TAIWAN = 0xB,
	RT_CHANNEL_DOMAIN_CHINA = 0xC,
	RT_CHANNEL_DOMAIN_SINGAPORE_INDIA_MEXICO = 0xD,
	RT_CHANNEL_DOMAIN_KOREA = 0xE,
	RT_CHANNEL_DOMAIN_TURKEY = 0xF,
	RT_CHANNEL_DOMAIN_JAPAN = 0x10,
	RT_CHANNEL_DOMAIN_FCC_NO_DFS = 0x11,
	RT_CHANNEL_DOMAIN_JAPAN_NO_DFS = 0x12,
	RT_CHANNEL_DOMAIN_WORLD_WIDE_5G = 0x13,
	RT_CHANNEL_DOMAIN_TAIWAN_NO_DFS = 0x14,
	RT_CHANNEL_DOMAIN_WORLD_NULL = 0x20,
	RT_CHANNEL_DOMAIN_ETSI1_NULL = 0x21,
	RT_CHANNEL_DOMAIN_FCC1_NULL = 0x22,
	RT_CHANNEL_DOMAIN_MKK1_NULL = 0x23,
	RT_CHANNEL_DOMAIN_ETSI2_NULL = 0x24,
	RT_CHANNEL_DOMAIN_FCC1_FCC1 = 0x25,
	RT_CHANNEL_DOMAIN_WORLD_ETSI1 = 0x26,
	RT_CHANNEL_DOMAIN_MKK1_MKK1 = 0x27,
	RT_CHANNEL_DOMAIN_WORLD_KCC1 = 0x28,
	RT_CHANNEL_DOMAIN_WORLD_FCC2 = 0x29,
	RT_CHANNEL_DOMAIN_FCC2_NULL = 0x2A,
	RT_CHANNEL_DOMAIN_WORLD_FCC3 = 0x30,
	RT_CHANNEL_DOMAIN_WORLD_FCC4 = 0x31,
	RT_CHANNEL_DOMAIN_WORLD_FCC5 = 0x32,
	RT_CHANNEL_DOMAIN_WORLD_FCC6 = 0x33,
	RT_CHANNEL_DOMAIN_FCC1_FCC7 = 0x34,
	RT_CHANNEL_DOMAIN_WORLD_ETSI2 = 0x35,
	RT_CHANNEL_DOMAIN_WORLD_ETSI3 = 0x36,
	RT_CHANNEL_DOMAIN_MKK1_MKK2 = 0x37,
	RT_CHANNEL_DOMAIN_MKK1_MKK3 = 0x38,
	RT_CHANNEL_DOMAIN_FCC1_NCC1 = 0x39,
	RT_CHANNEL_DOMAIN_FCC1_NCC2 = 0x40,
	RT_CHANNEL_DOMAIN_GLOBAL_NULL = 0x41,
	RT_CHANNEL_DOMAIN_ETSI1_ETSI4 = 0x42,
	RT_CHANNEL_DOMAIN_FCC1_FCC2 = 0x43,
	RT_CHANNEL_DOMAIN_FCC1_NCC3 = 0x44,
	RT_CHANNEL_DOMAIN_WORLD_ETSI5 = 0x45,
	RT_CHANNEL_DOMAIN_FCC1_FCC8 = 0x46,
	RT_CHANNEL_DOMAIN_WORLD_ETSI6 = 0x47,
	RT_CHANNEL_DOMAIN_WORLD_ETSI7 = 0x48,
	RT_CHANNEL_DOMAIN_WORLD_ETSI8 = 0x49,
	RT_CHANNEL_DOMAIN_WORLD_ETSI9 = 0x50,
	RT_CHANNEL_DOMAIN_WORLD_ETSI10 = 0x51,
	RT_CHANNEL_DOMAIN_WORLD_ETSI11 = 0x52,
	RT_CHANNEL_DOMAIN_FCC1_NCC4 = 0x53,
	RT_CHANNEL_DOMAIN_WORLD_ETSI12 = 0x54,
	RT_CHANNEL_DOMAIN_FCC1_FCC9 = 0x55,
	RT_CHANNEL_DOMAIN_WORLD_ETSI13 = 0x56,
	RT_CHANNEL_DOMAIN_FCC1_FCC10 = 0x57,
	RT_CHANNEL_DOMAIN_MKK2_MKK4 = 0x58,
	RT_CHANNEL_DOMAIN_MAX = 0x59,
	RT_CHANNEL_DOMAIN_REALTEK_DEFINE = 0x7F,
};

struct _driver_priv {
	int drv_registered;
	_mutex hw_init_mutex;
	_mutex h2c_fwcmd_mutex;
	_mutex setch_mutex;
	_mutex setbw_mutex;
};
typedef struct _driver_priv drv_priv;

struct _ADAPTIVITY_STATISTICS {
	s1Byte TH_L2H_ini_mode2;
	s1Byte TH_EDCCA_HL_diff_mode2;
	s1Byte TH_EDCCA_HL_diff_backup;
	s1Byte IGI_Base;
	u1Byte IGI_target;
	u1Byte NHMWait;
	s1Byte H2L_lb;
	s1Byte L2H_lb;
	BOOLEAN bFirstLink;
	BOOLEAN bCheck;
	BOOLEAN DynamicLinkAdaptivity;
	u1Byte APNumTH;
	u1Byte AdajustIGILevel;
};
typedef struct _ADAPTIVITY_STATISTICS ADAPTIVITY_STATISTICS;

struct _ODM_NOISE_MONITOR_ {
	s1Byte noise[1];
	s2Byte noise_all;
};
typedef struct _ODM_NOISE_MONITOR_ ODM_NOISE_MONITOR;
/* in rtl_bios_data.h
struct _FALSE_ALARM_STATISTICS {
	u4Byte Cnt_Parity_Fail;
	u4Byte Cnt_Rate_Illegal;
	u4Byte Cnt_Crc8_fail;
	u4Byte Cnt_Mcs_fail;
	u4Byte Cnt_Ofdm_fail;
	u4Byte Cnt_Ofdm_fail_pre;
	u4Byte Cnt_Cck_fail;
	u4Byte Cnt_all;
	u4Byte Cnt_Fast_Fsync;
	u4Byte Cnt_SB_Search_fail;
	u4Byte Cnt_OFDM_CCA;
	u4Byte Cnt_CCK_CCA;
	u4Byte Cnt_CCA_all;
	u4Byte Cnt_BW_USC;
	u4Byte Cnt_BW_LSC;
};
typedef struct _FALSE_ALARM_STATISTICS FALSE_ALARM_STATISTICS;
*/

enum _BASEBAND_CONFIG_PHY_REG_PG_VALUE_TYPE //: sint32_t
{
	PHY_REG_PG_RELATIVE_VALUE = 0x0, PHY_REG_PG_EXACT_VALUE = 0x1,
};
typedef enum _BASEBAND_CONFIG_PHY_REG_PG_VALUE_TYPE PHY_REG_PG_TYPE;

/* in rtl_bios_data.h
struct _atr_aligned4_ _CFO_TRACKING_ {
	BOOLEAN bATCStatus;
	BOOLEAN largeCFOHit;
	BOOLEAN bAdjust;
	u1Byte CrystalCap;
	u1Byte DefXCap;
	int CFO_tail[2];
	int CFO_ave_pre;
	u4Byte packetCount;
	u4Byte packetCount_pre;
	BOOLEAN bForceXtalCap;
	BOOLEAN bReset;
	u1Byte CFO_TH_XTAL_HIGH;
	u1Byte CFO_TH_XTAL_LOW;
	u1Byte CFO_TH_ATC;
};
typedef struct _CFO_TRACKING_ CFO_TRACKING;
*/
/* in rtl_bios_data.h
struct _atr_aligned8_ _ROM_INFO {
	u1Byte EEPROMVersion;
	u1Byte CrystalCap;
	u8Byte DebugComponents;
	u4Byte DebugLevel;
};
typedef struct _ROM_INFO ROM_INFO;
*/

typedef struct _ROM_INFO *PROM_INFO;

typedef struct sta_info *PSTA_INFO_T;

struct _ODM_Phy_Dbg_Info_ {
	s1Byte RxSNRdB[4];
	u4Byte NumQryPhyStatus;
	u4Byte NumQryPhyStatusCCK;
	u4Byte NumQryPhyStatusOFDM;
	u1Byte NumQryBeaconPkt;
	s4Byte RxEVM[4];
};
typedef struct _ODM_Phy_Dbg_Info_ ODM_PHY_DBG_INFO_T;

struct _ODM_Mac_Status_Info_ {
	u1Byte test;
};
typedef struct _ODM_Mac_Status_Info_ ODM_MAC_INFO;

struct _atr_aligned4_ _ODM_RA_Info_ {
	u1Byte RateID;
	u4Byte RateMask;
	u4Byte RAUseRate;
	u1Byte RateSGI;
	u1Byte RssiStaRA;
	u1Byte PreRssiStaRA;
	u1Byte SGIEnable;
	u1Byte DecisionRate;
	u1Byte PreRate;
	u1Byte HighestRate;
	u1Byte LowestRate;
	u4Byte NscUp;
	u4Byte NscDown;
	u2Byte RTY[5];
	u4Byte TOTAL;
	u2Byte DROP;
	u1Byte Active;
	u2Byte RptTime;
	u1Byte RAWaitingCounter;
	u1Byte RAPendingCounter;
	u1Byte RAINFO;
	u1Byte Initial_BW;
	u1Byte BW_setting;
	u1Byte DISPT;
	u1Byte DISRA;
	u1Byte Stage_RA;
	u1Byte PRE_BW;
	u1Byte MacID;
	u1Byte Try_state;
	u1Byte Try_done_cnt;
	u2Byte RA_counter;
	u1Byte Init_Rate_H;
	u1Byte Init_Rate_M;
	u1Byte Init_Rate_L;
	u4Byte Total_TX;
	u1Byte TRAINING_RATE;
	u1Byte STOP_PT_COUNTER;
	u1Byte MODE_SS;
	u1Byte PT_smooth_factor;
	u1Byte PTActive;
	u1Byte PTTryState;
	u1Byte PTStage;
	u1Byte PTStopCount;
	u1Byte PTPreRate;
	u1Byte PTPreRssi;
	u1Byte PTModeSS;
	u1Byte RAstage;
	u1Byte PTSmoothFactor;
};
typedef struct _ODM_RA_Info_ ODM_RA_INFO_T;
typedef struct _ODM_RA_Info_ *PODM_RA_INFO_T;

struct _FAST_ANTENNA_TRAINNING_ {
	u1Byte Bssid[6];
	u1Byte antsel_rx_keep_0;
	u1Byte antsel_rx_keep_1;
	u1Byte antsel_rx_keep_2;
	u4Byte antSumRSSI[7];
	u4Byte antRSSIcnt[7];
	u4Byte antAveRSSI[7];
	u1Byte FAT_State;
	u4Byte TrainIdx;
	u1Byte antsel_a[7];
	u1Byte antsel_b[7];
	u1Byte antsel_c[7];
	u4Byte MainAnt_Sum[7];
	u4Byte AuxAnt_Sum[7];
	u4Byte MainAnt_Cnt[7];
	u4Byte AuxAnt_Cnt[7];
	u4Byte MainAnt_Sum_CCK[7];
	u4Byte AuxAnt_Sum_CCK[7];
	u4Byte MainAnt_Cnt_CCK[7];
	u4Byte AuxAnt_Cnt_CCK[7];
	u1Byte RxIdleAnt;
	BOOLEAN bBecomeLinked;
	u4Byte MinMaxRSSI;
	u1Byte idx_AntDiv_counter_2G;
	u1Byte idx_AntDiv_counter_5G;
	u4Byte AntDiv_2G_5G;
	u4Byte CCK_counter_main;
	u4Byte CCK_counter_aux;
	u4Byte OFDM_counter_main;
	u4Byte OFDM_counter_aux;
};
typedef struct _FAST_ANTENNA_TRAINNING_ FAT_T;

struct _Dynamic_Initial_Gain_Threshold_ {
	BOOLEAN bStopDIG;
	BOOLEAN bPauseDIG;
	BOOLEAN bIgnoreDIG;
	BOOLEAN bPSDInProgress;
	u1Byte Dig_Enable_Flag;
	u1Byte Dig_Ext_Port_Stage;
	int RssiLowThresh;
	int RssiHighThresh;
	u4Byte FALowThresh;
	u4Byte FAHighThresh;
	u1Byte CurSTAConnectState;
	u1Byte PreSTAConnectState;
	u1Byte CurMultiSTAConnectState;
	u1Byte PreIGValue;
	u1Byte CurIGValue;
	u1Byte BackupIGValue;
	u1Byte BT30_CurIGI;
	u1Byte IGIBackup;
	s1Byte BackoffVal;
	s1Byte BackoffVal_range_max;
	s1Byte BackoffVal_range_min;
	u1Byte rx_gain_range_max;
	u1Byte rx_gain_range_min;
	u1Byte Rssi_val_min;
	u1Byte PreCCK_CCAThres;
	u1Byte CurCCK_CCAThres;
	u1Byte PreCCKPDState;
	u1Byte CurCCKPDState;
	u1Byte CCKPDBackup;
	u1Byte LargeFAHit;
	u1Byte ForbiddenIGI;
	u4Byte Recover_cnt;
	u1Byte DIG_Dynamic_MIN_0;
	u1Byte DIG_Dynamic_MIN_1;
	BOOLEAN bMediaConnect_0;
	BOOLEAN bMediaConnect_1;
	u4Byte AntDiv_RSSI_max;
	u4Byte RSSI_max;
	u1Byte *pbP2pLinkInProgress;
};
typedef struct _Dynamic_Initial_Gain_Threshold_ DIG_T;

struct _ODM_RATE_ADAPTIVE {
	u1Byte Type;
	u1Byte HighRSSIThresh;
	u1Byte LowRSSIThresh;
	u1Byte RATRState;
	u1Byte LdpcThres;
	BOOLEAN bLowerRtsRate;
	BOOLEAN bUseLdpc;
};
typedef struct _ODM_RATE_ADAPTIVE ODM_RATE_ADAPTIVE;

struct _Dynamic_Power_Saving_ {
	u1Byte PreCCAState;
	u1Byte CurCCAState;
	u1Byte PreRFState;
	u1Byte CurRFState;
	int Rssi_val_min;
	u1Byte initialize;
	u4Byte Reg874;
	u4Byte RegC70;
	u4Byte Reg85C;
	u4Byte RegA74;
};
typedef struct _Dynamic_Power_Saving_ PS_T;

struct _Dynamic_Primary_CCA {
	u1Byte PriCCA_flag;
	u1Byte intf_flag;
	u1Byte intf_type;
	u1Byte DupRTS_flag;
	u1Byte Monitor_flag;
	u1Byte CH_offset;
	u1Byte MF_state;
};
typedef struct _Dynamic_Primary_CCA Pri_CCA_T;

struct _RX_High_Power_ {
	u1Byte RXHP_flag;
	u1Byte PSD_func_trigger;
	u1Byte PSD_bitmap_RXHP[80];
	u1Byte Pre_IGI;
	u1Byte Cur_IGI;
	u1Byte Pre_pw_th;
	u1Byte Cur_pw_th;
	BOOLEAN First_time_enter;
	BOOLEAN RXHP_enable;
	u1Byte TP_Mode;
};
typedef struct _RX_High_Power_ RXHP_T;

struct _Rate_Adaptive_Table_ {
	u1Byte firstconnect;
};
typedef struct _Rate_Adaptive_Table_ RA_T;

struct _atr_aligned8_ _SW_Antenna_Switch_ {
	u1Byte Double_chk_flag;
	u1Byte try_flag;
	s4Byte PreRSSI;
	u1Byte CurAntenna;
	u1Byte PreAntenna;
	u1Byte RSSI_Trying;
	u1Byte TestMode;
	u1Byte bTriggerAntennaSwitch;
	u1Byte SelectAntennaMap;
	u1Byte RSSI_target;
	u1Byte reset_idx;
	u1Byte SWAS_NoLink_State;
	u4Byte SWAS_NoLink_BK_Reg860;
	u4Byte SWAS_NoLink_BK_Reg92c;
	BOOLEAN ANTA_ON;
	BOOLEAN ANTB_ON;
	u1Byte Ant5G;
	u1Byte Ant2G;
	s4Byte RSSI_sum_A;
	s4Byte RSSI_sum_B;
	s4Byte RSSI_cnt_A;
	s4Byte RSSI_cnt_B;
	u8Byte lastTxOkCnt;
	u8Byte lastRxOkCnt;
	u8Byte TXByteCnt_A;
	u8Byte TXByteCnt_B;
	u8Byte RXByteCnt_A;
	u8Byte RXByteCnt_B;
	u1Byte TrafficLoad;
	u1Byte Train_time;
	u1Byte Train_time_flag;
};
typedef struct _SW_Antenna_Switch_ SWAT_T;

struct _EDCA_TURBO_ {
	BOOLEAN bCurrentTurboEDCA;
	BOOLEAN bIsCurRDLState;
};
typedef struct _EDCA_TURBO_ EDCA_T;

struct _ANT_DETECTED_INFO {
	BOOLEAN bAntDetected;
	u4Byte dBForAntA;
	u4Byte dBForAntB;
	u4Byte dBForAntO;
};
typedef struct _ANT_DETECTED_INFO ANT_DETECTED_INFO;

struct _IQK_MATRIX_REGS_SETTING {
	BOOLEAN bIQKDone;
	s4Byte Value[1][8];
};
typedef struct _IQK_MATRIX_REGS_SETTING IQK_MATRIX_REGS_SETTING;

struct _atr_aligned8_ ODM_RF_Calibration_Structure {
	u4Byte RegA24;
	s4Byte RegE94;
	s4Byte RegE9C;
	s4Byte RegEB4;
	s4Byte RegEBC;
	u1Byte TXPowercount;
	BOOLEAN bTXPowerTrackingInit;
	BOOLEAN bTXPowerTracking;
	u1Byte TxPowerTrackControl;
	u1Byte TM_Trigger;
	u1Byte InternalPA5G[2];
	u1Byte ThermalMeter[2];
	u1Byte ThermalValue;
	u1Byte ThermalValue_LCK;
	u1Byte ThermalValue_IQK;
	u1Byte ThermalValue_DPK;
	u1Byte ThermalValue_AVG[8];
	u1Byte ThermalValue_AVG_index;
	u1Byte ThermalValue_RxGain;
	u1Byte ThermalValue_Crystal;
	u1Byte ThermalValue_DPKstore;
	u1Byte ThermalValue_DPKtrack;
	BOOLEAN TxPowerTrackingInProgress;
	BOOLEAN bReloadtxpowerindex;
	u1Byte bRfPiEnable;
	u4Byte TXPowerTrackingCallbackCnt;
	u1Byte bCCKinCH14;
	u1Byte CCK_index[1];
	s1Byte PowerIndexOffset_CCK[1];
	s1Byte DeltaPowerIndex_CCK[1];
	s1Byte DeltaPowerIndexLast_CCK[1];
	u1Byte OFDM_index[1];
	s1Byte PowerIndexOffset_OFDM[1];
	s1Byte DeltaPowerIndex_OFDM[1];
	s1Byte DeltaPowerIndexLast_OFDM[1];
	BOOLEAN bTxPowerChanged;
	s1Byte XtalOffset;
	s1Byte XtalOffsetLast;
	u1Byte ThermalValue_HP[8];
	u1Byte ThermalValue_HP_index;
	IQK_MATRIX_REGS_SETTING IQKMatrixRegSetting[14];
	u1Byte Delta_LCK;
	s1Byte BBSwingDiff2G;
	s1Byte BBSwingDiff5G;
	s1Byte DeltaSwingTableIdx_2GCCKA_P[30];
	s1Byte DeltaSwingTableIdx_2GCCKA_N[30];
	s1Byte DeltaSwingTableIdx_2GA_P[30];
	s1Byte DeltaSwingTableIdx_2GA_N[30];
	s1Byte DeltaSwingTableXtal_P[30];
	s1Byte DeltaSwingTableXtal_N[30];
	u4Byte RegC04;
	u4Byte Reg874;
	u4Byte RegC08;
	u4Byte RegB68;
	u4Byte RegB6C;
	u4Byte Reg870;
	u4Byte Reg860;
	u4Byte Reg864;
	BOOLEAN bIQKInitialized;
	BOOLEAN bLCKInProgress;
	BOOLEAN bAntennaDetected;
	BOOLEAN bNeedIQK;
	BOOLEAN bIQKInProgress;
	u1Byte Delta_IQK;
	u4Byte ADDA_backup[16];
	u4Byte IQK_MAC_backup[4];
	u4Byte IQK_BB_backup_recover[9];
	u4Byte IQK_BB_backup[9];
	u4Byte TxIQC_8723B[2][3][2];
	u4Byte RxIQC_8723B[2][2][2];
	u8Byte IQK_StartTime;
	u8Byte IQK_ProgressingTime;
	u4Byte LOK_Result;
	u4Byte APKoutput[2][2];
	u1Byte bAPKdone;
	u1Byte bAPKThermalMeterIgnore;
	BOOLEAN bDPKFail;
	u1Byte bDPdone;
	u1Byte bDPPathAOK;
	u1Byte bDPPathBOK;
	u4Byte TxLOK[2];
	u4Byte DpkTxAGC;
	s4Byte DpkGain;
	u4Byte DpkThermal[4];
};
typedef struct ODM_RF_Calibration_Structure ODM_RF_CAL_T;

struct _atr_aligned8_ DM_Out_Source_Dynamic_Mechanism_Structure {
	PADAPTER Adapter;
	BOOLEAN odm_ready;
	PHY_REG_PG_TYPE PhyRegPgValueType;
	u1Byte PhyRegPgVersion;
	u4Byte NumQryPhyStatusAll;
	u4Byte LastNumQryPhyStatusAll;
	u4Byte RxPWDBAve;
	BOOLEAN MPDIG_2G;
	u1Byte Times_2G;
	BOOLEAN bCckHighPower;
	u1Byte RFPathRxEnable;
	u1Byte ControlChannel;
	u1Byte SupportPlatform;
	u4Byte SupportAbility;
	u1Byte SupportInterface;
	u4Byte SupportICType;
	u1Byte CutVersion;
	u1Byte FabVersion;
	u1Byte RFType;
	u1Byte RFEType;
	u1Byte BoardType;
	u1Byte PackageType;
	u1Byte TypeGLNA;
	u1Byte TypeGPA;
	u1Byte TypeALNA;
	u1Byte TypeAPA;
	u1Byte ExtLNA;
	u1Byte ExtLNA5G;
	u1Byte ExtPA;
	u1Byte ExtPA5G;
	u1Byte ExtTRSW;
	u1Byte PatchID;
	BOOLEAN bInHctTest;
	BOOLEAN bWIFITest;
	BOOLEAN bDualMacSmartConcurrent;
	u4Byte BK_SupportAbility;
	u1Byte AntDivType;
	u1Byte odm_Regulation2_4G;
	u1Byte odm_Regulation5G;
	u1Byte u1Byte_temp;
	PADAPTER PADAPTER_temp;
	u1Byte *pMacPhyMode;
	u8Byte *pNumTxBytesUnicast;
	u8Byte *pNumRxBytesUnicast;
	u1Byte *pWirelessMode;
	u1Byte *pBandType;
	u1Byte *pSecChOffset;
	u1Byte *pSecurity;
	u1Byte *pBandWidth;
	u1Byte *pChannel;
	BOOLEAN DPK_Done;
	BOOLEAN *pbGetValueFromOtherMac;
	PADAPTER *pBuddyAdapter;
	BOOLEAN *pbMasterOfDMSP;
	BOOLEAN *pbScanInProcess;
	BOOLEAN *pbPowerSaving;
	u1Byte *pOnePathCCA;
	u1Byte *pAntennaTest;
	BOOLEAN *pbNet_closed;
	u1Byte *mp_mode;
	u1Byte *pu1ForcedIgiLb;
	BOOLEAN *pIsFcsModeEnable;
	pu2Byte pForcedDataRate;
	BOOLEAN bLinkInProcess;
	BOOLEAN bWIFI_Direct;
	BOOLEAN bWIFI_Display;
	BOOLEAN bLinked;
	BOOLEAN bsta_state;
	u1Byte RSSI_Min;
	u1Byte InterfaceIndex;
	BOOLEAN bIsMPChip;
	BOOLEAN bOneEntryOnly;
	BOOLEAN bBtEnabled;
	BOOLEAN bBtConnectProcess;
	u1Byte btHsRssi;
	BOOLEAN bBtHsOperation;
	BOOLEAN bBtDisableEdcaTurbo;
	BOOLEAN bBtLimitedDig;
	u1Byte RSSI_A;
	u1Byte RSSI_B;
	u8Byte RSSI_TRSW;
	u8Byte RSSI_TRSW_H;
	u8Byte RSSI_TRSW_L;
	u8Byte RSSI_TRSW_iso;
	u1Byte RxRate;
	BOOLEAN bNoisyState;
	u1Byte TxRate;
	u1Byte LinkedInterval;
	u1Byte preChannel;
	u4Byte TxagcOffsetValueA;
	BOOLEAN IsTxagcOffsetPositiveA;
	u4Byte TxagcOffsetValueB;
	BOOLEAN IsTxagcOffsetPositiveB;
	u8Byte lastTxOkCnt;
	u8Byte lastRxOkCnt;
	u4Byte BbSwingOffsetA;
	BOOLEAN IsBbSwingOffsetPositiveA;
	u4Byte BbSwingOffsetB;
	BOOLEAN IsBbSwingOffsetPositiveB;
	u1Byte antdiv_rssi;
	u1Byte AntType;
	u1Byte pre_AntType;
	u1Byte antdiv_period;
	u1Byte antdiv_select;
	u1Byte NdpaPeriod;
	BOOLEAN H2C_RARpt_connect;
	u2Byte NHM_cnt_0;
	u2Byte NHM_cnt_1;
	s1Byte TH_L2H_ini;
	s1Byte TH_EDCCA_HL_diff;
	s1Byte TH_L2H_ini_backup;
	BOOLEAN Carrier_Sense_enable;
	u1Byte Adaptivity_IGI_upper;
	BOOLEAN adaptivity_flag;
	u1Byte DCbackoff;
	BOOLEAN Adaptivity_enable;
	u1Byte APTotalNum;
	ADAPTIVITY_STATISTICS Adaptivity;
	ODM_NOISE_MONITOR noise_level;
	PSTA_INFO_T pODM_StaInfo[7];
	u2Byte CurrminRptTime;
	ODM_RA_INFO_T RAInfo[7];
	BOOLEAN RaSupport88E;
	ODM_PHY_DBG_INFO_T PhyDbgInfo;
	ODM_MAC_INFO *pMacInfo;
	FAT_T DM_FatTable;
	DIG_T DM_DigTable;
	PS_T DM_PSTable;
	Pri_CCA_T DM_PriCCA;
	RXHP_T DM_RXHP_Table;
	RA_T DM_RA_Table;
	PROM_INFO pROMInfo;
	FALSE_ALARM_STATISTICS FalseAlmCnt;
	CFO_TRACKING DM_CfoTrack;
	FALSE_ALARM_STATISTICS FlaseAlmCntBuddyAdapter;
	SWAT_T DM_SWAT_Table;
	BOOLEAN RSSI_test;
	BOOLEAN bNoBeaconIn2s;
	EDCA_T DM_EDCA_Table;
	u4Byte WMMEDCA_BE;
	BOOLEAN *pbDriverStopped;
	BOOLEAN *pbDriverIsGoingToPnpSetPowerSleep;
	BOOLEAN *pinit_adpt_in_progress;
	u1Byte bUseRAMask;
	ODM_RATE_ADAPTIVE RateAdaptive;
	ANT_DETECTED_INFO AntDetectedInfo;
	ODM_RF_CAL_T RFCalibrateInfo;
	u1Byte BbSwingIdxOfdm[1];
	u1Byte BbSwingIdxOfdmCurrent;
	u1Byte BbSwingIdxOfdmBase[1];
	BOOLEAN BbSwingFlagOfdm;
	u1Byte BbSwingIdxCck[1];
	u1Byte BbSwingIdxCckCurrent;
	u1Byte BbSwingIdxCckBase[1];
	u1Byte DefaultOfdmIndex;
	u1Byte DefaultCckIndex;
	BOOLEAN BbSwingFlagCck;
	s1Byte Absolute_OFDMSwingIdx[1];
	s1Byte Absolute_CCKSwingIdx[1];
	s1Byte Remnant_OFDMSwingIdx[1];
	s1Byte Remnant_CCKSwingIdx[1];
	s1Byte Modify_TxAGC_Value;
	BOOLEAN Modify_TxAGC_Flag_PathA;
	BOOLEAN Modify_TxAGC_Flag_PathB;
	BOOLEAN Modify_TxAGC_Flag_PathA_CCK;
};
typedef struct DM_Out_Source_Dynamic_Mechanism_Structure DM_ODM_T;
typedef struct DM_Out_Source_Dynamic_Mechanism_Structure *PDM_ODM_T;

enum _PWRTRACK_CONTROL_METHOD //: sint32_t
{
	BBSWING = 0x0, TXAGC = 0x1, MIX_MODE = 0x2,
};
typedef enum _PWRTRACK_CONTROL_METHOD PWRTRACK_METHOD;

typedef void (*FuncSetPwr)(PDM_ODM_T, PWRTRACK_METHOD, u1Byte, u1Byte);
typedef void (*FuncIQK)(PDM_ODM_T, u1Byte, u1Byte, u1Byte);
typedef void (*FuncLCK)(PDM_ODM_T);
typedef void (*FuncSwing)(PDM_ODM_T, ps1Byte *, ps1Byte *, ps1Byte *, ps1Byte *);
typedef void (*FuncSwingXtal)(PDM_ODM_T, ps1Byte *, ps1Byte *);
typedef void (*FuncSetXtal)(PDM_ODM_T);

struct _TXPWRTRACK_CFG {
	u1Byte SwingTableSize_CCK;
	u1Byte SwingTableSize_OFDM;
	u1Byte Threshold_IQK;
	u1Byte AverageThermalNum;
	u1Byte RfPathCount;
	u4Byte ThermalRegAddr;
	FuncSetPwr ODM_TxPwrTrackSetPwr;
	FuncIQK DoIQK;
	FuncLCK PHY_LCCalibrate;
	FuncSwing GetDeltaSwingTable;
	FuncSwingXtal GetDeltaSwingXtalTable;
	FuncSetXtal ODM_TxXtalTrackSetXtal;
};
typedef struct _TXPWRTRACK_CFG *PTXPWRTRACK_CFG;

struct _RSVDPAGE_LOC {
	uint8_t LocProbeRsp;
	uint8_t LocPsPoll;
	uint8_t LocNullData;
	uint8_t LocQosNull;
	uint8_t LocBTQosNull;
};
typedef struct _RSVDPAGE_LOC RSVDPAGE_LOC_8195A;
typedef struct _RSVDPAGE_LOC *PRSVDPAGE_LOC_8195A;

enum _RT_MEDIA_STATUS //: sint32_t
{
	RT_MEDIA_DISCONNECT = 0x0, RT_MEDIA_CONNECT = 0x1,
};

struct _H2CParam_RsvdPage_ {
	RSVDPAGE_LOC_8195A RsvdPageLoc;
	uint8_t *ReservedPagePacket;
	uint32_t TotalPacketLen;
};
typedef struct _H2CParam_RsvdPage_ H2CParam_RsvdPage;
typedef struct _H2CParam_RsvdPage_ *PH2CParam_RsvdPage;

struct _NDIS_802_11_VARIABLE_IEs {
	uint8_t ElementID;
	uint8_t Length;
	uint8_t data[1];
};
typedef struct _NDIS_802_11_VARIABLE_IEs *PNDIS_802_11_VARIABLE_IEs;

enum _NDIS_802_11_AUTHENTICATION_MODE //: sint32_t
{
	Ndis802_11AuthModeOpen = 0x0,
	Ndis802_11AuthModeShared = 0x1,
	Ndis802_11AuthModeAutoSwitch = 0x2,
	Ndis802_11AuthModeWPA = 0x3,
	Ndis802_11AuthModeWPAPSK = 0x4,
	Ndis802_11AuthModeWPANone = 0x5,
	Ndis802_11AuthModeWAPI = 0x6,
	Ndis802_11AuthModeMax = 0x7,
};
typedef enum _NDIS_802_11_AUTHENTICATION_MODE NDIS_802_11_AUTHENTICATION_MODE;

enum _NDIS_802_11_WEP_STATUS //: sint32_t
{
	Ndis802_11WEPEnabled = 0x0,
	Ndis802_11Encryption1Enabled = 0x0,
	Ndis802_11WEPDisabled = 0x1,
	Ndis802_11EncryptionDisabled = 0x1,
	Ndis802_11WEPKeyAbsent = 0x2,
	Ndis802_11Encryption1KeyAbsent = 0x2,
	Ndis802_11WEPNotSupported = 0x3,
	Ndis802_11EncryptionNotSupported = 0x3,
	Ndis802_11Encryption2Enabled = 0x4,
	Ndis802_11Encryption2KeyAbsent = 0x5,
	Ndis802_11Encryption3Enabled = 0x6,
	Ndis802_11Encryption3KeyAbsent = 0x7,
	Ndis802_11_EncrypteionWAPI = 0x8,
};
typedef enum _NDIS_802_11_WEP_STATUS NDIS_802_11_WEP_STATUS;

struct __attribute__((packed)) __attribute__((aligned(1))) rtk_sc {
	u8 pattern_type;
	u8 smac[6];
	u8 bssid[2][6];
	u8 ssid[32];
	u8 password[64];
	u32 ip_addr;
	u8 sync_pkt[9][6];
	u8 profile_pkt[256][6];
	u32 profile_pkt_len;
	u8 plain_buf[256];
	u32 plain_len;
	u8 key_buf[32];
	u32 key_len;
	u8 crypt_buf[256];
	u32 crypt_len;
	s32 pattern_index;
	struct pattern_ops *pattern[5];
	u8 max_pattern_num;
	u8 pin[65];
	u8 default_pin[65];
	u8 have_pin;
	u16 device_type;
	u8 device_name[64];
	u8 bcast_crypt_buf[256];
};

struct pattern_ops;

typedef s32 (*sc_check_pattern_call_back)(struct pattern_ops *, struct rtk_sc *);
typedef s32 (*sc_get_cipher_info_call_back)(struct pattern_ops *, struct rtk_sc *);
typedef s32 (*sc_generate_key_call_back)(struct pattern_ops *, struct rtk_sc *);
typedef s32 (*sc_decode_profile_call_back)(struct pattern_ops *, struct rtk_sc *);
typedef s32 (*sc_get_tlv_info_call_back)(struct pattern_ops *, struct rtk_sc *);

struct pattern_ops {
	u32 index;
	u32 flag;
	u8 name[32];
	sc_check_pattern_call_back check_pattern;
	sc_get_cipher_info_call_back get_cipher_info;
	sc_generate_key_call_back generate_key;
	sc_decode_profile_call_back decode_profile;
	sc_get_tlv_info_call_back get_tlv_info;
};

struct _atr_aligned2_ _WL_PWR_CFG_ { // __attribute__((packed))!?
	uint16_t offset;
	uint8_t cut_msk;
	u8 fab_msk :4;
	u8 interface_msk :4;
	u8 base :4;
	u8 cmd :4;
	uint8_t msk;
	uint8_t value;
};
typedef struct _WL_PWR_CFG_ WLAN_PWR_CFG;

struct cmd_hdl {
	uint32_t parmsize;
	uint8_t (*h2cfuns)(struct _ADAPTER *, uint8_t *);
};

struct _cmd_callback {
	uint32_t cmd_code;
	void (*callback)(_adapter *, struct cmd_obj *);
};

enum _ODM_Common_Info_Definition //: sint32_t
{
	ODM_CMNINFO_PLATFORM = 0x0,
	ODM_CMNINFO_ABILITY = 0x1,
	ODM_CMNINFO_INTERFACE = 0x2,
	ODM_CMNINFO_MP_TEST_CHIP = 0x3,
	ODM_CMNINFO_IC_TYPE = 0x4,
	ODM_CMNINFO_CUT_VER = 0x5,
	ODM_CMNINFO_FAB_VER = 0x6,
	ODM_CMNINFO_RF_TYPE = 0x7,
	ODM_CMNINFO_RFE_TYPE = 0x8,
	ODM_CMNINFO_BOARD_TYPE = 0x9,
	ODM_CMNINFO_PACKAGE_TYPE = 0xA,
	ODM_CMNINFO_EXT_LNA = 0xB,
	ODM_CMNINFO_5G_EXT_LNA = 0xC,
	ODM_CMNINFO_EXT_PA = 0xD,
	ODM_CMNINFO_5G_EXT_PA = 0xE,
	ODM_CMNINFO_GPA = 0xF,
	ODM_CMNINFO_APA = 0x10,
	ODM_CMNINFO_GLNA = 0x11,
	ODM_CMNINFO_ALNA = 0x12,
	ODM_CMNINFO_EXT_TRSW = 0x13,
	ODM_CMNINFO_PATCH_ID = 0x14,
	ODM_CMNINFO_BINHCT_TEST = 0x15,
	ODM_CMNINFO_BWIFI_TEST = 0x16,
	ODM_CMNINFO_SMART_CONCURRENT = 0x17,
	ODM_CMNINFO_DOMAIN_CODE_2G = 0x18,
	ODM_CMNINFO_DOMAIN_CODE_5G = 0x19,
	ODM_CMNINFO_EEPROMVERSION = 0x1A,
	ODM_CMNINFO_CRYSTALCAP = 0x1B,
	ODM_CMNINFO_MAC_PHY_MODE = 0x1C,
	ODM_CMNINFO_TX_UNI = 0x1D,
	ODM_CMNINFO_RX_UNI = 0x1E,
	ODM_CMNINFO_WM_MODE = 0x1F,
	ODM_CMNINFO_BAND = 0x20,
	ODM_CMNINFO_SEC_CHNL_OFFSET = 0x21,
	ODM_CMNINFO_SEC_MODE = 0x22,
	ODM_CMNINFO_BW = 0x23,
	ODM_CMNINFO_CHNL = 0x24,
	ODM_CMNINFO_FORCED_RATE = 0x25,
	ODM_CMNINFO_DMSP_GET_VALUE = 0x26,
	ODM_CMNINFO_BUDDY_ADAPTOR = 0x27,
	ODM_CMNINFO_DMSP_IS_MASTER = 0x28,
	ODM_CMNINFO_SCAN = 0x29,
	ODM_CMNINFO_POWER_SAVING = 0x2A,
	ODM_CMNINFO_ONE_PATH_CCA = 0x2B,
	ODM_CMNINFO_DRV_STOP = 0x2C,
	ODM_CMNINFO_PNP_IN = 0x2D,
	ODM_CMNINFO_INIT_ON = 0x2E,
	ODM_CMNINFO_ANT_TEST = 0x2F,
	ODM_CMNINFO_NET_CLOSED = 0x30,
	ODM_CMNINFO_MP_MODE = 0x31,
	ODM_CMNINFO_FORCED_IGI_LB = 0x32,
	ODM_CMNINFO_P2P_LINK = 0x33,
	ODM_CMNINFO_FCS_MODE = 0x34,
	ODM_CMNINFO_WIFI_DIRECT = 0x35,
	ODM_CMNINFO_WIFI_DISPLAY = 0x36,
	ODM_CMNINFO_LINK_IN_PROGRESS = 0x37,
	ODM_CMNINFO_LINK = 0x38,
	ODM_CMNINFO_STATION_STATE = 0x39,
	ODM_CMNINFO_RSSI_MIN = 0x3A,
	ODM_CMNINFO_DBG_COMP = 0x3B,
	ODM_CMNINFO_DBG_LEVEL = 0x3C,
	ODM_CMNINFO_RA_THRESHOLD_HIGH = 0x3D,
	ODM_CMNINFO_RA_THRESHOLD_LOW = 0x3E,
	ODM_CMNINFO_RF_ANTENNA_TYPE = 0x3F,
	ODM_CMNINFO_BT_ENABLED = 0x40,
	ODM_CMNINFO_BT_HS_CONNECT_PROCESS = 0x41,
	ODM_CMNINFO_BT_HS_RSSI = 0x42,
	ODM_CMNINFO_BT_OPERATION = 0x43,
	ODM_CMNINFO_BT_LIMITED_DIG = 0x44,
	ODM_CMNINFO_BT_DISABLE_EDCA = 0x45,
	ODM_CMNINFO_NO_BEACON_IN_2S = 0x46,
	ODM_CMNINFO_STA_STATUS = 0x47,
	ODM_CMNINFO_PHY_STATUS = 0x48,
	ODM_CMNINFO_MAC_STATUS = 0x49,
	ODM_CMNINFO_MAX = 0x4A,
};
typedef enum _ODM_Common_Info_Definition ODM_CMNINFO_E;

enum _ODM_Support_Ability_Definition // : sint32_t
{
	ODM_BB_DIG = 0x1,
	ODM_BB_RA_MASK = 0x2,
	ODM_BB_DYNAMIC_TXPWR = 0x4,
	ODM_BB_FA_CNT = 0x8,
	ODM_BB_RSSI_MONITOR = 0x10,
	ODM_BB_CCK_PD = 0x20,
	ODM_BB_ANT_DIV = 0x40,
	ODM_BB_PWR_SAVE = 0x80,
	ODM_BB_PWR_TRAIN = 0x100,
	ODM_BB_RATE_ADAPTIVE = 0x200,
	ODM_BB_PATH_DIV = 0x400,
	ODM_BB_PSD = 0x800,
	ODM_BB_RXHP = 0x1000,
	ODM_BB_ADAPTIVITY = 0x2000,
	ODM_BB_CFO_TRACKING = 0x4000,
	ODM_BB_NHM_CNT = 0x8000,
	ODM_BB_PRIMARY_CCA = 0x10000,
	ODM_MAC_EDCA_TURBO = 0x100000,
	ODM_MAC_EARLY_MODE = 0x200000,
	ODM_RF_TX_PWR_TRACK = 0x1000000,
	ODM_RF_RX_GAIN_TRACK = 0x2000000,
	ODM_RF_CALIBRATION = 0x4000000,
};

enum _RF_PATH //: sint32_t
{
	RF_PATH_A = 0x0, RF_PATH_B = 0x1, RF_PATH_C = 0x2, RF_PATH_D = 0x3,
};
typedef enum _RF_PATH RF_PATH;

enum _EXTCHNL_OFFSET //: sint32_t
{
	EXTCHNL_OFFSET_NO_EXT = 0x0,
	EXTCHNL_OFFSET_UPPER = 0x1,
	EXTCHNL_OFFSET_NO_DEF = 0x2,
	EXTCHNL_OFFSET_LOWER = 0x3,
};
typedef enum _EXTCHNL_OFFSET EXTCHNL_OFFSET;

enum MGN_RATE //: sint32_t
{
	MGN_1M = 0x2,
	MGN_2M = 0x4,
	MGN_5_5M = 0xB,
	MGN_6M = 0xC,
	MGN_9M = 0x12,
	MGN_11M = 0x16,
	MGN_12M = 0x18,
	MGN_18M = 0x24,
	MGN_24M = 0x30,
	MGN_36M = 0x48,
	MGN_48M = 0x60,
	MGN_54M = 0x6C,
	MGN_MCS32 = 0x7F,
	MGN_MCS0 = 0x80,
	MGN_MCS1 = 0x81,
	MGN_MCS2 = 0x82,
	MGN_MCS3 = 0x83,
	MGN_MCS4 = 0x84,
	MGN_MCS5 = 0x85,
	MGN_MCS6 = 0x86,
	MGN_MCS7 = 0x87,
	MGN_MCS8 = 0x88,
	MGN_MCS9 = 0x89,
	MGN_MCS10 = 0x8A,
	MGN_MCS11 = 0x8B,
	MGN_MCS12 = 0x8C,
	MGN_MCS13 = 0x8D,
	MGN_MCS14 = 0x8E,
	MGN_MCS15 = 0x8F,
	MGN_MCS16 = 0x90,
	MGN_MCS17 = 0x91,
	MGN_MCS18 = 0x92,
	MGN_MCS19 = 0x93,
	MGN_MCS20 = 0x94,
	MGN_MCS21 = 0x95,
	MGN_MCS22 = 0x96,
	MGN_MCS23 = 0x97,
	MGN_MCS24 = 0x98,
	MGN_MCS25 = 0x99,
	MGN_MCS26 = 0x9A,
	MGN_MCS27 = 0x9B,
	MGN_MCS28 = 0x9C,
	MGN_MCS29 = 0x9D,
	MGN_MCS30 = 0x9E,
	MGN_MCS31 = 0x9F,
	MGN_VHT1SS_MCS0 = 0xA0,
	MGN_VHT1SS_MCS1 = 0xA1,
	MGN_VHT1SS_MCS2 = 0xA2,
	MGN_VHT1SS_MCS3 = 0xA3,
	MGN_VHT1SS_MCS4 = 0xA4,
	MGN_VHT1SS_MCS5 = 0xA5,
	MGN_VHT1SS_MCS6 = 0xA6,
	MGN_VHT1SS_MCS7 = 0xA7,
	MGN_VHT1SS_MCS8 = 0xA8,
	MGN_VHT1SS_MCS9 = 0xA9,
	MGN_VHT2SS_MCS0 = 0xAA,
	MGN_VHT2SS_MCS1 = 0xAB,
	MGN_VHT2SS_MCS2 = 0xAC,
	MGN_VHT2SS_MCS3 = 0xAD,
	MGN_VHT2SS_MCS4 = 0xAE,
	MGN_VHT2SS_MCS5 = 0xAF,
	MGN_VHT2SS_MCS6 = 0xB0,
	MGN_VHT2SS_MCS7 = 0xB1,
	MGN_VHT2SS_MCS8 = 0xB2,
	MGN_VHT2SS_MCS9 = 0xB3,
	MGN_VHT3SS_MCS0 = 0xB4,
	MGN_VHT3SS_MCS1 = 0xB5,
	MGN_VHT3SS_MCS2 = 0xB6,
	MGN_VHT3SS_MCS3 = 0xB7,
	MGN_VHT3SS_MCS4 = 0xB8,
	MGN_VHT3SS_MCS5 = 0xB9,
	MGN_VHT3SS_MCS6 = 0xBA,
	MGN_VHT3SS_MCS7 = 0xBB,
	MGN_VHT3SS_MCS8 = 0xBC,
	MGN_VHT3SS_MCS9 = 0xBD,
	MGN_VHT4SS_MCS0 = 0xBE,
	MGN_VHT4SS_MCS1 = 0xBF,
	MGN_VHT4SS_MCS2 = 0xC0,
	MGN_VHT4SS_MCS3 = 0xC1,
	MGN_VHT4SS_MCS4 = 0xC2,
	MGN_VHT4SS_MCS5 = 0xC3,
	MGN_VHT4SS_MCS6 = 0xC4,
	MGN_VHT4SS_MCS7 = 0xC5,
	MGN_VHT4SS_MCS8 = 0xC6,
	MGN_VHT4SS_MCS9 = 0xC7,
	MGN_UNKNOWN = 0xC8,
};

struct _RT_CHANNEL_PLAN_2G {
	uint8_t Channel[14];
	uint8_t Len;
};
typedef struct _RT_CHANNEL_PLAN_2G RT_CHANNEL_PLAN_2G;

struct _RT_CHANNEL_PLAN_MAP {
	uint8_t ChannelPlan;
	uint8_t Index2G;
	uint8_t PwrLmt;
};
typedef struct _RT_CHANNEL_PLAN_MAP RT_CHANNEL_PLAN_MAP;

typedef int (*mac_monitor_ptr)(uint8_t *, char);

struct mlme_handler {
	uint32_t num;
	uint32_t (*func)(_adapter *, struct recv_frame *);
};

struct fwevent {
	uint32_t parmsize;
	void (*event_callback)(_adapter *, uint8_t *);
};

struct recv_buf {
	_list list;
	PADAPTER adapter;
	uint32_t len;
	uint8_t *phead;
	uint8_t *pdata;
	uint8_t *ptail;
	uint8_t *pend;
	_pkt *pskb;
};

struct recv_reorder_ctrl {
	_adapter *padapter;
	uint8_t enable;
	uint16_t indicate_seq;
	uint16_t wend_b;
	uint8_t wsize_b;
	_queue pending_recvframe_queue;
	_timer reordering_ctrl_timer;
};

enum _ODM_RF_RADIO_PATH // : sint32_t
{
	ODM_RF_PATH_A = 0x0,
	ODM_RF_PATH_B = 0x1,
	ODM_RF_PATH_C = 0x2,
	ODM_RF_PATH_D = 0x3,
	ODM_RF_PATH_AB = 0x4,
	ODM_RF_PATH_AC = 0x5,
	ODM_RF_PATH_AD = 0x6,
	ODM_RF_PATH_BC = 0x7,
	ODM_RF_PATH_BD = 0x8,
	ODM_RF_PATH_CD = 0x9,
	ODM_RF_PATH_ABC = 0xA,
	ODM_RF_PATH_ACD = 0xB,
	ODM_RF_PATH_BCD = 0xC,
	ODM_RF_PATH_ABCD = 0xD,
};
typedef enum _ODM_RF_RADIO_PATH ODM_RF_RADIO_PATH_E;

enum tag_PhyDM_TRx_MUX_Type //: sint32_t
{
	PhyDM_SHUTDOWN = 0x0,
	PhyDM_STANDBY_MODE = 0x1,
	PhyDM_TX_MODE = 0x2,
	PhyDM_RX_MODE = 0x3,
};
typedef enum tag_PhyDM_TRx_MUX_Type PhyDM_Trx_MUX_Type;

enum tag_PhyDM_MACEDCCA_Type //: sint32_t
{
	PhyDM_IGNORE_EDCCA = 0x0, PhyDM_DONT_IGNORE_EDCCA = 0x1,
};
typedef enum tag_PhyDM_MACEDCCA_Type PhyDM_MACEDCCA_Type;

enum tag_ODM_PauseDIG_Type //: sint32_t
{
	ODM_PAUSE_DIG = 0x1, ODM_RESUME_DIG = 0x2,
};
typedef enum tag_ODM_PauseDIG_Type ODM_Pause_DIG_TYPE;

enum tag_ODM_PauseCCKPD_Type //: sint32_t
{
	ODM_PAUSE_CCKPD = 0x1, ODM_RESUME_CCKPD = 0x2,
};
typedef enum tag_ODM_PauseCCKPD_Type ODM_Pause_CCKPD_TYPE;

struct _ODM_Per_Pkt_Info_ {
	u1Byte DataRate;
	u1Byte StationID;
	BOOLEAN bPacketMatchBSSID;
	BOOLEAN bPacketToSelf;
	BOOLEAN bPacketBeacon;
};
typedef struct _ODM_Per_Pkt_Info_ *PODM_PACKET_INFO_T;

enum _HAL_STATUS //: sint32_t
{
	HAL_STATUS_SUCCESS = 0x0, HAL_STATUS_FAILURE = 0x1,
};
typedef enum _HAL_STATUS HAL_STATUS;

struct _ODM_Phy_Status_Info_ {
	u1Byte RxPWDBAll;
	u1Byte SignalQuality;
	u1Byte RxMIMOSignalStrength[1];
	s1Byte RecvSignalPower;
	u1Byte SignalStrength;
};
typedef struct _ODM_Phy_Status_Info_ *PODM_PHY_INFO_T;

enum _ODM_RF_Config_Type // : sint32_t
{
	CONFIG_RF_RADIO = 0x0, CONFIG_RF_TXPWR_LMT = 0x1,
};
typedef enum _ODM_RF_Config_Type ODM_RF_Config_Type;

enum _ODM_BB_Config_Type //: sint32_t
{
	CONFIG_BB_PHY_REG = 0x0,
	CONFIG_BB_AGC_TAB = 0x1,
	CONFIG_BB_AGC_TAB_2G = 0x2,
	CONFIG_BB_AGC_TAB_5G = 0x3,
	CONFIG_BB_PHY_REG_PG = 0x4,
	CONFIG_BB_PHY_REG_MP = 0x5,
	CONFIG_BB_AGC_TAB_DIFF = 0x6,
};
typedef enum _ODM_BB_Config_Type ODM_BB_Config_Type;

enum _ODM_FW_Config_Type //: sint32_t
{
	CONFIG_FW_NIC = 0x0,
	CONFIG_FW_NIC_2 = 0x1,
	CONFIG_FW_AP = 0x2,
	CONFIG_FW_MP = 0x3,
	CONFIG_FW_WoWLAN = 0x4,
	CONFIG_FW_WoWLAN_2 = 0x5,
	CONFIG_FW_AP_WoWLAN = 0x6,
	CONFIG_FW_BT = 0x7,
};
typedef enum _ODM_FW_Config_Type ODM_FW_Config_Type;

enum _RATE_SECTION //: sint32_t
{
	CCK = 0x0,
	OFDM = 0x1,
	HT_MCS0_MCS7 = 0x2,
	HT_MCS8_MCS15 = 0x3,
	HT_MCS16_MCS23 = 0x4,
	HT_MCS24_MCS31 = 0x5,
	VHT_1SSMCS0_1SSMCS9 = 0x6,
	VHT_2SSMCS0_2SSMCS9 = 0x7,
	VHT_3SSMCS0_3SSMCS9 = 0x8,
	VHT_4SSMCS0_4SSMCS9 = 0x9,
};
typedef enum _RATE_SECTION RATE_SECTION;

struct map_mask_s {
	uint16_t mask_start;
	uint16_t mask_end;
};

struct _TxPowerInfo24G {
	uint8_t IndexCCK_Base[1][6];
	uint8_t IndexBW40_Base[1][6];
	int8_t OFDM_Diff[1][1];
	int8_t BW20_Diff[1][1];
};
typedef struct _TxPowerInfo24G TxPowerInfo24G;
typedef struct _TxPowerInfo24G *PTxPowerInfo24G;

#endif // _WLAN_LIB_H


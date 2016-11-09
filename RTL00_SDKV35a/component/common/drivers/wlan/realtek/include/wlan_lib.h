#ifndef _WIFI_LIB_H
#define _WIFI_LIB_H

//#include "wifi_constants.h"
//#include "wifi_structures.h"

#ifdef	__cplusplus
extern "C" {
#endif

// rom_rtw_message.o
enum $BFA04BDFA6C0C275C890D1E658AFCEEF : __int32
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

//--------------------------------
// freertos_ioctl.o
// Function declarations
extern int rtw_wx_set_autoreconnect(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_wx_get_autoreconnect(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_forwarding_set(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_set_ch_deauth(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int get_priv_size(int args);
extern int rtw_wx_del_custome_ie(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_wx_set_pscan_freq(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_wx_update_custome_ie(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_set_tos_value(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_get_tx_power(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_wx_set_custome_ie(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_pm_get(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_pm_set(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_wx_read32(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_wx_write32(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_wx_set_freq(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra);
extern int rtw_ex_set(net_device *dev, iw_request_info *info, iwreq_data *wdata, char *extra); 
extern void wireless_send_event(net_device *dev, unsigned int cmd, iwreq_data *wrqu, char *extra); 
extern void indicate_wx_custom_event(_adapter *padapter, char *msg); 
extern void indicate_wx_scan_result_present(__int64 padapter, __int64 a2);
extern void indicate_wx_scan_complete_event(__int64 padapter, __int64 a2);
extern void rtw_indicate_sta_assoc(__int64 padapter, __int64 buf);
extern void rtw_indicate_sta_disassoc(_adapter *padapter, uint8_t *addr); 
extern void rtw_indicate_wx_assoc_event(__int64 padapter, __int64 a2);
extern void rtw_indicate_wx_disassoc_event(__int64 padapter, __int64 a2);
extern int rtw_set_wpa_ie(_adapter *padapter, char *pie, int ielen);
extern void strtopsk(uint8_t *des, uint8_t *src, int len);
extern int rtw_wx_get_passphrase(net_device *dev, iw_request_info *a, iwreq_data *wrqu, char *extra); 
extern int rtw_wx_set_ap_essid(net_device *dev, iw_request_info *a, iwreq_data *wrqu, char *extra); 
extern void mac_reg_dump(_adapter *padapter); 
extern void bb_reg_dump(_adapter *padapter); 
extern void rf_reg_dump(_adapter *padapter, int a2, int a3);
extern int rtw_dbg_port(net_device *dev, iw_request_info *info, iwreq_data *wrqu, char *extra); 
extern int rtw_get_auto_channel(net_device *dev, u8 *channel_set, int channel_num);
extern int rtw_set_sta_num(int ap_sta_num);
extern int rtw_del_sta(net_device *dev, u8 *sta_addr); 
extern int rtw_ex_get_drv_ability(net_device *dev, iw_request_info *info, iw_point *wrqu, char *extra); 
extern int rtw_ex_get(net_device *dev, iw_request_info *info, iwreq_data *wdata, char *extra); 
extern void *rtw_ioctl(net_device *dev, iwreq *rq, int cmd);
// Data declarations
u8 g_user_ap_sta_num; // = 5u; 
const iw_priv_args rtw_private_args[26]; /* =
{
  { 35808u, 10239u, 0u, "write" },
  { 35809u, 10239u, 10256u, "read" },
  { 35819u, 10239u, 0u, "dbg" },
  { 35830u, 8256u, 0u, "pm_set" },
  { 35831u, 8256u, 0u, "pm_get" },
  { 35840u, 10239u, 0u, "PartialScan" },
  { 35841u, 10239u, 0u, "SetAutoRecnt" },
  { 35842u, 10239u, 10239u, "GetAutoRecnt" },
  { 35843u, 26623u, 0u, "SetCusIE" },
  { 35844u, 26623u, 0u, "UpdateIE" },
  { 35845u, 0u, 0u, "DelIE" },
  { 35846u, 8193u, 0u, "forwarding_set" },
  { 35847u, 10239u, 10239u, "get_tx_power" },
  { 35848u, 10239u, 0u, "set_tos_value" },
  { 35849u, 8193u, 0u, "SetChDeauth" },
  { 35822u, 9216u, 0u, "" },
  { 35823u, 9216u, 10239u, "" },
  { 0u, 9216u, 0u, "write_mac" },
  { 1u, 9216u, 0u, "set_ch_plan" },
  { 2u, 9216u, 10239u, "read_mac" },
  { 3u, 9216u, 10239u, "txpower" },
  { 4u, 9216u, 10239u, "get_client_list" },
  { 5u, 9216u, 10239u, "get_ap_info" },
  { 6u, 9216u, 10239u, "get_security" },
  { 8u, 9216u, 10239u, "get_drv_ability" },
  { 9u, 9216u, 10239u, "get_ch_plan" }
};  */
iw_handler rtw_private_handler[17]; /* =
{
  (iw_handler)0x58D,
  (iw_handler)0x4E1,
  (iw_handler)0xD59,
  (iw_handler)0x42D,
  (iw_handler)0x3E9,
  (iw_handler)0xE5,
  (iw_handler)5,
  (iw_handler)0x33,
  (iw_handler)0x33D,
  (iw_handler)0x167,
  (iw_handler)0x99,
  (iw_handler)0x41,
  (iw_handler)0x1F9,
  (iw_handler)0x1D9,
  (iw_handler)0x63,
  (iw_handler)0x699,
  (iw_handler)0x165D
};  */
char iw_priv_type_size[8]; // = { '\0', '\x01', '\x01', '\0', '\x04', '\b', '\x10', '\0' }; 
//--------------------------------
// freertos_intfs.o
// Function declarations
extern net_device_stats *rtw_net_get_stats(net_device *pnetdev); 
extern int netdev_if2_close(net_device *pnetdev); 
extern int netdev_close(net_device *pnetdev); 
extern void rtw_if1_deinit(PADAPTER if1); 
extern void rtw_reset_securitypriv(_adapter *adapter); 
extern void rtw_os_indicate_disconnect(_adapter *adapter); 
extern int rtw_init_netdev_name(net_device *pnetdev, const char *ifname); 
extern net_device *rtw_init_netdev(_adapter *old_padapter); 
extern int rtw_init_io_priv(_adapter *padapter, void (*set_intf_ops)(_io_ops *)); 
extern _adapter *rtw_drv_if2_init(_adapter *primary_padapter, char *name, void (*set_intf_ops)(_io_ops *)); 
extern void rtw_drv_if2_stop(_adapter *if2); 
extern void rtw_drv_if2_free(_adapter *primary_padapter); 
extern init_done_ptr netdev_open(net_device *pnetdev);
extern int netdev_if2_open(net_device *pnetdev); 
extern int netdev_if2_open(net_device *pnetdev); 
extern init_done_ptr netdev_open(net_device *pnetdev);
extern net_device *rtw_drv_probe(net_device *parent_dev, uint32_t mode); 
extern int rtw_dev_remove(net_device *pnetdev); 
extern void rtw_drv_entry(); 
extern void rtw_drv_halt(); 
// Data declarations
extern init_done_ptr p_wlan_init_done_callback; 
extern uint8_t rtw_power_percentage_idx; 
extern init_done_ptr p_wlan_uart_adapter_callback; 
extern uint8_t rtw_adaptivity_en; 
extern uint8_t rtw_adaptivity_mode = 1u; 
extern int8_t rtw_adaptivity_th_l2h_ini; 
extern drv_priv drvpriv; 
//--------------------------------
// hal_com.o
extern void dump_chip_info(int a1, int a2, int a3, int a4, HAL_VERSION ChipVersion);
extern int hal_com_get_channel_plan(_adapter *padapter, uint8_t hw_channel_plan, int sw_channel_plan, int def_channel_plan, BOOLEAN AutoLoadFail);
extern int HAL_IsLegalChannel(_adapter *Adapter, uint32_t Channel);
extern int MRateToHwRate(uint8_t rate);
extern signed int HwRateToMRate(uint8_t rate);
extern void HalSetBrateCfg(_adapter *Adapter, uint8_t *mBratesOS, uint16_t *pBrateCfg); 
extern signed int Hal_MappingOutPipe(_adapter *pAdapter, uint8_t NumOutPipe);
extern void hal_init_macaddr(_adapter *adapter); 
extern void hw_var_port_switch(_adapter *adapter); 
extern void SetHwReg(PADAPTER padapter, int variable, uint8_t *val);
extern signed int eqNByte(uint8_t *str1, uint8_t *str2, uint32_t num);
extern signed int GetU1ByteIntegerFromStringInDecimal(char *Str, uint8_t *pInt);
extern void switch_power_saving_mode(_adapter *padapter, int benable);
extern void rtw_bb_rf_gain_offset(_adapter *padapter); 
// Data declarations
extern u8 CSWTCH_15[132];
extern u8 CSWTCH_17[19];
//--------------------------------
// HalHWImg8195A_MAC.o
// Function declarations
extern void ODM_ReadAndConfig_MP_8195A_MAC_REG(PDM_ODM_T pDM_Odm); 
extern signed int ODM_GetVersion_MP_8195A_MAC_REG(); // return 26;
// Data declarations
extern u32 Array_MP_8195A_MAC_REG[194]; 
//--------------------------------
// HalHWImg8195A_RF.o
// Function declarations
extern signed int CheckPositive(PDM_ODM_T pDM_Odm, const u4Byte Condition1, const u4Byte Condition2, const u4Byte Condition3, const u4Byte Condition4);
extern void ODM_ReadAndConfig_MP_8195A_RadioA(PDM_ODM_T pDM_Odm); 
extern signed int ODM_GetVersion_MP_8195A_RadioA();
extern void ODM_ReadAndConfig_MP_8195A_RADIO_DIFF(PDM_ODM_T pDM_Odm, u4Byte *Array, u4Byte ArrayLen, u4Byte a4);
extern signed int ODM_GetVersion_MP_8195A_RADIO_DIFF();
extern void ODM_ReadAndConfig_MP_8195A_TxPowerTrack_QFN48(PDM_ODM_T pDM_Odm); 
extern void ODM_ReadAndConfig_MP_8195A_TxPowerTrack_QFN56(PDM_ODM_T pDM_Odm); 
extern void ODM_ReadAndConfig_MP_8195A_TxPowerTrack_TFBGA96(PDM_ODM_T pDM_Odm); 
extern void ODM_ReadAndConfig_MP_8195A_TXPWR_LMT(PDM_ODM_T pDM_Odm); 
extern void ODM_ReadAndConfig_MP_8195A_TxXtalTrack(PDM_ODM_T pDM_Odm); 
// Data declarations
extern u8 gDeltaSwingTableIdx_MP_2GCCKA_N_TxPowerTrack_TFBGA96_8195A[32];
extern u8 gDeltaSwingTableIdx_MP_2GCCKA_P_TxPowerTrack_TFBGA96_8195A[32];
extern u8 gDeltaSwingTableXtal_MP_N_TxXtalTrack_8195A[30];
extern u8 gDeltaSwingTableIdx_MP_2GCCKA_N_TxPowerTrack_QFN56_8195A[30];
extern u8 gDeltaSwingTableIdx_MP_2GCCKA_N_TxPowerTrack_QFN48_8195A[30];
extern u8 gDeltaSwingTableIdx_MP_2GCCKA_P_TxPowerTrack_QFN56_8195A[30];
extern const u8 Array_MP_8195A_TXPWR_LMT[1176];
extern u8 gDeltaSwingTableIdx_MP_2GA_P_TxPowerTrack_QFN56_8195A[30];
extern u32 Array_MP_8195A_RadioA[370];
extern u8 gDeltaSwingTableXtal_MP_P_TxXtalTrack_8195A[30];
extern u8 s1Byte gDeltaSwingTableIdx_MP_2GA_P_TxPowerTrack_TFBGA96_8195A[32];
extern u8 gDeltaSwingTableIdx_MP_2GCCKA_P_TxPowerTrack_QFN48_8195A[30];
//--------------------------------
// HalPhyRf_8195A.o
// Function declarations
extern void GetDeltaSwingTable_8195A(PDM_ODM_T pDM_Odm, ps1Byte *TemperatureUP_A, ps1Byte *TemperatureDOWN_A, ps1Byte *TemperatureUP_B, ps1Byte *TemperatureDOWN_B);
extern void GetDeltaSwingXtalTable_8195A(PDM_ODM_T pDM_Odm, ps1Byte *TemperatureUP_Xtal, ps1Byte *TemperatureDOWN_Xtal); 
extern void ODM_TxXtalTrackSetXtal_8195A(PDM_ODM_T pDM_Odm); 
extern void setIqkMatrix_8195A(PDM_ODM_T pDM_Odm, int OFDM_index, int RFPath, s4Byte IqkResult_X, s4Byte IqkResult_Y);
extern void Hal_MPT_CCKTxPowerAdjust(PADAPTER Adapter); 
extern void ODM_TxPwrTrackSetPwr_8195A(PDM_ODM_T pDM_Odm, PWRTRACK_METHOD Method, int RFPath, u8 ChannelMappedIndex);
extern void ConfigureTxpowerTrack_8195A(PTXPWRTRACK_CFG pConfig); 
extern int phy_PathA_IQK_8195A(PADAPTER pAdapter, BOOLEAN configPathB);
extern signed int phy_PathA_RxIQK8195A(PADAPTER pAdapter, BOOLEAN configPathB);
extern int phy_PathB_IQK_8195A(PADAPTER pAdapter);
extern signed int phy_PathB_RxIQK8195A(PADAPTER pAdapter, BOOLEAN configPathB);
extern void PHY_PathAFillIQKMatrix8195A(PADAPTER pAdapter, int bIQKOK, s4Byte (*result)[8], int final_candidate, BOOLEAN bTxOnly);
extern void PHY_PathBFillIQKMatrix8195A(PADAPTER pAdapter, int bIQKOK, s4Byte (*result)[8], int final_candidate, BOOLEAN bTxOnly);
extern signed int ODM_CheckPowerStatus(PADAPTER Adapter);
extern void PHY_SaveADDARegisters8195A(PADAPTER pAdapter, pu4Byte ADDAReg, pu4Byte ADDABackup, u4Byte RegisterNum); 
extern void PHY_SaveMACRegisters8195A(PADAPTER pAdapter, pu4Byte MACReg, pu4Byte MACBackup); 
extern void PHY_ReloadADDARegisters8195A(PADAPTER pAdapter, pu4Byte ADDAReg, pu4Byte ADDABackup, u4Byte RegiesterNum); 
extern void PHY_ReloadMACRegisters8195A(PADAPTER pAdapter, pu4Byte MACReg, pu4Byte MACBackup); 
extern void PHY_PathADDAOn8195A(PADAPTER pAdapter, pu4Byte ADDAReg, BOOLEAN isPathAOn, int is2T);
extern void PHY_MACSettingCalibration8195A(PADAPTER pAdapter, pu4Byte MACReg, pu4Byte MACBackup); 
extern void PHY_PathAStandBy8195A(PADAPTER pAdapter); 
extern void PHY_PIModeSwitch8195A(PADAPTER pAdapter, int PIMode);
extern signed int phy_SimularityCompare_8195A(PADAPTER pAdapter, s4Byte (*result)[8], int c1, int c2);
extern void phy_IQCalibrate_8195A(PADAPTER pAdapter, s4Byte (*result)[8], int t, int is2T);
extern void phy_LCCalibrate_8195A(PDM_ODM_T pDM_Odm, BOOLEAN is2T); 
extern void PHY_LCCalibrate_8195A(PDM_ODM_T pDM_Odm); 
extern void PHY_IQCalibrate_8195A(PADAPTER pAdapter, int bReCovery, int bRestore);
extern void DoIQK_8195A(PDM_ODM_T pDM_Odm, u8 DeltaThermalIndex, u8 ThermalValue, u8 Threshold); 
extern void phy_SetRFPathSwitch_8195A(PADAPTER pAdapter, int bMain, BOOLEAN is2T);
extern void PHY_SetRFPathSwitch_8195A(PADAPTER pAdapter, int bMain);
//--------------------------------
// rtk_wlan_if.o
// Function declarations
extern void timer_wrapper(_timerHandle timer_hdl); 
extern net_device *alloc_etherdev(int sizeof_priv); 
extern void free_netdev(net_device *dev); 
extern int dev_alloc_name(net_device *net_dev, const char *ifname); 
extern void init_timer_wrapper(); 
extern void deinit_timer_wrapper(int a1);
extern void init_timer(timer_list *timer); 
extern void mod_timer(timer_list *timer, uint32_t delay_time_ms); 
extern BOOL timer_pending(const timer_list *timer);
extern void cancel_timer_ex(timer_list *timer); 
extern void del_timer_sync(timer_list *timer); 
extern void rtw_init_timer(_timer *ptimer, void *adapter, TIMER_FUN pfunc, void *cntx, const char *name);
extern int rtw_cancel_timer(_timer *ptimer);
extern BOOL rltk_get_idx_bydev(net_device *dev);
extern int rltk_wlan_init(int idx_wlan, rtw_mode_t mode); 
extern void rltk_wlan_deinit(); 
extern int rltk_wlan_start(int idx_wlan); 
extern int rltk_wlan_check_isup(int idx);
extern void rltk_wlan_tx_inc(int idx); 
extern void rltk_wlan_tx_dec(int idx); 
extern sk_buff *rltk_wlan_get_recv_skb(int idx); 
extern sk_buff *rltk_wlan_alloc_skb(unsigned int total_len); 
extern void rltk_wlan_send_skb(int idx, sk_buff *skb); 
extern void rltk_netif_rx(sk_buff *skb); 
extern int rltk_del_station(const char *ifname, u8 *hwaddr); 
extern int rltk_get_auto_chl(const char *ifname, u8 *channel_set, int channel_num, int a4);
extern int rltk_set_tx_power_percentage(rtw_tx_pwr_percentage_t power_percentage_idx); 
extern int rltk_wlan_control(unsigned int cmd, void *data); 
extern int rltk_wlan_running(int idx);
extern void rltk_wlan_statistic(int idx, int a2, int a3);
extern int rltk_wlan_handshake_done(); 
extern int rltk_wlan_rf_on(); 
extern int rltk_wlan_rf_off(); 
extern int rltk_wlan_check_bus(); 
extern int rltk_wlan_wireless_mode(u8 mode); 
extern int rltk_wlan_set_wps_phase(int result);
extern void rltk_wlan_PRE_SLEEP_PROCESSING(); 
extern int rltk_wlan_is_connected_to_ap(); 
extern int rtw_ps_enable(int enable); 
extern int rltk_wifi_fw_test(int argc, char **argv); 
// Data declarations
extern _list timer_table;
extern net_device *rltk_wlan_info;
extern int timer_used_num;
extern int max_timer_used_num;
//--------------------------------
// rtl8195a_cmd.o
// Function declarations
extern int32_t FillH2CCmd8195A(PADAPTER padapter, int ElementID, __int64 CmdLen);
extern void rtl8195a_set_FwRsvdPage_cmd(PADAPTER padapter, PH2CParam_RsvdPage pRsvdPage); 
extern void rtl8195a_set_FwMediaStatusRpt_cmd(PADAPTER padapter, int mstatus, int macid);
extern void rtl8195a_set_FwMacIdConfig_cmd(_adapter *padapter, int mac_id, int raid, int bw, uint8_t sgi, uint32_t mask);
extern void rtl8195a_set_FwPwrMode_cmd(PADAPTER padapter, int psmode);
extern void rtl8195a_download_rsvd_page(PADAPTER padapter, int mstatus);
extern void rtl8195a_set_FwJoinBssRpt_cmd(PADAPTER padapter, int mstatus);
extern void rtl8195a_Add_RateATid(PADAPTER pAdapter, uint32_t bitmap, uint8_t *arg_ary, int rssi_level);
extern PADAPTER rtl8195a_set_BcnIgnoreEDCCA_cmd(PADAPTER result, int enable, int a3);
//--------------------------------
// rtl8195a_rf6052.o
// Function declarations
extern int PHY_ConfigRFWithTxPwrTrackParaFile(PADAPTER Adapter, char *pFileName); 
extern void PHY_RF6052SetBandwidth8195A(PADAPTER Adapter, CHANNEL_WIDTH Bandwidth); 
extern int PHY_RF6052_Config8195A(PADAPTER Adapter); 
//--------------------------------
// rtw_efuse.o
// Function declarations
extern void Efuse_PowerSwitch(PADAPTER pAdapter, uint8_t bWrite, uint8_t PwrState); 
extern int Efuse_GetCurrentSize(PADAPTER pAdapter, uint8_t efuseType, BOOLEAN bPseudoTest);
extern int Efuse_CalculateWordCnts(uint8_t word_en);
extern void EFUSE_GetEfuseDefinition(PADAPTER pAdapter, int efuseType, uint8_t type, void *pOut, BOOLEAN bPseudoTest);
extern int efuse_OneByteRead(PADAPTER pAdapter, int addr, uint8_t *data, int bPseudoTest);
extern int efuse_read8(PADAPTER padapter, int address, uint8_t *value);
extern int efuse_OneByteWrite(PADAPTER pAdapter, int addr, int data, int bPseudoTest);
extern int efuse_write8(PADAPTER padapter, int address, uint8_t *value);
extern int Efuse_PgPacketWrite(PADAPTER pAdapter, int offset, int word_en, uint8_t *data, BOOLEAN bPseudoTest);
extern void efuse_WordEnableDataRead(uint8_t word_en, uint8_t *sourdata, uint8_t *targetdata); 
extern int Efuse_WordEnableDataWrite(PADAPTER pAdapter, int efuse_addr, uint8_t word_en, uint8_t *data, BOOLEAN bPseudoTest);
extern int rtw_efuse_access(PADAPTER padapter, int bWrite, int start_addr, int cnts, uint8_t *data);
extern signed int efuse_GetCurrentSize(PADAPTER padapter, uint16_t *size);
extern signed int rtw_efuse_map_read(PADAPTER padapter, int addr, int cnts, uint8_t *data);
extern signed int rtw_efuse_map_write(PADAPTER padapter, int addr, int cnts, uint8_t *data);
extern void Efuse_ReadAllMap(PADAPTER pAdapter, int efuseType, uint8_t *Efuse, BOOLEAN bPseudoTest);
extern void EFUSE_ShadowRead(PADAPTER pAdapter, int Type, int Offset, uint32_t *Value);
extern void EFUSE_ShadowMapUpdate(PADAPTER pAdapter, int efuseType, BOOLEAN bPseudoTest, int a4);
//--------------------------------
// rtw_ieee80211.o
// Function declarations
extern void rtw_macaddr_cfg(uint8_t *mac_addr, int a2);
extern int rtw_get_cipher_info(wlan_network *pnetwork); 
extern void rtw_get_bcn_info(wlan_network *pnetwork); 
//--------------------------------
// rtw_wlan_util.o
// Function declarations
extern int cckrates_included(unsigned __int8 *rate, int ratelen); 
extern int cckratesonly_included(unsigned __int8 *rate, int ratelen); 
extern signed int networktype_to_raid_ex(PADAPTER padapter, int network_type);
extern signed int judge_network_type(_adapter *padapter, unsigned __int8 *rate, int ratelen);
extern int ratetbl_val_2wifirate(unsigned __int8 rate);
extern int is_basicrate(_adapter *padapter, int rate);
extern int ratetbl2rateset(_adapter *padapter, unsigned __int8 *rateset);
extern void get_rate_set(_adapter *padapter, unsigned __int8 *pbssrate, int *bssrate_len, int a4);
extern void UpdateBrateTbl(PADAPTER Adapter, uint8_t *mBratesOS); 
extern void UpdateBrateTblForSoftAP(uint8_t *bssrateset, uint32_t bssratelen); 
extern void Save_DM_Func_Flag(_adapter *padapter, int a2, int a3);
extern void Restore_DM_Func_Flag(_adapter *padapter); 
extern void Switch_DM_Func(_adapter *padapter, uint32_t mode, int enable);
extern void Set_MSR(_adapter *padapter, uint8_t type); 
extern int set_opmode(_adapter *padapter, NDIS_802_11_NETWORK_INFRASTRUCTURE networktype);
extern void SelectChannel(_adapter *padapter, int channel);
extern void SetBWMode(_adapter *padapter, int bwmode, int channel_offset);
extern void set_channel_bwmode(_adapter *padapter, int channel, int channel_offset, int bwmode);
extern uint8_t *get_my_bssid(WLAN_BSSID_EX *pnetwork); 
extern int get_beacon_interval(WLAN_BSSID_EX *bss, int a2, int a3);
extern int is_client_associated_to_ap(int result);
extern BOOL is_client_associated_to_ibss(_adapter *padapter);
extern int is_IBSS_empty(_adapter *padapter); 
extern unsigned int decide_wait_for_beacon_timeout(unsigned int bcn_interval); 
extern void invalidate_cam_all(_adapter *padapter); 
extern void write_cam(_adapter *padapter, uint8_t entry, int ctrl, uint8_t *mac, uint8_t *key);
extern void clear_cam_entry(_adapter *padapter, uint8_t entry); 
extern void flush_all_cam_entry(_adapter *padapter); 
extern int WMM_param_handler(_adapter *padapter, PNDIS_802_11_VARIABLE_IEs pIE); 
extern void WMMOnAssocRsp(_adapter *padapter); 
extern void HT_caps_handler(_adapter *padapter, PNDIS_802_11_VARIABLE_IEs pIE); 
extern void HT_info_handler(_adapter *padapter, PNDIS_802_11_VARIABLE_IEs pIE); 
extern void HTOnAssocRsp(_adapter *padapter); 
extern void ERP_IE_handler(_adapter *padapter, PNDIS_802_11_VARIABLE_IEs pIE); 
extern void VCS_update(_adapter *padapter, sta_info *psta); 
extern int rtw_check_bcn_info(ADAPTER *Adapter, uint8_t *pframe, uint32_t packet_len); 
extern void update_beacon_info(_adapter *padapter, uint8_t *pframe, unsigned int pkt_len, sta_info *psta); 
extern signed int is_ap_in_tkip(_adapter *padapter);
extern int wifirate2_ratetbl_inx(unsigned __int8 rate); 
extern int update_basic_rate(unsigned __int8 *ptn, unsigned int ptn_sz);
extern int update_supported_rate(unsigned __int8 *ptn, unsigned int ptn_sz);
extern int update_MCS_rate(HT_caps_element *pHT_caps);
extern int support_short_GI(_adapter *padapter, HT_caps_element *pHT_caps); 
extern int get_highest_rate_idx(uint32_t mask);
extern void Update_RA_Entry(_adapter *padapter, sta_info *psta); 
extern void enable_rate_adaptive(_adapter *padapter, sta_info *psta); 
extern void set_sta_rate(_adapter *padapter, sta_info *psta); 
extern void update_tx_basic_rate(_adapter *padapter, int wirelessmode);
extern signed int check_assoc_AP(uint8_t *pframe, unsigned int len);
extern void update_IOT_info(_adapter *padapter); 
extern void update_capinfo(PADAPTER Adapter, uint16_t updateCap); 
extern void update_wireless_mode(_adapter *padapter, uint32_t a2, int a3);
extern void update_bmc_sta_support_rate(_adapter *padapter, uint32_t mac_id); 
extern void update_TSF(mlme_ext_priv *pmlmeext, uint8_t *pframe, unsigned int len); 
extern void correct_TSF(_adapter *padapter, mlme_ext_priv *pmlmeext); 
// Data declarations
extern u8 CSWTCH_36[12]; // = { 2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108 }; 
extern const uint8_t ARTHEROS_OUI1[3]; // = { 0u, 3u, 127u }; 
extern const uint8_t ARTHEROS_OUI2[3]; // = { 0u, 19u, 116u }; 
extern const uint8_t REALTEK_OUI[3]; // = { 0u, 224u, 76u }; 
extern const uint8_t RALINK_OUI[3]; // = { 0u, 12u, 67u }; 
extern const uint8_t MARVELL_OUI[3]; // = { 0u, 80u, 67u }; 
extern const uint8_t CISCO_OUI[3]; // = { 0u, 64u, 150u }; 
extern const uint8_t rtw_basic_rate_cck[4]; // = { 130u, 132u, 139u, 150u }; 
extern const uint8_t BROADCOM_OUI1[3]; // = { 0u, 16u, 24u }; 
extern const uint8_t BROADCOM_OUI2[3]; // = { 0u, 10u, 247u }; 
extern const uint8_t rtw_basic_rate_mix[7]; // = { 130u, 132u, 139u, 150u, 140u, 152u, 176u }; 
extern const uint8_t rtw_basic_rate_ofdm[3]; // = { 140u, 152u, 176u }; 
extern const uint8_t AIRGOCAP_OUI[3]; // = { 0u, 10u, 245u }; 
//--------------------------------
// wifi_simple_config_parser.o
// Function declarations
extern s32 bytecopy(u8 *src, u8 *dst, u32 len); 
extern s32 rtk_sc_register_pattern(pattern_ops *pp); 
extern s32 rtk_sc_generate_key(pattern_ops *pp, rtk_sc *pSc); 
extern s32 rtk_sc_decode_profile(pattern_ops *pp, rtk_sc *pSc); 
extern s32 rtk_sc_get_tlv_info(pattern_ops *pp, rtk_sc *pSc); 
extern s32 mcast_udp_get_cipher_info(pattern_ops *pp, rtk_sc *pSc); 
extern s32 mcast_udp_get_pattern(pattern_ops *pp, rtk_sc *pSc); 
extern s32 bcast_udp_get_pattern(pattern_ops *pp, rtk_sc *pSc); 
extern s32 bcast_udp_get_cipher_info(pattern_ops *pp, rtk_sc *pSc); 
extern s32 rtk_clean_profile_value(); 
extern s32 mcast_udp_decode_profile(pattern_ops *pp, rtk_sc *pSc); 
extern s32 mcast_udp_generate_key(pattern_ops *pp, rtk_sc *pSc); 
extern s32 rtk_sc_check_packet(u8 *da, u8 *bssid, s32 length);
extern void whc_fix_channel(); 
extern void whc_unfix_channel(); 
extern void simple_config_lib_init(simple_config_lib_config *config); 
extern void simple_config_lib_deinit(); 
extern int parse_tlv_info_bcast(rtk_sc *pSc, unsigned __int8 *plain_info, int len);
extern s32 mcast_udp_get_profile(pattern_ops *pp, rtk_sc *pSc); 
extern void rtk_restart_simple_config(); 
extern void rtk_stop_simple_config(); 
extern s32 rtk_sc_init(char *custom_pin_code, simple_config_lib_config *lib_config); 
extern void rtk_sc_deinit(); 
extern int rtk_sc_check_profile(pattern_ops *pp, rtk_sc *pSc, void *backup_sc_ctx); 
extern signed int softAP_simpleConfig_parse(unsigned __int8 *buf, int len, void *backup_sc_ctx, void *psoftAP_ctx);
extern int rtl_pre_parse(u8 *mac_addr, u8 *buf, void *userdata, u8 **da, u8 **sa, unsigned int *len);
extern s32 rtk_start_parse_packet(u8 *da, u8 *sa, s32 len, void *user_data, void *backup_sc_ctx);
// Data declarations
// Data declarations
extern char algn_1; // weak
extern const u8 default_key_iv[8]; // = { 166u, 166u, 166u, 166u, 166u, 166u, 166u, 166u }; 
extern pattern_ops udp_bcast; /* =
{
  4u,  10u,
  {
    115u,    99u,    95u,    98u,    99u,    97u,    115u,    116u,    95u,    117u,
    100u,    112u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,
    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,
    0u
  },
  (sc_check_pattern_call_back)0x20D,
  (sc_get_cipher_info_call_back)0x2C9,
  (sc_generate_key_call_back)0x3B5,
  (sc_decode_profile_call_back)0x389,
  (sc_get_tlv_info_call_back)0x62D
};  */
extern u8 g_bssid[6]; 
extern s32 simple_config_status; 
extern u8 g_ios_mac[6]; // = { 2u, 0u, 0u, 0u, 0u, 0u }; 
extern u8 use_ios7_mac; 
extern u8 *custom_pin; 
extern pattern_ops udp_mcast_pin; /* =
{
  3u,  10u,
  {
    115u,    99u,    95u,    109u,    99u,    97u,    115u,    116u,    95u,    117u,
    100u,    112u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,
    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u
  },
  (sc_check_pattern_call_back)0x95,
  (sc_get_cipher_info_call_back)0x6B,
  (sc_generate_key_call_back)0x3B5,
  (sc_decode_profile_call_back)0x389,
  (sc_get_tlv_info_call_back)0x62D
};  */
extern pattern_ops udp_bcast_pin; /* =
{
  5u,  10u,
  {
    115u,    99u,    95u,    98u,    99u,    97u,    115u,    116u,    95u,    117u,    100u,
    112u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,
    0u,    0u,    0u,    0u,    0u,    0u,    0u,
    0u
  },
  (sc_check_pattern_call_back)0x20D,
  (sc_get_cipher_info_call_back)0x2C9,
  (sc_generate_key_call_back)0x3B5,
  (sc_decode_profile_call_back)0x389,
  (sc_get_tlv_info_call_back)0x62D
};  */
extern u8 get_channel_flag; 
extern u8 g_security_mode; // = 255u; 
extern u8 radom_value[4]; 
extern s32 profile_pkt_index; 
extern const u8 default_pin[9]; // = { 53u, 55u, 50u, 56u, 57u, 57u, 54u, 49u, 0u }; 
extern const u8 sc_device_name[21]; /* =
{
  115u,  105u,  109u,  112u,  108u,  101u,  95u,  99u,  111u,  110u,  102u,  105u,  103u,
  95u,  99u,  108u,  105u,  101u,  110u,  116u,  0u
};  */
extern simple_config_lib_config sc_api_fun; 
extern u8 fix_sa; 
extern u32 g_sc_pin_len; 
extern pattern_ops *pp; 
extern pattern_ops udp_mcast; /* =
{
  2u,  10u,
  {
    115u,    99u,    95u,    109u,    99u,    97u,    115u,    116u,    95u,    117u,    100u,
    112u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u,
    0u,    0u,    0u,    0u,    0u,    0u,    0u,    0u
  },
  (sc_check_pattern_call_back)0x95,
  (sc_get_cipher_info_call_back)0x6B,
  (sc_generate_key_call_back)0x3B5,
  (sc_decode_profile_call_back)0x389,
  (sc_get_tlv_info_call_back)0x62D
};  */
extern s32 sync_pkt_index; 
extern const u8 mcast_udp_buffer[65]; /* =
{
  56u,  67u,  109u,  84u,  47u,  32u,  74u,  40u,  51u,  95u,  97u,  69u,  32u,  82u,  95u,
  85u,  70u,  82u,  125u,  96u,  109u,  116u,  119u,  70u,  61u,  41u,  81u,  102u,  106u,  116u,
  110u,  94u,  83u,  95u,  49u,  47u,  102u,  102u,  103u,  60u,  95u,  67u,  55u,  121u,  119u,
  39u,  115u,  125u,  63u,  39u,  95u,  39u,  110u,  38u,  50u,  126u,  66u,  108u,  109u,  38u,
  95u,  107u,  63u,  54u,  0u
};  */
extern rtk_sc *g_sc_ctx; 
//--------------------------------
// wlan_ram_map.o
// Function declarations
extern void init_rom_wlan_ram_map()
//--------------------------------
// freertos_isr.o
// Function declarations
extern void rtw_interrupt_thread(thread_context context);
// Data declarations
extern _sema *pExportWlanIrqSemaphore;

//--------------------------------
// freertos_recv.o
// Function declarations
extern int rtw_os_recv_resource_init(recv_priv *precvpriv, _adapter *padapter); 
extern int rtw_os_recv_resource_alloc(_adapter *padapter, recv_frame *precvframe); 
extern int rtw_os_recvbuf_resource_alloc(_adapter *padapter, recv_buf *precvbuf); 
extern int rtw_os_recvbuf_resource_free(_adapter *padapter, recv_buf *precvbuf); 
extern int rtw_tkip_countermeasure(_adapter *padapter); 
extern void rtw_handle_tkip_mic_err(_adapter *padapter, int bgroup);
extern int rtw_recv_indicatepkt(_adapter *padapter, recv_frame *precv_frame); 
extern void rtw_init_recv_timer(recv_reorder_ctrl *preorder_ctrl); 

//--------------------------------
// freertos_skbuff.o
// Function declarations
extern void skb_fail_inc(int a1);
extern int skb_fail_get_and_rst(int a1);
extern void init_skb_pool(); 
extern void init_skb_data_pool(); 
extern sk_buff *alloc_skb(int size); 
extern void kfree_skb(sk_buff *skb); 
extern unsigned __int8 *skb_put(sk_buff *skb, unsigned int len); 
extern void skb_reserve(sk_buff *skb, unsigned int len); 
extern sk_buff *dev_alloc_skb(unsigned int length, unsigned int reserve_len); 
extern void skb_assign_buf(sk_buff *skb, unsigned __int8 *buf, unsigned int len); 
extern unsigned __int8 *skb_tail_pointer(const sk_buff *skb); 
extern unsigned __int8 *skb_end_pointer(const sk_buff *skb); 
extern void skb_set_tail_pointer(sk_buff *skb, const int offset); 
extern unsigned __int8 *skb_pull(sk_buff *skb, unsigned int len); 
extern sk_buff *skb_copy(const sk_buff *skb, int gfp_mask, unsigned int reserve_len); 
extern sk_buff *skb_clone(sk_buff *skb, int gfp_mask); 
// Data declarations
extern int skbbuf_used_num;
extern list_head skbdata_list; 
extern skb_data skb_data_pool[8];
extern skb_buf skb_pool[10]; 
extern int skbdata_used_num; 
extern int max_local_skb_num = 10; 
extern list_head wrapper_skbbuf_list; 
extern int max_skbdata_used_num; 
extern int max_skbbuf_used_num; 
extern int skb_fail_count; 
extern int max_skb_buf_num = 8; 
//--------------------------------
// freertos_xmit.o
// Function declarations
extern signed int rtw_remainder_len(pkt_file *pfile);
extern void rtw_open_pktfile(_pkt *pktptr, pkt_file *pfile); 
extern unsigned int rtw_pktfile_read(pkt_file *pfile, uint8_t *rmem, unsigned int rlen); 
extern BOOL rtw_endofpktfile(pkt_file *pfile);
extern int rtw_os_xmit_resource_alloc(_adapter *padapter, xmit_buf *pxmitbuf, uint32_t alloc_sz); 
extern void rtw_os_pkt_complete(_adapter *padapter, _pkt *pkt); 
extern void rtw_os_xmit_complete(_adapter *padapter, xmit_frame *pxframe); 
extern void rtw_os_xmit_schedule(_adapter *padapter, _irqL a2, int a3);
extern int rtw_xmit_entry(_pkt *pkt, _nic_hdl pnetdev, int a3);
extern int rtw_os_can_xmit(net_device *dev); 
//--------------------------------
// hal_intf.o
// Function declarations
extern int32_t rtw_hal_fill_h2c_cmd(int32_t result, uint8_t ElementID, uint32_t CmdLen, uint8_t *pCmdBuffer);
extern void rtw_hal_fill_fake_txdesc(_adapter *padapter, uint8_t *pDesc, uint32_t BufferLen, uint8_t IsPsPoll, uint8_t IsBTQosNull, uint8_t bDataFrame);
extern _adapter *rtw_hal_get_txbuff_rsvd_page_num(_adapter *result, bool wowlan);
extern void rtw_hal_chip_configure(_adapter *padapter); 
extern void rtw_hal_read_chip_info(_adapter *padapter); 
extern void rtw_hal_read_chip_version(_adapter *padapter); 
extern void rtw_hal_def_value_init(_adapter *padapter); 
extern void rtw_hal_free_data(_adapter *padapter); 
extern void rtw_hal_dm_init(_adapter *padapter); 
extern void rtw_hal_dm_deinit(_adapter *padapter); 
extern int rtw_hal_init(_adapter *padapter);
extern int rtw_hal_deinit(_adapter *padapter);
extern void rtw_hal_set_hwreg(_adapter *padapter, uint8_t variable, uint8_t *val); 
extern void rtw_hal_get_hwreg(_adapter *padapter, uint8_t variable, uint8_t *val); 
extern int rtw_hal_set_def_var(_adapter *padapter, HAL_DEF_VARIABLE eVariable, PVOID pValue);
extern int rtw_hal_get_def_var(_adapter *padapter, HAL_DEF_VARIABLE eVariable, PVOID pValue);
extern void rtw_hal_set_odm_var(_adapter *padapter, HAL_ODM_VARIABLE eVariable, PVOID pValue1, BOOLEAN bSet); 
extern void rtw_hal_get_odm_var(_adapter *padapter, HAL_ODM_VARIABLE eVariable, PVOID pValue1, BOOLEAN bSet); 
extern void rtw_hal_enable_interrupt(_adapter *padapter); 
extern void rtw_hal_disable_interrupt(_adapter *padapter); 
extern int rtw_hal_inirp_init(_adapter *padapter);
extern int rtw_hal_inirp_deinit(_adapter *padapter);
extern void rtw_hal_irp_reset(_adapter *padapter); 
extern int32_t rtw_hal_xmit(_adapter *padapter, xmit_frame *pxmitframe); 
extern int32_t rtw_hal_mgnt_xmit(_adapter *padapter, xmit_frame *pmgntframe); 
extern int32_t rtw_hal_init_xmit_priv(_adapter *padapter); 
extern void rtw_hal_free_xmit_priv(_adapter *padapter); 
extern int32_t rtw_hal_init_recv_priv(_adapter *padapter); 
extern void rtw_hal_free_recv_priv(_adapter *padapter); 
extern void rtw_hal_update_ra_mask(sta_info *psta, uint8_t rssi_level); 
extern void rtw_hal_add_ra_tid(_adapter *padapter, uint32_t bitmap, uint8_t *arg, uint8_t rssi_level); 
extern void rtw_hal_update_txdesc(_adapter *padapter, xmit_frame *pxmitframe, uint8_t *pbuf); 
extern void rtw_hal_clone_data(_adapter *dst_padapter, _adapter *src_padapter); 
extern void rtw_hal_start_thread(_adapter *padapter); 
extern void rtw_hal_stop_thread(_adapter *padapter); 
extern int rtw_hal_read_bbreg(_adapter *padapter, uint32_t RegAddr, uint32_t BitMask);
extern void rtw_hal_write_bbreg(_adapter *padapter, uint32_t RegAddr, uint32_t BitMask, uint32_t Data); 
extern int rtw_hal_read_rfreg(_adapter *padapter, uint32_t eRFPath, uint32_t RegAddr, uint32_t BitMask);
extern void rtw_hal_write_rfreg(_adapter *padapter, uint32_t eRFPath, uint32_t RegAddr, uint32_t BitMask, uint32_t Data);
extern int32_t rtw_hal_interrupt_handler(_adapter *padapter); 
extern void rtw_hal_set_bwmode(_adapter *padapter, CHANNEL_WIDTH Bandwidth, uint8_t Offset); 
extern void rtw_hal_set_chan(_adapter *padapter, uint8_t channel); 
extern void rtw_hal_set_chnl_bw(_adapter *padapter, int channel, CHANNEL_WIDTH Bandwidth, uint8_t Offset40, uint8_t Offset80);
extern void rtw_hal_dm_watchdog(_adapter *padapter); 
extern int32_t rtw_hal_recv_tasklet(_adapter *padapter); 
extern int32_t rtw_hal_macid_sleep(PADAPTER padapter, int macid, int a3);
extern int32_t rtw_hal_macid_wakeup(PADAPTER padapter, int macid, int a3);
extern void decide_chip_type_by_device_id(_adapter *padapter); 
//--------------------------------
// hal_phy.o
// Function declarations
extern uint32_t PHY_RFShadowRead(_adapter *Adapter, int eRFPath, uint32_t Offset);
extern void PHY_RFShadowWrite(_adapter *Adapter, int eRFPath, uint32_t Offset, uint32_t Data);
extern int PHY_RFShadowCompare(_adapter *Adapter, int eRFPath, uint32_t Offset);
extern void PHY_RFShadowRecorver(_adapter *Adapter, int eRFPath, uint32_t Offset);
extern void PHY_RFShadowCompareAll(_adapter *Adapter); 
extern void PHY_RFShadowRecorverAll(_adapter *Adapter); 
extern void PHY_RFShadowCompareFlagSet(_adapter *Adapter, int eRFPath, uint32_t Offset, uint8_t Type);
extern void PHY_RFShadowRecorverFlagSet(_adapter *Adapter, int eRFPath, uint32_t Offset, uint8_t Type);
extern void PHY_RFShadowCompareFlagSetAll(_adapter *Adapter); 
extern void PHY_RFShadowRecorverFlagSetAll(_adapter *Adapter); 
extern void PHY_RFShadowRefresh(_adapter *Adapter); 
// Data declarations
extern RF_SHADOW_T RF_Shadow[2][255]; 
//--------------------------------
// Hal8195ARateAdaptive.o
// Function declarations
extern void ODM_InitRAInfo(PDM_ODM_T pDM_Odm); 
extern signed int PT_Mode_Sel(PDM_ODM_T pDM_Odm, PODM_RA_INFO_T pRaInfo, int rate_idx);
extern void InitialRateUpdate(PDM_ODM_T pDM_Odm, PODM_RA_INFO_T pRaInfo, int rate, int trybit, u8 BW);
extern unsigned int RateUp_search_RateMask(PDM_ODM_T pDM_Odm, PODM_RA_INFO_T pRaInfo, u8 init_rate_idx);
extern int RateDown_search_RateMask(PDM_ODM_T pDM_Odm, PODM_RA_INFO_T pRaInfo, u8 init_rate_idx, int mod_step);
extern void StartRateByRSSI(PDM_ODM_T pDM_Odm, PODM_RA_INFO_T pRaInfo); 
extern signed int b64QamRate(int rate_idx, int Up_Down);
extern void RateUpRAM8195A(PDM_ODM_T pDM_Odm, PODM_RA_INFO_T pRaInfo); 
extern void RateDownTrying(PDM_ODM_T pDM_Odm, PODM_RA_INFO_T pRaInfo); 
extern void TryDone(PDM_ODM_T pDM_Odm, PODM_RA_INFO_T pRaInfo); 
extern void RateDownStepRAM8195A(PDM_ODM_T pDM_Odm, PODM_RA_INFO_T pRaInfo, int step);
extern void RateDecisionRAM8195A(PDM_ODM_T pDM_Odm, PODM_RA_INFO_T pRaInfo); 
extern void ArfrRefresh(PDM_ODM_T pDM_Odm, PODM_RA_INFO_T pRaInfo); 
extern void H2CHDL_Set_MACID_Config(PDM_ODM_T pDM_Odm, u8 *pbuf); 
extern void PHY_DM_RA_SetRSSI_8195A(PDM_ODM_T pDM_Odm, int MacID, u8 Rssi);
// Data declarations
extern u8 Noisy_State;
extern u8 ARFB_table[9][7]; /* =
{
  { 21u, 240u, 255u, 15u, 0u, 0u, 0u },
  { 21u, 240u, 15u, 0u, 0u, 0u, 0u },
  { 5u, 240u, 255u, 15u, 0u, 0u, 0u },
  { 5u, 240u, 15u, 0u, 0u, 0u, 0u },
  { 16u, 240u, 255u, 15u, 0u, 0u, 0u },
  { 16u, 240u, 15u, 0u, 0u, 0u, 0u },
  { 245u, 15u, 0u, 0u, 0u, 0u, 0u },
  { 240u, 15u, 0u, 0u, 0u, 0u, 0u },
  { 15u, 0u, 0u, 0u, 0u, 0u, 0u }
};  */
extern u8 TRYING_NECESSARY_idx[20]; /* =
{
  1u,  1u,  1u,  2u,  1u,  2u,  3u,  3u,  4u,  4u,  5u,
  5u,  2u,  4u,  6u,  7u,  7u,  8u,  8u,  8u
};  */
extern u8 DROPING_NECESSARY[20]; /* =
{
  1u,  1u,  1u,  1u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,
  8u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u
};  */
extern u8 PER_RATE_UP[20]; /* =
{
  10u,  10u,  10u,  10u,  20u,  20u,  10u,  10u,  2u,  2u,
  2u,  2u,  20u,  20u,  10u,  10u,  1u,  2u,  2u,  4u
};  */
extern u8 PER_RATE_DOWN[20]; /* =
{
  100u,  50u,  50u,  50u,  40u,  47u,  29u,  36u,  31u,  24u,
  7u,  10u,  40u,  45u,  35u,  30u,  33u,  25u,  11u,  10u
};  */
//--------------------------------
// HalPhyRf.o
// Function declarations
extern void ConfigureTxpowerTrack(PDM_ODM_T pDM_Odm, PTXPWRTRACK_CFG pConfig); 
extern void ODM_ClearTxPowerTrackingState(PDM_ODM_T pDM_Odm); 
extern void ODM_TXPowerTrackingCallback_ThermalMeter(PADAPTER Adapter); 
extern void ODM_ResetIQKResult(PDM_ODM_T pDM_Odm); 
extern int ODM_GetRightChnlPlaceforIQK(int chnl);
// Data declarations
//--------------------------------
// HalPwrSeqCmd.o
// Function declarations
extern signed int HalPwrSeqCmdParsing(_adapter *padapter, uint8_t CutVersion, uint8_t FabVersion, int InterfaceType, WLAN_PWR_CFG *PwrSeqCmd);
// Data declarations
//--------------------------------
// hci_intfs.o
// Function declarations
dvobj_priv *hci_dvobj_init(); 
extern void hci_dvobj_deinit(dvobj_priv *dvobj); 
extern void hci_dvobj_request_irq(dvobj_priv *dvobj); 
extern void hci_dvobj_free_irq(dvobj_priv *dvobj); 
// Data declarations
//--------------------------------
// rtw_ioctl_set.o
// Function declarations
extern int rtw_do_join(_adapter *padapter, _irqL a2);
extern int rtw_set_802_11_bssid(_adapter *padapter, uint8_t *bssid);
extern int rtw_set_802_11_ssid(_adapter *padapter, NDIS_802_11_SSID *ssid, int a3);
extern signed int rtw_set_802_11_infrastructure_mode(_adapter *padapter, NDIS_802_11_NETWORK_INFRASTRUCTURE networktype);
extern signed int rtw_set_802_11_bssid_list_scan(_adapter *padapter, NDIS_802_11_SSID *pssid, int ssid_max_num);
extern int rtw_set_802_11_authentication_mode(_adapter *padapter, NDIS_802_11_AUTHENTICATION_MODE authmode);
extern int rtw_set_802_11_add_wep(_adapter *padapter, NDIS_802_11_WEP *wep);
// Data declarations
//--------------------------------
// rtw_io.o
// Function declarations
extern int rtw_read8(ADAPTER *adapter, uint32_t addr);
extern int rtw_read16(ADAPTER *adapter, uint32_t addr);
extern int rtw_read32(ADAPTER *adapter, uint32_t addr);
extern int32_t rtw_write8(ADAPTER *adapter, uint32_t addr, int val);
extern int32_t rtw_write16(ADAPTER *adapter, uint32_t addr, int val);
extern int32_t rtw_write32(ADAPTER *adapter, uint32_t addr, uint32_t val); 
extern signed int rtw_read_port(ADAPTER *adapter, uint32_t addr, uint32_t cnt, uint8_t *mem, fifo_more_data *more_data);
extern signed int rtw_write_port(ADAPTER *adapter, uint32_t addr, uint32_t cnt, uint8_t *mem);
extern void rtw_set_chip_endian(ADAPTER *adapter); 
extern int rtw_get_chip_endian(PADAPTER padapter); 
// Data declarations
//--------------------------------
// rtw_cmd.o
// Function declarations
sint rtw_init_cmd_priv(cmd_priv *pcmdpriv); 
sint rtw_init_evt_priv(evt_priv *pevtpriv); 
void rtw_free_cmd_priv(cmd_priv *pcmdpriv); 
sint rtw_enqueue_cmd(_queue *queue, cmd_obj *obj); 
cmd_obj *rtw_dequeue_cmd(_queue *queue, _irqL a2, int a3);
list_head *rtw_observequeue_cmd(_queue *queue);
signed int rtw_init_cmd_priv(cmd_priv *pcmdpriv);
int rtw_cmd_filter(cmd_priv *pcmdpriv, cmd_obj *cmd_obj); 
void rtw_free_cmd_obj(cmd_obj *pcmd); 
int rtw_enqueue_cmd(cmd_priv *pcmdpriv, cmd_obj *cmd_obj);
void rtw_set_channel_plan_cmd_callback(_adapter *padapter, cmd_obj *pcmd); 
void rtw_survey_cmd_callback(_adapter *padapter, cmd_obj *pcmd); 
void rtw_disassoc_cmd_callback(_adapter *padapter, cmd_obj *pcmd, int a3);
void rtw_joinbss_cmd_callback(_adapter *padapter, cmd_obj *pcmd); 
void rtw_setstaKey_cmdrsp_callback(_adapter *padapter, cmd_obj *pcmd); 
void rtw_cmd_thread(thread_context context); 
int rtw_createbss_cmd(_adapter *padapter);
int rtw_joinbss_cmd(_adapter *padapter, wlan_network *pnetwork);
int rtw_disassoc_cmd(_adapter *padapter);
int rtw_setopmode_cmd(_adapter *padapter, NDIS_802_11_NETWORK_INFRASTRUCTURE networktype);
int rtw_setstakey_cmd(_adapter *padapter, uint8_t *psta, int unicast_key);
int rtw_clearstakey_cmd(_adapter *padapter, uint8_t *psta, uint8_t entry, int enqueue);
int rtw_addbareq_cmd(_adapter *padapter, int tid, uint8_t *addr);
int rtw_dynamic_chk_wk_cmd(_adapter *padapter);
cmd_obj *rtw_set_chplan_cmd(_adapter *padapter, int chplan, int enqueue);
void dynamic_chk_wk_hdl(_adapter *padapter, uint8_t *pbuf, int sz);
void lps_ctrl_wk_hdl(_adapter *padapter, int lps_ctrl_type, int a3);
int rtw_lps_ctrl_wk_cmd(_adapter *padapter, int lps_ctrl_type, int enqueue);
cmd_obj *rtw_sitesurvey_cmd(_adapter *padapter, NDIS_802_11_SSID *pssid, int ssid_max_num);
void rpt_timer_setting_wk_hdl(_adapter *padapter, uint16_t minRptTime, int a3);
int rtw_rpt_timer_cfg_cmd(_adapter *padapter, int minRptTime);
int rtw_ps_cmd(_adapter *padapter);
int rtw_chk_hi_queue_cmd(_adapter *padapter);
signed int rtw_drvextra_cmd_hdl(_adapter *padapter, unsigned __int8 *pbuf, int a3);
int rtw_c2h_wk_cmd(PADAPTER padapter);
// Data declarations
const cmd_hdl wlancmds[63]l
const _cmd_callback rtw_cmd_callback[63];
//--------------------------------
// netdev.o
// Function declarations
net_device *rtw_alloc_etherdev_with_old_priv(int sizeof_priv, void *old_priv); 
net_device *rtw_alloc_etherdev(int sizeof_priv); 
void rtw_free_netdev(net_device *netdev); 
// Data declarations
//--------------------------------
// phydm.o
// Function declarations
void ODM_CmnInfoInit(PDM_ODM_T pDM_Odm, ODM_CMNINFO_E CmnInfo, u4Byte Value); 
void ODM_CmnInfoHook(PDM_ODM_T pDM_Odm, ODM_CMNINFO_E CmnInfo, PVOID pValue); 
void ODM_CmnInfoPtrArrayHook(PDM_ODM_T pDM_Odm, ODM_CMNINFO_E CmnInfo, int Index, PVOID pValue);
void ODM_CmnInfoUpdate(PDM_ODM_T pDM_Odm, u4Byte CmnInfo, u8Byte Value); 
void odm_CommonInfoSelfInit(PDM_ODM_T pDM_Odm); 
void ODM_DMInit(PDM_ODM_T pDM_Odm); 
void odm_CommonInfoSelfUpdate(PDM_ODM_T pDM_Odm); 
void ODM_DMWatchdog(PDM_ODM_T pDM_Odm); 
void odm_CommonInfoSelfReset(PDM_ODM_T pDM_Odm); 
ADAPTIVITY_STATISTICS *PhyDM_Get_Structure(PDM_ODM_T pDM_Odm, int Structure_Type);
void odm_SwAntDetectInit(PDM_ODM_T pDM_Odm); 
// Data declarations
//--------------------------------
// rtl8195a_phycfg.o
// Function declarations
int PHY_QueryBBReg_8195A_Safe(PADAPTER Adapter, uint32_t RegAddr, uint32_t BitMask);
void PHY_SetBBReg_8195A_Safe(PADAPTER Adapter, uint32_t RegAddr, uint32_t BitMask, uint32_t Data); 
int phy_RFSerialRead_8195A(PADAPTER Adapter, RF_PATH eRFPath, uint32_t Offset);
uint32_t PHY_QueryRFReg_8195A(PADAPTER Adapter, uint32_t eRFPath, uint32_t RegAddr, uint32_t BitMask); 
void PHY_SetRFReg_8195A(PADAPTER Adapter, uint32_t eRFPath, uint32_t RegAddr, uint32_t BitMask, uint32_t Data);
BOOL PHY_MACConfig8195A(PADAPTER Adapter);
int PHY_BBConfig8195A(PADAPTER Adapter); 
int PHY_ConfigRFWithParaFile_8195A(PADAPTER Adapter, uint8_t *pFileName, RF_PATH eRFPath); 
void phy_PowerIndexCheck8195A(PADAPTER Adapter, uint8_t channel, uint8_t *cckPowerLevel, uint8_t *ofdmPowerLevel, uint8_t *BW20PowerLevel, uint8_t *BW40PowerLevel);
void PHY_SetTxPowerIndex_8195A(PADAPTER Adapter, uint32_t PowerIndex, int RFPath, int Rate);
void phy_TxPwrAdjInPercentage(PADAPTER Adapter, uint8_t *pTxPwrIdx); 
int PHY_GetTxPowerIndex_8195A(PADAPTER pAdapter, int RFPath, int Rate, CHANNEL_WIDTH BandWidth, uint8_t Channel);
void PHY_SetTxPowerLevel8195A(PADAPTER Adapter, int Channel);
void phy_SpurCalibration_8195A(PADAPTER pAdapter); 
void phy_SetRegBW_8195A(PADAPTER Adapter, CHANNEL_WIDTH CurrentBW); 
int phy_GetSecondaryChnl_8195A(PADAPTER Adapter);
void phy_PostSetBwMode8195A(PADAPTER Adapter); 
void phy_SwChnl8195A(PADAPTER pAdapter); 
void phy_SwChnlAndSetBwMode8195A(PADAPTER Adapter); 
void PHY_HandleSwChnlAndSetBW8195A(PADAPTER Adapter, int bSwitchChannel, int bSetBandWidth, uint8_t ChannelNum, CHANNEL_WIDTH ChnlWidth, EXTCHNL_OFFSET ExtChnlOffsetOf40MHz, EXTCHNL_OFFSET ExtChnlOffsetOf80MHz, uint8_t CenterFrequencyIndex1);
void PHY_SetBWMode8195A(PADAPTER Adapter, CHANNEL_WIDTH Bandwidth, int Offset);
void PHY_SwChnl8195A(PADAPTER Adapter, uint8_t channel); 
void PHY_SetSwChnlBWMode8195A(PADAPTER Adapter, uint8_t channel, CHANNEL_WIDTH Bandwidth, int Offset40, uint8_t Offset80);
// Data declarations
//--------------------------------
// rtl8195a_pmu_cmd.o
// Function declarations
void MediaConnection(PADAPTER padapter, int macid);
void MediaDisconnection(PADAPTER padapter, int macid);
void RATaskEnable(PADAPTER padapter); 
void SetMediaStatus(PADAPTER padapter, int macid, int status);
void H2CHDL_JoinInfo(PADAPTER padapter, uint8_t *pCmdBuffer); 
void H2CHDL_SetRsvdPage(PADAPTER padapter, uint8_t *pCmdBuffer); 
uint32_t H2CCmdCommon(PADAPTER padapter, int ElementID, uint8_t *pCmdBuffer);
// Data declarations
//--------------------------------
// rtl8195a_pmu_task.o
// Function declarations
void HalTimerEnable(uint32_t TimerId); 
void InitTDMATimer(int Period);
void ChangeStateByTDMA(PADAPTER padapter); 
void GetMinRateInRRSR(PADAPTER padapter); 
void CheckInReqState(PADAPTER padapter); 
void InitCheckStateTimer(); 
void InitGTimer1ms(PADAPTER padapter, uint8_t IRQDis, int TimerID, uint32_t Period);
void DeInitGTimer1ms(PADAPTER padapter, int TimerID);
void ChangeTransmiteRate(int offset, uint8_t rate);
void PowerBitSetting(int bPowerBit, int offset);
void ChkandChangePS(PPS_PARM pPSParm, int bPowerBit);
int IssueRsvdPagePacketSetting(int PageNum, int bHwSEQEn, uint8_t RtyLmt);
void InitRsvdPgPkt(); 
void IssuePSPoll(); 
signed int WaitTxStateMachineOk();
signed int IssueNullData(PPS_PARM pPSParm, int bPowerBit, uint8_t RtyLmt);
void WriteTxPause(uint8_t value, uint8_t rcode); 
void PsCloseRF(); 
void PsOpenRF(); 
void SetPwrStateReg(PPS_PARM pPSParm, int PwrStateType, uint8_t value);
BOOL ChkTxQueueIsEmpty();
void InitPS(PADAPTER padapter); 
void ConfigListenBeaconPeriod(PPS_PARM pPSParm, int RLBM, int AwakeInterval);
signed int PS_S2_Condition_Match(PPS_PARM pPSParm);
signed int PS_S4_Condition_Match(PADAPTER padapter);
unsigned int PS_32K_Condition_Match();
void PS_S2ToS3ToS0State(PADAPTER padapter, int nulldata0Allow);
void PS_S2ToS0State(PPS_PARM pPSParm); 
void PS_S3ToS2orS0State(PPS_PARM pPSParm); 
void PS_S0ToS1ToS2State(PADAPTER padapter); 
void PS_S1ToS0orS2State(PPS_PARM pPSParm); 
void PS_S2ToS4State(PADAPTER padapter); 
void PS_S2ToS5State(PPS_PARM pPSParm); 
void PS_S5ToS2State(PPS_PARM pPSParm); 
void PS_S0ToS6State(PADAPTER padapter); 
void PS_S6ToS0State(PPS_PARM pPSParm); 
void CheckTSFIsStable(int ReqState);
void WaitHWStateReady(); 
void SysClkDown(PPS_PARM pPSParm); 
void SysClkUp(PPS_PARM pPSParm); 
void ResetPSParm(PADAPTER padapter); 
void PS_S4ToS2State(PPS_PARM pPSParm, int ReleaseTxPause);
void SleepTo32K(PPS_PARM pPSParm); 
void Change_PS_State(PADAPTER padapter, int request_ps_state, int nulldata0Allow);
void Legacy_PS_Setting(PADAPTER padapter); 
void PSModeSetting(PADAPTER padapter, int on);
void ChangePSStateByRPWM(PADAPTER padapter); 
void ChangeTDMAState(PADAPTER padapter); 
void TDMAChangeStateTask(PADAPTER padapter, _irqL a2);
void EnterPS(PADAPTER padapter); 
void SetSmartPSTimer(PADAPTER padapter); 
void GTimer7Handle(void *Data); 
void SmartPS2InitTimerAndToGetRxPkt(PADAPTER padapter); 
void PS_OnBeacon(PADAPTER padapter); 
void PSBcnEarlyProcess(PADAPTER padapter); 
void PSMtiBcnEarlyProcess(PADAPTER padapter); 
void PSRxBcnProcess(PADAPTER padapter); 
void TxPktInPSOn(PADAPTER padapter); 
void PsBcnToProcess(PADAPTER padapter); 
void GTimer6Handle(void *Data); 
signed int RPWMProcess(PADAPTER padapter, int benter32k);
void PSSetMode(PADAPTER padapter, PLEGACY_PS_PARM pparm); 
void SpeRPT(PADAPTER padapter); 
void ISR_BcnEarly(PADAPTER padapter); 
void ISR_MtiBcnEarly(PADAPTER padapter); 
void ISR_RxBcn(PADAPTER padapter); 
void ISR_RxBCMD1(PADAPTER padapter); 
void ISR_RxBCMD0(PADAPTER padapter); 
void ISR_RxUCMD1(PADAPTER padapter); 
void ISR_RxUCMD0(PADAPTER padapter); 
void ISR_TxPktIn(PADAPTER padapter); 
void H2CHDL_SetPwrMode(PADAPTER padapter, uint8_t *pCmdBuffer); 
void CheckInReqStateTask(PADAPTER padapter, int a2, int a3);
uint32_t HalGetNullTxRpt(PADAPTER padapter); 
void ISR_TBTT(PADAPTER padapter); 
void H2CHDL_BcnIgnoreEDCCA(PADAPTER padapter, uint8_t *pCmdBuffer); 
void PMUInitial(PADAPTER padapter); 
void PMUTask(PADAPTER padapter); 
// Data declarations
BOOL bCheckStateTIMER; 
uint32_t WifiMcuCmdBitMap_20974; 
//--------------------------------
// rtl8195a_recv.o
// Function declarations
int32_t rtl8195a_init_recv_priv(_adapter *padapter); 
// Data declarations
//--------------------------------
// rtl8195a_rxdesc.o
// Function declarations
void process_rssi(_adapter *padapter, recv_frame *prframe); 
int32_t translate2dbm(int signal_strength_idx);
void rtl8195a_query_rx_desc_status(recv_frame *precvframe, uint8_t *pdesc); 
void rtl8195a_query_rx_phy_status(recv_frame *precvframe, uint8_t *pphy_status, int a3);
// Data declarations
//--------------------------------
// rtl8195a_xmit.o
// Function declarations
uint8_t *GetTxBufDesc(_adapter *padapter, int queue_index);
void UpdateFirstTxbdtoXmitBuf(_adapter *padapter, xmit_frame *pxmitframe);
BOOL check_nic_enough_desc(_adapter *padapter, pkt_attrib *pattrib);
int32_t rtl8195ab_init_xmit_priv(PADAPTER padapter); 
void rtl8195ab_free_xmit_priv(PADAPTER padapter); 
uint32_t GetDmaTxbdIdx(uint32_t ff_hwaddr); 
xmit_buf *rtl8195a_enqueue_xmitbuf(rtw_tx_ring *ring, xmit_buf *pxmitbuf);
list_head *rtl8195a_dequeue_xmitbuf(rtw_tx_ring *ring);
signed int SetTxbdForLxDMARtl8195ab(_adapter *padapter, xmit_frame *pxmitframe, tx_buf_desc *pTxbd);
void UpdateTxbdHostIndex(_adapter *padapter, uint32_t ff_hwaddr); 
xmit_buf *SetXimtBuf(xmit_frame *pxmitframe); 
int FreeXimtBuf(xmit_buf *pxmitbuf);
int rtw_dump_xframe(_adapter *padapter, xmit_frame *pxmitframe);
BOOL check_tx_desc_resource(_adapter *padapter, int prio);
list_head *rtw_dequeue_xframe(xmit_priv *pxmitpriv, hw_xmit *phwxmit_i, sint entry);
int32_t rtw_xmitframe_coalesce(_adapter *padapter, _pkt *pkt, xmit_frame *pxmitframe); 
void rtl8195ab_xmitframe_resume(_adapter *padapter); 
int32_t rtl8195ab_mgnt_xmit(_adapter *padapter, xmit_frame *pmgntframe); 
int32_t rtl8195ab_hal_xmit(_adapter *padapter, xmit_frame *pxmitframe); 
int32_t rtl8195ab_hal_xmitframe_enqueue(_adapter *padapter, xmit_frame *pxmitframe); 
// Data declarations
//--------------------------------
// rtw_intfs.o
// Function declarations
signed int rtw_init_default_value(_adapter *padapter);
void rtw_cancel_all_timer(_adapter *padapter); 
signed int rtw_free_drv_sw(_adapter *padapter);
signed int rtw_reset_drv_sw(_adapter *padapter);
signed int rtw_init_drv_sw(_adapter *padapter);
int rtw_start_drv_threads(_adapter *padapter);
void rtw_stop_drv_threads(_adapter *padapter); 
// Data declarations
//--------------------------------
// rtw_mlme.o
// Function declarations
void rtw_free_mlme_ie_data(uint8_t **ppie, uint32_t *plen); 
void rtw_init_mlme_timer(_adapter *padapter); 
void rtw_del_mlme_timer(mlme_priv *pmlmepriv); 
void reconnect_timer_hdl(void *FunctionContext); 
uint8_t *rtw_init_mlme_priv(_adapter *padapter, int a2, int a3);
void rtw_mfree_mlme_priv_lock(mlme_priv *pmlmepriv); 
void rtw_free_mlme_priv_ie_data(mlme_priv *pmlmepriv); 
void rtw_free_mlme_priv(mlme_priv *pmlmepriv); 
sint rtw_enqueue_network(_queue *queue, wlan_network *pnetwork); 
list_head *rtw_alloc_network(mlme_priv *pmlmepriv, _irqL a2, int a3);
void rtw_free_network(mlme_priv *pmlmepriv, wlan_network *pnetwork, int isfreeall);
void rtw_free_network_nolock(mlme_priv *pmlmepriv, wlan_network *pnetwork); 
_queue *rtw_find_network(_queue *scanned_queue, uint8_t *addr);
void rtw_free_network_queue(_adapter *padapter, int isfreeall, int a3);
sint rtw_if_up(_adapter *padapter); 
void rtw_generate_random_ibss(uint8_t *pibss); 
uint8_t *rtw_get_capability_from_ie(uint8_t *ie); 
int rtw_get_capability(WLAN_BSSID_EX *bss, int a2, int a3);
uint8_t *rtw_get_beacon_interval_from_ie(uint8_t *ie); 
uint8_t *rtw_init_mlme_priv(_adapter *padapter, int a2, int a3);
uint32_t rtw_is_same_ibss(_adapter *adapter, wlan_network *pnetwork);
int is_same_network(WLAN_BSSID_EX *src, WLAN_BSSID_EX *dst, int a3);
list_head *rtw_get_oldest_wlan_network(_queue *scanned_queue);
void update_network(WLAN_BSSID_EX *dst, WLAN_BSSID_EX *src, _adapter *padapter, int update_ie);
void rtw_update_scanned_network(_adapter *adapter, WLAN_BSSID_EX *target); 
void rtw_add_network(_adapter *adapter, WLAN_BSSID_EX *pnetwork, int a3);
void rtw_survey_event_callback(_adapter *adapter, uint8_t *pbuf, int a3);
void rtw_free_assoc_resources(_adapter *adapter, int lock_scanned_queue); 
void rtw_indicate_connect(_adapter *padapter); 
void rtw_indicate_disconnect(_adapter *padapter); 
void rtw_joinbss_event_callback(_adapter *adapter, uint8_t *pbuf); 
signed int search_max_mac_id(_adapter *padapter);
void rtw_stassoc_hw_rpt(_adapter *adapter, sta_info *psta); 
void rtw_stassoc_event_callback(_adapter *adapter, uint8_t *pbuf); 
void rtw_stadel_event_callback(_adapter *adapter, uint8_t *pbuf); 
void rtw_join_timeout_handler(_adapter *adapter, _irqL a2, int a3, int a4);
void rtw_join_timeout_handler(void *FunctionContext, _irqL a2, int a3, int a4);
void rtw_scan_timeout_handler(_adapter *adapter, _irqL a2, int a3);
void rtw_scan_timeout_handler(void *FunctionContext); 
void rtw_dynamic_check_timer_handlder(_adapter *adapter); 
void dynamic_check_timer_handlder(void *FunctionContext); 
int rtw_select_and_join_from_scanned_queue(mlme_priv *pmlmepriv, _irqL a2, int a3);
void rtw_surveydone_event_callback(_adapter *adapter, uint8_t *pbuf); 
sint rtw_set_auth(_adapter *adapter, security_priv *psecuritypriv); 
sint rtw_set_key(_adapter *adapter, security_priv *psecuritypriv, sint keyid, uint8_t set_tx); 
unsigned int rtw_restruct_wmm_ie(_adapter *adapter, uint8_t *in_ie, uint8_t *out_ie, unsigned int in_len, unsigned int initial_out_len);
sint rtw_restruct_sec_ie(_adapter *adapter, uint8_t *in_ie, uint8_t *out_ie, unsigned int in_len); 
void rtw_joinbss_reset(_adapter *padapter); 
unsigned int rtw_restructure_ht_ie(_adapter *padapter, uint8_t *in_ie, uint8_t *out_ie, unsigned int in_len, unsigned int *pout_len);
void rtw_update_ht_cap(_adapter *padapter, uint8_t *pie, unsigned int ie_len); 
void rtw_joinbss_event_prehandle(_adapter *adapter, uint8_t *pbuf, int a3);
void rtw_issue_addbareq_cmd(_adapter *padapter, xmit_frame *pxmitframe); 
sint rtw_linked_check(_adapter *padapter); 
sint rtw_buddy_adapter_up(sint result);
sint check_buddy_fwstate(sint result, sint state);
// Data declarations
uint8_t auto_reconnect_running; 
void (*p_wlan_autoreconnect_hdl)(rtw_security_t, char *, int, char *, int, int); 
//--------------------------------
// rtw_mlme_ext.o
// Function declarations
unsigned __int8 *get_da(unsigned __int8 *pframe); 
unsigned __int8 *get_sa(unsigned __int8 *pframe); 
signed int OnAction(_adapter *padapter, recv_frame *precv_frame);
signed int DoReserved(_adapter *padapter, recv_frame *precv_frame);
void mgt_dispatcher(_adapter *padapter, mlme_handler *ptable, recv_frame *precv_frame);
int rtw_is_channel_set_contains_channel(RT_CHANNEL_INFO *channel_set, const uint32_t channel_num, int *pchannel_idx); 
int init_hw_mlme_ext(_adapter *padapter); 
unsigned int init_channel_set(_adapter *padapter, int ChannelPlan, RT_CHANNEL_INFO *channel_set);
void free_mlme_ext_priv(mlme_ext_priv *pmlmeext); 
void mgt_dispatcher(_adapter *padapter, recv_frame *precv_frame, int a3);
unsigned int OnAction_public(_adapter *padapter, recv_frame *precv_frame); 
signed int OnAction_p2p(_adapter *padapter, recv_frame *precv_frame);
xmit_frame *alloc_mgtxmitframe(xmit_priv *pxmitpriv); 
xmit_frame *alloc_FwRsvdframe(xmit_priv *pxmitpriv, uint32_t size); 
void update_mgnt_tx_rate(_adapter *padapter, uint8_t rate); 
void update_mgntframe_attrib(_adapter *padapter, pkt_attrib *pattrib); 
xmit_frame *rtw_build_mgnt_frame(_adapter *padapter, uint8_t *data, int len);
void dump_mgntframe(_adapter *padapter, xmit_frame *pmgntframe); 
int rtw_send_mgnt(_adapter *padapter, uint8_t *data, int len, uint16_t flags);
void issue_action_BSSCoexistPacket(_adapter *padapter); 
uint32_t update_hidden_ssid(uint8_t *ies, uint32_t ies_len, int hidden_ssid_mode);
void issue_beacon(_adapter *padapter); 
void issue_probersp(_adapter *padapter, unsigned __int8 *da, uint8_t is_valid_p2p_probereq); 
signed int OnProbeReq(_adapter *padapter, recv_frame *precv_frame);
void issue_probereq(_adapter *padapter, NDIS_802_11_SSID *pssid, int blnbc);
void issue_auth(_adapter *padapter, sta_info *psta, int status);
signed int OnAuth(_adapter *padapter, recv_frame *precv_frame);
void issue_asocrsp(_adapter *padapter, unsigned __int16 status, sta_info *pstat, int pkt_type); 
void issue_assocreq(_adapter *padapter); 
void issue_nulldata(_adapter *padapter, unsigned int power_mode); 
void issue_qos_nulldata(_adapter *padapter, unsigned __int8 *da, uint16_t tid); 
void issue_deauth(_adapter *padapter, unsigned __int8 *da, uint32_t reason); 
void issue_action_BA(_adapter *padapter, unsigned __int8 *raddr, unsigned __int8 action, unsigned __int16 status); 
signed int OnAction_back(_adapter *padapter, recv_frame *precv_frame);
signed int send_beacon(_adapter *padapter);
signed int collect_bss_info(_adapter *padapter, recv_frame *precv_frame, WLAN_BSSID_EX *bssid);
void start_clnt_auth(_adapter *padapter); 
void start_clnt_assoc(_adapter *padapter); 
signed int OnAuthClient(_adapter *padapter, recv_frame *precv_frame);
int report_scan_result_one(_adapter *padapter, WLAN_BSSID_EX *bssid); 
int add_site_survey(_adapter *padapter, WLAN_BSSID_EX *bssid); 
void report_survey_event(_adapter *padapter, recv_frame *precv_frame); 
signed int OnProbeRsp(_adapter *padapter, recv_frame *precv_frame);
void report_surveydone_event(_adapter *padapter); 
void report_join_res(_adapter *padapter, int res); 
signed int OnAssocRsp(_adapter *padapter, recv_frame *precv_frame);
void report_del_sta_event(_adapter *padapter, unsigned __int8 *MacAddr, unsigned __int16 reason); 
signed int receive_disconnect(_adapter *padapter, unsigned __int8 *MacAddr, unsigned __int16 reason);
signed int OnBeacon(_adapter *padapter, recv_frame *precv_frame);
signed int OnDeAuth(_adapter *padapter, recv_frame *precv_frame);
signed int OnDisassoc(_adapter *padapter, recv_frame *precv_frame);
void report_add_sta_event(_adapter *padapter, unsigned __int8 *MacAddr, int cam_idx); 
signed int OnAssocReq(_adapter *padapter, recv_frame *precv_frame);
signed int rtw_port_switch_chk(_adapter *adapter);
void update_sta_info(_adapter *padapter, sta_info *psta); 
void mlmeext_sta_del_event_callback(_adapter *padapter); 
void linked_info_dump(_adapter *padapter, _irqL a2, int a3);
void linked_rx_signal_strehgth_display(_adapter *padapter, int a2);
void linked_status_chk(_adapter *padapter, int a2);
void survey_timer_hdl(_adapter *padapter, int a2);
void survey_timer_hdl(void *FunctionContext); 
void link_timer_hdl(_adapter *padapter); 
void link_timer_hdl(void *FunctionContext); 
void addba_timer_hdl(sta_info *psta); 
int NULL_hdl(_adapter *padapter, uint8_t *pbuf);
int setopmode_hdl(_adapter *padapter, uint8_t *pbuf, int a3);
int disconnect_hdl(_adapter *padapter, unsigned __int8 *pbuf, int a3);
int setauth_hdl(_adapter *padapter, unsigned __int8 *pbuf);
int setkey_hdl(_adapter *padapter, uint8_t *pbuf, int a3, int a4);
signed int set_stakey_hdl(_adapter *padapter, uint8_t *pbuf);
int set_tx_beacon_cmd(_adapter *padapter);
int mlme_evt_hdl(_adapter *padapter, unsigned __int8 *pbuf);
int tx_beacon_hdl(_adapter *padapter, unsigned __int8 *pbuf, int a3);
sint check_buddy_mlmeinfo_state(sint result, uint32_t state);
void site_survey(_adapter *padapter, int a2, int a3);
int sitesurvey_cmd_hdl(_adapter *padapter, uint8_t *pbuf, int a3);
int concurrent_chk_start_clnt_join(_adapter *padapter); 
void start_clnt_join(_adapter *padapter, int a2, int a3);
signed int join_cmd_hdl(_adapter *padapter, uint8_t *pbuf, int a3);
void concurrent_chk_joinbss_done(_adapter *padapter, int join_res); 
void mlmeext_joinbss_event_callback(_adapter *padapter, int join_res, int a3);
signed int set_chplan_hdl(_adapter *padapter, unsigned __int8 *pbuf);
void init_mlme_ext_timer(_adapter *padapter); 
int init_mlme_ext_priv(_adapter *padapter); 
// Data declarations
const RT_CHANNEL_PLAN_2G RTW_ChannelPlan2G[8]; /* =
{
  { { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 0u }, 13u },
  { { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 0u }, 13u },
  { { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 0u, 0u, 0u }, 11u },
  { { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u }, 14u },
  { { 10u, 11u, 12u, 13u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u }, 4u },
  { { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 0u }, 13u },
  { { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 0u }, 13u },
  { { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 0u }, 13u }
};  */
const uint8_t WPS_OUI[4]; // = { 0u, 80u, 242u, 4u }; 
mac_monitor_ptr mac_monitor_callback; 
mlme_handler mlme_sta_tbl[14]; /* =
{
  { 0u, (unsigned int (*)(_adapter *, recv_frame *))0x2FA1 },
  { 16u, (unsigned int (*)(_adapter *, recv_frame *))0x28A9 },
  { 32u, (unsigned int (*)(_adapter *, recv_frame *))0x2FA1 },
  { 48u, (unsigned int (*)(_adapter *, recv_frame *))0x28A9 },
  { 64u, (unsigned int (*)(_adapter *, recv_frame *))0xAB5 },
  { 80u, (unsigned int (*)(_adapter *, recv_frame *))0x27A1 },
  { 0u, (unsigned int (*)(_adapter *, recv_frame *))0x61 },
  { 0u, (unsigned int (*)(_adapter *, recv_frame *))0x61 },
  { 128u, (unsigned int (*)(_adapter *, recv_frame *))0x2ADD },
  { 144u, (unsigned int (*)(_adapter *, recv_frame *))0x61 },
  { 160u, (unsigned int (*)(_adapter *, recv_frame *))0x2DBD },
  { 176u, (unsigned int (*)(_adapter *, recv_frame *))0x213D },
  { 192u, (unsigned int (*)(_adapter *, recv_frame *))0x2C65 },
  { 208u, (unsigned int (*)(_adapter *, recv_frame *))0x4F }
};  */
_UNKNOWN unk_4AE4; // weak
list_head *mf_list_head; 
const uint8_t WMM_INFO_OUI[6]; // = { 0u, 80u, 242u, 2u, 0u, 1u }; 
uint8_t pscan_retry_cnt_21430; 
const uint8_t RTW_WPA_OUI[4]; // = { 0u, 80u, 242u, 1u }; 
const uint8_t WMM_PARA_OUI[6]; // = { 0u, 80u, 242u, 2u, 1u, 1u }; 
const RT_CHANNEL_PLAN_MAP RTW_ChannelPlanMap[8]; /* =
{
  { 32u, 0u, 3u },
  { 33u, 1u, 2u },
  { 34u, 2u, 0u },
  { 35u, 3u, 1u },
  { 36u, 4u, 2u },
  { 42u, 5u, 0u },
  { 71u, 7u, 4u },
  { 88u, 6u, 1u }
};  */
const uint8_t null_addr[6]; // = { 0u, 0u, 0u, 0u, 0u, 0u }; 
const uint8_t WMM_OUI[4]; // = { 0u, 80u, 242u, 2u }; 
const fwevent wlanevents[24]; /* =
{
  { 0u, &rtw_dummy_event_callback },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, &rtw_survey_event_callback },
  { 4u, &rtw_surveydone_event_callback },
  { 0u, &rtw_joinbss_event_callback },
  { 12u, &rtw_stassoc_event_callback },
  { 12u, &rtw_stadel_event_callback },
  { 0u, NULL },
  { 0u, &rtw_dummy_event_callback },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, NULL },
  { 0u, NULL }
};  */
//--------------------------------
// rtw_promisc.o
// Function declarations
unsigned __int8 *get_hdr_bssid(unsigned __int8 *pframe); 
int filter_packet(unsigned __int8 *buf, int length);
signed int promisc_get_encrypt(_adapter *padapter, uint8_t *bssid);
void promisc_info_get(_adapter *padapter, recv_frame *prframe, ieee80211_frame_info_t *ppromisc_info, int a4);
void promisc_set_enable(_adapter *padapter, int enabled, int len_used);
void promisc_deinit(_adapter *padapter); 
int promisc_recv_func(_adapter *padapter, recv_frame *rframe); 
int promisc_set(rtw_rcr_level_t enabled, void (*callback)(unsigned __int8 *, unsigned int, void *), int len_used);
int promisc_set_mgntframe(int result);
int is_promisc_enabled();
void promisc_issue_probereq(); 
void promisc_issue_probersp(unsigned __int8 *da); 
int promisc_get_fixed_channel(void *fixed_bssid, uint8_t *ssid, int *ssid_length); 
// Data declarations
void (*promisc_callback_all)(unsigned __int8 *, unsigned int, void *); 
_sema promisc_sema; 
const unsigned __int8 zero_bssid[6]; // = { 0u, 0u, 0u, 0u, 0u, 0u }; 
void (*promisc_callback)(unsigned __int8 *, unsigned int, void *); 
//--------------------------------
// rtw_psk.o
// Function declarations
void SetEAPOL_KEYIV(OCTET_STRING ocDst, __int64 a2, OCTET32_INTEGER oc32Counter);
void ToDrv_SetPTK(_adapter *padapter, sta_info *psta); 
void Message_ReplayCounter_OC2LI(int a1, LARGE_INTEGER *li);
int Message_SmallerEqualReplayCounter(LARGE_INTEGER li1, int a2);
void Message_setReplayCounter(int a1, unsigned int h, unsigned int l);
void INCLargeInteger(LARGE_INTEGER *x); 
void INCOctet16_INTEGER(OCTET16_INTEGER *x); 
OCTET32_INTEGER *INCOctet32_INTEGER(OCTET32_INTEGER *x); 
void ToDrv_DisconnectSTA(_adapter *padapter, sta_info *psta, int reason); 
int CheckMIC(OCTET_STRING EAPOLMsgRecvd, unsigned __int8 *key, int keylen);
void CalcMIC(OCTET_STRING EAPOLMsgSend, int algo, unsigned __int8 *key, int keylen);
int DecWPA2KeyData(WPA_STA_INFO *pStaInfo, unsigned __int8 *key, int keylen, unsigned __int8 *kek, int keklen, unsigned __int8 *kout);
int DecGTK(OCTET_STRING EAPOLMsgRecvd, unsigned __int8 *kek, int keklen, int keylen, unsigned __int8 *kout);
void ToDrv_SetGTK(_adapter *padapter); 
void init_wpa_sta_info(_adapter *padapter, sta_info *psta); 
void SendEAPOL(_adapter *padapter, sta_info *psta, int resend); 
void ClientSendEAPOL(_adapter *padapter, sta_info *psta, int resend); 
void ResendTimeout(void *task_psta, _irqL a2);
void EAPOLKeyRecvd(_adapter *padapter, sta_info *psta); 
void ClientEAPOLKeyRecvd(_adapter *padapter, sta_info *psta); 
void set_wpa_global_PSK(unsigned __int8 *key); 
void psk_derive(_adapter *padapter, unsigned __int8 *passphrase, unsigned __int8 *ssid); 
void psk_init(_adapter *padapter, unsigned __int8 *pie, int ielen);
int psk_strip_rsn_pairwise(uint8_t *ie, int ie_len);
int psk_strip_wpa_pairwise(uint8_t *ie, int ie_len);
int tkip_send_mic_failure_report(_adapter *padapter); 
// Data declarations
uint8_t psk_essid[2][36]; 
uint8_t psk_passphrase[2][65]; 
char PMKID_KDE_TYPE_17744[6]; 
uint8_t wpa_global_PSK[2][40]; 
//--------------------------------
// rtw_pwrctrl.o
// Function declarations
void pwr_state_check_handler(void *FunctionContext); 
void ips_enter(_adapter *padapter); 
int ips_leave(_adapter *padapter); 
signed int rtw_pwr_unassociated_idle(_adapter *adapter);
void rtw_ps_processor(_adapter *padapter); 
void rtw_set_rpwm(PADAPTER padapter, uint8_t pslv); 
int PS_RDY_CHECK(_adapter *padapter);
void rtw_set_ps_mode(PADAPTER padapter, int ps_mode, int smart_ps, int bcn_ant_mode);
int32_t LPS_RF_ON_check(PADAPTER padapter, uint32_t delay_ms); 
void LPS_Enter(PADAPTER padapter); 
void LPS_Leave(PADAPTER padapter); 
void LeaveAllPowerSaveMode(PADAPTER Adapter); 
void rtw_init_pwrctrl_priv(PADAPTER padapter); 
void rtw_free_pwrctrl_priv(PADAPTER adapter); 
int rtw_pwr_wakeup(_adapter *padapter, uint32_t ips_deffer_ms, const char *caller); 
int rtw_pm_set_lps(_adapter *padapter, int mode);
int rtw_pm_set_ips(_adapter *padapter, int mode);
int rtw_pm_set_tdma_param(_adapter *padapter, uint8_t tdma_slot_period, uint8_t tdma_rfon_period_len_1, uint8_t tdma_rfon_period_len_2, uint8_t tdma_rfon_period_len_3);
int rtw_pm_set_lps_dtim(_adapter *padapter, uint8_t lps_dtim); 
int rtw_pm_get_lps_dtim(_adapter *padapter);
// Data declarations
//--------------------------------
// rtw_recv.o
// Function declarations
uint8_t *recvframe_pull(uint8_t *result, sint sz);
uint8_t *recvframe_pull_tail(uint8_t *result, sint sz);
void rtw_signal_stat_timer_hdl(void *FunctionContext); 
void rtw_init_sta_recv_priv(sta_recv_priv *psta_recvpriv); 
sint rtw_init_recv_priv(recv_priv *precvpriv, _adapter *padapter); 
void rtw_mfree_recv_priv_lock(recv_priv *precvpriv); 
list_head *rtw_alloc_recvframe(_queue *pfree_recv_queue);
list_head *rtw_alloc_recvframe(_queue *pfree_recv_queue, _irqL a2, int a3);
int rtw_free_recvframe(recv_frame *precvframe, _queue *pfree_recv_queue); 
sint rtw_enqueue_recvframe(recv_frame *precvframe, _queue *queue); 
sint rtw_enqueue_recvframe(recv_frame *precvframe, _queue *queue); 
void rtw_free_recvframe_queue(_queue *pframequeue, _queue *pfree_recv_queue); 
int rtw_free_uc_swdec_pending_queue(_adapter *adapter, _irqL a2, int a3);
void rtw_free_recv_priv(recv_priv *precvpriv); 
sint rtw_enqueue_recvbuf_to_head(recv_buf *precvbuf, _queue *queue); 
uint32_t rtw_free_buf_pending_queue(_adapter *adapter); 
sint rtw_enqueue_recvbuf(recv_buf *precvbuf, _queue *queue); 
list_head *rtw_dequeue_recvbuf(_queue *queue, _irqL a2, int a3);
sint recvframe_chkmic(_adapter *adapter, recv_frame *precvframe, int a3, int a4);
recv_frame *decryptor(_adapter *padapter, recv_frame *precv_frame); 
recv_frame *portctrl(_adapter *adapter, recv_frame *precv_frame); 
sint recv_decache(recv_frame *precv_frame, uint8_t bretry, stainfo_rxcache *prxcache); 
void process_pwrbit_data(_adapter *padapter, recv_frame *precv_frame, int a3, int a4);
void process_wmmps_data(_adapter *padapter, recv_frame *precv_frame); 
void count_rx_stats(_adapter *padapter, recv_frame *prframe, sta_info *sta); 
sint sta2sta_data_frame(_adapter *adapter, recv_frame *precv_frame, sta_info **psta, int a4);
sint ap2sta_data_frame(_adapter *adapter, recv_frame *precv_frame, sta_info **psta); 
sint sta2ap_data_frame(_adapter *adapter, recv_frame *precv_frame, sta_info **psta); 
sint validate_recv_ctrl_frame(_adapter *padapter, recv_frame *precv_frame); 
sint validate_recv_data_frame(_adapter *adapter, recv_frame *precv_frame); 
sint wlanhdr_to_ethhdr(recv_frame *precvframe, int a2, int a3);
recv_frame *recvframe_defrag(_adapter *adapter, _queue *defrag_q); 
_queue *recvframe_chk_defrag(PADAPTER padapter, recv_frame *precv_frame);
sint validate_recv_mgnt_frame(PADAPTER padapter, recv_frame **pprecv_frame); 
sint validate_recv_frame(_adapter *adapter, recv_frame **pprecv_frame); 
int amsdu_to_msdu(_adapter *padapter, recv_frame *prframe); 
int check_indicate_seq(recv_reorder_ctrl *preorder_ctrl, int seq_num);
int enqueue_reorder_recvframe(recv_reorder_ctrl *preorder_ctrl, recv_frame *prframe); 
int recv_indicatepkts_in_order(_adapter *padapter, recv_reorder_ctrl *preorder_ctrl, int bforced); 
int recv_indicatepkt_reorder(_adapter *padapter, recv_frame *prframe, int a3);
void rtw_reordering_ctrl_timeout_handler(void *pcontext, _irqL a2);
int process_recv_indicatepkts(_adapter *padapter, recv_frame *prframe, int a3);
int recv_func_prehandle(_adapter *padapter, recv_frame *rframe, int a3);
int recv_func_posthandle(_adapter *padapter, recv_frame *prframe); 
int recv_func(_adapter *padapter, recv_frame *rframe, int a3);
int32_t rtw_recv_entry(recv_frame *precvframe, int a2, int a3);
void rtw_recv_tasklet(thread_context context); 
// Data declarations
const uint8_t SNAP_ETH_TYPE_APPLETALK_AARP[2]; // = { 128u, 243u }; 
const uint8_t SNAP_ETH_TYPE_IPX[2]; // = { 129u, 55u }; 
const uint8_t rtw_bridge_tunnel_header[6]; // = { 170u, 170u, 3u, 0u, 0u, 248u }; 
const uint8_t oui_8021h[3] =; // { 0u, 0u, 248u }; 
const uint8_t rtw_rfc1042_header[6]; // = { 170u, 170u, 3u, 0u, 0u, 0u }; 
const uint8_t SNAP_HDR_APPLETALK_DDP[3]; // = { 8u, 0u, 7u }; 
const uint8_t SNAP_ETH_TYPE_APPLETALK_DDP[2]; // = { 128u, 155u }; 
const uint8_t oui_rfc1042[3]; // = { 0u, 0u, 0u }; 
//--------------------------------
// rtw_security.o
// Function declarations
void rtw_wep_encrypt(_adapter *padapter, uint8_t *pxmitframe); 
void rtw_wep_decrypt(_adapter *padapter, uint8_t *precvframe); 
signed int rtw_tkip_encrypt(_adapter *padapter, uint8_t *pxmitframe);
int rtw_tkip_decrypt(_adapter *padapter, uint8_t *precvframe);
signed int rtw_aes_encrypt(_adapter *padapter, uint8_t *pxmitframe, int a3, int a4);
int rtw_aes_decrypt(_adapter *padapter, uint8_t *precvframe, int a3, int a4);
void rtw_use_tkipkey_handler(void *FunctionContext); 
int rtw_init_sec_priv(_adapter *padapter);
void rtw_free_sec_priv(security_priv *psecpriv); 
// Data declarations
//--------------------------------
// rtw_sta_mgt.o
// Function declarations
int wifi_mac_hash(uint8_t *mac);
void rtw_init_stainfo(sta_info *psta); 
int rtw_init_sta_priv(_adapter *padapter);
void rtw_free_sta_xmit_priv_lock(sta_xmit_priv *psta_xmitpriv); 
void rtw_mfree_stainfo(sta_info *psta); 
void rtw_mfree_sta_priv_lock(sta_priv *pstapriv); 
signed int rtw_free_sta_priv(sta_priv *pstapriv);
void init_addba_retry_timer(_adapter *padapter, sta_info *psta); 
sta_info *rtw_alloc_stainfo(sta_priv *pstapriv, uint8_t *hwaddr, _irqL a3, _irqL a4);
signed int rtw_free_stainfo(_adapter *padapter, sta_info *psta, int a3);
sta_info *rtw_get_stainfo(sta_priv *pstapriv, uint8_t *hwaddr, int a3, int a4);
signed int rtw_init_bcmc_stainfo(_adapter *padapter, int a2, int a3);
sta_info *rtw_get_bcmc_stainfo(_adapter *padapter, int a2, int a3);
void rtw_free_all_stainfo(_adapter *padapter, _irqL a2, int a3);
// Data declarations
//--------------------------------
// rtw_xmit.o
// Function declarations
void init_txservq(tx_servq *ptxservq); 
void set_qos(pkt_file *ppktfile, pkt_attrib *pattrib); 
void rtw_init_sta_xmit_priv(sta_xmit_priv *psta_xmitpriv); 
void rtw_mfree_xmit_priv_lock(xmit_priv *pxmitpriv); 
int qos_acm(uint8_t acm_mask, int priority);
int32_t xmitframe_addmic(_adapter *padapter, xmit_frame *pxmitframe); 
int32_t xmitframe_swencrypt(_adapter *padapter, xmit_frame *pxmitframe); 
int32_t rtw_make_wlanhdr(_adapter *padapter, uint8_t *hdr, pkt_attrib *pattrib); 
int32_t rtw_txframes_pending(_adapter *padapter); 
int32_t rtw_txframes_sta_ac_pending(_adapter *padapter, pkt_attrib *pattrib); 
void rtw_txframes_update_attrib_vcs_info(_adapter *padapter, xmit_frame *pxmitframe); 
int rtw_calculate_wlan_pkt_size_by_attribue(pkt_attrib *pattrib);
int32_t rtw_put_snap(uint8_t *data, int h_proto);
void rtw_update_protection(_adapter *padapter, uint8_t *ie, unsigned int ie_len); 
void rtw_count_tx_stats(PADAPTER padapter, xmit_frame *pxmitframe, __int64 sz);
int32_t rtw_free_xmitbuf_ext(xmit_priv *pxmitpriv, xmit_buf *pxmitbuf, int a3);
list_head *rtw_alloc_xmitframe(xmit_priv *pxmitpriv, _irqL a2);
int32_t rtw_free_xmitframe(xmit_priv *pxmitpriv, xmit_frame *pxmitframe); 
void rtw_free_xmitframe_queue(xmit_priv *pxmitpriv, _queue *pframequeue); 
tx_servq *rtw_get_sta_pending(_adapter *padapter, sta_info *psta, sint up, uint8_t *ac); 
sta_info *rtw_xmit_classifier(_adapter *padapter, xmit_frame *pxmitframe);
BOOL rtw_xmitframe_enqueue(_adapter *padapter, xmit_frame *pxmitframe);
void rtw_alloc_hwxmits(_adapter *padapter); 
void rtw_free_hwxmits(_adapter *padapter); 
void rtw_free_xmit_priv(xmit_priv *pxmitpriv); 
void rtw_init_hwxmits(hw_xmit *phwxmit, sint entry); 
int32_t rtw_init_xmit_priv(xmit_priv *pxmitpriv, _adapter *padapter); 
signed int rtw_get_ff_hwaddr(xmit_frame *pxmitframe);
sint xmitframe_enqueue_for_sleeping_sta(_adapter *padapter, xmit_frame *pxmitframe); 
void dequeue_xmitframes_to_sleeping_queue(_adapter *padapter, sta_info *psta, _queue *pframequeue); 
void stop_sta_xmit(_adapter *padapter, sta_info *psta); 
void wakeup_sta_to_xmit(_adapter *padapter, sta_info *psta); 
void xmit_delivery_enabled_frames(_adapter *padapter, sta_info *psta, int a3);
void rtw_xmit_tasklet(thread_context context); 
int32_t rtw_xmit(_adapter *padapter, _pkt **ppkt); 
BOOL rtw_sctx_chk_waring_status(int status);
void rtw_sctx_done_err(submit_ctx **sctx, int status); 
list_head *rtw_alloc_xmitbuf(xmit_priv *pxmitpriv, _irqL a2);
int32_t rtw_free_xmitbuf(xmit_priv *pxmitpriv, xmit_buf *pxmitbuf, int a3);
xmit_buf *rtw_alloc_xmitbuf_ext(xmit_priv *pxmitpriv, uint32_t size, int a3);
void rtw_sctx_done(submit_ctx **sctx); 
// Data declarations
//--------------------------------
// phydm_RegConfig8195A.o
// Function declarations
void __fastcall odm_ConfigRFReg_8195A(PDM_ODM_T pDM_Odm, u4Byte Addr, u4Byte Data, ODM_RF_RADIO_PATH_E RF_PATH, u4Byte RegAddr);
void __fastcall odm_ConfigRF_RadioA_8195A(PDM_ODM_T pDM_Odm, u4Byte Addr, u4Byte Data); // idb
void __fastcall odm_ConfigBB_AGC_8195A(PDM_ODM_T pDM_Odm, u4Byte Addr, u4Byte Bitmask, u4Byte Data); // idb
void __fastcall odm_ConfigBB_PHY_REG_PG_8195A(PDM_ODM_T pDM_Odm, u4Byte Band, u4Byte RfPath, u4Byte TxNum, u4Byte Addr, u4Byte Bitmask, u4Byte Data);
void __fastcall odm_ConfigBB_PHY_8195A(PDM_ODM_T pDM_Odm, u4Byte Addr, u4Byte Bitmask, u4Byte Data); // idb
void __fastcall odm_ConfigBB_TXPWR_LMT_8195A(PDM_ODM_T pDM_Odm, int Regulation, int Band, int Bandwidth, u1Byte RateSection, u1Byte RfPath, u1Byte Channel, u1Byte PowerLimit);
// Data declarations
//--------------------------------
// lxbus_hci_intf.o
// Function declarations
dvobj_priv *hci_lxbus_dvobj_init(); // idb
void __fastcall hci_lxbus_dvobj_deinit(dvobj_priv *dvobj); // idb
void __fastcall hci_lxbus_dvobj_request_irq(dvobj_priv *dvobj); // idb
void __fastcall hci_lxbus_free_irq(dvobj_priv *dvobj); // idb
void __fastcall hci_lxbus_intf_stop(PADAPTER padapter); // idb
// Data declarations
//--------------------------------
// lxbus_intf.o
// Function declarations
signed int __fastcall lextra_bus_dma_Interrupt(void *data);
// Data declarations
//--------------------------------
// lxbus_ops.o
// Function declarations
void __fastcall rtl8195a_free_rx_ring(_adapter *padapter); // idb
int __fastcall bus_write32(dvobj_priv *pintfhdl, uint32_t addr, uint32_t val, int32_t *err); // idb
int __fastcall bus_write16(dvobj_priv *pintfhdl, uint32_t addr, int val, int32_t *err);
int __fastcall bus_write8(dvobj_priv *pintfhdl, uint32_t addr, int val, int32_t *err);
uint32_t __fastcall bus_read32(dvobj_priv *pintfhdl, uint32_t addr, int32_t *err); // idb
int __fastcall bus_read16(dvobj_priv *pintfhdl, uint32_t addr, int32_t *err);
int __fastcall bus_read8(dvobj_priv *pintfhdl, uint32_t addr, int32_t *err);
void __fastcall rtl8195a_free_tx_ring(_adapter *padapter, unsigned int prio); // idb
signed int __fastcall rtl8195a_init_desc_ring(_adapter *padapter);
signed int __fastcall rtl8195a_free_desc_ring(_adapter *padapter);
void __fastcall rtl8195a_reset_desc_ring(_adapter *padapter, _irqL a2, int a3);
void __fastcall InitLxDmaRtl8195a(_adapter *Adapter); // idb
void __fastcall rtl8195a_prepare_bcn_tasklet(void *priv); // idb
signed int __fastcall get_txdesc_buf_addr(int ff_hwaddr);
signed int __fastcall rtl8195a_check_txdesc_closed(_adapter *padapter, uint32_t queue_idx, uint32_t index);
void __fastcall rtl8195a_tx_isr(PADAPTER Adapter, int prio); // idb
signed int __fastcall InterruptRecognized8195a(PADAPTER Adapter);
void __fastcall InitInterrupt8195a(PADAPTER padapter); // idb
void __fastcall EnableDMA8195a(PADAPTER padapter); // idb
void __fastcall EnableInterrupt8195a(PADAPTER padapter); // idb
void __fastcall DisableDMA8195a(PADAPTER padapter); // idb
void __fastcall DisableInterrupt8195a(PADAPTER padapter); // idb
void __fastcall UpdateInterruptMask8195a(PADAPTER Adapter, uint32_t *pAddMSRB, uint32_t *pRemoveMSR); // idb
signed int __fastcall CheckRxTgRtl8195a(_adapter *padapter, uint8_t *rx_desc, uint16_t rx_queue_idx);
int __fastcall rtl8192ee_check_rxdesc_remain(_adapter *padapter, int rx_queue_idx);
void __fastcall rtl8195a_recv_tasklet(void *priv); // idb
void __fastcall rtl8195a_tx_int_handler(_adapter *padapter, int a2, int a3);
int32_t __fastcall InterruptHandle8195a(_adapter *padapter, int a2, int a3);
void __fastcall rtl8195a_xmit_tasklet(void *priv); // idb
void __fastcall lxbus_set_intf_ops(_io_ops *pops); // idb
// Data declarations
uint8_t rx_ring_pool[4][2104]; // idb
u16 CSWTCH_48[8]; // = { 928, 932, 936, 940, 936, 944, 952, 936 }; // idb
uint8_t stop_report_count_20629; // idb
//--------------------------------
// phydm_ACS.o
// Function declarations
void __fastcall phydm_CLMInit(PVOID pDM_VOID, u2Byte sampleNum); // idb
void __fastcall phydm_CLMtrigger(PVOID pDM_VOID); // idb
int __fastcall phydm_checkCLMready(PVOID pDM_VOID);
int __fastcall phydm_getCLMresult(PVOID pDM_VOID);
void __fastcall phydm_NHMInit(PVOID pDM_VOID, u2Byte sampleNum, int round);
void __fastcall phydm_NHMtrigger(PVOID pDM_VOID); // idb
void __fastcall phydm_FalseAlarmCounterStatistics(PVOID pDM_VOID); // idb
void __fastcall phydm_getNHMresult(PVOID pDM_VOID, unsigned int *fa_crc32_total_cnt, unsigned int *cca_count, unsigned int *nhm_cnt_exp_sum, unsigned __int8 round);
// Data declarations
//--------------------------------
// PhyDM_Adaptivity.o
// Function declarations
void __fastcall Phydm_CheckAdaptivity(PVOID pDM_VOID); // idb
void __fastcall Phydm_NHMCounterStatisticsInit(PVOID pDM_VOID); // idb
void __fastcall Phydm_GetNHMCounterStatistics(PVOID pDM_VOID); // idb
void __fastcall Phydm_NHMCounterStatisticsReset(PVOID pDM_VOID); // idb
void __fastcall Phydm_NHMCounterStatistics(PVOID pDM_VOID); // idb
void __fastcall Phydm_SetEDCCAThreshold(PVOID pDM_VOID, s1Byte H2L, s1Byte L2H); // idb
void __fastcall Phydm_SetTRxMux(PVOID pDM_VOID, PhyDM_Trx_MUX_Type txMode, PhyDM_Trx_MUX_Type rxMode); // idb
void __fastcall Phydm_MACEDCCAState(PVOID pDM_VOID, PhyDM_MACEDCCA_Type State); // idb
BOOL __fastcall Phydm_CalNHMcnt(PVOID pDM_VOID);
void __fastcall Phydm_CheckEnvironment(PVOID pDM_VOID); // idb
void __fastcall Phydm_SearchPwdBLowerBound(PVOID pDM_VOID); // idb
void __fastcall Phydm_AdaptivityInit(PVOID pDM_VOID); // idb
void __fastcall Phydm_Adaptivity(PVOID pDM_VOID, int IGI);
// Data declarations
//--------------------------------
// PhyDM_AntDiv.o
// Function declarations
void __fastcall ODM_SwAntDivRestAfterLink(PDM_ODM_T pDM_Odm); // idb
// Data declarations
//--------------------------------
// phydm_CfoTracking.o
// Function declarations
void __fastcall ODM_CfoTrackingInit(PVOID pDM_VOID);
void __fastcall ODM_CfoTracking(PVOID pDM_VOID);
void __fastcall ODM_ParsingCFO(PVOID pDM_VOID, PVOID pPktinfo_VOID, s1Byte *pcfotail);
// Data declarations
//--------------------------------
// phydm_debug.o
// Function declarations
void __fastcall ODM_InitDebugSetting(PDM_ODM_T pDM_Odm); // idb
// Data declarations
//--------------------------------
// phydm_DIG.o
// Function declarations
void __fastcall ODM_ChangeDynamicInitGainThresh(PVOID pDM_VOID, u4Byte DM_Type, u4Byte DM_Value);
int __fastcall getIGIForDiff(int value_IGI);
void __fastcall ODM_Write_DIG(PVOID pDM_VOID, u1Byte CurrentIGI);
void __fastcall odm_PauseDIG(PVOID pDM_VOID, ODM_Pause_DIG_TYPE PauseType, u1Byte IGIValue);
u1Byte __fastcall odm_ForbiddenIGICheck(PVOID pDM_VOID, u1Byte DIG_Dynamic_MIN, u1Byte CurrentIGI);
void __fastcall ODM_Write_CCK_CCA_Thres(PVOID pDM_VOID, u1Byte CurCCK_CCAThres);
void __fastcall odm_PauseCCKPacketDetection(PVOID pDM_VOID, ODM_Pause_CCKPD_TYPE PauseType, u1Byte CCKPDThreshold);
void __fastcall odm_DIGInit(PVOID pDM_VOID);
BOOLEAN __fastcall odm_DigAbort(PVOID pDM_VOID);
void __fastcall odm_DIGbyRSSI_LPS(PVOID pDM_VOID);
void __fastcall odm_FAThresholdCheck(PVOID pDM_VOID, u4Byte *dm_FA_thres);
void __fastcall odm_DIG(PVOID pDM_VOID);
void __fastcall odm_FalseAlarmCounterStatistics(PVOID pDM_VOID);
void __fastcall odm_CCKPacketDetectionThresh(PVOID pDM_VOID);
// Data declarations
BOOLEAN bPaused_20545; // idb
//--------------------------------
// phydm_HWConfig.o
// Function declarations
u1Byte __fastcall odm_QueryRxPwrPercentage(s1Byte AntPower);
s4Byte __fastcall odm_SignalScaleMapping_92CSeries_patch_RT_CID_819x_Lenovo(PDM_ODM_T pDM_Odm, s4Byte CurrSig);
s4Byte __fastcall odm_SignalScaleMapping_92CSeries_patch_RT_CID_819x_Netcore(PDM_ODM_T pDM_Odm, s4Byte CurrSig);
s4Byte __fastcall odm_SignalScaleMapping_92CSeries(PDM_ODM_T pDM_Odm, s4Byte CurrSig);
s4Byte __fastcall odm_SignalScaleMapping(PDM_ODM_T pDM_Odm, s4Byte CurrSig);
void __fastcall odm_RxPhyStatus8195A_Parsing(PDM_ODM_T pDM_Odm, PODM_PHY_INFO_T pPhyInfo, pu1Byte pPhyStatus, PODM_PACKET_INFO_T pPktinfo);
void __fastcall odm_Process_RSSIForDM_8195A(PDM_ODM_T pDM_Odm, PODM_PHY_INFO_T pPhyInfo, PODM_PACKET_INFO_T pPktinfo, pu1Byte pPhyStatus);
void __fastcall ODM_PhyStatusQuery_8195A(PDM_ODM_T pDM_Odm, PODM_PHY_INFO_T pPhyInfo, pu1Byte pPhyStatus, PODM_PACKET_INFO_T pPktinfo);
HAL_STATUS __fastcall ODM_ConfigRFWithHeaderFile(PDM_ODM_T pDM_Odm, ODM_RF_Config_Type ConfigType, ODM_RF_RADIO_PATH_E eRFPath);
HAL_STATUS __fastcall ODM_ConfigRFWithTxPwrTrackHeaderFile(PDM_ODM_T pDM_Odm);
HAL_STATUS __fastcall ODM_ConfigBBWithHeaderFile(PDM_ODM_T pDM_Odm, ODM_BB_Config_Type ConfigType);
HAL_STATUS __fastcall ODM_ConfigMACWithHeaderFile(PDM_ODM_T pDM_Odm);
HAL_STATUS __fastcall ODM_ConfigFWWithHeaderFile(PDM_ODM_T pDM_Odm, ODM_FW_Config_Type ConfigType, u1Byte *pFirmware, u4Byte *pSize);
u4Byte __fastcall ODM_GetHWImgVersion(PDM_ODM_T pDM_Odm);
// Data declarations
//--------------------------------
// phydm_interface.o
// Function declarations
u1Byte __fastcall ODM_Read1Byte(PDM_ODM_T pDM_Odm, u4Byte RegAddr);
u2Byte __fastcall ODM_Read2Byte(PDM_ODM_T pDM_Odm, u4Byte RegAddr);
u4Byte __fastcall ODM_Read4Byte(PDM_ODM_T pDM_Odm, u4Byte RegAddr);
void __fastcall ODM_Write1Byte(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u1Byte Data);
void __fastcall ODM_Write2Byte(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u2Byte Data);
void __fastcall ODM_Write4Byte(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u4Byte Data);
void __fastcall ODM_SetMACReg(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u4Byte BitMask, u4Byte Data);
u4Byte __fastcall ODM_GetMACReg(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u4Byte BitMask);
void __fastcall ODM_SetBBReg(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u4Byte BitMask, u4Byte Data);
u4Byte __fastcall ODM_GetBBReg(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u4Byte BitMask);
// void __usercall ODM_SetRFReg(PDM_ODM_T pDM_Odm@<R0>, ODM_RF_RADIO_PATH_E eRFPath@<R1>, u4Byte RegAddr@<R2>, u4Byte BitMask@<R3>, u4Byte Data);
u4Byte __fastcall ODM_GetRFReg(PDM_ODM_T pDM_Odm, ODM_RF_RADIO_PATH_E eRFPath, u4Byte RegAddr, u4Byte BitMask);
void __fastcall ODM_AllocateMemory(PDM_ODM_T pDM_Odm, PVOID *pPtr, u4Byte length);
void __fastcall ODM_FreeMemory(PDM_ODM_T pDM_Odm, PVOID pPtr, u4Byte length);
void __fastcall ODM_MoveMemory(PDM_ODM_T pDM_Odm, PVOID pDest, PVOID pSrc, u4Byte Length);
u8Byte __fastcall ODM_GetCurrentTime(PDM_ODM_T pDM_Odm);
u8Byte __fastcall ODM_GetProgressingTime(PDM_ODM_T pDM_Odm, u8Byte Start_Time);
// Data declarations
//--------------------------------
// phydm_PowerTracking.o
// Function declarations
signed int __fastcall getSwingIndex(PVOID pDM_VOID);
void __fastcall odm_TXPowerTrackingThermalMeterInit(PVOID pDM_VOID); // idb
void __fastcall odm_TXPowerTrackingCheckIOT(PVOID pDM_VOID); // idb
void __fastcall ODM_TXPowerTrackingCheck(PVOID pDM_VOID); // idb
// Data declarations
const u4Byte OFDMSwingTable_New[43];
//--------------------------------
// phydm_RaInfo.o
// Function declarations
void __fastcall odm_RSSIMonitorInit(PVOID pDM_VOID);
void __fastcall ODM_RAPostActionOnAssoc(PVOID pDM_VOID);
void __fastcall odm_RSSIMonitorCheckIOT(PVOID pDM_VOID);
void __fastcall odm_RSSIMonitorCheck(PVOID pDM_VOID);
void __fastcall odm_RateAdaptiveMaskInit(PVOID pDM_VOID);
BOOLEAN __fastcall ODM_RAStateCheck(PVOID pDM_VOID, s4Byte RSSI, BOOLEAN bForceUpdate, pu1Byte pRATRState);
void __fastcall odm_RefreshRateAdaptiveMaskIOT(PVOID pDM_VOID);
void __fastcall odm_RefreshRateAdaptiveMask(PVOID pDM_VOID);
u4Byte __fastcall ODM_Get_Rate_Bitmap(PVOID pDM_VOID, u4Byte macid, u4Byte ra_mask, u1Byte rssi_level);
// Data declarations
//--------------------------------
//
// Function declarations
// Data declarations
//--------------------------------
//
// Function declarations
// Data declarations
//--------------------------------
//
// Function declarations
// Data declarations
//--------------------------------
//
// Function declarations
// Data declarations



#ifdef	__cplusplus
}
#endif
#endif // _WIFI_LIB_H

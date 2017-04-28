/*
	wlan_lib.h
	RTL871x pvvx
*/
#ifndef _WIFI_LIB_H
#define _WIFI_LIB_H

//#include "wifi_constants.h"
//#include "wifi_structures.h"

#ifdef	__cplusplus
extern "C" {
#endif

#include "wireless.h"
#include "wifi_lib.h"

// rom_rtw_message.o
//--------------------------------
// freertos_ioctl.o
// Function declarations
extern int rtw_wx_set_autoreconnect(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_wx_get_autoreconnect(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_forwarding_set(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_set_ch_deauth(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int get_priv_size(int args);
extern int rtw_wx_del_custome_ie(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_wx_set_pscan_freq(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_wx_update_custome_ie(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_set_tos_value(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_get_tx_power(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_wx_set_custome_ie(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_pm_get(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_pm_set(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_wx_read32(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_wx_write32(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_wx_set_freq(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_ex_set(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);
extern void wireless_send_event(struct net_device *dev, unsigned int cmd, union iwreq_data *wrqu, char *extra);
extern void indicate_wx_custom_event(_adapter *padapter, char *msg); 
extern void indicate_wx_scan_result_present(uint64_t padapter, uint64_t a2);
extern void indicate_wx_scan_complete_event(uint64_t padapter, uint64_t a2);
extern void rtw_indicate_sta_assoc(uint64_t padapter, uint64_t buf);
extern void rtw_indicate_sta_disassoc(_adapter *padapter, uint8_t *addr); 
extern void rtw_indicate_wx_assoc_event(uint64_t padapter, uint64_t a2);
extern void rtw_indicate_wx_disassoc_event(uint64_t padapter, uint64_t a2);
extern int rtw_set_wpa_ie(_adapter *padapter, char *pie, int ielen);
extern void strtopsk(uint8_t *des, uint8_t *src, int len);
extern int rtw_wx_get_passphrase(struct net_device *dev, struct iw_request_info *a, union iwreq_data *wrqu, char *extra);
extern int rtw_wx_set_ap_essid(struct net_device *dev, struct iw_request_info *a, union iwreq_data *wrqu, char *extra);
extern void mac_reg_dump(_adapter *padapter); 
extern void bb_reg_dump(_adapter *padapter); 
extern void rf_reg_dump(_adapter *padapter); // , int a2, int a3);
extern int rtw_dbg_port(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
extern int rtw_get_auto_channel(struct net_device *dev, u8 *channel_set, int channel_num);
extern int rtw_set_sta_num(int ap_sta_num);
extern int rtw_del_sta(struct net_device *dev, u8 *sta_addr);
extern int rtw_ex_get_drv_ability(struct net_device *dev, struct iw_request_info *info, struct iw_point *wrqu, char *extra);
extern int rtw_ex_get(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);
extern void *rtw_ioctl(struct net_device *dev, struct iwreq *rq, int cmd);
// Data declarations
extern u8 g_user_ap_sta_num; // = 5u;
extern const struct iw_priv_args rtw_private_args[26]; /* =
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
extern iw_handler rtw_private_handler[17]; /* =
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
extern char iw_priv_type_size[8]; // = { '\0', '\x01', '\x01', '\0', '\x04', '\b', '\x10', '\0' };
//--------------------------------
// freertos_intfs.o
// Function declarations
extern struct net_device_stats *rtw_net_get_stats(struct net_device *pnetdev);
extern int netdev_if2_close(struct net_device *pnetdev);
extern int netdev_close(struct net_device *pnetdev);
extern void rtw_if1_deinit(PADAPTER if1); 
extern void rtw_reset_securitypriv(_adapter *adapter); 
extern void rtw_os_indicate_disconnect(_adapter *adapter); 
extern int rtw_init_netdev_name(struct net_device *pnetdev, const char *ifname);
extern struct net_device *rtw_init_netdev(_adapter *old_padapter);
extern int rtw_init_io_priv(_adapter *padapter, void (*set_intf_ops)(struct _io_ops *));
extern _adapter *rtw_drv_if2_init(_adapter *primary_padapter, char *name, void (*set_intf_ops)(struct _io_ops *));
extern void rtw_drv_if2_stop(_adapter *if2); 
extern void rtw_drv_if2_free(_adapter *primary_padapter); 
extern init_done_ptr netdev_open(struct net_device *pnetdev);
extern int netdev_if2_open(struct net_device *pnetdev);
extern int netdev_if2_open(struct net_device *pnetdev);
extern init_done_ptr netdev_open(struct net_device *pnetdev);
extern struct net_device *rtw_drv_probe(struct net_device *parent_dev, uint32_t mode);
extern int rtw_dev_remove(struct net_device *pnetdev);
extern void rtw_drv_entry(); 
extern void rtw_drv_halt(); 
// Data declarations
extern init_done_ptr p_wlan_init_done_callback; 
extern uint8_t rtw_power_percentage_idx; 
extern init_done_ptr p_wlan_uart_adapter_callback; 
extern uint8_t rtw_adaptivity_en; 
extern uint8_t rtw_adaptivity_mode;
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
extern u8 gDeltaSwingTableIdx_MP_2GA_P_TxPowerTrack_TFBGA96_8195A[32];
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
extern struct net_device *alloc_etherdev(int sizeof_priv);
extern void free_netdev(struct net_device *dev);
extern int dev_alloc_name(struct net_device *net_dev, const char *ifname);
extern void init_timer_wrapper(); 
extern void deinit_timer_wrapper();
extern void init_timer(struct timer_list *timer);
extern void mod_timer(struct timer_list *timer, uint32_t delay_time_ms);
extern BOOL timer_pending(const struct timer_list *timer);
extern void cancel_timer_ex(struct timer_list *timer);
extern void del_timer_sync(struct timer_list *timer);
extern void rtw_init_timer(_timer *ptimer, void *adapter, TIMER_FUN pfunc, void *cntx, const char *name);
extern u8 rtw_cancel_timer(_timer *ptimer);
extern BOOL rltk_get_idx_bydev(struct net_device *dev);
extern int rltk_wlan_init(int idx_wlan, rtw_mode_t mode); 
extern void rltk_wlan_deinit(); 
extern int rltk_wlan_start(int idx_wlan);
extern int rltk_wlan_check_isup(int idx);
extern void rltk_wlan_tx_inc(int idx); 
extern void rltk_wlan_tx_dec(int idx); 
extern struct sk_buff *rltk_wlan_get_recv_skb(int idx);
extern struct sk_buff *rltk_wlan_alloc_skb(unsigned int total_len);
extern void rltk_wlan_send_skb(int idx, struct sk_buff *skb);
extern void rltk_netif_rx(struct sk_buff *skb);
extern int rltk_del_station(const char *ifname, u8 *hwaddr); 
extern int rltk_get_auto_chl(const char *ifname, u8 *channel_set, int channel_num);
extern int rltk_set_tx_power_percentage(rtw_tx_pwr_percentage_t power_percentage_idx); 
extern int rltk_wlan_control(unsigned long cmd, void *data);
extern unsigned char rltk_wlan_running(unsigned char idx);
extern void rltk_wlan_statistic(unsigned char idx);
extern int rltk_wlan_handshake_done(); 
extern int rltk_wlan_rf_on(); 
extern int rltk_wlan_rf_off(); 
extern int rltk_wlan_check_bus(); 
extern int rltk_wlan_wireless_mode(u8 mode); 
extern int rltk_wlan_set_wps_phase(unsigned char is_trigger_wps);
extern void rltk_wlan_PRE_SLEEP_PROCESSING(); 
extern int rltk_wlan_is_connected_to_ap(); 
extern int rtw_ps_enable(int enable); 
extern int rltk_wifi_fw_test(int argc, char **argv); 
// Data declarations
extern _list timer_table;
extern Rltk_wlan_t rltk_wlan_info[2]; // in wrapper.h
extern int timer_used_num;
extern int max_timer_used_num;
//--------------------------------
// rtl8195a_cmd.o
// Function declarations
extern int32_t FillH2CCmd8195A(PADAPTER padapter, int ElementID, uint64_t CmdLen);
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
extern int rtw_get_cipher_info(struct wlan_network *pnetwork);
extern void rtw_get_bcn_info(struct wlan_network *pnetwork);
//--------------------------------
// rtw_wlan_util.o
// Function declarations
extern int cckrates_included(u8 *rate, int ratelen);
extern int cckratesonly_included(u8 *rate, int ratelen);
extern signed int networktype_to_raid_ex(PADAPTER padapter, int network_type);
extern signed int judge_network_type(_adapter *padapter, u8 *rate, int ratelen);
extern int ratetbl_val_2wifirate(u8 rate);
extern int is_basicrate(_adapter *padapter, int rate);
extern int ratetbl2rateset(_adapter *padapter, u8 *rateset);
extern void get_rate_set(_adapter *padapter, u8 *pbssrate, int *bssrate_len, int a4);
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
extern void VCS_update(_adapter *padapter, struct sta_info *psta);
extern int rtw_check_bcn_info(_adapter *Adapter, uint8_t *pframe, uint32_t packet_len);
extern void update_beacon_info(_adapter *padapter, uint8_t *pframe, unsigned int pkt_len, struct sta_info *psta);
extern signed int is_ap_in_tkip(_adapter *padapter);
extern int wifirate2_ratetbl_inx(u8 rate);
extern int update_basic_rate(u8 *ptn, unsigned int ptn_sz);
extern int update_supported_rate(u8 *ptn, unsigned int ptn_sz);
extern int update_MCS_rate(struct HT_caps_element *pHT_caps);
extern int support_short_GI(_adapter *padapter, struct HT_caps_element *pHT_caps);
extern int get_highest_rate_idx(uint32_t mask);
extern void Update_RA_Entry(_adapter *padapter, struct sta_info *psta);
extern void enable_rate_adaptive(_adapter *padapter, struct sta_info *psta);
extern void set_sta_rate(_adapter *padapter, struct sta_info *psta);
extern void update_tx_basic_rate(_adapter *padapter, int wirelessmode);
extern signed int check_assoc_AP(uint8_t *pframe, unsigned int len);
extern void update_IOT_info(_adapter *padapter); 
extern void update_capinfo(PADAPTER Adapter, uint16_t updateCap); 
extern void update_wireless_mode(_adapter *padapter, uint32_t a2, int a3);
extern void update_bmc_sta_support_rate(_adapter *padapter, uint32_t mac_id); 
extern void update_TSF(struct mlme_ext_priv *pmlmeext, uint8_t *pframe, unsigned int len);
extern void correct_TSF(_adapter *padapter, struct mlme_ext_priv *pmlmeext);
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
extern s32 rtk_sc_register_pattern(struct pattern_ops *pp);
extern s32 rtk_sc_generate_key(struct pattern_ops *pp, struct rtk_sc *pSc);
extern s32 rtk_sc_decode_profile(struct pattern_ops *pp, struct rtk_sc *pSc);
extern s32 rtk_sc_get_tlv_info(struct pattern_ops *pp, struct rtk_sc *pSc);
extern s32 mcast_udp_get_cipher_info(struct pattern_ops *pp, struct rtk_sc *pSc);
extern s32 mcast_udp_get_pattern(struct pattern_ops *pp, struct rtk_sc *pSc);
extern s32 bcast_udp_get_pattern(struct pattern_ops *pp, struct rtk_sc *pSc);
extern s32 bcast_udp_get_cipher_info(struct pattern_ops *pp, struct rtk_sc *pSc);
extern s32 rtk_clean_profile_value(); 
extern s32 mcast_udp_decode_profile(struct pattern_ops *pp, struct rtk_sc *pSc);
extern s32 mcast_udp_generate_key(struct pattern_ops *pp, struct rtk_sc *pSc);
extern s32 rtk_sc_check_packet(u8 *da, u8 *bssid, s32 length);
extern void whc_fix_channel(); 
extern void whc_unfix_channel(); 
extern void simple_config_lib_init(struct simple_config_lib_config *config);
extern void simple_config_lib_deinit(); 
extern int parse_tlv_info_bcast(struct rtk_sc *pSc, u8 *plain_info, int len);
extern s32 mcast_udp_get_profile(struct pattern_ops *pp, struct rtk_sc *pSc);
extern void rtk_restart_simple_config(); 
extern void rtk_stop_simple_config(); 
extern s32 rtk_sc_init(char *custom_pin_code, struct simple_config_lib_config *lib_config);
extern void rtk_sc_deinit(); 
extern int rtk_sc_check_profile(struct pattern_ops *pp, struct rtk_sc *pSc, void *backup_sc_ctx);
extern signed int softAP_simpleConfig_parse(u8 *buf, int len, void *backup_sc_ctx, void *psoftAP_ctx);
extern int rtl_pre_parse(u8 *mac_addr, u8 *buf, void *userdata, u8 **da, u8 **sa, unsigned int *len);
extern s32 rtk_start_parse_packet(u8 *da, u8 *sa, s32 len, void *user_data, void *backup_sc_ctx);
// Data declarations
// Data declarations
extern char algn_1; // weak
extern const u8 default_key_iv[8]; // = { 166u, 166u, 166u, 166u, 166u, 166u, 166u, 166u }; 
extern struct pattern_ops udp_bcast; /* =
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
extern struct pattern_ops udp_mcast_pin; /* =
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
extern struct pattern_ops udp_bcast_pin; /* =
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
extern struct simple_config_lib_config sc_api_fun;
extern u8 fix_sa; 
extern u32 g_sc_pin_len; 
extern struct pattern_ops *pp;
extern struct pattern_ops udp_mcast; /* =
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
extern struct rtk_sc *g_sc_ctx;
//--------------------------------
// wlan_ram_map.o
// Function declarations
extern void init_rom_wlan_ram_map();
//--------------------------------
// freertos_isr.o
// Function declarations
extern void rtw_interrupt_thread(thread_context context);
// Data declarations
extern _sema *pExportWlanIrqSemaphore;

//--------------------------------
// freertos_recv.o
// Function declarations
extern int rtw_os_recv_resource_init(struct recv_priv *precvpriv, _adapter *padapter);
extern int rtw_os_recv_resource_alloc(_adapter *padapter, struct recv_frame *precvframe);
extern int rtw_os_recvbuf_resource_alloc(_adapter *padapter, struct recv_buf *precvbuf);
extern int rtw_os_recvbuf_resource_free(_adapter *padapter, struct recv_buf *precvbuf);
extern int rtw_tkip_countermeasure(_adapter *padapter); 
extern void rtw_handle_tkip_mic_err(_adapter *padapter, int bgroup);
extern int rtw_recv_indicatepkt(_adapter *padapter, struct recv_frame *precv_frame);
extern void rtw_init_recv_timer(struct recv_reorder_ctrl *preorder_ctrl);

//--------------------------------
// freertos_skbuff.o
// Function declarations
extern void skb_fail_inc(int a1);
extern int skb_fail_get_and_rst(int a1);
extern void init_skb_pool(); 
extern void init_skb_data_pool(); 
extern struct sk_buff *alloc_skb(int size);
extern void kfree_skb(struct sk_buff *skb);
extern u8 *skb_put(struct sk_buff *skb, unsigned int len);
extern void skb_reserve(struct sk_buff *skb, unsigned int len);
extern struct sk_buff *dev_alloc_skb(unsigned int length, unsigned int reserve_len);
extern void skb_assign_buf(struct sk_buff *skb, u8 *buf, unsigned int len);
extern u8 *skb_tail_pointer(const struct sk_buff *skb);
extern u8 *skb_end_pointer(const struct sk_buff *skb);
extern void skb_set_tail_pointer(struct sk_buff *skb, const int offset);
extern u8 *skb_pull(struct sk_buff *skb, unsigned int len);
extern struct sk_buff *skb_copy(const struct sk_buff *skb, int gfp_mask, unsigned int reserve_len);
extern struct sk_buff *skb_clone(struct sk_buff *skb, int gfp_mask);
// Data declarations
extern int skbbuf_used_num;
extern struct list_head skbdata_list;
/*
// skbuff.h, wifi_skbuf.c, rtw_opt_skbuf.c
struct skb_buf {
	struct list_head list;
	struct sk_buff skb;
};

struct skb_data {
	struct list_head list;
	unsigned char buf[MAX_SKB_BUF_SIZE];
	atomic_t ref;
};
extern struct skb_data skb_data_pool[8];
extern struct skb_buf skb_pool[10];
*/
extern int skbdata_used_num; 
extern int max_local_skb_num; // = 10; 
extern struct list_head wrapper_skbbuf_list;
extern int max_skbdata_used_num; 
extern int max_skbbuf_used_num; 
extern int skb_fail_count; 
extern int max_skb_buf_num; // = 8; 
//--------------------------------
// freertos_xmit.o
// Function declarations
extern signed int rtw_remainder_len(struct pkt_file *pfile);
extern void rtw_open_pktfile(_pkt *pktptr, struct pkt_file *pfile);
extern unsigned int rtw_pktfile_read(struct pkt_file *pfile, uint8_t *rmem, unsigned int rlen);
extern BOOL rtw_endofpktfile(struct pkt_file *pfile);
extern int rtw_os_xmit_resource_alloc(_adapter *padapter, struct xmit_buf *pxmitbuf, uint32_t alloc_sz);
extern void rtw_os_pkt_complete(_adapter *padapter, _pkt *pkt); 
extern void rtw_os_xmit_complete(_adapter *padapter, struct xmit_frame *pxframe);
extern void rtw_os_xmit_schedule(_adapter *padapter, _irqL a2, int a3);
extern int rtw_xmit_entry(_pkt *pkt, _nic_hdl pnetdev, int a3);
extern int rtw_os_can_xmit(struct net_device *dev);
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
extern int32_t rtw_hal_xmit(_adapter *padapter, struct xmit_frame *pxmitframe);
extern int32_t rtw_hal_mgnt_xmit(_adapter *padapter, struct xmit_frame *pmgntframe);
extern int32_t rtw_hal_init_xmit_priv(_adapter *padapter); 
extern void rtw_hal_free_xmit_priv(_adapter *padapter); 
extern int32_t rtw_hal_init_recv_priv(_adapter *padapter); 
extern void rtw_hal_free_recv_priv(_adapter *padapter); 
extern void rtw_hal_update_ra_mask(struct sta_info *psta, uint8_t rssi_level);
extern void rtw_hal_add_ra_tid(_adapter *padapter, uint32_t bitmap, uint8_t *arg, uint8_t rssi_level); 
extern void rtw_hal_update_txdesc(_adapter *padapter, struct xmit_frame *pxmitframe, uint8_t *pbuf);
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
extern struct dvobj_priv *hci_dvobj_init();
extern void hci_dvobj_deinit(struct dvobj_priv *dvobj);
extern void hci_dvobj_request_irq(struct dvobj_priv *dvobj);
extern void hci_dvobj_free_irq(struct dvobj_priv *dvobj);
// Data declarations
//--------------------------------
// rtw_ioctl_set.o
// Function declarations
extern int rtw_do_join(_adapter *padapter, _irqL a2);
extern int rtw_set_802_11_bssid(_adapter *padapter, uint8_t *bssid);
extern int rtw_set_802_11_ssid(_adapter *padapter, NDIS_802_11_SSID *ssid, int a3);
extern signed int rtw_set_802_11_infrastructure_mode(_adapter *padapter, NDIS_802_11_NETWORK_INFRASTRUCTURE networktype);
extern signed int rtw_set_802_11_bssid_list_scan(_adapter *padapter, NDIS_802_11_SSID *pssid, int ssid_max_num);
extern int rtw_set_802_11_authentication_mode(_adapter *padapter, struct NDIS_802_11_AUTHENTICATION_MODE authmode);
extern int rtw_set_802_11_add_wep(_adapter *padapter, NDIS_802_11_WEP *wep);
// Data declarations
//--------------------------------
// rtw_io.o
// Function declarations
extern int rtw_read8(_adapter *adapter, uint32_t addr);
extern int rtw_read16(_adapter *adapter, uint32_t addr);
extern int rtw_read32(_adapter *adapter, uint32_t addr);
extern int32_t rtw_write8(_adapter *adapter, uint32_t addr, int val);
extern int32_t rtw_write16(_adapter *adapter, uint32_t addr, int val);
extern int32_t rtw_write32(_adapter *adapter, uint32_t addr, uint32_t val);
extern signed int rtw_read_port(_adapter *adapter, uint32_t addr, uint32_t cnt, uint8_t *mem, struct fifo_more_data *more_data);
extern signed int rtw_write_port(_adapter *adapter, uint32_t addr, uint32_t cnt, uint8_t *mem);
extern void rtw_set_chip_endian(_adapter *adapter);
extern int rtw_get_chip_endian(_adapter padapter);
// Data declarations
//--------------------------------
// rtw_cmd.o
// Function declarations
extern sint rtw_init_cmd_priv(struct cmd_priv *pcmdpriv);
extern sint rtw_init_evt_priv(struct evt_priv *pevtpriv);
extern void rtw_free_cmd_priv(struct cmd_priv *pcmdpriv);
extern sint rtw_enqueue_cmd(_queue *queue, struct cmd_obj *obj);
extern struct cmd_obj *rtw_dequeue_cmd(_queue *queue, _irqL a2, int a3);
extern struct list_head *rtw_observequeue_cmd(_queue *queue);
extern signed int rtw_init_cmd_priv(struct cmd_priv *pcmdpriv);
extern int rtw_cmd_filter(struct cmd_priv *pcmdpriv, struct cmd_obj *cmd_obj);
extern void rtw_free_cmd_obj(struct cmd_obj *pcmd);
//extern int rtw_enqueue_cmd(struct cmd_priv *pcmdpriv, struct cmd_obj *cmd_obj);
extern void rtw_set_channel_plan_cmd_callback(_adapter *padapter, struct cmd_obj *pcmd);
extern void rtw_survey_cmd_callback(_adapter *padapter, struct cmd_obj *pcmd);
extern void rtw_disassoc_cmd_callback(_adapter *padapter, struct cmd_obj *pcmd, int a3);
extern void rtw_joinbss_cmd_callback(_adapter *padapter, struct cmd_obj *pcmd);
extern void rtw_setstaKey_cmdrsp_callback(_adapter *padapter, struct cmd_obj *pcmd);
extern void rtw_cmd_thread(thread_context context);
extern int rtw_createbss_cmd(_adapter *padapter);
extern int rtw_joinbss_cmd(_adapter *padapter, struct wlan_network *pnetwork);
extern int rtw_disassoc_cmd(_adapter *padapter);
extern int rtw_setopmode_cmd(_adapter *padapter, NDIS_802_11_NETWORK_INFRASTRUCTURE networktype);
extern int rtw_setstakey_cmd(_adapter *padapter, uint8_t *psta, int unicast_key);
extern int rtw_clearstakey_cmd(_adapter *padapter, uint8_t *psta, uint8_t entry, int enqueue);
extern int rtw_addbareq_cmd(_adapter *padapter, int tid, uint8_t *addr);
extern int rtw_dynamic_chk_wk_cmd(_adapter *padapter);
extern struct cmd_obj *rtw_set_chplan_cmd(_adapter *padapter, int chplan, int enqueue);
extern void dynamic_chk_wk_hdl(_adapter *padapter, uint8_t *pbuf, int sz);
extern void lps_ctrl_wk_hdl(_adapter *padapter, int lps_ctrl_type, int a3);
extern int rtw_lps_ctrl_wk_cmd(_adapter *padapter, int lps_ctrl_type, int enqueue);
extern struct cmd_obj *rtw_sitesurvey_cmd(_adapter *padapter, NDIS_802_11_SSID *pssid, int ssid_max_num);
extern void rpt_timer_setting_wk_hdl(_adapter *padapter, uint16_t minRptTime, int a3);
extern int rtw_rpt_timer_cfg_cmd(_adapter *padapter, int minRptTime);
extern int rtw_ps_cmd(_adapter *padapter);
extern int rtw_chk_hi_queue_cmd(_adapter *padapter);
extern signed int rtw_drvextra_cmd_hdl(_adapter *padapter, u8 *pbuf, int a3);
extern int rtw_c2h_wk_cmd(PADAPTER padapter);
// Data declarations
extern const struct cmd_hdl wlancmds[63];
extern const struct _cmd_callback rtw_cmd_callback[63];
//--------------------------------
// netdev.o
// Function declarations
extern struct net_device *rtw_alloc_etherdev_with_old_priv(int sizeof_priv, void *old_priv);
extern struct net_device *rtw_alloc_etherdev(int sizeof_priv);
extern void rtw_free_netdev(struct net_device *netdev);
// Data declarations
//--------------------------------
// phydm.o
// Function declarations
extern void ODM_CmnInfoInit(PDM_ODM_T pDM_Odm, ODM_CMNINFO_E CmnInfo, u4Byte Value);
extern void ODM_CmnInfoHook(PDM_ODM_T pDM_Odm, ODM_CMNINFO_E CmnInfo, PVOID pValue);
extern void ODM_CmnInfoPtrArrayHook(PDM_ODM_T pDM_Odm, ODM_CMNINFO_E CmnInfo, int Index, PVOID pValue);
extern void ODM_CmnInfoUpdate(PDM_ODM_T pDM_Odm, u4Byte CmnInfo, u8Byte Value);
extern void odm_CommonInfoSelfInit(PDM_ODM_T pDM_Odm);
extern void ODM_DMInit(PDM_ODM_T pDM_Odm);
extern void odm_CommonInfoSelfUpdate(PDM_ODM_T pDM_Odm);
extern void ODM_DMWatchdog(PDM_ODM_T pDM_Odm);
extern void odm_CommonInfoSelfReset(PDM_ODM_T pDM_Odm);
extern ADAPTIVITY_STATISTICS *PhyDM_Get_Structure(PDM_ODM_T pDM_Odm, int Structure_Type);
extern void odm_SwAntDetectInit(PDM_ODM_T pDM_Odm);
// Data declarations
//--------------------------------
// rtl8195a_phycfg.o
// Function declarations
extern int PHY_QueryBBReg_8195A_Safe(PADAPTER Adapter, uint32_t RegAddr, uint32_t BitMask);
extern void PHY_SetBBReg_8195A_Safe(PADAPTER Adapter, uint32_t RegAddr, uint32_t BitMask, uint32_t Data);
extern int phy_RFSerialRead_8195A(PADAPTER Adapter, RF_PATH eRFPath, uint32_t Offset);
extern uint32_t PHY_QueryRFReg_8195A(PADAPTER Adapter, uint32_t eRFPath, uint32_t RegAddr, uint32_t BitMask);
extern void PHY_SetRFReg_8195A(PADAPTER Adapter, uint32_t eRFPath, uint32_t RegAddr, uint32_t BitMask, uint32_t Data);
extern BOOL PHY_MACConfig8195A(PADAPTER Adapter);
extern int PHY_BBConfig8195A(PADAPTER Adapter);
extern int PHY_ConfigRFWithParaFile_8195A(PADAPTER Adapter, uint8_t *pFileName, RF_PATH eRFPath);
extern void phy_PowerIndexCheck8195A(PADAPTER Adapter, uint8_t channel, uint8_t *cckPowerLevel, uint8_t *ofdmPowerLevel, uint8_t *BW20PowerLevel, uint8_t *BW40PowerLevel);
extern void PHY_SetTxPowerIndex_8195A(PADAPTER Adapter, uint32_t PowerIndex, int RFPath, int Rate);
extern void phy_TxPwrAdjInPercentage(PADAPTER Adapter, uint8_t *pTxPwrIdx);
extern int PHY_GetTxPowerIndex_8195A(PADAPTER pAdapter, int RFPath, int Rate, CHANNEL_WIDTH BandWidth, uint8_t Channel);
extern void PHY_SetTxPowerLevel8195A(PADAPTER Adapter, int Channel);
extern void phy_SpurCalibration_8195A(PADAPTER pAdapter);
extern void phy_SetRegBW_8195A(PADAPTER Adapter, CHANNEL_WIDTH CurrentBW);
extern int phy_GetSecondaryChnl_8195A(PADAPTER Adapter);
extern void phy_PostSetBwMode8195A(PADAPTER Adapter);
extern void phy_SwChnl8195A(PADAPTER pAdapter);
extern void phy_SwChnlAndSetBwMode8195A(PADAPTER Adapter);
extern void PHY_HandleSwChnlAndSetBW8195A(PADAPTER Adapter, int bSwitchChannel, int bSetBandWidth, uint8_t ChannelNum, CHANNEL_WIDTH ChnlWidth, EXTCHNL_OFFSET ExtChnlOffsetOf40MHz, EXTCHNL_OFFSET ExtChnlOffsetOf80MHz, uint8_t CenterFrequencyIndex1);
extern void PHY_SetBWMode8195A(PADAPTER Adapter, CHANNEL_WIDTH Bandwidth, int Offset);
extern void PHY_SwChnl8195A(PADAPTER Adapter, uint8_t channel);
extern void PHY_SetSwChnlBWMode8195A(PADAPTER Adapter, uint8_t channel, CHANNEL_WIDTH Bandwidth, int Offset40, uint8_t Offset80);
// Data declarations
//--------------------------------
// rtl8195a_pmu_cmd.o
// Function declarations
extern void MediaConnection(PADAPTER padapter, int macid);
extern void MediaDisconnection(PADAPTER padapter, int macid);
extern void RATaskEnable(PADAPTER padapter); 
extern void SetMediaStatus(PADAPTER padapter, int macid, int status);
extern void H2CHDL_JoinInfo(PADAPTER padapter, uint8_t *pCmdBuffer); 
extern void H2CHDL_SetRsvdPage(PADAPTER padapter, uint8_t *pCmdBuffer); 
extern uint32_t H2CCmdCommon(PADAPTER padapter, int ElementID, uint8_t *pCmdBuffer);
// Data declarations
//--------------------------------
// rtl8195a_pmu_task.o
// Function declarations
extern void HalTimerEnable(uint32_t TimerId); 
extern void InitTDMATimer(int Period);
extern void ChangeStateByTDMA(PADAPTER padapter); 
extern void GetMinRateInRRSR(PADAPTER padapter); 
extern void CheckInReqState(PADAPTER padapter); 
extern void InitCheckStateTimer(); 
extern void InitGTimer1ms(PADAPTER padapter, uint8_t IRQDis, int TimerID, uint32_t Period);
extern void DeInitGTimer1ms(PADAPTER padapter, int TimerID);
extern void ChangeTransmiteRate(int offset, uint8_t rate);
extern void PowerBitSetting(int bPowerBit, int offset);
extern void ChkandChangePS(PPS_PARM pPSParm, int bPowerBit);
extern int IssueRsvdPagePacketSetting(int PageNum, int bHwSEQEn, uint8_t RtyLmt);
extern void InitRsvdPgPkt(); 
extern void IssuePSPoll(); 
extern signed int WaitTxStateMachineOk();
extern signed int IssueNullData(PPS_PARM pPSParm, int bPowerBit, uint8_t RtyLmt);
extern void WriteTxPause(uint8_t value, uint8_t rcode); 
extern void PsCloseRF(); 
extern void PsOpenRF(); 
extern void SetPwrStateReg(PPS_PARM pPSParm, int PwrStateType, uint8_t value);
extern BOOL ChkTxQueueIsEmpty();
extern void InitPS(PADAPTER padapter); 
extern void ConfigListenBeaconPeriod(PPS_PARM pPSParm, int RLBM, int AwakeInterval);
extern signed int PS_S2_Condition_Match(PPS_PARM pPSParm);
extern signed int PS_S4_Condition_Match(PADAPTER padapter);
extern unsigned int PS_32K_Condition_Match();
extern void PS_S2ToS3ToS0State(PADAPTER padapter, int nulldata0Allow);
extern void PS_S2ToS0State(PPS_PARM pPSParm); 
extern void PS_S3ToS2orS0State(PPS_PARM pPSParm); 
extern void PS_S0ToS1ToS2State(PADAPTER padapter); 
extern void PS_S1ToS0orS2State(PPS_PARM pPSParm); 
extern void PS_S2ToS4State(PADAPTER padapter); 
extern void PS_S2ToS5State(PPS_PARM pPSParm); 
extern void PS_S5ToS2State(PPS_PARM pPSParm); 
extern void PS_S0ToS6State(PADAPTER padapter); 
extern void PS_S6ToS0State(PPS_PARM pPSParm); 
extern void CheckTSFIsStable(int ReqState);
extern void WaitHWStateReady(); 
extern void SysClkDown(PPS_PARM pPSParm); 
extern void SysClkUp(PPS_PARM pPSParm); 
extern void ResetPSParm(PADAPTER padapter); 
extern void PS_S4ToS2State(PPS_PARM pPSParm, int ReleaseTxPause);
extern void SleepTo32K(PPS_PARM pPSParm); 
extern void Change_PS_State(PADAPTER padapter, int request_ps_state, int nulldata0Allow);
extern void Legacy_PS_Setting(PADAPTER padapter); 
extern void PSModeSetting(PADAPTER padapter, int on);
extern void ChangePSStateByRPWM(PADAPTER padapter); 
extern void ChangeTDMAState(PADAPTER padapter); 
extern void TDMAChangeStateTask(PADAPTER padapter, _irqL a2);
extern void EnterPS(PADAPTER padapter); 
extern void SetSmartPSTimer(PADAPTER padapter); 
extern void GTimer7Handle(void *Data); 
extern void SmartPS2InitTimerAndToGetRxPkt(PADAPTER padapter); 
extern void PS_OnBeacon(PADAPTER padapter); 
extern void PSBcnEarlyProcess(PADAPTER padapter); 
extern void PSMtiBcnEarlyProcess(PADAPTER padapter); 
extern void PSRxBcnProcess(PADAPTER padapter); 
extern void TxPktInPSOn(PADAPTER padapter); 
extern void PsBcnToProcess(PADAPTER padapter); 
extern void GTimer6Handle(void *Data); 
extern signed int RPWMProcess(PADAPTER padapter, int benter32k);
extern void PSSetMode(PADAPTER padapter, PLEGACY_PS_PARM pparm); 
extern void SpeRPT(PADAPTER padapter); 
extern void ISR_BcnEarly(PADAPTER padapter); 
extern void ISR_MtiBcnEarly(PADAPTER padapter); 
extern void ISR_RxBcn(PADAPTER padapter); 
extern void ISR_RxBCMD1(PADAPTER padapter); 
extern void ISR_RxBCMD0(PADAPTER padapter); 
extern void ISR_RxUCMD1(PADAPTER padapter); 
extern void ISR_RxUCMD0(PADAPTER padapter); 
extern void ISR_TxPktIn(PADAPTER padapter); 
extern void H2CHDL_SetPwrMode(PADAPTER padapter, uint8_t *pCmdBuffer); 
extern void CheckInReqStateTask(PADAPTER padapter, int a2, int a3);
extern uint32_t HalGetNullTxRpt(PADAPTER padapter); 
extern void ISR_TBTT(PADAPTER padapter); 
extern void H2CHDL_BcnIgnoreEDCCA(PADAPTER padapter, uint8_t *pCmdBuffer); 
extern void PMUInitial(PADAPTER padapter); 
extern void PMUTask(PADAPTER padapter); 
// Data declarations
extern BOOL bCheckStateTIMER; 
extern uint32_t WifiMcuCmdBitMap_20974; 
//--------------------------------
// rtl8195a_recv.o
// Function declarations
extern int32_t rtl8195a_init_recv_priv(_adapter *padapter); 
// Data declarations
//--------------------------------
// rtl8195a_rxdesc.o
// Function declarations
extern void process_rssi(_adapter *padapter, struct recv_frame *prframe);
extern int32_t translate2dbm(int signal_strength_idx);
extern void rtl8195a_query_rx_desc_status(struct recv_frame *precvframe, uint8_t *pdesc);
extern void rtl8195a_query_rx_phy_status(struct recv_frame *precvframe, uint8_t *pphy_status, int a3);
// Data declarations
//--------------------------------
// rtl8195a_xmit.o
// Function declarations
extern uint8_t *GetTxBufDesc(_adapter *padapter, int queue_index);
extern void UpdateFirstTxbdtoXmitBuf(_adapter *padapter, struct xmit_frame *pxmitframe);
extern BOOL check_nic_enough_desc(_adapter *padapter, struct pkt_attrib *pattrib);
extern int32_t rtl8195ab_init_xmit_priv(PADAPTER padapter); 
extern void rtl8195ab_free_xmit_priv(PADAPTER padapter); 
extern uint32_t GetDmaTxbdIdx(uint32_t ff_hwaddr); 
extern struct xmit_buf *rtl8195a_enqueue_xmitbuf(struct rtw_tx_ring *ring, struct xmit_buf *pxmitbuf);
extern struct list_head *rtl8195a_dequeue_xmitbuf(struct rtw_tx_ring *ring);
extern signed int SetTxbdForLxDMARtl8195ab(_adapter *padapter, struct xmit_frame *pxmitframe, struct tx_buf_desc *pTxbd);
extern void UpdateTxbdHostIndex(_adapter *padapter, uint32_t ff_hwaddr); 
extern struct xmit_buf *SetXimtBuf(struct xmit_frame *pxmitframe);
extern int FreeXimtBuf(struct xmit_buf *pxmitbuf);
extern int rtw_dump_xframe(_adapter *padapter, struct xmit_frame *pxmitframe);
extern BOOL check_tx_desc_resource(_adapter *padapter, int prio);
extern struct list_head *rtw_dequeue_xframe(struct xmit_priv *pxmitpriv, struct hw_xmit *phwxmit_i, sint entry);
extern int32_t rtw_xmitframe_coalesce(_adapter *padapter, _pkt *pkt, struct xmit_frame *pxmitframe);
extern void rtl8195ab_xmitframe_resume(_adapter *padapter); 
extern int32_t rtl8195ab_mgnt_xmit(_adapter *padapter, struct xmit_frame *pmgntframe);
extern int32_t rtl8195ab_hal_xmit(_adapter *padapter, struct xmit_frame *pxmitframe);
extern int32_t rtl8195ab_hal_xmitframe_enqueue(_adapter *padapter, struct xmit_frame *pxmitframe);
// Data declarations
//--------------------------------
// rtw_intfs.o
// Function declarations
extern signed int rtw_init_default_value(_adapter *padapter);
extern void rtw_cancel_all_timer(_adapter *padapter); 
extern signed int rtw_free_drv_sw(_adapter *padapter);
extern signed int rtw_reset_drv_sw(_adapter *padapter);
extern signed int rtw_init_drv_sw(_adapter *padapter);
extern int rtw_start_drv_threads(_adapter *padapter);
extern void rtw_stop_drv_threads(_adapter *padapter); 
// Data declarations
//--------------------------------
// rtw_mlme.o
// Function declarations
extern void rtw_free_mlme_ie_data(uint8_t **ppie, uint32_t *plen); 
extern void rtw_init_mlme_timer(_adapter *padapter); 
extern void rtw_del_mlme_timer(struct mlme_priv *pmlmepriv);
extern void reconnect_timer_hdl(void *FunctionContext); 
extern uint8_t *rtw_init_mlme_priv(_adapter *padapter, int a2, int a3);
extern void rtw_mfree_mlme_priv_lock(struct mlme_priv *pmlmepriv);
extern void rtw_free_mlme_priv_ie_data(struct mlme_priv *pmlmepriv);
extern void rtw_free_mlme_priv(struct mlme_priv *pmlmepriv);
extern sint rtw_enqueue_network(_queue *queue, struct wlan_network *pnetwork);
extern struct list_head *rtw_alloc_network(struct mlme_priv *pmlmepriv, _irqL a2, int a3);
extern void rtw_free_network(struct mlme_priv *pmlmepriv, struct wlan_network *pnetwork, int isfreeall);
extern void rtw_free_network_nolock(struct mlme_priv *pmlmepriv, struct wlan_network *pnetwork);
extern _queue *rtw_find_network(_queue *scanned_queue, uint8_t *addr);
extern void rtw_free_network_queue(_adapter *padapter, int isfreeall, int a3);
extern sint rtw_if_up(_adapter *padapter); 
extern void rtw_generate_random_ibss(uint8_t *pibss); 
extern uint8_t *rtw_get_capability_from_ie(uint8_t *ie); 
extern int rtw_get_capability(WLAN_BSSID_EX *bss, int a2, int a3);
extern uint8_t *rtw_get_beacon_interval_from_ie(uint8_t *ie); 
extern uint8_t *rtw_init_mlme_priv(_adapter *padapter, int a2, int a3);
extern uint32_t rtw_is_same_ibss(_adapter *adapter, struct wlan_network *pnetwork);
extern int is_same_network(WLAN_BSSID_EX *src, WLAN_BSSID_EX *dst, int a3);
extern struct list_head *rtw_get_oldest_wlan_network(_queue *scanned_queue);
extern void update_network(WLAN_BSSID_EX *dst, WLAN_BSSID_EX *src, _adapter *padapter, int update_ie);
extern void rtw_update_scanned_network(_adapter *adapter, WLAN_BSSID_EX *target); 
extern void rtw_add_network(_adapter *adapter, WLAN_BSSID_EX *pnetwork, int a3);
extern void rtw_survey_event_callback(_adapter *adapter, uint8_t *pbuf, int a3);
extern void rtw_free_assoc_resources(_adapter *adapter, int lock_scanned_queue); 
extern void rtw_indicate_connect(_adapter *padapter); 
extern void rtw_indicate_disconnect(_adapter *padapter); 
extern void rtw_joinbss_event_callback(_adapter *adapter, uint8_t *pbuf); 
extern signed int search_max_mac_id(_adapter *padapter);
extern void rtw_stassoc_hw_rpt(_adapter *adapter, struct sta_info *psta);
extern void rtw_stassoc_event_callback(_adapter *adapter, uint8_t *pbuf); 
extern void rtw_stadel_event_callback(_adapter *adapter, uint8_t *pbuf); 
extern void rtw_join_timeout_handler(_adapter *adapter, _irqL a2, int a3, int a4);
//void rtw_join_timeout_handler(void *FunctionContext, _irqL a2, int a3, int a4);
extern void rtw_scan_timeout_handler(_adapter *adapter, _irqL a2, int a3);
//void rtw_scan_timeout_handler(void *FunctionContext);
extern void rtw_dynamic_check_timer_handlder(_adapter *adapter); 
extern void dynamic_check_timer_handlder(void *FunctionContext); 
extern int rtw_select_and_join_from_scanned_queue(struct mlme_priv *pmlmepriv, _irqL a2, int a3);
extern void rtw_surveydone_event_callback(_adapter *adapter, uint8_t *pbuf); 
extern sint rtw_set_auth(_adapter *adapter, struct security_priv *psecuritypriv);
extern sint rtw_set_key(_adapter *adapter, struct security_priv *psecuritypriv, sint keyid, uint8_t set_tx);
extern unsigned int rtw_restruct_wmm_ie(_adapter *adapter, uint8_t *in_ie, uint8_t *out_ie, unsigned int in_len, unsigned int initial_out_len);
extern sint rtw_restruct_sec_ie(_adapter *adapter, uint8_t *in_ie, uint8_t *out_ie, unsigned int in_len); 
extern void rtw_joinbss_reset(_adapter *padapter); 
extern unsigned int rtw_restructure_ht_ie(_adapter *padapter, uint8_t *in_ie, uint8_t *out_ie, unsigned int in_len, unsigned int *pout_len);
extern void rtw_update_ht_cap(_adapter *padapter, uint8_t *pie, unsigned int ie_len); 
extern void rtw_joinbss_event_prehandle(_adapter *adapter, uint8_t *pbuf, int a3);
extern void rtw_issue_addbareq_cmd(_adapter *padapter, struct xmit_frame *pxmitframe);
extern sint rtw_linked_check(_adapter *padapter); 
extern sint rtw_buddy_adapter_up(sint result);
extern sint check_buddy_fwstate(sint result, sint state);
// Data declarations
extern uint8_t auto_reconnect_running; 
extern void (*p_wlan_autoreconnect_hdl)(rtw_security_t, char *, int, char *, int, int); 
//--------------------------------
// rtw_mlme_ext.o
// Function declarations
extern u8 *get_da(u8 *pframe);
extern u8 *get_sa(u8 *pframe);
extern signed int OnAction(_adapter *padapter, struct recv_frame *precv_frame);
extern signed int DoReserved(_adapter *padapter, struct recv_frame *precv_frame);
extern void mgt_dispatcher(_adapter *padapter, struct mlme_handler *ptable, struct recv_frame *precv_frame);
extern int rtw_is_channel_set_contains_channel(RT_CHANNEL_INFO *channel_set, const uint32_t channel_num, int *pchannel_idx); 
extern int init_hw_mlme_ext(_adapter *padapter); 
extern unsigned int init_channel_set(_adapter *padapter, int ChannelPlan, RT_CHANNEL_INFO *channel_set);
extern void free_mlme_ext_priv(struct mlme_ext_priv *pmlmeext);
//void mgt_dispatcher(_adapter *padapter, struct recv_frame *precv_frame, int a3);
extern unsigned int OnAction_public(_adapter *padapter, struct recv_frame *precv_frame);
extern signed int OnAction_p2p(_adapter *padapter, struct recv_frame *precv_frame);
extern struct xmit_frame *alloc_mgtxmitframe(struct xmit_priv *pxmitpriv);
extern struct xmit_frame *alloc_FwRsvdframe(struct xmit_priv *pxmitpriv, uint32_t size);
extern void update_mgnt_tx_rate(_adapter *padapter, uint8_t rate); 
extern void update_mgntframe_attrib(_adapter *padapter, struct pkt_attrib *pattrib);
extern struct xmit_frame *rtw_build_mgnt_frame(_adapter *padapter, uint8_t *data, int len);
extern void dump_mgntframe(_adapter *padapter, struct xmit_frame *pmgntframe);
extern int rtw_send_mgnt(_adapter *padapter, uint8_t *data, int len, uint16_t flags);
extern void issue_action_BSSCoexistPacket(_adapter *padapter); 
extern uint32_t update_hidden_ssid(uint8_t *ies, uint32_t ies_len, int hidden_ssid_mode);
extern void issue_beacon(_adapter *padapter); 
extern void issue_probersp(_adapter *padapter, u8 *da, uint8_t is_valid_p2p_probereq);
extern signed int OnProbeReq(_adapter *padapter, struct recv_frame *precv_frame);
extern void issue_probereq(_adapter *padapter, NDIS_802_11_SSID *pssid, int blnbc);
extern void issue_auth(_adapter *padapter, struct sta_info *psta, int status);
extern signed int OnAuth(_adapter *padapter, struct recv_frame *precv_frame);
extern void issue_asocrsp(_adapter *padapter, uint16_t status, struct sta_info *pstat, int pkt_type);
extern void issue_assocreq(_adapter *padapter); 
extern void issue_nulldata(_adapter *padapter, unsigned int power_mode); 
extern void issue_qos_nulldata(_adapter *padapter, u8 *da, uint16_t tid);
extern void issue_deauth(_adapter *padapter, u8 *da, uint32_t reason);
extern void issue_action_BA(_adapter *padapter, u8 *raddr, u8 action, uint16_t status);
extern signed int OnAction_back(_adapter *padapter, struct recv_frame *precv_frame);
extern signed int send_beacon(_adapter *padapter);
extern signed int collect_bss_info(_adapter *padapter, struct recv_frame *precv_frame, WLAN_BSSID_EX *bssid);
extern void start_clnt_auth(_adapter *padapter); 
extern void start_clnt_assoc(_adapter *padapter); 
extern signed int OnAuthClient(_adapter *padapter, struct recv_frame *precv_frame);
extern int report_scan_result_one(_adapter *padapter, WLAN_BSSID_EX *bssid); 
extern int add_site_survey(_adapter *padapter, WLAN_BSSID_EX *bssid); 
extern void report_survey_event(_adapter *padapter, struct recv_frame *precv_frame);
extern signed int OnProbeRsp(_adapter *padapter, struct recv_frame *precv_frame);
extern void report_surveydone_event(_adapter *padapter); 
extern void report_join_res(_adapter *padapter, int res); 
extern signed int OnAssocRsp(_adapter *padapter, struct recv_frame *precv_frame);
extern void report_del_sta_event(_adapter *padapter, u8 *MacAddr, uint16_t reason);
extern signed int receive_disconnect(_adapter *padapter, u8 *MacAddr, uint16_t reason);
extern signed int OnBeacon(_adapter *padapter, struct recv_frame *precv_frame);
extern signed int OnDeAuth(_adapter *padapter, struct recv_frame *precv_frame);
extern signed int OnDisassoc(_adapter *padapter, struct recv_frame *precv_frame);
extern void report_add_sta_event(_adapter *padapter, u8 *MacAddr, int cam_idx);
extern signed int OnAssocReq(_adapter *padapter, struct recv_frame *precv_frame);
extern signed int rtw_port_switch_chk(_adapter *adapter);
extern void update_sta_info(_adapter *padapter, struct sta_info *psta);
extern void mlmeext_sta_del_event_callback(_adapter *padapter); 
extern void linked_info_dump(_adapter *padapter, _irqL a2, int a3);
extern void linked_rx_signal_strehgth_display(_adapter *padapter, int a2);
extern void linked_status_chk(_adapter *padapter, int a2);
extern void survey_timer_hdl(_adapter *padapter, int a2);
//void survey_timer_hdl(void *FunctionContext);
extern void link_timer_hdl(_adapter *padapter); 
//void link_timer_hdl(void *FunctionContext);
extern void addba_timer_hdl(struct sta_info *psta);
extern int NULL_hdl(_adapter *padapter, uint8_t *pbuf);
extern int setopmode_hdl(_adapter *padapter, uint8_t *pbuf, int a3);
extern int disconnect_hdl(_adapter *padapter, u8 *pbuf, int a3);
extern int setauth_hdl(_adapter *padapter, u8 *pbuf);
extern int setkey_hdl(_adapter *padapter, uint8_t *pbuf, int a3, int a4);
extern signed int set_stakey_hdl(_adapter *padapter, uint8_t *pbuf);
extern int set_tx_beacon_cmd(_adapter *padapter);
extern int mlme_evt_hdl(_adapter *padapter, u8 *pbuf);
extern int tx_beacon_hdl(_adapter *padapter, u8 *pbuf, int a3);
extern sint check_buddy_mlmeinfo_state(sint result, uint32_t state);
extern void site_survey(_adapter *padapter, int a2, int a3);
extern int sitesurvey_cmd_hdl(_adapter *padapter, uint8_t *pbuf, int a3);
extern int concurrent_chk_start_clnt_join(_adapter *padapter); 
extern void start_clnt_join(_adapter *padapter, int a2, int a3);
extern signed int join_cmd_hdl(_adapter *padapter, uint8_t *pbuf, int a3);
extern void concurrent_chk_joinbss_done(_adapter *padapter, int join_res); 
extern void mlmeext_joinbss_event_callback(_adapter *padapter, int join_res, int a3);
extern signed int set_chplan_hdl(_adapter *padapter, u8 *pbuf);
extern void init_mlme_ext_timer(_adapter *padapter); 
extern int init_mlme_ext_priv(_adapter *padapter); 
// Data declarations
extern const RT_CHANNEL_PLAN_2G RTW_ChannelPlan2G[8]; /* =
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
extern const uint8_t WPS_OUI[4]; // = { 0u, 80u, 242u, 4u }; 
extern mac_monitor_ptr mac_monitor_callback; 
extern struct mlme_handler mlme_sta_tbl[14]; /* =
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
//_UNKNOWN unk_4AE4; // weak
extern struct list_head *mf_list_head;
extern const uint8_t WMM_INFO_OUI[6]; // = { 0u, 80u, 242u, 2u, 0u, 1u }; 
extern uint8_t pscan_retry_cnt_21430; 
extern const uint8_t RTW_WPA_OUI[4]; // = { 0u, 80u, 242u, 1u }; 
extern const uint8_t WMM_PARA_OUI[6]; // = { 0u, 80u, 242u, 2u, 1u, 1u }; 
extern const RT_CHANNEL_PLAN_MAP RTW_ChannelPlanMap[8]; /* =
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
extern const uint8_t null_addr[6]; // = { 0u, 0u, 0u, 0u, 0u, 0u }; 
extern const uint8_t WMM_OUI[4]; // = { 0u, 80u, 242u, 2u }; 
extern const struct fwevent wlanevents[24]; /* =
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
extern u8 *get_hdr_bssid(u8 *pframe);
extern int filter_packet(u8 *buf, int length);
extern signed int promisc_get_encrypt(_adapter *padapter, uint8_t *bssid);
extern void promisc_info_get(_adapter *padapter, struct recv_frame *prframe, ieee80211_frame_info_t *ppromisc_info, int a4);
extern void promisc_set_enable(_adapter *padapter, int enabled, int len_used);
extern void promisc_deinit(_adapter *padapter); 
extern int promisc_recv_func(_adapter *padapter, struct recv_frame *rframe);
extern int promisc_set(rtw_rcr_level_t enabled, void (*callback)(u8 *, unsigned int, void *), int len_used);
extern int promisc_set_mgntframe(int result);
extern int is_promisc_enabled();
extern void promisc_issue_probereq(); 
extern void promisc_issue_probersp(u8 *da);
extern int promisc_get_fixed_channel(void *fixed_bssid, uint8_t *ssid, int *ssid_length); 
// Data declarations
extern void (*promisc_callback_all)(u8 *, unsigned int, void *);
extern _sema promisc_sema; 
extern const u8 zero_bssid[6]; // = { 0u, 0u, 0u, 0u, 0u, 0u };
extern void (*promisc_callback)(u8 *, unsigned int, void *);
//--------------------------------
// rtw_psk.o
// Function declarations
extern void SetEAPOL_KEYIV(OCTET_STRING ocDst, uint64_t a2, OCTET32_INTEGER oc32Counter);
extern void ToDrv_SetPTK(_adapter *padapter, struct sta_info *psta);
extern void Message_ReplayCounter_OC2LI(int a1, LARGE_INTEGER *li);
extern int Message_SmallerEqualReplayCounter(LARGE_INTEGER li1, int a2);
extern void Message_setReplayCounter(int a1, unsigned int h, unsigned int l);
extern void INCLargeInteger(LARGE_INTEGER *x); 
extern void INCOctet16_INTEGER(OCTET16_INTEGER *x); 
extern OCTET32_INTEGER *INCOctet32_INTEGER(OCTET32_INTEGER *x); 
extern void ToDrv_DisconnectSTA(_adapter *padapter, struct sta_info *psta, int reason);
extern int CheckMIC(OCTET_STRING EAPOLMsgRecvd, u8 *key, int keylen);
extern void CalcMIC(OCTET_STRING EAPOLMsgSend, int algo, u8 *key, int keylen);
extern int DecWPA2KeyData(WPA_STA_INFO *pStaInfo, u8 *key, int keylen, u8 *kek, int keklen, u8 *kout);
extern int DecGTK(OCTET_STRING EAPOLMsgRecvd, u8 *kek, int keklen, int keylen, u8 *kout);
extern void ToDrv_SetGTK(_adapter *padapter); 
extern void init_wpa_sta_info(_adapter *padapter, struct sta_info *psta);
extern void SendEAPOL(_adapter *padapter, struct sta_info *psta, int resend);
extern void ClientSendEAPOL(_adapter *padapter, struct sta_info *psta, int resend);
extern void ResendTimeout(void *task_psta, _irqL a2);
extern void EAPOLKeyRecvd(_adapter *padapter, struct sta_info *psta);
extern void ClientEAPOLKeyRecvd(_adapter *padapter, struct sta_info *psta);
extern void set_wpa_global_PSK(u8 *key);
extern void psk_derive(_adapter *padapter, u8 *passphrase, u8 *ssid);
extern void psk_init(_adapter *padapter, u8 *pie, int ielen);
extern int psk_strip_rsn_pairwise(uint8_t *ie, int ie_len);
extern int psk_strip_wpa_pairwise(uint8_t *ie, int ie_len);
extern int tkip_send_mic_failure_report(_adapter *padapter); 
// Data declarations
extern uint8_t psk_essid[2][36]; 
extern uint8_t psk_passphrase[2][65]; 
extern char PMKID_KDE_TYPE_17744[6]; 
extern uint8_t wpa_global_PSK[2][40]; 
//--------------------------------
// rtw_pwrctrl.o
// Function declarations
extern void pwr_state_check_handler(void *FunctionContext); 
extern void ips_enter(_adapter *padapter); 
extern int ips_leave(_adapter *padapter); 
extern signed int rtw_pwr_unassociated_idle(_adapter *adapter);
extern void rtw_ps_processor(_adapter *padapter); 
extern void rtw_set_rpwm(PADAPTER padapter, uint8_t pslv); 
extern int PS_RDY_CHECK(_adapter *padapter);
extern void rtw_set_ps_mode(PADAPTER padapter, int ps_mode, int smart_ps, int bcn_ant_mode);
extern int32_t LPS_RF_ON_check(PADAPTER padapter, uint32_t delay_ms); 
extern void LPS_Enter(PADAPTER padapter); 
extern void LPS_Leave(PADAPTER padapter); 
extern void LeaveAllPowerSaveMode(PADAPTER Adapter); 
extern void rtw_init_pwrctrl_priv(PADAPTER padapter); 
extern void rtw_free_pwrctrl_priv(PADAPTER adapter); 
extern int rtw_pwr_wakeup(_adapter *padapter, uint32_t ips_deffer_ms, const char *caller); 
extern int rtw_pm_set_lps(_adapter *padapter, int mode);
extern int rtw_pm_set_ips(_adapter *padapter, int mode);
extern int rtw_pm_set_tdma_param(_adapter *padapter, uint8_t tdma_slot_period, uint8_t tdma_rfon_period_len_1, uint8_t tdma_rfon_period_len_2, uint8_t tdma_rfon_period_len_3);
extern int rtw_pm_set_lps_dtim(_adapter *padapter, uint8_t lps_dtim); 
extern int rtw_pm_get_lps_dtim(_adapter *padapter);
// Data declarations
//--------------------------------
// rtw_recv.o
// Function declarations
extern uint8_t *recvframe_pull(uint8_t *result, sint sz);
extern uint8_t *recvframe_pull_tail(uint8_t *result, sint sz);
extern void rtw_signal_stat_timer_hdl(void *FunctionContext); 
extern void rtw_init_sta_recv_priv(struct sta_recv_priv *psta_recvpriv);
extern sint rtw_init_recv_priv(struct recv_priv *precvpriv, _adapter *padapter);
extern void rtw_mfree_recv_priv_lock(struct recv_priv *precvpriv);
extern struct list_head *rtw_alloc_recvframe(_queue *pfree_recv_queue);
//struct list_head *rtw_alloc_recvframe(_queue *pfree_recv_queue, _irqL a2, int a3);
extern int rtw_free_recvframe(struct recv_frame *precvframe, _queue *pfree_recv_queue);
extern sint rtw_enqueue_recvframe(struct recv_frame *precvframe, _queue *queue);
extern sint rtw_enqueue_recvframe(struct recv_frame *precvframe, _queue *queue);
extern void rtw_free_recvframe_queue(_queue *pframequeue, _queue *pfree_recv_queue); 
extern int rtw_free_uc_swdec_pending_queue(_adapter *adapter, _irqL a2, int a3);
extern void rtw_free_recv_priv(struct recv_priv *precvpriv);
extern sint rtw_enqueue_recvbuf_to_head(struct recv_buf *precvbuf, _queue *queue);
extern uint32_t rtw_free_buf_pending_queue(_adapter *adapter); 
extern sint rtw_enqueue_recvbuf(struct recv_buf *precvbuf, _queue *queue);
extern struct list_head *rtw_dequeue_recvbuf(_queue *queue, _irqL a2, int a3);
extern sint recvframe_chkmic(_adapter *adapter, struct recv_frame *precvframe, int a3, int a4);
extern struct recv_frame *decryptor(_adapter *padapter, struct recv_frame *precv_frame);
extern struct recv_frame *portctrl(_adapter *adapter, struct recv_frame *precv_frame);
extern sint recv_decache(struct recv_frame *precv_frame, uint8_t bretry, struct stainfo_rxcache *prxcache);
extern void process_pwrbit_data(_adapter *padapter, struct recv_frame *precv_frame, int a3, int a4);
extern void process_wmmps_data(_adapter *padapter, struct recv_frame *precv_frame);
extern void count_rx_stats(_adapter *padapter, struct recv_frame *prframe, struct sta_info *sta);
extern sint sta2sta_data_frame(_adapter *adapter, struct recv_frame *precv_frame, struct sta_info **psta, int a4);
extern sint ap2sta_data_frame(_adapter *adapter, struct recv_frame *precv_frame, struct sta_info **psta);
extern sint sta2ap_data_frame(_adapter *adapter, struct recv_frame *precv_frame, struct sta_info **psta);
extern sint validate_recv_ctrl_frame(_adapter *padapter, struct recv_frame *precv_frame);
extern sint validate_recv_data_frame(_adapter *adapter, struct recv_frame *precv_frame);
extern sint wlanhdr_to_ethhdr(struct recv_frame *precvframe, int a2, int a3);
extern struct recv_frame *recvframe_defrag(_adapter *adapter, _queue *defrag_q);
extern _queue *recvframe_chk_defrag(PADAPTER padapter, struct recv_frame *precv_frame);
extern sint validate_recv_mgnt_frame(PADAPTER padapter, struct recv_frame **pprecv_frame);
extern sint validate_recv_frame(_adapter *adapter, struct recv_frame **pprecv_frame);
extern int amsdu_to_msdu(_adapter *padapter, struct recv_frame *prframe);
extern int check_indicate_seq(struct recv_reorder_ctrl *preorder_ctrl, int seq_num);
extern int enqueue_reorder_recvframe(struct recv_reorder_ctrl *preorder_ctrl, struct recv_frame *prframe);
extern int recv_indicatepkts_in_order(_adapter *padapter, struct recv_reorder_ctrl *preorder_ctrl, int bforced);
extern int recv_indicatepkt_reorder(_adapter *padapter, struct recv_frame *prframe, int a3);
extern void rtw_reordering_ctrl_timeout_handler(void *pcontext, _irqL a2);
extern int process_recv_indicatepkts(_adapter *padapter, struct recv_frame *prframe, int a3);
extern int recv_func_prehandle(_adapter *padapter, struct recv_frame *rframe, int a3);
extern int recv_func_posthandle(_adapter *padapter, struct recv_frame *prframe);
extern int recv_func(_adapter *padapter, struct recv_frame *rframe, int a3);
extern int32_t rtw_recv_entry(struct recv_frame *precvframe, int a2, int a3);
extern void rtw_recv_tasklet(thread_context context); 
// Data declarations
extern const uint8_t SNAP_ETH_TYPE_APPLETALK_AARP[2]; // = { 128u, 243u }; 
extern const uint8_t SNAP_ETH_TYPE_IPX[2]; // = { 129u, 55u }; 
extern const uint8_t rtw_bridge_tunnel_header[6]; // = { 170u, 170u, 3u, 0u, 0u, 248u }; 
extern const uint8_t oui_8021h[3]; // = { 0u, 0u, 248u };
extern const uint8_t rtw_rfc1042_header[6]; // = { 170u, 170u, 3u, 0u, 0u, 0u }; 
extern const uint8_t SNAP_HDR_APPLETALK_DDP[3]; // = { 8u, 0u, 7u }; 
extern const uint8_t SNAP_ETH_TYPE_APPLETALK_DDP[2]; // = { 128u, 155u }; 
extern const uint8_t oui_rfc1042[3]; // = { 0u, 0u, 0u }; 
//--------------------------------
// rtw_security.o
// Function declarations
extern void rtw_wep_encrypt(_adapter *padapter, uint8_t *pxmitframe); 
extern void rtw_wep_decrypt(_adapter *padapter, uint8_t *precvframe); 
extern signed int rtw_tkip_encrypt(_adapter *padapter, uint8_t *pxmitframe);
extern int rtw_tkip_decrypt(_adapter *padapter, uint8_t *precvframe);
extern signed int rtw_aes_encrypt(_adapter *padapter, uint8_t *pxmitframe, int a3, int a4);
extern int rtw_aes_decrypt(_adapter *padapter, uint8_t *precvframe, int a3, int a4);
extern void rtw_use_tkipkey_handler(void *FunctionContext); 
extern int rtw_init_sec_priv(_adapter *padapter);
extern void rtw_free_sec_priv(struct security_priv *psecpriv);
// Data declarations
//--------------------------------
// rtw_sta_mgt.o
// Function declarations
extern int wifi_mac_hash(uint8_t *mac);
extern void rtw_init_stainfo(struct sta_info *psta);
extern int rtw_init_sta_priv(_adapter *padapter);
extern void rtw_free_sta_xmit_priv_lock(struct sta_xmit_priv *psta_xmitpriv);
extern void rtw_mfree_stainfo(struct sta_info *psta);
extern void rtw_mfree_sta_priv_lock(struct sta_priv *pstapriv);
extern signed int rtw_free_sta_priv(struct sta_priv *pstapriv);
extern void init_addba_retry_timer(_adapter *padapter, struct sta_info *psta);
extern struct sta_info *rtw_alloc_stainfo(struct sta_priv *pstapriv, uint8_t *hwaddr, _irqL a3, _irqL a4);
extern signed int rtw_free_stainfo(_adapter *padapter, struct sta_info *psta, int a3);
extern struct sta_info *rtw_get_stainfo(struct sta_priv *pstapriv, uint8_t *hwaddr, int a3, int a4);
extern signed int rtw_init_bcmc_stainfo(_adapter *padapter, int a2, int a3);
extern struct sta_info *rtw_get_bcmc_stainfo(_adapter *padapter, int a2, int a3);
extern void rtw_free_all_stainfo(_adapter *padapter, _irqL a2, int a3);
// Data declarations
//--------------------------------
// rtw_xmit.o
// Function declarations
extern void init_txservq(struct tx_servq *ptxservq);
extern void set_qos(struct pkt_file *ppktfile, struct pkt_attrib *pattrib);
extern void rtw_init_sta_xmit_priv(struct sta_xmit_priv *psta_xmitpriv);
extern void rtw_mfree_xmit_priv_lock(struct xmit_priv *pxmitpriv);
extern int qos_acm(uint8_t acm_mask, int priority);
extern int32_t xmitframe_addmic(_adapter *padapter, struct xmit_frame *pxmitframe);
extern int32_t xmitframe_swencrypt(_adapter *padapter, struct xmit_frame *pxmitframe);
extern int32_t rtw_make_wlanhdr(_adapter *padapter, uint8_t *hdr, struct pkt_attrib *pattrib);
extern int32_t rtw_txframes_pending(_adapter *padapter); 
extern int32_t rtw_txframes_sta_ac_pending(_adapter *padapter, struct pkt_attrib *pattrib);
extern void rtw_txframes_update_attrib_vcs_info(_adapter *padapter, struct xmit_frame *pxmitframe);
extern int rtw_calculate_wlan_pkt_size_by_attribue(struct pkt_attrib *pattrib);
extern int32_t rtw_put_snap(uint8_t *data, int h_proto);
extern void rtw_update_protection(_adapter *padapter, uint8_t *ie, unsigned int ie_len); 
extern void rtw_count_tx_stats(PADAPTER padapter, struct xmit_frame *pxmitframe, uint64_t sz);
extern int32_t rtw_free_xmitbuf_ext(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf, int a3);
extern struct list_head *rtw_alloc_xmitframe(struct xmit_priv *pxmitpriv, _irqL a2);
extern int32_t rtw_free_xmitframe(struct xmit_priv *pxmitpriv, struct xmit_frame *pxmitframe);
extern void rtw_free_xmitframe_queue(struct xmit_priv *pxmitpriv, _queue *pframequeue);
extern struct tx_servq *rtw_get_sta_pending(_adapter *padapter, struct sta_info *psta, sint up, uint8_t *ac);
extern struct sta_info *rtw_xmit_classifier(_adapter *padapter, struct xmit_frame *pxmitframe);
extern BOOL rtw_xmitframe_enqueue(_adapter *padapter, struct xmit_frame *pxmitframe);
extern void rtw_alloc_hwxmits(_adapter *padapter); 
extern void rtw_free_hwxmits(_adapter *padapter); 
extern void rtw_free_xmit_priv(struct xmit_priv *pxmitpriv);
extern void rtw_init_hwxmits(struct hw_xmit *phwxmit, sint entry);
extern int32_t rtw_init_xmit_priv(struct xmit_priv *pxmitpriv, _adapter *padapter);
extern signed int rtw_get_ff_hwaddr(struct xmit_frame *pxmitframe);
extern sint xmitframe_enqueue_for_sleeping_sta(_adapter *padapter, struct xmit_frame *pxmitframe);
extern void dequeue_xmitframes_to_sleeping_queue(_adapter *padapter, struct sta_info *psta, _queue *pframequeue);
extern void stop_sta_xmit(_adapter *padapter, struct sta_info *psta);
extern void wakeup_sta_to_xmit(_adapter *padapter, struct sta_info *psta);
extern void xmit_delivery_enabled_frames(_adapter *padapter, struct sta_info *psta, int a3);
extern void rtw_xmit_tasklet(thread_context context); 
extern int32_t rtw_xmit(_adapter *padapter, _pkt **ppkt); 
extern BOOL rtw_sctx_chk_waring_status(int status);
extern void rtw_sctx_done_err(struct submit_ctx **sctx, int status);
extern struct list_head *rtw_alloc_xmitbuf(struct xmit_priv *pxmitpriv, _irqL a2);
extern int32_t rtw_free_xmitbuf(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf, int a3);
extern struct xmit_buf *rtw_alloc_xmitbuf_ext(struct xmit_priv *pxmitpriv, uint32_t size, int a3);
extern void rtw_sctx_done(struct submit_ctx **sctx);
// Data declarations
//--------------------------------
// phydm_RegConfig8195A.o
// Function declarations
extern void odm_ConfigRFReg_8195A(PDM_ODM_T pDM_Odm, u4Byte Addr, u4Byte Data, ODM_RF_RADIO_PATH_E RF_PATH, u4Byte RegAddr);
extern void odm_ConfigRF_RadioA_8195A(PDM_ODM_T pDM_Odm, u4Byte Addr, u4Byte Data);
extern void odm_ConfigBB_AGC_8195A(PDM_ODM_T pDM_Odm, u4Byte Addr, u4Byte Bitmask, u4Byte Data);
extern void odm_ConfigBB_PHY_REG_PG_8195A(PDM_ODM_T pDM_Odm, u4Byte Band, u4Byte RfPath, u4Byte TxNum, u4Byte Addr, u4Byte Bitmask, u4Byte Data);
extern void odm_ConfigBB_PHY_8195A(PDM_ODM_T pDM_Odm, u4Byte Addr, u4Byte Bitmask, u4Byte Data);
extern void odm_ConfigBB_TXPWR_LMT_8195A(PDM_ODM_T pDM_Odm, int Regulation, int Band, int Bandwidth, u1Byte RateSection, u1Byte RfPath, u1Byte Channel, u1Byte PowerLimit);
// Data declarations
//--------------------------------
// lxbus_hci_intf.o
// Function declarations
extern struct dvobj_priv *hci_lxbus_dvobj_init();
extern void hci_lxbus_dvobj_deinit(struct dvobj_priv *dvobj);
extern void hci_lxbus_dvobj_request_irq(struct dvobj_priv *dvobj);
extern void hci_lxbus_free_irq(struct dvobj_priv *dvobj);
extern void hci_lxbus_intf_stop(PADAPTER padapter);
// Data declarations
//--------------------------------
// lxbus_intf.o
// Function declarations
extern signed int lextra_bus_dma_Interrupt(void *data);
// Data declarations
//--------------------------------
// lxbus_ops.o
// Function declarations
extern void rtl8195a_free_rx_ring(_adapter *padapter);
extern int bus_write32(struct dvobj_priv *pintfhdl, uint32_t addr, uint32_t val, int32_t *err);
extern int bus_write16(struct dvobj_priv *pintfhdl, uint32_t addr, int val, int32_t *err);
extern int bus_write8(struct dvobj_priv *pintfhdl, uint32_t addr, int val, int32_t *err);
extern uint32_t bus_read32(struct dvobj_priv *pintfhdl, uint32_t addr, int32_t *err);
extern int bus_read16(struct dvobj_priv *pintfhdl, uint32_t addr, int32_t *err);
extern int bus_read8(struct dvobj_priv *pintfhdl, uint32_t addr, int32_t *err);
extern void rtl8195a_free_tx_ring(_adapter *padapter, unsigned int prio);
extern signed int rtl8195a_init_desc_ring(_adapter *padapter);
extern signed int rtl8195a_free_desc_ring(_adapter *padapter);
extern void rtl8195a_reset_desc_ring(_adapter *padapter, _irqL a2, int a3);
extern void InitLxDmaRtl8195a(_adapter *Adapter);
extern void rtl8195a_prepare_bcn_tasklet(void *priv);
extern signed int get_txdesc_buf_addr(int ff_hwaddr);
extern signed int rtl8195a_check_txdesc_closed(_adapter *padapter, uint32_t queue_idx, uint32_t index);
extern void rtl8195a_tx_isr(PADAPTER Adapter, int prio);
extern signed int InterruptRecognized8195a(PADAPTER Adapter);
extern void InitInterrupt8195a(PADAPTER padapter);
extern void EnableDMA8195a(PADAPTER padapter);
extern void EnableInterrupt8195a(PADAPTER padapter);
extern void DisableDMA8195a(PADAPTER padapter);
extern void DisableInterrupt8195a(PADAPTER padapter);
extern void UpdateInterruptMask8195a(PADAPTER Adapter, uint32_t *pAddMSRB, uint32_t *pRemoveMSR);
extern signed int CheckRxTgRtl8195a(_adapter *padapter, uint8_t *rx_desc, uint16_t rx_queue_idx);
extern int rtl8192ee_check_rxdesc_remain(_adapter *padapter, int rx_queue_idx);
extern void rtl8195a_recv_tasklet(void *priv);
extern void rtl8195a_tx_int_handler(_adapter *padapter, int a2, int a3);
extern int32_t InterruptHandle8195a(_adapter *padapter, int a2, int a3);
extern void rtl8195a_xmit_tasklet(void *priv);
extern void lxbus_set_intf_ops(struct _io_ops *pops);
// Data declarations
extern uint8_t rx_ring_pool[4][2104];
extern u16 CSWTCH_48[8]; // = { 928, 932, 936, 940, 936, 944, 952, 936 };
extern uint8_t stop_report_count_20629;
//--------------------------------
// phydm_ACS.o
// Function declarations
extern void phydm_CLMInit(PVOID pDM_VOID, u2Byte sampleNum);
extern void phydm_CLMtrigger(PVOID pDM_VOID);
extern int phydm_checkCLMready(PVOID pDM_VOID);
extern int phydm_getCLMresult(PVOID pDM_VOID);
extern void phydm_NHMInit(PVOID pDM_VOID, u2Byte sampleNum, int round);
extern void phydm_NHMtrigger(PVOID pDM_VOID);
extern void phydm_FalseAlarmCounterStatistics(PVOID pDM_VOID);
extern void phydm_getNHMresult(PVOID pDM_VOID, unsigned int *fa_crc32_total_cnt, unsigned int *cca_count, unsigned int *nhm_cnt_exp_sum, u8 round);
// Data declarations
//--------------------------------
// PhyDM_Adaptivity.o
// Function declarations
extern void Phydm_CheckAdaptivity(PVOID pDM_VOID);
extern void Phydm_NHMCounterStatisticsInit(PVOID pDM_VOID);
extern void Phydm_GetNHMCounterStatistics(PVOID pDM_VOID);
extern void Phydm_NHMCounterStatisticsReset(PVOID pDM_VOID);
extern void Phydm_NHMCounterStatistics(PVOID pDM_VOID);
extern void Phydm_SetEDCCAThreshold(PVOID pDM_VOID, s1Byte H2L, s1Byte L2H);
extern void Phydm_SetTRxMux(PVOID pDM_VOID, PhyDM_Trx_MUX_Type txMode, PhyDM_Trx_MUX_Type rxMode);
extern void Phydm_MACEDCCAState(PVOID pDM_VOID, PhyDM_MACEDCCA_Type State);
extern BOOL Phydm_CalNHMcnt(PVOID pDM_VOID);
extern void Phydm_CheckEnvironment(PVOID pDM_VOID);
extern void Phydm_SearchPwdBLowerBound(PVOID pDM_VOID);
extern void Phydm_AdaptivityInit(PVOID pDM_VOID);
extern void Phydm_Adaptivity(PVOID pDM_VOID, int IGI);
// Data declarations
//--------------------------------
// PhyDM_AntDiv.o
// Function declarations
extern void ODM_SwAntDivRestAfterLink(PDM_ODM_T pDM_Odm);
// Data declarations
//--------------------------------
// phydm_CfoTracking.o
// Function declarations
extern void ODM_CfoTrackingInit(PVOID pDM_VOID);
extern void ODM_CfoTracking(PVOID pDM_VOID);
extern void ODM_ParsingCFO(PVOID pDM_VOID, PVOID pPktinfo_VOID, s1Byte *pcfotail);
// Data declarations
//--------------------------------
// phydm_debug.o
// Function declarations
extern void ODM_InitDebugSetting(PDM_ODM_T pDM_Odm);
// Data declarations
//--------------------------------
// phydm_DIG.o
// Function declarations
extern void ODM_ChangeDynamicInitGainThresh(PVOID pDM_VOID, u4Byte DM_Type, u4Byte DM_Value);
extern int getIGIForDiff(int value_IGI);
extern void ODM_Write_DIG(PVOID pDM_VOID, u1Byte CurrentIGI);
extern void odm_PauseDIG(PVOID pDM_VOID, ODM_Pause_DIG_TYPE PauseType, u1Byte IGIValue);
extern u1Byte odm_ForbiddenIGICheck(PVOID pDM_VOID, u1Byte DIG_Dynamic_MIN, u1Byte CurrentIGI);
extern void ODM_Write_CCK_CCA_Thres(PVOID pDM_VOID, u1Byte CurCCK_CCAThres);
extern void odm_PauseCCKPacketDetection(PVOID pDM_VOID, ODM_Pause_CCKPD_TYPE PauseType, u1Byte CCKPDThreshold);
extern void odm_DIGInit(PVOID pDM_VOID);
extern BOOLEAN odm_DigAbort(PVOID pDM_VOID);
extern void odm_DIGbyRSSI_LPS(PVOID pDM_VOID);
extern void odm_FAThresholdCheck(PVOID pDM_VOID, u4Byte *dm_FA_thres);
extern void odm_DIG(PVOID pDM_VOID);
extern void odm_FalseAlarmCounterStatistics(PVOID pDM_VOID);
extern void odm_CCKPacketDetectionThresh(PVOID pDM_VOID);
// Data declarations
extern BOOLEAN bPaused_20545;
//--------------------------------
// phydm_HWConfig.o
// Function declarations
extern u1Byte odm_QueryRxPwrPercentage(s1Byte AntPower);
extern s4Byte odm_SignalScaleMapping_92CSeries_patch_RT_CID_819x_Lenovo(PDM_ODM_T pDM_Odm, s4Byte CurrSig);
extern s4Byte odm_SignalScaleMapping_92CSeries_patch_RT_CID_819x_Netcore(PDM_ODM_T pDM_Odm, s4Byte CurrSig);
extern s4Byte odm_SignalScaleMapping_92CSeries(PDM_ODM_T pDM_Odm, s4Byte CurrSig);
extern s4Byte odm_SignalScaleMapping(PDM_ODM_T pDM_Odm, s4Byte CurrSig);
extern void odm_RxPhyStatus8195A_Parsing(PDM_ODM_T pDM_Odm, PODM_PHY_INFO_T pPhyInfo, pu1Byte pPhyStatus, PODM_PACKET_INFO_T pPktinfo);
extern void odm_Process_RSSIForDM_8195A(PDM_ODM_T pDM_Odm, PODM_PHY_INFO_T pPhyInfo, PODM_PACKET_INFO_T pPktinfo, pu1Byte pPhyStatus);
extern void ODM_PhyStatusQuery_8195A(PDM_ODM_T pDM_Odm, PODM_PHY_INFO_T pPhyInfo, pu1Byte pPhyStatus, PODM_PACKET_INFO_T pPktinfo);
extern HAL_STATUS ODM_ConfigRFWithHeaderFile(PDM_ODM_T pDM_Odm, ODM_RF_Config_Type ConfigType, ODM_RF_RADIO_PATH_E eRFPath);
extern HAL_STATUS ODM_ConfigRFWithTxPwrTrackHeaderFile(PDM_ODM_T pDM_Odm);
extern HAL_STATUS ODM_ConfigBBWithHeaderFile(PDM_ODM_T pDM_Odm, ODM_BB_Config_Type ConfigType);
extern HAL_STATUS ODM_ConfigMACWithHeaderFile(PDM_ODM_T pDM_Odm);
extern HAL_STATUS ODM_ConfigFWWithHeaderFile(PDM_ODM_T pDM_Odm, ODM_FW_Config_Type ConfigType, u1Byte *pFirmware, u4Byte *pSize);
extern u4Byte ODM_GetHWImgVersion(PDM_ODM_T pDM_Odm);
// Data declarations
//--------------------------------
// phydm_interface.o
// Function declarations
extern u1Byte ODM_Read1Byte(PDM_ODM_T pDM_Odm, u4Byte RegAddr);
extern u2Byte ODM_Read2Byte(PDM_ODM_T pDM_Odm, u4Byte RegAddr);
extern u4Byte ODM_Read4Byte(PDM_ODM_T pDM_Odm, u4Byte RegAddr);
extern void ODM_Write1Byte(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u1Byte Data);
extern void ODM_Write2Byte(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u2Byte Data);
extern void ODM_Write4Byte(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u4Byte Data);
extern void ODM_SetMACReg(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u4Byte BitMask, u4Byte Data);
extern u4Byte ODM_GetMACReg(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u4Byte BitMask);
extern void ODM_SetBBReg(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u4Byte BitMask, u4Byte Data);
extern u4Byte ODM_GetBBReg(PDM_ODM_T pDM_Odm, u4Byte RegAddr, u4Byte BitMask);
// void __usercall ODM_SetRFReg(PDM_ODM_T pDM_Odm@<R0>, ODM_RF_RADIO_PATH_E eRFPath@<R1>, u4Byte RegAddr@<R2>, u4Byte BitMask@<R3>, u4Byte Data);
extern u4Byte ODM_GetRFReg(PDM_ODM_T pDM_Odm, ODM_RF_RADIO_PATH_E eRFPath, u4Byte RegAddr, u4Byte BitMask);
extern void ODM_AllocateMemory(PDM_ODM_T pDM_Odm, PVOID *pPtr, u4Byte length);
extern void ODM_FreeMemory(PDM_ODM_T pDM_Odm, PVOID pPtr, u4Byte length);
extern void ODM_MoveMemory(PDM_ODM_T pDM_Odm, PVOID pDest, PVOID pSrc, u4Byte Length);
extern u8Byte ODM_GetCurrentTime(PDM_ODM_T pDM_Odm);
extern u8Byte ODM_GetProgressingTime(PDM_ODM_T pDM_Odm, u8Byte Start_Time);
// Data declarations
//--------------------------------
// phydm_PowerTracking.o
// Function declarations
extern signed int getSwingIndex(PVOID pDM_VOID);
extern void odm_TXPowerTrackingThermalMeterInit(PVOID pDM_VOID);
extern void odm_TXPowerTrackingCheckIOT(PVOID pDM_VOID);
extern void ODM_TXPowerTrackingCheck(PVOID pDM_VOID);
// Data declarations
extern const u4Byte OFDMSwingTable_New[43]; /* =
{
  188743725u,  201326640u,  213909555u,  226492470u,  239075385u,  251658300u,  268435520u,
  285212740u,  301989960u,  318767180u,  339738705u,  360710230u,  381681755u,  402653280u,
  427819110u,  452984940u,  478150770u,  507510905u,  536871040u,  570425480u,  603979920u,
  637534360u,  679477410u,  717226155u,  759169205u,  805306560u,  851443915u,  901775575u,
  956301540u,  1015021810u,  1073742080u,  1136656655u,  1203765535u,  1275068720u,  1350566210u,
  1430258005u,  1514144105u,  1606418815u,  1698693525u,  1803551150u,  1908408775u,  2021655010u,
  2139095550u
}; */
//--------------------------------
// phydm_RaInfo.o
// Function declarations
extern void odm_RSSIMonitorInit(PVOID pDM_VOID);
extern void ODM_RAPostActionOnAssoc(PVOID pDM_VOID);
extern void odm_RSSIMonitorCheckIOT(PVOID pDM_VOID);
extern void odm_RSSIMonitorCheck(PVOID pDM_VOID);
extern void odm_RateAdaptiveMaskInit(PVOID pDM_VOID);
extern BOOLEAN ODM_RAStateCheck(PVOID pDM_VOID, s4Byte RSSI, BOOLEAN bForceUpdate, pu1Byte pRATRState);
extern void odm_RefreshRateAdaptiveMaskIOT(PVOID pDM_VOID);
extern void odm_RefreshRateAdaptiveMask(PVOID pDM_VOID);
extern u4Byte ODM_Get_Rate_Bitmap(PVOID pDM_VOID, u4Byte macid, u4Byte ra_mask, u1Byte rssi_level);
//-------------------------------------------------------------------------
// hal_com_phycfg.o
// Function declarations
extern int PHY_GetTxPowerByRateBase(PADAPTER Adapter, int Band, int RfPath, int TxNum, RATE_SECTION RateSection);
extern void phy_SetTxPowerByRateBase(PADAPTER Adapter, int Band, int RfPath, RATE_SECTION RateSection, uint8_t TxNum, uint8_t Value);
extern void PHY_GetRateValuesOfTxPowerByRate(PADAPTER pAdapter, uint32_t RegAddr, uint32_t BitMask, uint32_t Value, uint8_t *RateIndex, int8_t *PwrByRateVal, uint8_t *RateNum);
extern void PHY_StoreTxPowerByRateNew(PADAPTER pAdapter, uint32_t Band, uint32_t RfPath, uint32_t TxNum, uint32_t RegAddr, uint32_t BitMask, uint32_t Data);
extern void PHY_InitTxPowerByRate(PADAPTER pAdapter);
extern void PHY_StoreTxPowerByRate(PADAPTER pAdapter, uint32_t Band, uint32_t RfPath, uint32_t TxNum, uint32_t RegAddr, uint32_t BitMask, uint32_t Data);
extern signed int phy_GetChnlIndex(int Channel, uint8_t *ChannelIdx);
extern signed int PHY_GetTxPowerIndexBase(PADAPTER pAdapter, int RFPath, int Rate, CHANNEL_WIDTH BandWidth, uint8_t Channel, PBOOLEAN bIn24G);
extern PADAPTER PHY_GetTxPowerTrackingOffset(PADAPTER result, int RFPath, int Rate);
extern int PHY_GetRateIndexOfTxPowerByRate(uint8_t Rate);
extern unsigned int PHY_GetTxPowerByRate(PADAPTER pAdapter, int Band, int RFPath, int TxNum, uint8_t Rate);
extern void phy_StoreTxPowerByRateBase(PADAPTER pAdapter);
extern void PHY_SetTxPowerByRate(PADAPTER pAdapter, int Band, int RFPath, int TxNum, uint8_t Rate, int8_t Value);
extern void phy_ConvertTxPowerByRateInDbmToRelativeValues(PADAPTER pAdapter);
extern void PHY_TxPowerByRateConfiguration(PADAPTER pAdapter);
extern void PHY_SetTxPowerIndexByRateArray(PADAPTER pAdapter, int RFPath, CHANNEL_WIDTH BandWidth, uint8_t Channel, uint8_t *Rates, uint8_t RateArraySize);
extern void PHY_SetTxPowerIndexByRateSection(PADAPTER pAdapter, int RFPath, uint8_t Channel, int RateSection);
extern void PHY_SetTxPowerLevelByPath(PADAPTER Adapter, uint8_t channel, int path);
extern signed int phy_GetWorldWideLimit(int8_t *LimitTable, int regulation, int16_t channel);
extern int phy_GetChannelIndexOfTxPowerLimit(int Band, uint8_t Channel);
extern int PHY_GetTxPowerLimit(PADAPTER Adapter, uint32_t RegPwrTblSel, BAND_TYPE Band, CHANNEL_WIDTH Bandwidth, uint8_t RfPath, uint8_t DataRate, uint8_t Channel);
extern void PHY_ConvertTxPowerLimitToPowerIndex(PADAPTER Adapter);
extern void PHY_InitTxPowerLimit(PADAPTER Adapter);
extern void PHY_SetTxPowerLimit(PADAPTER Adapter, int Regulation, int Band, int Bandwidth, uint8_t RateSection, uint8_t RfPath, uint8_t Channel, uint8_t PowerLimit);
extern int PHY_GetTxPowerIndex(PADAPTER pAdapter, int RFPath, int Rate, CHANNEL_WIDTH BandWidth, uint8_t Channel);
// Data declarations
//-------------------------------------------------------------------------
// Data declarations

extern int dword_A50; // = 605557260; // weak
extern int dword_A58; // = 2206368128; // weak
extern u8 CSWTCH_14[132];/* =
{
  1,  0,  0,  0,  0,  0,  0,  2,  4,  0,  0,  0,  0,  0,  5,  0,  0,  0,
  3,  0,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  9,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  11,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  12,  13,
  14,  15,  16,  17,  18,  19 }; */
//--------------------------------
// rtl8195a_hal_init.o
// Function declarations
extern void Hal_GetEfuseDefinition(PADAPTER padapter, uint8_t efuseType, int type, void *pOut, uint8_t bPseudoTest);
extern void ResumeTxBeacon(PADAPTER padapter);
extern void UpdateHalRAMask8195A(PADAPTER padapter, uint32_t mac_id, uint8_t rssi_level);
extern void HalLittleWifiMCUThreadRtl8195a(thread_context context);
extern void HalCheckInReqStateThreadRtl8195a(thread_context context);
extern void HalTDMAChangeStateThreadRtl8195a(thread_context context);
extern void rtl8195a_read_chip_version(PADAPTER padapter);
extern signed int Hal_EfuseWordEnableDataWrite(PADAPTER padapter, int efuse_addr, uint8_t word_en, uint8_t *data, uint8_t bPseudoTest);
extern void Hal_EfusePowerSwitch(PADAPTER padapter, int bWrite, int PwrState);
extern void rtl8195a_free_hal_data(PADAPTER padapter);
extern void StopTxBeacon(PADAPTER padapter);
extern void SetHalODMVar8195A(PADAPTER Adapter, HAL_ODM_VARIABLE eVariable, PVOID pValue1, int bSet);
extern void rtl8195a_start_thread(_adapter *padapter);
extern void rtl8195a_stop_thread(_adapter *padapter);
extern void Hal_ReadEFuse(PADAPTER padapter, int efuseType, int _offset, int _size_byte, uint8_t *pbuf, uint8_t bPseudoTest);
extern void GetHalODMVar8195A(PADAPTER Adapter, HAL_ODM_VARIABLE eVariable, PVOID pValue1, BOOLEAN bSet);
extern signed int rtw_flash_map_update(PADAPTER padapter, uint8_t *configTbl);
extern void rtw_flash_map_erase(PADAPTER padapter, int a2, int a3, uint32_t a4);
extern int32_t Hal_EfusePgPacketWrite(PADAPTER padapter, uint8_t offset, int word_en, uint8_t *pData, uint8_t bPseudoTest);
extern int Hal_EfuseGetCurrentSize(PADAPTER pAdapter, uint8_t efuseType, int bPseudoTest);
extern signed int rtw_flash_map_write(PADAPTER padapter, uint16_t addr, uint16_t cnts, uint8_t *data);
extern int32_t rtl8195a_FirmwareDownload(PADAPTER padapter, BOOLEAN bUsedWoWLANFw);
extern void rtl8195a_InitBeaconParameters(PADAPTER padapter);
extern void InitBurstPktLen_8195AB(PADAPTER Adapter);
extern void rtl8195a_set_hal_ops(struct hal_ops *pHalFunc);
extern int32_t rtl8195a_InitLLTTable(PADAPTER padapter);
extern signed int Hal_GetChnlGroup8195A(int Channel, uint8_t *pGroup);
extern signed int rtw_flash_read(PADAPTER padapter, int addr, int cnts, uint8_t *data);
extern signed int rtw_flash_write(PADAPTER padapter, int addr, int cnts, uint8_t *data);
extern int rtw_config_map_read(PADAPTER padapter, int addr, int cnts, uint8_t *data, uint8_t efuse);
extern int rtw_config_map_write(PADAPTER padapter, int addr, int cnts, uint8_t *data, uint8_t efuse);
extern void Hal_InitPGData(PADAPTER padapter, uint8_t *PROMContent, int a3, int a4);
extern void Hal_EfuseParseIDCode(PADAPTER padapter, uint8_t *hwinfo);
extern void Hal_ReadPowerValueFromPROM_8195A(PADAPTER Adapter, PTxPowerInfo24G pwrInfo24G, uint8_t *PROMContent, int AutoLoadFail);
extern void Hal_EfuseParseTxPowerInfo_8195A(PADAPTER padapter, uint8_t *PROMContent, int AutoLoadFail);
extern void Hal_EfuseParseEEPROMVer_8195A(PADAPTER padapter, uint8_t *hwinfo, int AutoLoadFail);
extern void Hal_EfuseParsePackageType_8195A(PADAPTER pAdapter, uint8_t *hwinfo, int a3);
extern void Hal_EfuseParseChnlPlan_8195A(PADAPTER padapter, uint8_t *hwinfo, BOOLEAN AutoLoadFail);
extern void Hal_EfuseParseCustomerID_8195A(PADAPTER padapter, uint8_t *hwinfo, int AutoLoadFail);
extern void Hal_EfuseParseXtal_8195A(PADAPTER pAdapter, uint8_t *hwinfo, int AutoLoadFail);
extern void Hal_EfuseParseThermalMeter_8195A(PADAPTER padapter, uint8_t *PROMContent, int AutoLoadFail);
extern void Hal_ReadRFGainOffset(PADAPTER Adapter, uint8_t *PROMContent, int AutoloadFail);
extern int BWMapping_8195A(PADAPTER Adapter, struct pkt_attrib *pattrib);
extern signed int SCMapping_8195A(PADAPTER Adapter, struct pkt_attrib *pattrib);
extern void rtl8195a_update_txdesc(struct xmit_frame *pxmitframe, uint8_t *pbuf);
extern void rtl8195a_fill_fake_txdesc(PADAPTER padapter, uint8_t *pDesc, uint32_t BufferLen, int IsPsPoll, uint8_t IsBTQosNull, uint8_t bDataFrame);
extern void SetHwReg8195A(PADAPTER padapter, int variable, uint8_t *val);
extern void GetHwReg8195A(PADAPTER padapter, int variable, uint8_t *val);
extern signed int SetHalDefVar8195A(PADAPTER padapter, HAL_DEF_VARIABLE variable, void *pval);
extern signed int GetHalDefVar8195A(PADAPTER padapter, HAL_DEF_VARIABLE variable, void *pval);
// Data declarations
extern const struct map_mask_s efuse_map_mask[4]; // = { { 32u, 207u }, { 220u, 221u }, { 282u, 311u }, { 313u, 319u } };
extern const struct map_mask_s flash_map_mask[2]; // = { { 32u, 311u }, { 313u, 319u } };
//--------------------------------
// rtw_ap.o
// Function declarations
extern int rtw_ht_operation_update(_adapter *padapter);
extern void associated_clients_update_0(_adapter *padapter, int updated, int a3);
extern signed int chk_sta_is_alive(struct sta_info *psta);
extern void add_RATid(_adapter *padapter, struct sta_info *psta, int rssi_level);
extern void update_sta_info_apmode(_adapter *padapter, struct sta_info *psta, int a3);
extern void update_beacon(_adapter *padapter, int ie_id, uint8_t *oui, uint8_t tx);
extern int rtw_check_beacon_data(_adapter *padapter, uint8_t *pbuf, int len);
extern void associated_clients_update(_adapter *padapter, int updated, int a3);
extern void bss_cap_update_on_sta_join(_adapter *padapter, struct sta_info *psta);
extern signed int bss_cap_update_on_sta_leave(_adapter *padapter, struct sta_info *psta);
extern signed int ap_free_sta(_adapter *padapter, struct sta_info *psta, int reason);
extern void expire_timeout_chk(_adapter *padapter);
extern int rtw_sta_flush(_adapter *padapter);
extern void free_mlme_ap_info(_adapter *padapter, _irqL a2);
extern void sta_info_update(_adapter *padapter, struct sta_info *psta);
extern void ap_sta_info_defer_update(_adapter *padapter, struct sta_info *psta);
extern void start_ap_mode(_adapter *padapter);
extern void init_mlme_ap_info(_adapter *padapter);
extern void stop_ap_mode(_adapter *padapter, _irqL a2);
extern uint32_t rtw_generate_bcn_ie(_adapter *adapter, uint8_t *ssid, int ssid_len, uint8_t *ie);
extern int set_hidden_ssid(const char *ifname, uint8_t value);
// Data declarations
//--------------------------------

#ifdef	__cplusplus
}
#endif
#endif // _WIFI_LIB_H


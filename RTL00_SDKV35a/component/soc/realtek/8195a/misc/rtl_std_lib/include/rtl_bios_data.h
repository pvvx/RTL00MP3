/*
 * rtl_bios_data.h
 *
 *  Created on: 12/02/2017
 *      Author: pvvx
 *
 *  This variables declared in ROM code!
 *  Variables use fixed addresses!
 *  (see *.ld script)
 */

#ifndef _RTL_BIOS_DATA_H_
#define _RTL_BIOS_DATA_H_

#include "platform_autoconf.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/reent.h>
// component/soc/realtek/common/bsp/
#include "basic_types.h"
// component/soc/realtek/8195a/fwlib/
#include "rtl8195a/rtl8195a.h"
#include "hal_gpio.h"
#include "hal_irqn.h"
#include "hal_timer.h"
#include "hal_sdr_controller.h"
// component/soc/realtek/8195a/fwlib/
#include "ram_lib/wlan/realtek/wlan_ram_map/rom/rom_wlan_ram_map.h"
// component/soc/realtek/8195a/misc/driver/
#include "rtl_consol.h"
// component/soc/realtek/8195a/misc/rtl_std_lib/
#include "include/rtl_lib.h"
#include "include/rt_lib_rom.h"
#include "libc/rom/string/rom_libc_string.h"
#include "libgloss/rtl8195a/rom/rom_libgloss_retarget.h"

//#include "rom/rom_libgloss_retarget.h"

typedef void (*START_FUNC)(void);

/* ROM + startup.c */
extern IRQ_FUN NewVectorTable[64];		// 10000000
extern IRQ_FUN UserIrqFunTable[64];		// 10000100
extern u32 UserIrqDataTable[64];		// 10000200

/* ROM + diag.h */
extern u32 CfgSysDebugWarn;		// 10000300
extern u32 CfgSysDebugInfo;		// 10000304
extern u32 CfgSysDebugErr;		// 10000308
extern u32 ConfigDebugWarn;		// 1000030c
extern u32 ConfigDebugInfo;		// 10000310
extern u32 ConfigDebugErr;		// 10000314


/* ROM + hal_timer.h & .. */
extern HAL_TIMER_OP HalTimerOp; 		// 10000318
extern u16 GPIOState[11];				// 10000334
extern u32 gTimerRecord;				// 1000034C
/* ROM + hal_ssi.h */
extern u32 SSI_DBG_CONFIG;				// 10000350
extern PHAL_GPIO_ADAPTER _pHAL_Gpio_Adapter;	// 10000354

/* ROM + rtl8195a_timer.c */
extern IRQ_FUN Timer2To7VectorTable[MAX_TIMER_VECTOR_TABLE_NUM];	// 10000358

/* ROM + Rand() */
extern u32 _rand_z4, _rand_z3, _rand_z2, _rand_z1, _rand_first; // 10000370..

/* ROM + rtl_consol.c */
extern volatile UART_LOG_CTL *pUartLogCtl;	// 10000384
extern UART_LOG_BUF UartLogBuf;				// 10000388
extern volatile UART_LOG_CTL UartLogCtl;	// 10000408
extern u8 UartLogHistoryBuf[UART_LOG_HISTORY_LEN][UART_LOG_CMD_BUFLEN]; // 10000430 UartLogHistoryBuf[5][127] !
extern u8 *ArgvArray[MAX_ARGV]; 	// 100006AC *ArgvArray[10] !

/* ROM + ?? */
extern struct _rom_wlan_ram_map rom_wlan_ram_map; // 100006D4
typedef struct _FALSE_ALARM_STATISTICS {
	u32 Cnt_Parity_Fail;
	u32 Cnt_Rate_Illegal;
	u32 Cnt_Crc8_fail;
	u32 Cnt_Mcs_fail;
	u32 Cnt_Ofdm_fail;
	u32 Cnt_Ofdm_fail_pre;
	u32 Cnt_Cck_fail;
	u32 Cnt_all;
	u32 Cnt_Fast_Fsync;
	u32 Cnt_SB_Search_fail;
	u32 Cnt_OFDM_CCA;
	u32 Cnt_CCK_CCA;
	u32 Cnt_CCA_all;
	u32 Cnt_BW_USC;
	u32 Cnt_BW_LSC;
} FALSE_ALARM_STATISTICS;
extern FALSE_ALARM_STATISTICS FalseAlmCnt;	// 100006E0

typedef struct _rom_info {
	u8 EEPROMVersion;
	u8 CrystalCap;
	u64 DebugComponents;
	u32 DebugLevel;
} ROM_INFO;
extern ROM_INFO ROMInfo;					// 10000720

typedef struct _CFO_TRACKING_ {
	BOOL bATCStatus;
	BOOL largeCFOHit;
	BOOL bAdjust;
	u8 CrystalCap;
	u8 DefXCap;
	u32 CFO_tail[2];
	u32 CFO_ave_pre;
	u32 packetCount;
	u32 packetCount_pre;
	BOOL bForceXtalCap;
	BOOL bReset;
	u8 CFO_TH_XTAL_HIGH;
	u8 CFO_TH_XTAL_LOW;
	u8 CFO_TH_ATC;
}CFO_TRACKING;
extern CFO_TRACKING DM_CfoTrack;			// 10000738

/* in rom_libgloss_retarget.h
struct _rom_libgloss_ram_map {
	int   (*libgloss_close)(int fildes);
	int   (*libgloss_fstat)(int fildes , struct stat *st);
	int   (*libgloss_isatty)(int file);
	int   (*libgloss_lseek)(int file , int ptr , int dir);
	int   (*libgloss_open)(char *file , int flags , int mode);
	int	  (*libgloss_read)(int file , char *ptr , int len);
	int	  (*libgloss_write)(int file , const char *ptr , int len);
	void* (*libgloss_sbrk)(int incr);
};
*/
extern struct _rom_libgloss_ram_map rom_libgloss_ram_map;	// 10000760
struct malloc_chunk
{
  size_t prev_size;
  size_t size;
  struct malloc_chunk *fd;
  struct malloc_chunk *bk;
};
extern struct malloc_chunk * __rtl_malloc_av_[258]; 		// 10000780 __rom_mallocr_init_v1_00(), _rtl_free_r_v1_00()..
extern u32 __rtl_malloc_trim_threshold; 	// 10000b88  __rom_mallocr_init_v1_00()
extern u32 __rtl_malloc_top_pad; 			// 10000b8c  __rom_mallocr_init_v1_00()
extern u8 * __rtl_malloc_sbrk_base; 		// 10000b90 __rom_mallocr_init_v1_00()
extern u32 __rtl_malloc_max_sbrked_mem;		// 10000b94 __rom_mallocr_init_v1_00()
extern u32 __rtl_malloc_max_total_mem;		// 10000b98 __rom_mallocr_init_v1_00()
struct mallinfo
{
  int arena;
  int ordblks;
  int smblks;
  int hblks;
  int hblkhd;
  int usmblks;
  int fsmblks;
  int uordblks;
  int fordblks;
  int keepcost;
};
extern struct mallinfo __rtl_malloc_current_mallinfo; // 10000b9c __rom_mallocr_init_v1_00()

/* IMAGE1 HEAD: ROM + startup.c (bootloader) */
extern RAM_START_FUNCTION gRamStartFun;		// 10000bc8 = { PreProcessForVendor + 1 };
extern RAM_START_FUNCTION gRamPatchWAKE;	// 10000bcc = { RtlBootToSram + 1 };
extern RAM_START_FUNCTION gRamPatchFun0;	// 10000bd0 = { RtlBootToSram + 1 };
extern RAM_START_FUNCTION gRamPatchFun1;	// 10000bd4 = { RtlBootToSram + 1 };
extern RAM_START_FUNCTION gRamPatchFun2;	// 10000bd8 = { RtlBootToSram + 1 };
//extern uint8 RAM_IMG1_VALID_PATTEN[8];		// 10000bdc = { 0x23, 0x79, 0x16, 0x88, 0xff, 0xff, 0xff, 0xff };

/* ROM + hal_sdr_controller.c */
extern u32 rand_x; 							// 10000be4: ChangeRandSeed_rom(), Sdr_Rand2_rom()
#define REC_NUM 512
extern u32 AvaWds[2][REC_NUM];				// 10000be8
extern DRAM_DEVICE_INFO SdrDramInfo;		// 10001be8
#define DRAM_DEVICE_INFO_INIT() { \
  &SdrDramDev,		\
  &SdrDramModeReg,	\
  &SdrDramTiming,	\
  DRAM_TIMING_TCK,	\
  DFI_RATIO_1 }
extern DRAM_TIMING_INFO SdrDramTiming;		// 10001bfc
#define DRAM_TIMING_INFO_INIT() {		\
  DRAM_TIMING_TRFC,         /* TrfcPs; */	\
  DRAM_TIMING_TREFI,        /* TrefiPs; */	\
  DRAM_TIMING_TWRMAXTCK,    /* WrMaxTck; */\
  DRAM_TIMING_TRCD,         /* TrcdPs; */	\
  DRAM_TIMING_TRP,          /* TrpPs; */	\
  DRAM_TIMING_TRAS,         /* TrasPs; */	\
  DRAM_TIMING_TRRD,         /* TrrdTck; */	\
  DRAM_TIMING_TWR,          /* TwrPs; */	\
  DRAM_TIMING_TWTR,         /* TwtrTck; */	\
  /* 13090, */     /* TrtpPs; */			\
  DRAM_TIMING_TMRD,         /* TmrdTck; */	\
  DRAM_TIMING_TRTP,         /* TrtpTck; */	\
  DRAM_TIMING_TCCD,         /* TccdTck; */	\
  DRAM_TIMING_TRC }         /* TrcPs; */
extern DRAM_MODE_REG_INFO SdrDramModeReg;	// 10001c30
#define DRAM_MODE_REG_INFO_INIT() { \
  BST_LEN_4,	\
  SENQUENTIAL, 	\
  0x3,  /* Mode0Cas: 3 */ \
  0x0,  /* Mode0Wr */ \
  0,    /* Mode1DllEnN */ \
  0,    /* Mode1AllLat */ \
  0 }   /* Mode2Cwl */
extern DRAM_INFO SdrDramDev;				// 10001c4c
#define DRAM_INFO_INIT() {  DRAM_INFO_TYPE, DRAM_INFO_COL_ADDR_WTH,DRAM_INFO_BANK_SZ, DRAM_INFO_DQ_WTH }
extern SPIC_INIT_PARA SpicInitParaAllClk[SpicMaxMode][CPU_CLK_TYPE_NO]; // 100021ec [144=0x90]

/* ROM + "C" standard library */
extern struct _reent * _rtl_impure_ptr;		// 10001c60 = { &impure_reent };
extern struct _reent impure_reent;			// 10001c68 = _REENT_INIT(impure_reent);

/* ROM ? UserData? */
extern u32 _rom_unc_data[9];				// 100020e8

/* ROM + hal_sdr_controller.c: Sdr_Rand2() */
extern u32 _sdr_rnd2_c, _sdr_rnd2_z, _sdr_rnd2_y; // 100020BC, 100020B8, 100020B4

/* *.ld */
extern u8 __rom_bss_start__, __rom_bss_end__;
extern u8 __image1_bss_start__, __image1_bss_end__;
extern START_FUNC __image2_entry_func__;
//extern RAM_START_FUNCTION __image2_entry_func__;
extern u8 __image2_validate_code__;

#ifndef STACK_TOP
#define STACK_TOP 0x1ffffffc
#endif

#endif /* _RTL_BIOS_DATA_H_ */

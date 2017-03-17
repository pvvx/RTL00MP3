/*
 * rtl_bios_data.c
 *
 *  Created on: 12/02/2017
 *      Author: pvvx
 *
 * This variables declared in ROM code!
 * Variables use fixed addresses!
 *  (see *.ld script)
 */

#include "rtl_bios_data.h"
extern void * UartLogRomCmdTable;
/* ROM + startup.c */
RAM_DEDECATED_VECTOR_TABLE_SECTION IRQ_FUN NewVectorTable[64];	// 10000000
RAM_USER_IRQ_FUN_TABLE_SECTION IRQ_FUN UserIrqFunTable[64];		// 10000100
RAM_USER_IRQ_DATA_TABLE_SECTION u32 UserIrqDataTable[64];		// 10000200
/* ROM + ... */
HAL_RAM_BSS_SECTION PHAL_GPIO_ADAPTER _pHAL_Gpio_Adapter;	// 10000354
/* ROM + hal_ssi.h */
HAL_RAM_BSS_SECTION u32 SSI_DBG_CONFIG;		// 10000350
/* ROM + hal_timer.h & .. */
HAL_RAM_BSS_SECTION u32 gTimerRecord;			// 1000034C
HAL_RAM_BSS_SECTION u16 GPIOState[11];			// 10000334
HAL_RAM_BSS_SECTION HAL_TIMER_OP HalTimerOp;	// 10000318
/* ROM + diag.h */
HAL_RAM_BSS_SECTION u32 ConfigDebugErr;		// 10000314
HAL_RAM_BSS_SECTION u32 ConfigDebugInfo;	// 10000310
HAL_RAM_BSS_SECTION u32 ConfigDebugWarn;	// 1000030C
HAL_RAM_BSS_SECTION u32 CfgSysDebugErr;		// 10000308
HAL_RAM_BSS_SECTION u32 CfgSysDebugInfo;	// 10000304
HAL_RAM_BSS_SECTION u32 CfgSysDebugWarn;	// 10000300
/* ROM + rtl8195a_timer.c */
SECTION_RAM_TIMER2TO7_VECTOR_TABLE IRQ_FUN Timer2To7VectorTable[MAX_TIMER_VECTOR_TABLE_NUM]; // 10000358 Timer2To7VectorTable[6] !
/* ROM + Rand() */
INFRA_RAM_BSS_SECTION u32 _rand_z4, _rand_z3, _rand_z2, _rand_z1, _rand_first; // 10000370..
/* ROM + RTL_CONSOL */
MON_RAM_BSS_SECTION	u8 *ArgvArray[MAX_ARGV]; // 100006AC *ArgvArray[10] !
MON_RAM_BSS_SECTION u8 UartLogHistoryBuf[UART_LOG_HISTORY_LEN][UART_LOG_CMD_BUFLEN]; // 10000430 UartLogHistoryBuf[5][127] !
MON_RAM_BSS_SECTION volatile UART_LOG_CTL UartLogCtl; // 10000408
/* = {
		.NewIdx = 0,
		.SeeIdx = 0,
		.RevdNo  = UART_LOG_HISTORY_LEN,
		.EscSTS = 0,
		.ExecuteCmd = 0,
		.ExecuteEsc = 0,
		.BootRdy = 0,
		.Resvd = 0,
		.pTmpLogBuf = &UartLogBuf,
		.pfINPUT = (void*) &DiagPrintf,
		.pCmdTbl = (PCOMMAND_TABLE) &UartLogRomCmdTable,
		.CmdTblSz = 6,
		.CRSTS = 0,
		.pHistoryBuf = &UartLogHistoryBuf,
		.TaskRdy = 0
//		.Sema
}; */
MON_RAM_BSS_SECTION UART_LOG_BUF			UartLogBuf;		// 10000388
MON_RAM_BSS_SECTION volatile UART_LOG_CTL	*pUartLogCtl = &UartLogCtl;	// 10000384
/* ROM + LIB C */
LIBC_RAM_BSS_SECTION int __rtl_errno;		// 10000bc4 __rtl_sread_v1_00(), __rtl_write_v1_00(), __rtl_lseek_v1_00(), __rtl_close_v1_00(), __rtl_sbrk_v1_00()..
LIBC_RAM_BSS_SECTION struct mallinfo __rtl_malloc_current_mallinfo; // 10000b9c __rom_mallocr_init_v1_00()
LIBC_RAM_BSS_SECTION u32 __rtl_malloc_max_total_mem; // 10000b98 __rom_mallocr_init_v1_00()
LIBC_RAM_BSS_SECTION u32 __rtl_malloc_max_sbrked_mem; // 10000b94 __rom_mallocr_init_v1_00()
LIBC_RAM_BSS_SECTION u8 * __rtl_malloc_sbrk_base; // 10000b90 __rom_mallocr_init_v1_00()
LIBC_RAM_BSS_SECTION u32 __rtl_malloc_top_pad; 	// 10000b8c  __rom_mallocr_init_v1_00()
LIBC_RAM_BSS_SECTION u32 __rtl_malloc_trim_threshold; // 10000b88  __rom_mallocr_init_v1_00()
LIBC_RAM_BSS_SECTION struct malloc_chunk *__rtl_malloc_av_[258]; 		// 0x10000780 __rom_mallocr_init_v1_00(), _rtl_free_r_v1_00()..
LIBC_RAM_BSS_SECTION __attribute__((aligned(0x10))) struct _rom_libgloss_ram_map rom_libgloss_ram_map; // 10000760

// 10000BA0..10000BC0: 9 x dw __rom_mallocr_init_v1_00()

/* ROM: ROM_odm_FalseAlarmCounterStatistics() + .. */
#define _WLAN_RAM_MAP_SECTION __attribute__((__section__(".ram.rom.wlanmap")))
_WLAN_RAM_MAP_SECTION CFO_TRACKING DM_CfoTrack;	// 10000738
_WLAN_RAM_MAP_SECTION ROM_INFO ROMInfo;	// 10000720
_WLAN_RAM_MAP_SECTION __attribute__((aligned(0x10))) FALSE_ALARM_STATISTICS FalseAlmCnt; // 100006E0
WLAN_RAM_MAP_SECTION struct _rom_wlan_ram_map rom_wlan_ram_map; // 100006D4

#ifndef PRESENT_IMAGE1
/* IMAGE1 HEAD */
START_RAM_FUN_SECTION RAM_FUNCTION_START_TABLE __ram_start_table_start__;
/*
START_RAM_FUN_A_SECTION RAM_START_FUNCTION gRamStartFun;	// 10000bc8 = { PreProcessForVendor + 1 };
START_RAM_FUN_B_SECTION RAM_START_FUNCTION gRamPatchWAKE;	// 10000bcc = { RtlBootToSram + 1 };
START_RAM_FUN_C_SECTION RAM_START_FUNCTION gRamPatchFun0;	// 10000bd0 = { RtlBootToSram + 1 };
START_RAM_FUN_D_SECTION RAM_START_FUNCTION gRamPatchFun1;	// 10000bd4 = { RtlBootToSram + 1 };
START_RAM_FUN_E_SECTION RAM_START_FUNCTION gRamPatchFun2;	// 10000bd8 = { RtlBootToSram + 1 };
*/
#endif

IMAGE1_VALID_PATTEN_SECTION uint8 RAM_IMG1_VALID_PATTEN[8] = IMG1_VALID_PATTEN_INIT(); // 10000bdc

//#ifdef NOT_USE_LIBROM_A

/* ROM + hal_sdr_controller.c */
//HAL_FLASH_DATA_SECTION
//SPIC_INIT_PARA SpicInitParaAllClk[SpicMaxMode][CPU_CLK_TYPE_NO]; // SpicInitParaAllClk[3][6] !

/* ROM + hal_sdr_controller.c */
HAL_CUT_B_RAM_DATA_SECTION DRAM_INFO SdrDramDev = DRAM_INFO_INIT(); // 10001c4c
HAL_CUT_B_RAM_DATA_SECTION DRAM_MODE_REG_INFO SdrDramModeReg = DRAM_MODE_REG_INFO_INIT(); // 10001c30
HAL_CUT_B_RAM_DATA_SECTION DRAM_TIMING_INFO SdrDramTiming = DRAM_TIMING_INFO_INIT(); // 10001bfc
HAL_CUT_B_RAM_DATA_SECTION DRAM_DEVICE_INFO SdrDramInfo = DRAM_DEVICE_INFO_INIT(); // 10001be8
HAL_CUT_B_RAM_DATA_SECTION u32 AvaWds[2][REC_NUM]; // 10000be8

/* ROM + hal_sdr_controller.c: Sdr_Rand2() */
HAL_CUT_B_RAM_DATA_SECTION u32 rand_x = 123456789; // 10000be4

#define RTL_REENT_INIT(var) \
  { 0, \
    &(var).__sf[0], \
    &(var).__sf[1], \
    &(var).__sf[2], \
    0, \
    "", \
    0, \
	0x0437DC, \
    0, \
    _NULL, \
    _NULL, \
    0, \
    _NULL, \
    _NULL, \
    0, \
    _NULL, \
    { \
      { \
        0, \
        _NULL, \
        "", \
        {0, 0, 0, 0, 0, 0, 0, 0, 0}, \
        0, \
        1, \
        { \
          {_RAND48_SEED_0, _RAND48_SEED_1, _RAND48_SEED_2}, \
          {_RAND48_MULT_0, _RAND48_MULT_1, _RAND48_MULT_2}, \
          _RAND48_ADD \
        }, \
        {0, {0}}, \
        {0, {0}}, \
        {0, {0}}, \
        "", \
        "", \
        0, \
        {0, {0}}, \
        {0, {0}}, \
        {0, {0}}, \
        {0, {0}}, \
        {0, {0}} \
      } \
    }, \
    _REENT_INIT_ATEXIT \
    _NULL, \
    {_NULL, 0, _NULL} \
  }
// ROM: _rtl_impure_ptr + impure_reent in lib_rom.a ".hal.ram.data"
__attribute__((section(".libc.reent"))) struct _reent impure_reent = RTL_REENT_INIT(impure_reent); // 10001c60
__attribute__((section(".libc.reent"))) struct _reent * _rtl_impure_ptr = { &impure_reent }; // 10001c68

/* ROM ? */
__attribute__((__section__(".rom.unc.data"))) u32 _rom_unc_data[9]; // 100020e8

/* ROM + hal_sdr_controller.c: Sdr_Rand2() */
__attribute__((__section__(".sdr.rand2.data"))) u32 _sdr_rnd2_c = 7654321, _sdr_rnd2_z = 521288629, _sdr_rnd2_y = 362436;
// 100020BC, 100020B8, 100020B4

HAL_GPIO_ADAPTER PINMUX_RAM_DATA_SECTION gBoot_Gpio_Adapter; // 100020C0 [300=0x12c]
//SPIC_INIT_PARA HAL_FLASH_DATA_SECTION SpicInitParaAllClk[SpicMaxMode][CPU_CLK_TYPE_NO]; // 100021ec [144=0x90]

#ifndef PRESENT_IMAGE2
IMAGE2_START_RAM_FUN_SECTION RAM_START_FUNCTION gImage2EntryFun0; //=	{ InfraStart + 1 };
#endif
IMAGE2_VALID_PATTEN_SECTION _RAM_IMG2_VALID_PATTEN RAM_IMG2_VALID_PATTEN = RAM_IMG2_VALID_PATTEN_INIT();

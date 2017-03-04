/* 
 *  (SRAM) Debug BootLoader
 *  Created on: 12/02/2017
 *      Author: pvvx
 */

#include "platform_autoconf.h"
#include "rtl_bios_data.h"
#include "diag.h"
#include "rtl8195a/rtl8195a_sys_on.h"

//-------------------------------------------------------------------------
// Data declarations
//extern u32 STACK_TOP;
//extern volatile UART_LOG_CTL * pUartLogCtl;

#define DEFAULT_BAUDRATE UART_BAUD_RATE_38400

#define BOOT_RAM_TEXT_SECTION __attribute__((section(".ram.boot.text")))
//#define BOOT_RAM_RODATA_SECTION __attribute__((section(".ram.boot.rodata")))
//#define BOOT_RAM_DATA_SECTION __attribute__((section(".ram.boot.data")))
//#define BOOT_RAM_BSS_SECTION __attribute__((section(".ram.boot.bss")))

//-------------------------------------------------------------------------
// Function declarations
LOCAL void RtlBootToSram(void); // image1
LOCAL void EnterImage15(void); // image1
LOCAL void JtagOn(void); // image1

extern _LONG_CALL_ VOID HalCpuClkConfig(unsigned char CpuType);
extern _LONG_CALL_ VOID VectorTableInitRtl8195A(u32 StackP);
extern _LONG_CALL_ VOID HalInitPlatformLogUartV02(VOID);
extern _LONG_CALL_ VOID HalInitPlatformTimerV02(VOID);

//#pragma arm section code = ".ram.boot.text";
//#pragma arm section rodata = ".ram.boot.rodata", rwdata = ".ram.boot.data", zidata = ".ram.boot.bss";

typedef void (*START_FUNC)(void);

PRAM_FUNCTION_START_TABLE __attribute__((section(".data.pRamStartFun"))) pRamStartFun =
		(PRAM_FUNCTION_START_TABLE) 0x10000BC8;

/* Start table: */
START_RAM_FUN_SECTION RAM_FUNCTION_START_TABLE __ram_start_table_start__ = {
		RtlBootToSram + 1,	// StartFun(),	Run if ( v400001F4 & 0x8000000 ) && ( v40000210 & 0x80000000 )
		RtlBootToSram + 1,	// PatchWAKE(),	Run if ( v40000210 & 0x20000000 )
		RtlBootToSram + 1,	//- PatchFun0(), Run if ( v40000210 & 0x10000000 )
		RtlBootToSram + 1,	//+ PatchFun1(), Run if ( v400001F4 & 0x8000000 ) && ( v40000210 & 0x8000000 )
		EnterImage15 + 1};	// PatchFun2(), Run for Init console, if ( v40000210 & 0x4000000 )

/* Set Debug Flags */
LOCAL void BOOT_RAM_TEXT_SECTION SetDebugFlgs() {
#if CONFIG_DEBUG_LOG > 2
	CfgSysDebugWarn = -1;
	CfgSysDebugInfo = -1;
	CfgSysDebugErr = -1;
	ConfigDebugWarn = -1;
	ConfigDebugInfo = -1;
	ConfigDebugErr = -1;
#elif CONFIG_DEBUG_LOG > 1
	CfgSysDebugWarn = -1;
//	CfgSysDebugInfo = 0;
	CfgSysDebugErr = -1;
	ConfigDebugWarn = -1;
//	ConfigDebugInfo = 0;
	ConfigDebugErr = -1;
#elif CONFIG_DEBUG_LOG > 0
//	CfgSysDebugWarn = 0;
//	CfgSysDebugInfo = 0;
	CfgSysDebugErr = -1;
//	ConfigDebugWarn = 0;
//	ConfigDebugInfo = 0;
	ConfigDebugErr = -1;
#else
//	CfgSysDebugWarn = 0;
//	CfgSysDebugInfo = 0;
//	CfgSysDebugErr = 0;
//	ConfigDebugWarn = 0;
//	ConfigDebugInfo = 0;
//	ConfigDebugErr = 0;
#endif
}

/* RTL Console ROM */
LOCAL void BOOT_RAM_TEXT_SECTION RtlConsolRam(void) {
//	DiagPrintf("\r\nRTL Console ROM\r\n");
	pUartLogCtl->pTmpLogBuf->UARTLogBuf[0] = '?';
	pUartLogCtl->pTmpLogBuf->BufCount = 1;
	pUartLogCtl->ExecuteCmd = 1;
	RtlConsolTaskRom(pUartLogCtl);
}

/* JTAG On */
LOCAL void BOOT_RAM_TEXT_SECTION JtagOn(void) {
	ACTCK_VENDOR_CCTRL(ON);
	SLPCK_VENDOR_CCTRL(ON);
	HalPinCtrlRtl8195A(JTAG, 0, 1);
}

/* Enter Image 1.5 */
LOCAL void BOOT_RAM_TEXT_SECTION EnterImage15(void) {
	SetDebugFlgs();
	DBG_8195A("\rCPU CLK: %d Hz, SOC FUNC EN: %p\r\n", HalGetCpuClk(), HAL_PERI_ON_READ32(REG_SOC_FUNC_EN));
	DBG_8195A("==!== Enter Image 1.5 ====\nImg2 Sign: %s, InfaStart @ 0x%08x\r\n",
			&__image2_validate_code__, __image2_entry_func__);
	if (_strcmp((const char *) &__image2_validate_code__, IMG2_SIGN_TXT)) {
		DBG_MISC_ERR("Invalid Image2 Signature!\n");
		RtlConsolRam();
	}
	__image2_entry_func__();
}

/* RtlBootToSram */
LOCAL void BOOT_RAM_TEXT_SECTION RtlBootToSram(void) {
	JtagOn(); /* JTAG On */
	_memset(&__rom_bss_start__, 0, &__rom_bss_end__ - &__rom_bss_start__);
	__asm__ __volatile__ ("cpsid f\n");
	HAL_SYS_CTRL_WRITE32(REG_SYS_SYSPLL_CTRL1, HAL_SYS_CTRL_READ32(REG_SYS_SYSPLL_CTRL1) & ( ~BIT_SYS_SYSPLL_DIV5_3));
	HalCpuClkConfig(2); // 41.666666 MHz
//	HAL_SYS_CTRL_WRITE32(REG_SYS_SYSPLL_CTRL1, HAL_SYS_CTRL_READ32(REG_SYS_SYSPLL_CTRL1) | BIT_SYS_SYSPLL_DIV5_3); // 50.000 MHz
	VectorTableInitRtl8195A(STACK_TOP);	// 0x1FFFFFFC
	HalInitPlatformLogUartV02();
	HalInitPlatformTimerV02();
	__asm__ __volatile__ ("cpsie f\n");
	SpicInitRtl8195AV02(1, 0); // StartupSpicBaudRate InitBaudRate 1, SpicBitMode 1 StartupSpicBitMode
//	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & 0x1FFFFF); // Clear debug flags
	EnterImage15();
}

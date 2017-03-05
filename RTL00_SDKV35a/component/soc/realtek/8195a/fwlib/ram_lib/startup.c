/* 
 *  StartUp SDK
 *  Created on: 02/03/2017
 *      Author: pvvx
 */

#include "rtl8195a.h"
#include "diag.h"
#include "hal_spi_flash.h"
#include "hal_api.h"
#include "hal_platform.h"
#include "diag.h"
#include "hal_diag.h"
#include "rtl8195a_uart.h"
#include "rtl8195a/rtl8195a_peri_on.h"
#include "hal_peri_on.h"
#include "rtl_bios_data.h"
#include "wifi_conf.h"
#include "rtl_consol.h"

//#define INFRA_START_SECTION __attribute__((section(".infra.ram.start")))

//-------------------------------------------------------------------------
// Function declarations
void InfraStart(void);
extern void HalWdgIntrHandle(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler(void);
//extern void ShowRamBuildInfo(void); // app_start.c: VOID ShowRamBuildInfo(VOID)
void HalNMIHandler_Patch(void);
void SDIO_Device_Off(void);
void VectorTableOverrideRtl8195A(u32 StackP);
void SYSPlatformInit(void);

//-------------------------------------------------------------------------
// Data declarations
extern u8 __bss_start__, __bss_end__;
//extern HAL_TIMER_OP	HalTimerOp;

IMAGE2_START_RAM_FUN_SECTION RAM_START_FUNCTION gImage2EntryFun0 =
	{ InfraStart + 1 };

//----- HalNMIHandler_Patch
void HalNMIHandler_Patch(void) {
	DBG_8195A_HAL("%s:NMI Error!\n", __func__);
	if ( HAL_READ32(VENDOR_REG_BASE, 0) < 0)
		HalWdgIntrHandle(); // ROM: HalWdgIntrHandle = 0x3485;
}

/*
//----- VectorTableOverrideRtl8195A
void INFRA_START_SECTION VectorTableOverrideRtl8195A(u32 StackP) {
	NewVectorTable[2] = HalNMIHandler_Patch;
}
*/

//----- SYSPlatformInit
void INFRA_START_SECTION SYSPlatformInit(void) {
	HAL_SYS_CTRL_WRITE32(REG_SYS_EFUSE_SYSCFG0,
			(HAL_SYS_CTRL_READ32(REG_SYS_EFUSE_SYSCFG0)
			& (~(BIT_MASK_SYS_EEROM_LDO_PAR_07_04 << BIT_SHIFT_SYS_EEROM_LDO_PAR_07_04)))
			| BIT_SYS_EEROM_LDO_PAR_07_04(6)); // & 0xF0FFFFFF | 0x6000000
	HAL_SYS_CTRL_WRITE32(REG_SYS_XTAL_CTRL1,
			(HAL_SYS_CTRL_READ32(REG_SYS_XTAL_CTRL1)
			& (~(BIT_MASK_SYS_XTAL_DRV_RF1 << BIT_SHIFT_SYS_XTAL_DRV_RF1)))
			| BIT_SYS_XTAL_DRV_RF1(1)); //  & 0xFFFFFFE7 | 8;
}

//----- SDIO_Device_Off
void INFRA_START_SECTION SDIO_Device_Off(void) {
	HAL_PERI_ON_WRITE32(REG_PESOC_HCI_CLK_CTRL0,
			HAL_PERI_ON_READ32(REG_PESOC_HCI_CLK_CTRL0)
			& (~BIT_SOC_ACTCK_SDIO_DEV_EN));
	HAL_PERI_ON_WRITE32(REG_SOC_HCI_COM_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_HCI_COM_FUNC_EN)
			& (~(BIT_SOC_HCI_SDIOD_ON_EN | BIT_SOC_HCI_SDIOD_OFF_EN)));
	HAL_PERI_ON_WRITE32(REG_HCI_PINMUX_CTRL,
			HAL_PERI_ON_READ32(REG_HCI_PINMUX_CTRL)
			& (~(BIT_HCI_SDIOD_PIN_EN)));
}

__weak void __low_level_init(void) {
	// weak function
}

// weak main function !
__weak int main(void) {
	HalPinCtrlRtl8195A(JTAG, 0, 1);

	DiagPrintf("\r\nRTL Console ROM: Start - press key 'Up', Help '?'\r\n");
	while (pUartLogCtl->ExecuteEsc != 1);
	pUartLogCtl->RevdNo = 0;
	pUartLogCtl->BootRdy = 1;
	DiagPrintf("\r<RTL8710AF>");
	while (1) {
		while (pUartLogCtl->ExecuteCmd != 1);
		UartLogCmdExecute(pUartLogCtl);
		DiagPrintf("\r<RTL8710AF>");
		pUartLogCtl->ExecuteCmd = 0;
	}

	return 0;
}

//----- InfraStart
void INFRA_START_SECTION InfraStart(void) {
	NewVectorTable[2] = HalNMIHandler_Patch;
#ifdef CONFIG_TIMER_MODULE
	HalTimerOpInit_Patch((VOID*) (&HalTimerOp));
#endif
//	HAL_SYS_CTRL_WRITE32(REG_SYS_CLK_CTRL0,	HAL_SYS_CTRL_READ32(REG_SYS_CLK_CTRL0) | BIT4);
	DBG_8195A("==!== Enter Image 2 ====\n");
//	ShowRamBuildInfo(); // app_start.c: VOID ShowRamBuildInfo(VOID)
	memset(&__bss_start__, 0, &__bss_end__ - &__bss_start__);
	int flash_en = HAL_PERI_ON_READ32(REG_SOC_FUNC_EN)
			& (1 << BIT_SOC_FLASH_EN);
	if (flash_en) {
		if(!SpicCmpDataForCalibrationRtl8195A()) {
			DBG_8195A("ReInit  SPIC...\n");
			SpicInitRtl8195AV02(1,0);
//			if(!SpicCmpDataForCalibrationRtl8195A()) {
				// TODO: Spic Not Init!
				// DBG_8195A("Spic error Init!\n");
//			};
		};
		SpicNVMCalLoadAll();
		SpicReadIDRtl8195A();
	};
    while(!(HAL_READ8(LOG_UART_REG_BASE, 0x14) & BIT6)); //	иначе глючит LogUART, если переключение CLK приходится на вывод символов !
#if	CPU_CLOCK_SEL_DIV5_3
	// 6 - 200000000 Hz, 7 - 10000000 Hz, 8 - 50000000 Hz, 9 - 25000000 Hz, 10 - 12500000 Hz, 11 - 4000000 Hz
	HalCpuClkConfig(CPU_CLOCK_SEL_VALUE);
	*((int *)(SYSTEM_CTRL_BASE+REG_SYS_SYSPLL_CTRL1)) |= (1<<17);// REG_SYS_SYSPLL_CTRL1 |= BIT_SYS_SYSPLL_DIV5_3
#else
	// 0 - 166666666 Hz, 1 - 83333333 Hz, 2 - 41666666 Hz, 3 - 20833333 Hz, 4 - 10416666 Hz, 5 - 4000000 Hz
	*((int *) (SYSTEM_CTRL_BASE + REG_SYS_SYSPLL_CTRL1)) &= ~(1 << 17); // REG_SYS_SYSPLL_CTRL1 &= ~BIT_SYS_SYSPLL_DIV5_3
	HalCpuClkConfig(CPU_CLOCK_SEL_VALUE);
#endif
	HalReInitPlatformLogUartV02();
/*	HAL_LOG_UART_ADAPTER pUartAdapter;
	pUartAdapter.BaudRate = UART_BAUD_RATE_38400;
	HalLogUartSetBaudRate(&pUartAdapter); */
	SystemCoreClockUpdate();
	SYSPlatformInit();
	En32KCalibration();
	InitSoCPM();
	SDIO_Device_Off();
	VectorTableInitForOSRtl8195A(&vPortSVCHandler, &xPortPendSVHandler,
			&xPortSysTickHandler);
	if (flash_en)
		SpicDisableRtl8195A();
#ifdef CONFIG_SDR_EN
	// clear SDRAM bss
	extern u8 __sdram_bss_start__[];
	extern u8 __sdram_bss_end__[];
	if((int)__sdram_bss_end__-(int)__sdram_bss_start__ > 0)
	memset(__sdram_bss_start__, 0, (int)__sdram_bss_end__-(int)__sdram_bss_start__);
#endif
	// force SP align to 8 byte not 4 byte (initial SP is 4 byte align)
	__asm(
			"mov r0, sp\n"
			"bic r0, r0, #7\n"
			"mov sp, r0\n"
	);
	__low_level_init();
	main();
}


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
extern void ShowRamBuildInfo(void); // app_start.c: VOID ShowRamBuildInfo(VOID)
void HalNMIHandler_Patch(void);
void SDIO_Device_Off(void);
void VectorTableOverrideRtl8195A(u32 StackP);
void SYSPlatformInit(void);

//-------------------------------------------------------------------------
// Data declarations
extern u8 __bss_start__, __bss_end__;

IMAGE2_START_RAM_FUN_SECTION RAM_START_FUNCTION gImage2EntryFun0 = { InfraStart
		+ 1 };

// HAL_GPIO_ADAPTER PINMUX_RAM_DATA_SECTION gBoot_Gpio_Adapter;

//----- HalNMIHandler_Patch
void HalNMIHandler_Patch(void) {
	DBG_8195A_HAL("%s:NMI Error!\n", __func__);
	if ( HAL_READ32(VENDOR_REG_BASE, 0) < 0)
		HalWdgIntrHandle(); // ROM: HalWdgIntrHandle = 0x3485;
}

//----- VectorTableOverrideRtl8195A
void INFRA_START_SECTION VectorTableOverrideRtl8195A(u32 StackP) {
	NewVectorTable[2] = HalNMIHandler_Patch;
}

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

//----- InfraStart
void INFRA_START_SECTION InfraStart(void) {
	NewVectorTable[2] = HalNMIHandler_Patch;
	HAL_SYS_CTRL_WRITE32(REG_SYS_CLK_CTRL0,
			HAL_SYS_CTRL_READ32(REG_SYS_CLK_CTRL0) | BIT4);
	if (HalCommonInit() != HAL_OK)
		DBG_8195A("Hal Common Init Failed.\n");
	DBG_8195A("==!== Enter Image 2 ====\n");
	ShowRamBuildInfo(); // app_start.c: VOID ShowRamBuildInfo(VOID)
	memset(&__bss_start__, 0, &__bss_end__ - &__bss_start__);
	int clk = (HAL_SYS_CTRL_READ32(REG_SYS_CLK_CTRL0)
			>> BIT_SHIFT_PESOC_OCP_CPU_CK_SEL) & 1;
	if (clk) {
		SpicNVMCalLoadAll();
		SpicReadIDRtl8195A();
	}
	SystemCoreClockUpdate();
	SYSPlatformInit();
	En32KCalibration();
	InitSoCPM();
	SDIO_Device_Off();
	VectorTableInitForOSRtl8195A(&vPortSVCHandler, &xPortPendSVHandler,
			&xPortSysTickHandler);
	if (clk)
		SpicDisableRtl8195A();
	_AppStart();
}


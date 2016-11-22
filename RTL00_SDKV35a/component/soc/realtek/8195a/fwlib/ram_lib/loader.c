/*
 * loader.c
 *
 *  Created on: 17 нояб. 2016 г.
 *      Author: PVV
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
#include "wifi_conf.h"


//-------------------------------------------------------------------------
// Data declarations
extern u32 * NewVectorTable; // LD: NewVectorTable = 0x10000000;
extern START_FUNC __image2_entry_func__;
extern u8 __image2_validate_code__;
extern u8 __image1_bss_start__, __image1_bss_end__;
extern u8 __rom_bss_start__, __rom_bss_end__;
extern u8 __bss_start__, __bss_end__;

typedef struct __RAM_IMG2_VALID_PATTEN__ {
	char rtkwin[7];
	u8 x[13];
} _RAM_IMG2_VALID_PATTEN, *_PRAM_IMG2_VALID_PATTEN;

const uint8_t __attribute__((section(".image1.validate.rodata"))) RAM_IMG1_VALID_PATTEN[8] =
		{ 0x23, 0x79, 0x16, 0x88, 0xff, 0xff, 0xff, 0xff };

PRAM_FUNCTION_START_TABLE __attribute__((section(".data.pRamStartFun"))) pRamStartFun =
		(PRAM_FUNCTION_START_TABLE) 0x10000BC8;
RAM_START_FUNCTION __attribute__((section(".start.ram.data.a"))) gRamStartFun =
		{ PreProcessForVendor + 1 };
RAM_START_FUNCTION __attribute__((section(".start.ram.data.b"))) gRamPatchWAKE =
		{ RtlBootToSram + 1 };
RAM_START_FUNCTION __attribute__((section(".start.ram.data.c"))) gRamPatchFun0 =
		{ RtlBootToSram + 1 };
RAM_START_FUNCTION __attribute__((section(".start.ram.data.d"))) gRamPatchFun1 =
		{ RtlBootToSram + 1 };
RAM_START_FUNCTION __attribute__((section(".start.ram.data.e"))) gRamPatchFun2 =
		{ RtlBootToSram + 1 };

RAM_START_FUNCTION __attribute__((section(".image2.ram.data"))) gImage2EntryFun0 =
		{ InfraStart + 1 };
_RAM_IMG2_VALID_PATTEN __attribute__((section(".image2.validate.rodata"))) RAM_IMG2_VALID_PATTEN =
		{ { "RTKWin" }, { 0xff, 0, 1, 1, 0, 0x95, 0x81, 1, 1, 0, 0, 0, 0 } };

HAL_GPIO_ADAPTER __attribute__((section(".hal.ram.data"))) gBoot_Gpio_Adapter;


void sub_100037EC(void)
{
	HAL_SYS_CTRL_WRITE32(REG_SYS_REGU_CTRL0, (HAL_SYS_CTRL_READ32(REG_SYS_REGU_CTRL0) & 0xFFFFF) | 0xE00000);
	HAL_SYS_CTRL_WRITE32(REG_SYS_REGU_CTRL0, HAL_SYS_CTRL_READ32(REG_SYS_REGU_CTRL0) | 2);
}

void __attribute__((section(".hal.ram.text"))) PreProcessForVendor(void) {
	u8 efuse0xF8_data;
	// memset((void *)0x10000300, 0 , 0x10000BC8-0x10000300); ???
	HalPinCtrlRtl8195A(JTAG, 0, 1);
	HalDelayUs(1000);
	HALEFUSEOneByteReadROM(HAL_SYS_CTRL_READ32(REG_SYS_EFUSE_CTRL), 0xF8,
				&efuse0xF8_data, L25EOUTVOLTAGE);
	if(efuse0xF8_data == CHIP_ID_8711AF) { // ??
		HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | 0x200000) ; 1<<21

		sub_100037EC();
	}
	int flash_enable = HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & BIT_SOC_FLASH_EN;


}

void sub_1000441A(void)
{
	ConfigDebugErr = -1;
	ConfigDebugWarn = 0;
	ConfigDebugInfo = 0;
}

//----- InfraStart
void __attribute__((section(".infra.ram.start"))) InfraStart(void) {
	NewVectorTable[2] = HalNMIHandler_Patch;

	sub_100022F0();
	memset((void *)0x10000300, 0 , 0x10000BC8-0x10000300);

	HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL, HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_ACTCK_VENDOR_REG_EN);
	HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL, HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_SLPCK_VENDOR_REG_EN);
	HalPinCtrlRtl8195A(JTAG, 0, 1);
	HAL_PERI_ON_WRITE32(REG_GPIO_SHTDN_CTRL, 0x7FF);
	HAL_PERI_ON_WRITE32(REG_CPU_PERIPHERAL_CTRL, HAL_PERI_ON_READ32(REG_CPU_PERIPHERAL_CTRL) | BIT_SPI_FLSH_PIN_EN);
	HAL_PERI_ON_WRITE32(REG_CPU_PERIPHERAL_CTRL, HAL_SYHAL_PERI_ON_READ32S_CTRL_READ32(REG_CPU_PERIPHERAL_CTRL) | BIT_LOG_UART_PIN_EN);
	VectorTableInitRtl8195A(0x1FFFFFFC); // 0x1FFFFFFC StackP
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT_SOC_FLASH_EN) ;
	HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL, HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_ACTCK_FLASH_EN);
	HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL, HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_SLPCK_FLASH_EN);
	HalPinCtrlRtl8195A(SPI_FLASH, 0, 1);
	sub_1000367C();
	HAL_SYS_CTRL_WRITE32(REG_SYS_CLK_CTRL1, HAL_SYS_CTRL_READ32(REG_SYS_CLK_CTRL1) & 0x8F);
	sub_1000441A();
	HAL_UART_READ32(UART_REV_BUF_OFF);
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & (~BIT_SOC_LOG_UART_EN));
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT_SOC_LOG_UART_EN);
	HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL, HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_ACTCK_LOG_UART_EN);
	LOG_UART_ADAPTER UartAdapter;
	UartAdapter.BaudRate = UART_BAUD_RATE_38400;
    UartAdapter.FIFOControl = 0xC1;
    UartAdapter.IntEnReg = 0x00;
    UartAdapter.Parity = UART_PARITY_DISABLE;
    UartAdapter.Stop = UART_STOP_1BIT;
    UartAdapter.DataLength = UART_DATA_LEN_8BIT;
    HalLogUartInit(UartAdapter); //    sub_10004434(38400, 193, 0, v14);
    TIMER_ADAPTER tim_adapter;
    tim_adapter.IrqHandle.IrqFun = &UartLogIrqHandle;
	tim_adapter.IrqHandle.IrqNum = UART_LOG_IRQ;
	tim_adapter.IrqHandle.Data = 0;
	tim_adapter.IrqHandle.Priority = 5;

	VectorIrqRegisterRtl8195A(&tim_adapter.IrqHandle);
    UartAdapter.IntEnReg = 0x05;
    HalLogUartInit(UartAdapter);
    HAL_PERI_ON_WRITE32(REG_PON_ISO_CTRL, 3); // ? USB ?
    HAL_PERI_ON_WRITE32(REG_OSC32K_CTRL, HAL_PERI_ON_READ32(REG_OSC32K_CTRL) | BIT_32K_POW_CKGEN_EN);
    HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT_SOC_GTIMER_EN);
    HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL, HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_ACTCK_TIMER_EN);
    HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL, HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_SLPCK_TIMER_EN);

	tim_adapter.TimerIrqPriority = 0;
	tim_adapter.TimerLoadValueUs = 0;
	tim_adapter.TimerMode = FREE_RUN_MODE;
	tim_adapter.IrqDis = 1;
	tim_adapter.TimerId = 1;
	HalTimerInitRtl8195a((PTIMER_ADAPTER) &tim_adapter);
	SpicInitRtl8195A(1, 1); // InitBaudRate 1, SpicBitMode 1
	SpicFlashInitRtl8195A(1); // SpicBitMode 1
	DBG_8195A("===== Enter Image 1.5 ====\nImg2 Sign: %s, InfaStart @ 0x%08x\n",
			&__image2_validate_code__, __image2_entry_func__);


	HAL_SYS_CTRL_WRITE32(REG_SYS_CLK_CTRL0,
			HAL_SYS_CTRL_READ32(REG_SYS_CLK_CTRL0) | BIT4);
	if (HalCommonInit() != HAL_OK) DBG_8195A("Hal Common Init Failed.\n");
	DBG_8195A("===== Enter Image 2 ====\n");
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
	if (clk) SpicDisableRtl8195A();
	_AppStart();
}


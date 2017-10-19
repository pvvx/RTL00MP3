/* 
 *  StartUp USDK v0.2 (19/10/2017)
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

//-------------------------------------------------------------------------
// Function declarations
void InfraStart(void);
//extern void HalWdgIntrHandle(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler(void);
extern void rtl_libc_init(void);
extern _LONG_CALL_ void HalCpuClkConfig(unsigned char CpuType);
extern void PSHalInitPlatformLogUart(void);
extern _LONG_CALL_ void UartLogCmdExecute(PUART_LOG_CTL pUartLogCtlExe);
extern void HalReInitPlatformTimer(void);
extern void SystemCoreClockUpdate (void);
extern void En32KCalibration(void);
extern void SdrCtrlInit(void);
extern void InitSoCPM(void);
extern u32 SdrControllerInit(void);
//extern void ShowRamBuildInfo(void); // app_start.c: VOID ShowRamBuildInfo(VOID)
//void HalNMIHandler_Patch(void);
void SDIO_Device_Off(void);
//void VectorTableOverrideRtl8195A(u32 StackP);
void SYSPlatformInit(void);

#define FIX_SDR_CALIBRATION // for speed :)
//-------------------------------------------------------------------------
// Data declarations
extern u8 __bss_start__, __bss_end__;
extern const unsigned char cus_sig[32]; // images name
//extern HAL_TIMER_OP	HalTimerOp;

IMAGE2_START_RAM_FUN_SECTION RAM_START_FUNCTION gImage2EntryFun0 =
	{ InfraStart + 1 };
#ifdef CONFIG_SDR_EN
#ifdef FIX_SDR_CALIBRATION // for speed :)
#include "rtl8195a/rtl8195a_sdr.h"
LOCAL void sdr_init(void) {
	HAL_SYS_CTRL_WRITE32(REG_SYS_REGU_CTRL0,
		((HAL_SYS_CTRL_READ32(REG_SYS_REGU_CTRL0) & 0xfffff) | BIT_SYS_REGU_LDO25M_ADJ(0x03))); // ROM: BIT_SYS_REGU_LDO25M_ADJ(0x0e)? HAL RAM BIT_SYS_REGU_LDO25M_ADJ(0x03)
	LDO25M_CTRL(ON);
	SRAM_MUX_CFG(0x2);
	SDR_CLK_SEL(SDR_CLOCK_SEL_VALUE); //  REG_PESOC_CLK_SEL
	HAL_PERI_ON_WRITE32(REG_GPIO_PULL_CTRL4, 0);
	ACTCK_SDR_CCTRL(ON);
	SLPCK_SDR_CCTRL(ON);
	HalPinCtrlRtl8195A(SDR, 0, ON); //	SDR_PIN_FCTRL(ON);
	MEM_CTRL_FCTRL(ON);
//	HalDelayUs(1000);
	// read calibration data from system data FLASH_SDRC_PARA_BASE
	union { u8 b[8]; u16 s[4]; u32 l[2]; u64 d;} value;
	u32 faddr = SPI_FLASH_BASE + FLASH_SDRC_PARA_BASE + CPU_CLOCK_SEL_VALUE*8 + CPU_CLOCK_SEL_DIV5_3*8*8; // step 8 in FLASH_SDRC_PARA_BASE[64 + 64 bytes]
	value.d	= *((volatile u64 *)faddr);
	if(value.s[0] == 0xFE01 && (value.b[4]^value.b[5]) == 0xFF && (value.b[6]^value.b[7]) == 0xFF) {
		DBG_8195A("SDR flash calibration [%08x]: %02x-%02x ", faddr, value.b[4], value.b[6]);
	} else {
		value.b[4] = 0; // TapCnt
#if CONFIG_CPU_CLK < 6
		value.b[6] = 0x11; // RdPipe
#elif CONFIG_CPU_CLK == 7
		value.b[6] = 0x23; // RdPipe
#else
		value.b[6] = 0x19; // RdPipe
#endif
		DBG_8195A("Use fix SDR calibration: %02x-%02x ", value.b[4], value.b[6]);
	}
	// set all_mode _idle
	HAL_SDR_WRITE32(REG_SDR_CSR, 0x700);
	// WRAP_MISC setting
	HAL_SDR_WRITE32(REG_SDR_MISC,	0x00000001);
	// PCTL setting
	HAL_SDR_WRITE32(REG_SDR_DCR,	0x00000008);
	HAL_SDR_WRITE32(REG_SDR_IOCR,	(u32)value.b[4] << PCTL_IOCR_RD_PIPE_BFO);
    HAL_SDR_WRITE32(REG_SDR_EMR2,	0x00000000);
    HAL_SDR_WRITE32(REG_SDR_EMR1,	0x00000006);
	HAL_SDR_WRITE32(REG_SDR_MR,		0x00000022);
	HAL_SDR_WRITE32(REG_SDR_DRR,	0x09030e07);
	HAL_SDR_WRITE32(REG_SDR_TPR0,	0x00002652);
	HAL_SDR_WRITE32(REG_SDR_TPR1,	0x00068873);
	HAL_SDR_WRITE32(REG_SDR_TPR2,	0x00000042);
	// start to init
	HAL_SDR_WRITE32(REG_SDR_CCR, 0x01);
	while ((HAL_SDR_READ32(REG_SDR_CCR) & 0x1) == 0x0)
		DBG_8195A(".");
	// enter mem_mode
	HAL_SDR_WRITE32(REG_SDR_CSR, 0x600);
	SDR_DDL_FCTRL((u32)value.b[6]);
	DBG_8195A(" ok\n");
}
#endif // FIX_SDR_CALIBRATION
#endif // CONFIG_SDR_EN
/*
//----- HalNMIHandler_Patch
void HalNMIHandler_Patch(void) {
	DBG_8195A_HAL("%s:NMI Error!\n", __func__);
	if ( HAL_READ32(VENDOR_REG_BASE, 0) < 0)
		HalWdgIntrHandle(); // ROM: HalWdgIntrHandle = 0x3485;
}
*/

/*
 *  16 bytes FIFO ... 16*11/38400 = 0.004583 sec
 *  (0.005/5)*166666666 = 166666.666
 */
LOCAL void INFRA_START_SECTION loguart_wait_tx_fifo_empty(void) {
	int x = 16384;
	while((!(HAL_READ8(LOG_UART_REG_BASE, 0x14) & BIT6)) && x--);
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

// weak __low_level_init function!
__weak void __low_level_init(void) {
}

// weak main function!
__weak int main(void) {
	DiagPrintf("\r\nRTL Console ROM: Start - press key 'Up', Help '?'\r\n");
	while (pUartLogCtl->ExecuteEsc != 1);
	pUartLogCtl->RevdNo = 0;
	pUartLogCtl->BootRdy = 1;
	DiagPrintf("\r<RTL>");
	while (1) {
		while (pUartLogCtl->ExecuteCmd != 1);
		UartLogCmdExecute((PUART_LOG_CTL) pUartLogCtl);
		DiagPrintf("\r<RTL>");
		pUartLogCtl->ExecuteCmd = 0;
	}
	return 0;
}

//----- InfraStart
void INFRA_START_SECTION InfraStart(void) {
//	NewVectorTable[2] = HalNMIHandler_Patch;
	DBG_8195A("===== Enter Image: %s ====\n", cus_sig);
//	ShowRamBuildInfo(); // app_start.c: VOID ShowRamBuildInfo(VOID)
	memset(&__bss_start__, 0, &__bss_end__ - &__bss_start__);
	rtl_libc_init(); // ROM Lib C init (rtl_printf!)
//	SYSPlatformInit();
//	SDIO_Device_Off();
	//- Должно быть в boot
extern HAL_GPIO_ADAPTER gBoot_Gpio_Adapter;
	memset(&gBoot_Gpio_Adapter, 0, sizeof(gBoot_Gpio_Adapter));
	_pHAL_Gpio_Adapter = &gBoot_Gpio_Adapter;
	VectorTableInitRtl8195A(STACK_TOP);	// 0x1FFFFFFC
	loguart_wait_tx_fifo_empty(); //	иначе глючит LogUART, если переключение CLK приходится на вывод символов !
#if 1 // if set CLK CPU
	if(HalGetCpuClk() != PLATFORM_CLOCK) {
		//----- CLK CPU
#if	CPU_CLOCK_SEL_DIV5_3
		// 6 - 200000000 Hz, 7 - 10000000 Hz, 8 - 50000000 Hz, 9 - 25000000 Hz, 10 - 12500000 Hz, 11 - 4000000 Hz
		HalCpuClkConfig(CPU_CLOCK_SEL_VALUE);
		*((int *)(SYSTEM_CTRL_BASE+REG_SYS_SYSPLL_CTRL1)) |= (1<<17);// REG_SYS_SYSPLL_CTRL1 |= BIT_SYS_SYSPLL_DIV5_3
#else
		// 0 - 166666666 Hz, 1 - 83333333 Hz, 2 - 41666666 Hz, 3 - 20833333 Hz, 4 - 10416666 Hz, 5 - 4000000 Hz
		*((int *) (SYSTEM_CTRL_BASE + REG_SYS_SYSPLL_CTRL1)) &= ~(1 << 17); // REG_SYS_SYSPLL_CTRL1 &= ~BIT_SYS_SYSPLL_DIV5_3
		HalCpuClkConfig(CPU_CLOCK_SEL_VALUE);
#endif // CPU_CLOCK_SEL_DIV5_3
	};
#endif
	PSHalInitPlatformLogUart(); // HalInitPlatformLogUartV02(); // Show "<RTL8195A>"... :(
	HalReInitPlatformTimer(); // HalInitPlatformTimerV02(); HalTimerOpInit_Patch((VOID*) (&HalTimerOp));
	SystemCoreClockUpdate();
	En32KCalibration();

	//---- Spic
//	_memset(SpicInitParaAllClk, 0, sizeof(SpicInitParaAllClk));
	*(uint32 *)(&SpicInitParaAllClk[0][0].BaudRate) = 0x01310202; // patch
	*(uint32 *)(&SpicInitParaAllClk[1][0].BaudRate) = 0x11311301; // patch
//	*(uint32 *)(&SpicInitParaAllClk[2][0].BaudRate) = 0x21311301; // patch
	SPI_FLASH_PIN_FCTRL(ON);
/*
//	uint8 SpicBaudRate = CPU_CLK_TYPE_NO - 1 - ((HAL_SYS_CTRL_READ32(REG_SYS_CLK_CTRL1) >> 4) & 7);
	uint8 SpicBaudRate = 3; // HAL_SYS_CTRL_READ32(REG_SYS_CLK_CTRL1) >> 4) & 7;
	DBG_8195A("SpicBaudRate = %d\n", SpicBaudRate);
	SpicInitRtl8195AV02(SpicBaudRate, SpicDualBitMode);
	if(!SpicCmpDataForCalibrationRtl8195A()) {
			DBG_8195A("ReInit Spic to SIO...\n");
			SpicInitRtl8195AV02(SpicBaudRate, SpicOneBitMode);
			if(!SpicCmpDataForCalibrationRtl8195A()) {
				DBG_8195A("Error Init Spic!\n");
			};
	};
*/
//	SpicFlashInitRtl8195A(SpicDualBitMode); //	SpicReadIDRtl8195A(); SpicDualBitMode
#ifdef CONFIG_SDR_EN
	//---- SDRAM
	uint8 ChipId = HalGetChipId();
	if (ChipId >= CHIP_ID_8195AM) {
		if((HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & BIT(21)) == 0) { // SDR not init?
 #ifdef FIX_SDR_CALIBRATION // for speed :)
			sdr_init();
 #else	// not FIX_SDR_CALIBRATION
			SdrCtrlInit();
			if(!SdrControllerInit()) {
				DBG_8195A("SDR Controller Init fail!\n");
			};
 #endif // FIX_SDR_CALIBRATION
		};
		// clear SDRAM bss
		extern uint8 __sdram_bss_start__[];
		extern uint8 __sdram_bss_end__[];
		if((uint32)__sdram_bss_end__-(uint32)__sdram_bss_start__ > 0)
			memset(__sdram_bss_start__, 0, (uint32)__sdram_bss_end__-(uint32)__sdram_bss_start__);
	}
	else // if (ChipId < CHIP_ID_8195AM)
	{
		//----- SDRAM Off
		SDR_PIN_FCTRL(OFF);
		LDO25M_CTRL(OFF);
	};
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT(21)); // Flag SDRAM Init or None
#else
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & (~BIT(21))); // Flag SDRAM Not Init
#endif // CONFIG_SDR_EN
	//----- Close Flash
	SPI_FLASH_PIN_FCTRL(OFF);

	InitSoCPM();
	VectorTableInitForOSRtl8195A(&vPortSVCHandler, &xPortPendSVHandler,
			&xPortSysTickHandler);

#if	CONFIG_DEBUG_LOG > 4
	DBG_8195A("\rSet CPU CLK: %d Hz, SOC FUNC EN: %p\r\n", HalGetCpuClk(), HAL_PERI_ON_READ32(REG_SOC_FUNC_EN));
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


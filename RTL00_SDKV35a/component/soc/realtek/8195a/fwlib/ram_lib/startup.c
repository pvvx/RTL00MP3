/* 
 * BootLoader
 * startup.o sdk-ameba-rtl8710af-v3.5a_without_NDA_GCC_V1.0.0
 * pvvx 2016
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
#include "rtl_consol.h"

#ifndef USE_SRC_ONLY_BOOT
#define USE_SRC_ONLY_BOOT	0
#endif

#if USE_SRC_ONLY_BOOT
#define rtl_memset _memset
#define rtl_strcmp _strcmp
#define rtl_memcpy _memcpy
#endif

#define VREG32(addr) (*((volatile u32*)(addr)))

typedef void (*START_FUNC)(void);

#define DEFAULT_BAUDRATE UART_BAUD_RATE_38400
#define StartupSpicBitMode    SpicDualBitMode // SpicOneBitMode
#define StartupSpicBaudRate  0

//-------------------------------------------------------------------------
// Function declarations
void PreProcessForVendor(void); // image1
void RtlBootToSram(void); // image1
u32 StartupHalLogUartInit(u32 uart_irq); // image1
void StartupHalInitPlatformLogUart(void); // image1
int IsForceLoadDefaultImg2(void); // image1
void StartupHalSpicInit(int InitBaudRate); // image1
int _GetChipId(void); // image1
void RtlConsolRam(void); // image1
extern VOID UartLogIrqHandle(VOID * Data); // in ROM
extern int RtlConsolRom(int); // in ROM
extern PHAL_GPIO_ADAPTER _pHAL_Gpio_Adapter;
extern VOID SpicUserReadRtl8195A(IN u32 Length, IN u32 addr, IN u8 * data,
		IN u8 BitMode);
#if !USE_SRC_ONLY_BOOT
void InfraStart(void);
extern void HalWdgIntrHandle(void);
extern int wifi_off(void); // in wifi_conf.c
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler(void);
extern void ShowRamBuildInfo(void); // app_start.c: VOID ShowRamBuildInfo(VOID)
void HalNMIHandler_Patch(void);
void SDIO_Device_Off(void);
void VectorTableOverrideRtl8195A(u32 StackP);
void SYSPlatformInit(void);
void HalHardFaultHandler_Patch_c(u32 HardDefaultArg);
void __HalReInitPlatformLogUart(void);
void _ReloadImg(void);
void _ReloadImg_user_define(void);
void _CPUResetHandler(void);
void _CPUReset(void);
void HalHardFaultHandler_user_define(u32 HardDefaultArg);
#endif

//-------------------------------------------------------------------------
// Data declarations
extern START_FUNC __image2_entry_func__;
extern u8 __image2_validate_code__;
extern u8 __image1_bss_start__, __image1_bss_end__;
extern u8 __rom_bss_start__, __rom_bss_end__;
//extern u32 STACK_TOP;
#define STACK_TOP 0x1FFFFFFC

#if !USE_SRC_ONLY_BOOT
extern u32 * NewVectorTable; // LD: NewVectorTable = 0x10000000;
extern u8 __bss_start__, __bss_end__;
#endif

//extern volatile UART_LOG_CTL * pUartLogCtl;
extern int UartLogCmdExecute(volatile u8 *);
/*
typedef struct __RAM_IMG2_VALID_PATTEN__ {
	char rtkwin[7];
	u8 x[13];
} _RAM_IMG2_VALID_PATTEN, *_PRAM_IMG2_VALID_PATTEN;
*/
const uint8_t IMAGE1_VALID_PATTEN_SECTION RAM_IMG1_VALID_PATTEN[8] =
		{ 0x23, 0x79, 0x16, 0x88, 0xff, 0xff, 0xff, 0xff };

PRAM_FUNCTION_START_TABLE __attribute__((section(".data.pRamStartFun"))) pRamStartFun =
		(PRAM_FUNCTION_START_TABLE) 0x10000BC8;

#include <reent.h>

struct _reent __attribute__((section(".libc.reent"))) impure_reent = _REENT_INIT(impure_reent);
//struct _reent * __attribute__((section(".libc.reent"))) _rtl_impure_ptr = { &impure_data };
//struct _reent * __attribute__((at(0x1098))) __attribute__((section(".libc.reent"))) _rtl_impure_ptr = { &impure_data };
struct _reent * __attribute__((at(0x10001c60))) __attribute__((section(".libc.reent"))) _rtl_impure_ptr = { &impure_reent };

/* ROM */
MON_RAM_BSS_SECTION
    volatile UART_LOG_CTL    *pUartLogCtl;

MON_RAM_BSS_SECTION
    UART_LOG_BUF             UartLogBuf;

MON_RAM_BSS_SECTION
    volatile UART_LOG_CTL    UartLogCtl;

MON_RAM_BSS_SECTION
    u8                       *ArgvArray[MAX_ARGV]; // *ArgvArray[10] !

MON_RAM_BSS_SECTION
    u8  UartLogHistoryBuf[UART_LOG_HISTORY_LEN][UART_LOG_CMD_BUFLEN]; // UartLogHistoryBuf[5][127] !


RAM_START_FUNCTION START_RAM_FUN_A_SECTION gRamStartFun =
		{ PreProcessForVendor + 1 };
RAM_START_FUNCTION START_RAM_FUN_B_SECTION gRamPatchWAKE =
		{ RtlBootToSram + 1 };
RAM_START_FUNCTION START_RAM_FUN_C_SECTION gRamPatchFun0 =
		{ RtlBootToSram + 1 };
RAM_START_FUNCTION START_RAM_FUN_D_SECTION gRamPatchFun1 =
		{ RtlBootToSram + 1 };
RAM_START_FUNCTION START_RAM_FUN_E_SECTION gRamPatchFun2 =
		{ RtlBootToSram + 1 };

#if !USE_SRC_ONLY_BOOT
RAM_START_FUNCTION IMAGE2_START_RAM_FUN_SECTION gImage2EntryFun0 =
		{ InfraStart + 1 };
#else
RAM_START_FUNCTION IMAGE2_START_RAM_FUN_SECTION gImage2EntryFun0 =
		{ 0x100 };
#endif // !USE_SRC_ONLY_BOOT
_RAM_IMG2_VALID_PATTEN IMAGE2_VALID_PATTEN_SECTION RAM_IMG2_VALID_PATTEN =
		{ { IMG2_SIGN_TXT }, { 0xff, 0, 1, 1, 0, 0x95, 0x81, 1, 1, 0, 0, 0, 0 } }; // "RTKWin"

HAL_GPIO_ADAPTER PINMUX_RAM_DATA_SECTION gBoot_Gpio_Adapter;

#pragma arm section code = ".hal.ram.text"
#pragma arm section rodata = ".hal.ram.rodata", rwdata = ".hal.ram.data", zidata = ".hal.ram.bss"

#if !USE_SRC_ONLY_BOOT
//----- HalNMIHandler_Patch
void HalNMIHandler_Patch(void) {
	DBG_8195A_HAL("%s:NMI Error!\n", __func__);
	if ( HAL_READ32(VENDOR_REG_BASE, 0) < 0)
		HalWdgIntrHandle(); // ROM: HalWdgIntrHandle = 0x3485;
}
#endif // !USE_SRC_ONLY_BOOT


void __attribute__((section(".hal.ram.text"))) SetDebugFlgs() {
#if CONFIG_DEBUG_LOG > 2
	ConfigDebugErr = -1;
	ConfigDebugWarn = -1;
	ConfigDebugInfo = -1;
#elif CONFIG_DEBUG_LOG > 1
	ConfigDebugErr = -1;
	ConfigDebugWarn = -1;
	ConfigDebugInfo = 0;
#elif CONFIG_DEBUG_LOG > 0
	ConfigDebugErr = -1;
	ConfigDebugWarn = 0;
	ConfigDebugInfo = 0;
#else
	ConfigDebugErr = 0;
	ConfigDebugWarn = 0;
	ConfigDebugInfo = 0;
#endif
}

void __attribute__((section(".hal.ram.text"))) InitSpic(void)
{
	VREG32(0x40006000) = 0x01000300;
	VREG32(0x40006004) = 0x1;
	VREG32(0x400060E0) = 0x0B;
	VREG32(0x400060E4) = 0x3B;
	VREG32(0x400060E8) = 0x3B;
	VREG32(0x400060EC) = 0x6B;
	VREG32(0x400060F0) = 0xEB;
	VREG32(0x400060F4) = 0x02;
	VREG32(0x400060F8) = 0xA2;
	VREG32(0x400060FC) = 0xA2;
	VREG32(0x40006100) = 0x32;
	VREG32(0x40006104) = 0x38;
	VREG32(0x40006108) = 0x06;
	VREG32(0x4000610C) = 0x05;
	VREG32(0x40006110) = 0x51;
	VREG32(0x40006114) = 0x01;
	VREG32(0x40006118) = 0x03;
	VREG32(0x4000611C) = 0x20030013;
	VREG32(0x40006120) = 0x202;
	VREG32(0x40006124) = 0x0E;
}

//----- StartupHalLogUartInit
u32 __attribute__((section(".hal.ram.text"))) StartupHalLogUartInit(u32 uart_irq) {
	HAL_UART_WRITE32(UART_DLH_OFF, 0);

	u32 SysClock = (HalGetCpuClk() >> 2);
	u32 SampleRate = (16 * DEFAULT_BAUDRATE);
	u32 Divisor = SysClock / SampleRate;
	u32 Remaind = ((SysClock * 10) / SampleRate) - (Divisor * 10);
	if (Remaind > 4) Divisor++;
	// set DLAB bit to 1
//	HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, 0);
	HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF, RUART_LINE_CTL_REG_DLAB_ENABLE);
	HAL_UART_WRITE32(UART_DLL_OFF, Divisor & 0xff);
	HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF, 3);
	HAL_UART_WRITE32(UART_FIFO_CTL_REG_OFF, FIFO_CTL_DEFAULT_WITH_FIFO);
	HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, uart_irq);
	if (uart_irq) {
		// Enable Peripheral_IRQ Setting for Log_Uart
		HAL_WRITE32(VENDOR_REG_BASE, PERIPHERAL_IRQ_EN, 0x1000000);
		// Cortex-M3 SCB->AIRCR
		HAL_WRITE32(0xE000ED00, 0x0C,
				(HAL_READ32(0xE000ED00, 0x0C) & 0x0F8FF) | 0x5FA0300);
		HAL_WRITE8(0xE000E100, 0x313, 0xE0); // HAL_WRITE8(0xE000E100, 0x313, 0xE0);
		HAL_WRITE32(0xE000E100, 0, 0x80000); // NVIC enable external interrupt[?] ?
	}
	return 0;
}

//----- StartupHalInitPlatformLogUart
void __attribute__((section(".hal.ram.text"))) StartupHalInitPlatformLogUart(
		void) {
	HAL_UART_READ32(UART_REV_BUF_OFF);
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & (~(BIT_SOC_LOG_UART_EN))); //   40000210 &= 0xFFFFEFFF; // ~(1<<12)
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT_SOC_LOG_UART_EN); // 40000210 |= 0x1000u;
	ACTCK_LOG_UART_CCTRL(ON);	 // 40000230 |= 0x1000u;
	StartupHalLogUartInit(IER_ERBFI | IER_ELSI);
}

void __attribute__((section(".hal.ram.text"))) RtlConsolRam(void)
{
//	__asm__ __volatile__ ("cpsid f\n");
//	HalCpuClkConfig(0); // 0 - 166666666 Hz, 1 - 83333333 Hz, 2 - 41666666 Hz, 3 - 20833333 Hz, 4 - 10416666 Hz, 5 - 4000000 Hz
//	ConfigDebugErr = -1;
//	VectorTableInitRtl8195A(STACK_TOP); // 0x1FFFFFFC);
//	HalInitPlatformLogUartV02();
//    HalReInitPlatformLogUartV02();
//	HalInitPlatformTimerV02();
	__asm__ __volatile__ ("cpsie f\n");

	DiagPrintf("\r\nRTL Console ROM: Start - press 'ESC' key, Help '?'\r\n");
	while(!pUartLogCtl->ExecuteEsc);
	pUartLogCtl->EscSTS = 0;
	pUartLogCtl->BootRdy = 1;
	DiagPrintf("\r<RTL>");
	while(1) {
		while(!pUartLogCtl->ExecuteCmd);
		UartLogCmdExecute(pUartLogCtl);
		DiagPrintf("\r<RTL>");
		pUartLogCtl->ExecuteCmd = 0;
	}
}

//----- RtlBootToSram
void __attribute__((section(".hal.ram.text"))) RtlBootToSram(void) {
	TIMER_ADAPTER tim_adapter;
	/* JTAG On */
	ACTCK_VENDOR_CCTRL(ON);
	SLPCK_VENDOR_CCTRL(ON);
	HalPinCtrlRtl8195A(JTAG, 0, 1);

	memset(&__rom_bss_start__, 0, &__rom_bss_end__ - &__rom_bss_start__);

	/* Flash & LogUart On */
	HAL_PERI_ON_WRITE32(REG_GPIO_SHTDN_CTRL, 0x7FF);
	SPI_FLASH_PIN_FCTRL(ON);
	HAL_PERI_ON_WRITE32(REG_CPU_PERIPHERAL_CTRL,
			HAL_PERI_ON_READ32(REG_CPU_PERIPHERAL_CTRL) | BIT_SPI_FLSH_PIN_EN); // 400002C0 |= 0x1u;
	HAL_PERI_ON_WRITE32(REG_CPU_PERIPHERAL_CTRL,
			HAL_PERI_ON_READ32(REG_CPU_PERIPHERAL_CTRL)	| BIT_LOG_UART_PIN_EN); // 400002C0 |= 0x100000u;

	VectorTableInitRtl8195A(STACK_TOP); // 0x1FFFFFFC

	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT_SOC_FLASH_EN); // 40000210 |= 0x10u;
	ACTCK_FLASH_CCTRL(ON);
	SLPCK_FLASH_CCTRL(ON);
	HalPinCtrlRtl8195A(SPI_FLASH, 0, 1);

	SpicNVMCalLoadAll();

	HAL_SYS_CTRL_WRITE32(REG_SYS_CLK_CTRL1,
			HAL_SYS_CTRL_READ32(REG_SYS_CLK_CTRL1) & 0x8F); // VREG32(0x40000014) &= 0x8F;

	SetDebugFlgs();

	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & (~(BIT_SOC_LOG_UART_EN))); //   40000210 &= 0xFFFFEFFF; // ~(1<<12)
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT_SOC_LOG_UART_EN); // 40000210 |= 0x1000u;

	ACTCK_LOG_UART_CCTRL(ON);
//	SLPCK_LOG_UART_CCTRL(ON);

	tim_adapter.IrqHandle.IrqFun = &UartLogIrqHandle;
	tim_adapter.IrqHandle.IrqNum = UART_LOG_IRQ;
	tim_adapter.IrqHandle.Data = 0;
	tim_adapter.IrqHandle.Priority = 5;

	StartupHalLogUartInit(0);
	VectorIrqRegisterRtl8195A(&tim_adapter.IrqHandle);
	StartupHalLogUartInit(IER_ERBFI | IER_ELSI);

	HAL_PERI_ON_WRITE32(REG_PON_ISO_CTRL, 3); // VREG32(0x40000204) = 3;
	HAL_PERI_ON_WRITE32(REG_OSC32K_CTRL,
			HAL_PERI_ON_READ32(REG_OSC32K_CTRL) | BIT_32K_POW_CKGEN_EN); // VREG32(0x40000270) |= 1u;
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT_SOC_GTIMER_EN); // VREG32(0x40000210) |= 0x10000u;

	ACTCK_TIMER_CCTRL(ON);
	SLPCK_TIMER_CCTRL(ON);

	tim_adapter.TimerIrqPriority = 0;
	tim_adapter.TimerLoadValueUs = 0;
	tim_adapter.TimerMode = FREE_RUN_MODE;
	tim_adapter.IrqDis = 1;
	tim_adapter.TimerId = 1;
	HalTimerInitRtl8195a((PTIMER_ADAPTER) &tim_adapter);

	SpicInitRtl8195A(1, StartupSpicBitMode); // StartupSpicBaudRate InitBaudRate 1, SpicBitMode 1 StartupSpicBitMode
	SpicFlashInitRtl8195A(StartupSpicBitMode); // SpicBitMode 1 StartupSpicBitMode
	DBG_8195A("==*== Enter Image 1.5 ====\nImg2 Sign: %s, InfaStart @ 0x%08x\n",
			&__image2_validate_code__, __image2_entry_func__);
	if (strcmp((const char * )&__image2_validate_code__, IMG2_SIGN_TXT)) {
		DBG_MISC_ERR("Invalid Image2 Signature!\n");
		RtlConsolRam();
	}
//	InitSpic();
	__image2_entry_func__();
}

//----- SYSCpuClkConfig
void __attribute__((section(".hal.ram.text"))) SYSCpuClkConfig(int ChipID, int SysCpuClk) {
	int flg = 0;
	DBG_SPIF_INFO("SYSCpuClkConfig(0x%x)\n", SysCpuClk);
	if(HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & BIT_SOC_FLASH_EN) {
		SpicWaitWipRtl8195A(); //_SpicWaitWipDoneRefinedRtl8195A(); ???
	    flg = 1;
	}
//	if (ChipID == CHIP_ID_8710AF && (!SysCpuClk)) SysCpuClk = 1;
	HalCpuClkConfig(SysCpuClk);
	HalDelayUs(1000);
	StartupHalInitPlatformLogUart();
	if (flg) {
		SpicOneBitCalibrationRtl8195A(SysCpuClk); // extern u32 SpicOneBitCalibrationRtl8195A(IN u8 SysCpuClk);
/*
		// Disable SPI_FLASH User Mode
	    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
        HAL_SPI_WRITE32(REG_SPIC_VALID_CMD,
                (HAL_SPI_READ32(REG_SPIC_VALID_CMD)|(FLASH_VLD_DUAL_CMDS))); */
        SpicCalibrationRtl8195A(StartupSpicBitMode, 0);
	}
}

//----- IsForceLoadDefaultImg2
int __attribute__((section(".hal.ram.text"))) IsForceLoadDefaultImg2(void) {
	u8 gpio_pin[4];
	HAL_GPIO_PIN GPIO_Pin;
	HAL_GPIO_PIN_STATE flg;
	int result = 0;

	*((u32 *) &gpio_pin) = HAL_READ32(SPI_FLASH_BASE, FLASH_SYSTEM_DATA_ADDR + 0x08); // config data + 8
	_pHAL_Gpio_Adapter = (int) &gBoot_Gpio_Adapter;
	for(int i = 0; i < 2; i++) {
		u8 x = gpio_pin[i];
		if (x != 0xff) {
			GPIO_Pin.pin_name = HAL_GPIO_GetIPPinName_8195a(x & 0x7F);
			if (x & 0x80) {
				GPIO_Pin.pin_mode = DIN_PULL_LOW;
				flg = GPIO_PIN_HIGH;
			} else {
				GPIO_Pin.pin_mode = DIN_PULL_HIGH;
				flg = GPIO_PIN_LOW;
			}
			HAL_GPIO_Init_8195a(&GPIO_Pin);
			result |= HAL_GPIO_ReadPin_8195a(&GPIO_Pin) == flg;
			HAL_GPIO_DeInit_8195a(&GPIO_Pin);
		}
	}
	_pHAL_Gpio_Adapter->IrqHandle.IrqFun = NULL;
	return result;
}

//----- GetChipId
int __attribute__((section(".hal.ram.text"))) _GetChipId() {
	u8 chip_id = CHIP_ID_8195AM;
	if (HALEFUSEOneByteReadROM(HAL_SYS_CTRL_READ32(REG_SYS_EFUSE_CTRL), 0xF8,
			&chip_id, L25EOUTVOLTAGE) != 1)
		DBG_MISC_INFO("Get Chip ID Failed\r");
	return chip_id;
}

//----- StartupHalSpicInit
void __attribute__((section(".hal.ram.text"))) StartupHalSpicInit(
		int InitBaudRate) {
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT4); // HAL_SYS_CTRL_READ32
	HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL,
			HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_ACTCK_FLASH_EN | BIT_SOC_SLPCK_FLASH_EN);
	HalPinCtrlRtl8195A(SPI_FLASH,
			((HAL_SYS_CTRL_READ32(REG_SYS_SYSTEM_CFG1) & 0xF0000000)
					== 0x30000000), 1);
	SpicInitRtl8195A(InitBaudRate, StartupSpicBitMode);
}


void __attribute__((section(".hal.ram.text"))) flashcpy(u32 raddr, u32 faddr, s32 size) {
	while(size > 0) {
		HAL_WRITE32(0, raddr, HAL_READ32(SPI_FLASH_BASE, faddr));
		raddr+=4;
		faddr+=4;
		size-=4;
	}
}
//----- PreProcessForVendor
void __attribute__((section(".hal.ram.text"))) PreProcessForVendor(void) {
	START_FUNC entry_func;
	u32 run_image;
	u32 Image2Addr = *(u32 *)(0x1006FFFC);
	u32 v16 = 0, v17;
#if 0
	u8 efuse0xD3_data;
	HALEFUSEOneByteReadROM(HAL_SYS_CTRL_READ32(REG_SYS_EFUSE_CTRL), 0xD3,
			&efuse0xD3_data, L25EOUTVOLTAGE);
	if (efuse0xD3_data & 1)
#endif
			HalPinCtrlRtl8195A(JTAG, 0, 1);
	SetDebugFlgs();
	int chip_id = _GetChipId();
	int flash_enable = HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & BIT_SOC_FLASH_EN; // v6 = ...
	int spic_init = 0;
///	InitSpic();
	if (flash_enable) {
		entry_func = &__image2_entry_func__;
		spic_init = 1;
	} else {
		entry_func = (START_FUNC) Image2Addr;
		if (chip_id != CHIP_ID_8711AN) { // 0xFB
			StartupHalSpicInit(StartupSpicBaudRate); // BaudRate 1
			spic_init = 1;
		}
	}
	DBG_8195A("BOOT from Flash: %s\n", (flash_enable) ? "YES" : "NO");
	memset(&__image1_bss_start__, 0,
			&__image1_bss_end__ - &__image1_bss_start__);
	HalDelayUs(1000);
	int sdr_enable = 0;

#ifdef CONFIG_SDR_EN
	if (chip_id > CHIP_ID_8711AF || chip_id == CHIP_ID_8710AM) {
		SdrCtrlInit();
		sdr_enable = 1;
	}
	else {
		HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT(21));
	}
#else
//	SdrPowerOff();
    SDR_PIN_FCTRL(OFF);
    LDO25M_CTRL(OFF);
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT(21));
#endif

	if (spic_init) SpicNVMCalLoadAll();
	SYSCpuClkConfig(chip_id, 0);
	StartupHalInitPlatformLogUart(); // double !?
	__asm__ __volatile__ ("cpsie f\n");
	DBG_8195A("===== Enter Image 1 ====\n");
	if (spic_init) {
		SpicReadIDRtl8195A();
		SpicFlashInitRtl8195A(StartupSpicBitMode); // SpicBitMode 1
	}
#ifdef CONFIG_SDR_EN
	if (sdr_enable)	SdrControllerInit();
#endif
	if (flash_enable) {

		u32 img1size = (*(u16 *) (SPI_FLASH_BASE + 0x18)) << 10; // size in 1024 bytes
		if (img1size == 0 || img1size >= 0x3FFFC00)
			img1size = *(u32 *) (SPI_FLASH_BASE + 0x10) + 32;
		u32 * prdflash = (u32 *) (img1size + SPI_FLASH_BASE + 8);
		u32 sign1 = *prdflash++;
		u32 sign2 = *prdflash;
		{
			v16 = -1;
			v17 = -1;
			if (sign2 == IMG_SIGN2_RUN) {
				if (sign1 == IMG_SIGN1_RUN) {
					v16 = img1size;
					v17 = -1;
				} else if (sign1 == IMG_SIGN1_SWP) {
					v17 = img1size;
					v16 = -1;
				}
			}
			u32 OTA_addr = *(u32 *) (SPI_FLASH_BASE + FLASH_SYSTEM_DATA_ADDR); // config sector data
			if (OTA_addr != -1) {
				u32 image2size = *(u32 *) (img1size + SPI_FLASH_BASE);
				if (OTA_addr >= (img1size + image2size)
						&& !(OTA_addr & 0xFFF)) {
					prdflash = (u32 *) (OTA_addr + SPI_FLASH_BASE + 8);
					sign1 = *prdflash++;
					sign2 = *prdflash;
					if (sign2 == IMG_SIGN2_RUN) {
						if (sign1 == IMG_SIGN1_RUN) v16 = OTA_addr;
						else if (sign1 == IMG_SIGN1_SWP) v17 = OTA_addr;
					}
LABEL_41: 			if (IsForceLoadDefaultImg2()) {
						if (v17 != -1) run_image = v17;
						else {
							run_image = v16;
							if (run_image == -1) {
								DiagPrintf("Fatal: no fw\n");
								RtlConsolRam();
							}
						}
					} else {
						if (v16 != -1) run_image = v16;
						else {
							run_image = v17;
							if (run_image == -1) {
								DiagPrintf("Fatal: no fw\n");
								RtlConsolRam();
							}
						}
					}
					u8 * pstr;
					if (run_image == v17)
						pstr = "load OLD fw %d\n";
					else {
						if (run_image != v16) {
LABEL_55: 					prdflash = run_image + SPI_FLASH_BASE;
							u32 img_size = *prdflash++;
							u32 Image2Addr = *prdflash;
							DBG_8195A("Flash Image2: Addr 0x%x, Len %d, Load to SRAM 0x%x\n", run_image, img_size, Image2Addr); // debug!
							flashcpy(Image2Addr, run_image+16, img_size);
//							SpicUserReadFourByteRtl8195A(img_size, run_image + 16, Image2Addr, StartupSpicBitMode); // SpicDualBitMode
							prdflash = run_image + img_size + SPI_FLASH_BASE + 16;
							u32 sdram_image_size = *prdflash++; // +0x10
							u32 sdram_load_addr = *prdflash; // +0x14
							DBG_8195A("Image3 length: 0x%x, Image3 Addr: 0x%x\n",
									sdram_image_size, sdram_load_addr);
							if ((sdram_image_size - 1) <= 0xFFFFFFFD
							&& *((u32 *)(sdram_load_addr)) == SDR_SDRAM_BASE) { // sdram_load_addr
								if (!sdr_enable) {
									DBG_MISC_ERR("FW/HW conflict. No DRAM on board.\n");
									RtlConsolRam();
								}
								DBG_8195A("Image3 length: 0x%x, Image3 Addr: 0x%x\n",
										sdram_image_size, sdram_load_addr);
//								SpicUserReadRtl8195A(sdram_image_size, run_image + img_size + 32, SDR_SDRAM_BASE, StartupSpicBitMode);
							} else DBG_8195A("No Image3\n");

							entry_func = *(u32 *)Image2Addr;
							DBG_8195A("Img2 Sign: %s, InfaStart @ 0x%08x \n",
									(const char * )(Image2Addr + 4),
									entry_func); // *(u32 *)Image2Addr);
							if (strcmp((const char * )(Image2Addr + 4),
									IMG2_SIGN_TXT)) {
								DBG_MISC_ERR("Invalid Image2 Signature\n");
								RtlConsolRam();
							}
#if 0
	DBG_8195A("CLK CPU: %d Hz\n", HalGetCpuClk());
	RtlConsolRam();
#else
#endif
							(void) (entry_func)();
							return;
						}
						pstr = "load NEW fw %d\n";
					} // if (run_image == v17) else
					DiagPrintf(pstr, ((run_image - OTA_addr) <= 0));
					goto LABEL_55;
				}
				DBG_MISC_ERR("OTA addr 0x%x INVALID\n", OTA_addr);
			}
			OTA_addr = -1;
			goto LABEL_41;
		}
	} // if (flash_enable)
	if (strcmp((const char * )(Image2Addr + 4), IMG2_SIGN_TXT)) {
		DBG_MISC_ERR("Invalid Image2 Signature\n", 2 * ConfigDebugErr);
		RtlConsolRam();
	}
	(void) (entry_func)();
}

#if !USE_SRC_ONLY_BOOT
//----- HalHardFaultHandler_Patch_c
void HalHardFaultHandler_Patch_c(u32 HardDefaultArg) {
	u32 v1;
	int v2;
	int v3;

	v1 = HardDefaultArg;
	if ((VREG32(0xE000ED28) & 0x82)
			&& (unsigned int) (VREG32(0xE000ED38) - 0x40080000) < 0x40000) {
		DBG_8195A("\n.");
		v2 = *(u32 *) (v1 + 24);
		if ((*(u16 *) v2 & 0xF800) <= 0xE000) v3 = v2 + 2;
		else v3 = v2 + 4;
		*(u32 *) (v1 + 24) = v3;
	} else {
		HalHardFaultHandler_user_define(HardDefaultArg);
		HalHardFaultHandler(v1); // ROM: HalHardFaultHandler = 0x911;
	}
}

//----- VectorTableOverrideRtl8195A
void __attribute__((section(".infra.ram.start"))) VectorTableOverrideRtl8195A(u32 StackP) {
	NewVectorTable[2] = HalNMIHandler_Patch;
}

//----- SYSPlatformInit
void __attribute__((section(".infra.ram.start"))) SYSPlatformInit(void) {
	HAL_SYS_CTRL_WRITE32(REG_SYS_EFUSE_SYSCFG0,
			(HAL_SYS_CTRL_READ32(REG_SYS_EFUSE_SYSCFG0) & (~(BIT_MASK_SYS_EEROM_LDO_PAR_07_04 << BIT_SHIFT_SYS_EEROM_LDO_PAR_07_04))) | BIT_SYS_EEROM_LDO_PAR_07_04(6)); // & 0xF0FFFFFF | 0x6000000
	HAL_SYS_CTRL_WRITE32(REG_SYS_XTAL_CTRL1,
			(HAL_SYS_CTRL_READ32(REG_SYS_XTAL_CTRL1) & (~(BIT_MASK_SYS_XTAL_DRV_RF1 << BIT_SHIFT_SYS_XTAL_DRV_RF1))) | BIT_SYS_XTAL_DRV_RF1(1)); //  & 0xFFFFFFE7 | 8;
}

//----- InfraStart
void __attribute__((section(".infra.ram.start"))) InfraStart(void) {
	NewVectorTable[2] = HalNMIHandler_Patch;
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
	VectorTableInitForOSRtl8195A(&vPortSVCHandler, &xPortPendSVHandler,	&xPortSysTickHandler);
	if (clk) SpicDisableRtl8195A();
	_AppStart();
}

//----- SDIO_Device_Off
void SDIO_Device_Off(void) {
	HAL_PERI_ON_WRITE32(REG_PESOC_HCI_CLK_CTRL0,
			HAL_PERI_ON_READ32(REG_PESOC_HCI_CLK_CTRL0) & (~BIT_SOC_ACTCK_SDIO_DEV_EN));
	HAL_PERI_ON_WRITE32(REG_SOC_HCI_COM_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_HCI_COM_FUNC_EN) & (~(BIT_SOC_HCI_SDIOD_ON_EN | BIT_SOC_HCI_SDIOD_OFF_EN)));
	HAL_PERI_ON_WRITE32(REG_HCI_PINMUX_CTRL,
			HAL_PERI_ON_READ32(REG_HCI_PINMUX_CTRL) & (~(BIT_HCI_SDIOD_PIN_EN)));
}

//----- __HalReInitPlatformLogUart
void __HalReInitPlatformLogUart(void) {
	LOG_UART_ADAPTER UartAdapter;
	UartAdapter.BaudRate = DEFAULT_BAUDRATE;
	HalLogUartInit(UartAdapter);
}

void _ReloadImg_user_define(void) {

}

//----- ReloadImg
void _ReloadImg(void) {
	u32 img1size;
	u32 img1addr;
	unsigned int i;
	u32 img2addr;
	u32 img_addr1;
	u32 img_addr2;
	u32 ota_addr;
	const char * pstr;

	img1size = *(u32 *) (SPI_FLASH_BASE + 0x10);
	img1addr = *(u32 *) (SPI_FLASH_BASE + 0x14);
	DBG_8195A("Image1 length: 0x%x, Image Addr: 0x%x\n", img1size,
			img1addr);
	for (i = 32; i < img1size + 32; i += 4)
		*(u32 *) (img1addr - 32 + i) = *(u32 *) (i + SPI_FLASH_BASE);
	img2addr = *(u16 *) (SPI_FLASH_BASE + 0x16) << 10;
	if (!(img2addr)) img2addr = img1size + 32;
	u32 * prdflash = (u32 *) (img1size + SPI_FLASH_BASE + 8);
	u32 sign1 = *prdflash++; // v4 = *(u32 *)(img2addr + SPI_FLASH_BASE + 8);
	u32 sign2 = *prdflash; // v5 = *(u32 *)(img2addr + SPI_FLASH_BASE + 12);
	if (sign1 == IMG_SIGN1_RUN) {
		if (sign2 == IMG_SIGN2_RUN) {
			img_addr1 = img2addr;
LABEL_11: 	img_addr2 = -1;
			goto LABEL_16;
		}
LABEL_14: img_addr1 = -1;
		goto LABEL_11;
	}
	if (sign1 != IMG_SIGN1_SWP || sign2 != IMG_SIGN2_RUN)	goto LABEL_14;
	img_addr2 = img2addr;
	img_addr1 = -1;
LABEL_16: ota_addr = *(u32 *) (SPI_FLASH_BASE + 0x9000);
	if (ota_addr == -1) {
LABEL_21: ota_addr = -1;
		goto LABEL_22;
	}
	if (ota_addr < (img2addr + *(u32 *) (img2addr + SPI_FLASH_BASE))
			|| (ota_addr & 0xFFF)) {
		DBG_MISC_ERR("OTA addr 0x%x INVALID\n");
		goto LABEL_21;
	}
	prdflash = (u32 *) (ota_addr + SPI_FLASH_BASE + 8);
	sign1 = *prdflash++; // v9 = *(u32 *)(ota_addr + SPI_FLASH_BASE + 8);
	sign2 = *prdflash; // v11 = *(u32 *)(ota_addr + SPI_FLASH_BASE + 12);
	if (sign1 == IMG_SIGN1_RUN) {
		sign1 = IMG_SIGN2_RUN;
		if (sign2 == IMG_SIGN2_RUN) {
			img_addr1 = ota_addr;
			goto LABEL_33;
		}
		goto LABEL_22;
	}
	if (sign1 != IMG_SIGN1_SWP || (sign1 = IMG_SIGN2_RUN, sign2 != IMG_SIGN2_RUN)) {
LABEL_22: 	if (img_addr1 == -1) {
			if (img_addr2 == -1) {
				DBG_MISC_ERR("Fatal:no fw\n", ota_addr,
						2 * ConfigDebugErr);
				RtlConsolRam();
			}
			img_addr1 = img_addr2;
LABEL_28: 	pstr = "load OLD fw %d\n";
			if (ConfigDebugErr & _DBG_MISC_) {
LABEL_36: 		DiagPrintf(pstr,
					((unsigned int) (img_addr1 - ota_addr) <= 0));
			}
			goto IMG2_LOAD_START;
		}
		goto LABEL_33;
	}
	if (img_addr1 == -1) {
		img_addr1 = *(u32 *) (SPI_FLASH_BASE + 0x9000); // ota_addr
		goto LABEL_28;
	}
	img_addr2 = *(u32 *) (SPI_FLASH_BASE + 0x9000);
LABEL_33: if (img_addr1 == img_addr2)
		goto LABEL_28;
	if (ConfigDebugErr & _DBG_MISC_) //  DBG_8195A
	{
		pstr = "load NEW fw %d\n";
		goto LABEL_36;
	}
	u32 v13;
IMG2_LOAD_START:
	v13 = *(u32 *) (img_addr1 + SPI_FLASH_BASE + 4);
	u32 v15 = *(u32 *) (img_addr1 + SPI_FLASH_BASE) + img_addr1;
	for (i = img_addr1 + 16;; i += 4) {
		if (i >= (unsigned int) (v15 + 16))	break;
		*(u32 *) (v13 - 16 - img_addr1 + i) = *(u32 *) (i + SPI_FLASH_BASE);
	}
	u32 v16 = *(u32 *) (v15 + SPI_FLASH_BASE);
	if ((unsigned int) (v16 - 1) <= 0xFFFFFFFD
			&& *(u32 *) (v15 + SPI_FLASH_BASE + 0x14) == 0x30000000) {
		DBG_8195A("Image3 length: 0x%x, Image3 Addr: 0x%x\n",
				*(u32 *)(v15 + SPI_FLASH_BASE + 0x10));
		for (i = v15 + 32; i < (v16 + v15 + 32); i += 4)
			*(u32 *) (0x2FFFFFE0 - v15 + i) = *(u32 *) (i + SPI_FLASH_BASE);
	} else
		DBG_8195A("No Image3\n");
	_ReloadImg_user_define();
}

//----- CPUResetHandler
void _CPUResetHandler(void) {
	memset(&__rom_bss_start__, 0, &__rom_bss_end__ - &__rom_bss_start__);

	ConfigDebugErr = -1;

	HalCpuClkConfig(0);
	VectorTableInitRtl8195A(STACK_TOP); // 0x1FFFFFFC
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & BIT23);
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & BIT_SOC_FLASH_EN);
	HAL_PERI_ON_WRITE32(REG_SOC_PERI_BD_FUNC0_EN,
			HAL_PERI_ON_READ32(REG_SOC_PERI_BD_FUNC0_EN) | BIT8 | BIT9);
	HalPinCtrlRtl8195A(SPI_FLASH, 0, 1);
	HalTimerOpInit_Patch(&HalTimerOp);
	HalDelayUs(1000);
	__HalReInitPlatformLogUart();
	_ReloadImg();
	InfraStart();
}
//----- CPUReset
void _CPUReset(void) // __noreturn
{
	wifi_off();
	pRamStartFun->RamPatchFun1 = _CPUResetHandler;
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & BIT27);
	HAL_WRITE32(0xE000ED00, 0x0C, 0x5FA0003);  //
	while (1);
}

void HalHardFaultHandler_user_define(u32 HardDefaultArg) {

}
#endif // !USE_SRC_ONLY_BOOT

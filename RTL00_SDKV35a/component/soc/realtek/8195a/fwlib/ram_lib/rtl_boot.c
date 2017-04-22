/* 
 *  BootLoader Ver 0.2
 *  Created on: 12/02/2017
 *      Author: pvvx
 */

#include "platform_autoconf.h"
#include "rtl_bios_data.h"
#include "diag.h"
#include "rtl8195a/rtl8195a_sys_on.h"

#include "hal_spi_flash.h"

//-------------------------------------------------------------------------
// Data declarations
#ifndef DEFAULT_BAUDRATE
#define DEFAULT_BAUDRATE 38400
#endif

/* 0 - 166666666 Hz, 1 - 83333333 Hz, 2 - 41666666 Hz, 3 - 20833333 Hz, 4 - 10416666 Hz, 5 - 4000000? Hz,
   6 - 200000000 Hz, 7 - 10000000 Hz, 8 - 50000000 Hz, 9 - 25000000 Hz, 10 - 12500000 Hz, 11 - 4000000? Hz */
#define DEFAULT_BOOT_CLK_CPU 1 // Warning! If Start CLK > 100 MHz -> Errors SPIC function in Ameba SDK!

#ifdef DEFAULT_BOOT_CLK_CPU
#if DEFAULT_BOOT_CLK_CPU < 6
#define DEFAULT_BOOT_CPU_CLOCK_SEL_DIV5_3		0
#define DEFAULT_BOOT_CPU_CLOCK_SEL_VALUE DEFAULT_BOOT_CLK_CPU
#else
#define DEFAULT_BOOT_CPU_CLOCK_SEL_DIV5_3		1
#define DEFAULT_BOOT_CPU_CLOCK_SEL_VALUE (DEFAULT_BOOT_CLK_CPU-6)
#endif
#endif // DEFAULT_BOOT_CLK_CPU

#define BOOT_RAM_TEXT_SECTION // __attribute__((section(".boot.text")))

//-------------------------------------------------------------------------
typedef struct _seg_header {
	uint32 size;
	uint32 ldaddr;
} IMGSEGHEAD, *PIMGSEGHEAD;

typedef struct _img2_header {
	IMGSEGHEAD seg;
	uint32 sign[2];
	void (*startfunc)(void);
	uint8 rtkwin[7];
	uint8 ver[13];
	uint8 name[32];
} IMG2HEAD, *PIMG2HEAD;

#ifndef FLASH_SECTOR_SIZE
#define FLASH_SECTOR_SIZE 		4096
#endif
//-------------------------------------------------------------------------
// Function declarations
LOCAL void RtlBootToFlash(void); // image1
LOCAL void RtlBoot1ToSram(void); // image1
LOCAL void RtlBoot2ToSram(void); // image1
LOCAL void RtlBoot3ToSram(void); // image1
LOCAL void RtlBoot4ToSram(void); // image1

extern _LONG_CALL_ VOID HalCpuClkConfig(unsigned char CpuType);
extern _LONG_CALL_ VOID VectorTableInitRtl8195A(u32 StackP);
extern _LONG_CALL_ VOID HalInitPlatformLogUartV02(VOID);
extern _LONG_CALL_ VOID HalInitPlatformTimerV02(VOID);
//extern _LONG_CALL_ VOID DramInit_rom(IN DRAM_DEVICE_INFO *DramInfo);
//extern _LONG_CALL_ u32 SdrCalibration_rom(VOID);
extern _LONG_CALL_ int SdrControllerInit_rom(PDRAM_DEVICE_INFO pDramInfo);
extern _LONG_CALL_ u32 SpicCmpDataForCalibrationRtl8195A(void); // compare read_data and golden_data
//extern _LONG_CALL_ VOID SpicWaitWipDoneRtl8195A(SPIC_INIT_PARA SpicInitPara);		// wait spi-flash status register[0] = 0
//extern _LONG_CALL_ VOID SpicLoadInitParaFromClockRtl8195A(u8 CpuClkMode, u8 BaudRate, PSPIC_INIT_PARA pSpicInitPara);
//extern _LONG_CALL_ VOID RtlConsolInit(IN u32 Boot, IN u32 TBLSz, IN VOID *pTBL);

//#pragma arm section code = ".boot.text";
//#pragma arm section rodata = ".boot.rodata", rwdata = ".boot.data", zidata = ".boot.bss";

typedef void (*START_FUNC)(void);

//-------------------------------------------------------------------------
/* Start table: */
START_RAM_FUN_SECTION RAM_FUNCTION_START_TABLE __ram_start_table_start__ = {
		RtlBootToFlash + 1,	// StartFun(),	Run if ( v400001F4 & 0x8000000 ) && ( v40000210 & 0x80000000 )
		RtlBoot1ToSram + 1,	// PatchWAKE(),	Run if ( v40000210 & 0x20000000 )
		RtlBoot2ToSram + 1,	// PatchFun0(), Run if ( v40000210 & 0x10000000 )
		RtlBoot3ToSram + 1,	// PatchFun1(), Run if ( v400001F4 & 0x8000000 ) && ( v40000210 & 0x8000000 )
		RtlBoot4ToSram + 1 };// PatchFun2(), Run for Init console, if ( v40000210 & 0x4000000 )
//-------------------------------------------------------------------------
/* Set Debug Flags */
LOCAL void BOOT_RAM_TEXT_SECTION SetDebugFlgs() {
#if CONFIG_DEBUG_LOG > 3
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

/* JTAG On */
LOCAL void BOOT_RAM_TEXT_SECTION JtagOn(void) {
	ACTCK_VENDOR_CCTRL(ON);
	SLPCK_VENDOR_CCTRL(ON);
	HalPinCtrlRtl8195A(JTAG, 0, 1);
}

/* GetChipId() */
LOCAL uint8 INFRA_START_SECTION _Get_ChipId() {
	uint8 ChipId = CHIP_ID_8710AF;
	if (HALEFUSEOneByteReadROM(HAL_SYS_CTRL_READ32(REG_SYS_EFUSE_CTRL), 0xF8,
			&ChipId, L25EOUTVOLTAGE) != 1)
		DBG_8195A("Get Chip ID Failed\r");
	return ChipId;
}

/*
 *  16 bytes FIFO ... 16*11/38400 = 0.004583 sec
 *  (0.005/5)*166666666 = 166666.666 Tcpu
 */
LOCAL void INFRA_START_SECTION loguart_wait_tx_fifo_empty(void) {
	if (HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & BIT_SOC_LOG_UART_EN) {
		int x = 16384;
		while ((!(HAL_READ8(LOG_UART_REG_BASE, 0x14) & BIT6)) && x--)
			; //	иначе глючит LogUART, если переключение CLK приходится на вывод символов !
	}
}

extern SPIC_INIT_PARA SpicInitParaAllClk[SpicMaxMode][CPU_CLK_TYPE_NO]; // 100021ec [144=0x90]
LOCAL uint32 InitTabParaAllClk[3 * CPU_CLK_TYPE_NO] = {
		// SIO
		0x01310102,	// 72t/byte
		0x03310101, // 0201 - 40t, 0101 - 39t, 0102 - 72t, 0103 - 104t
		0x05310001, // 39t
		0x07310001,
		0x09310001,
		0x0B310001,
		// DIO
		0x11311301,	// BaudRate = 1, RdDummyCyle = 19, DelayLine = 63, DIO
		0x13311201, // 1201 - 36t
		0x15311101, // 1101 - 35t
		0x17311101,
		0x19311101,
		0x1B311101
		// QIO
/* MXIC Flash only DIO!
		0x21311301,	// BaudRate = 1, RdDummyCyle = 19, DelayLine = 63, DIO
		0x23311201, // 1201 - 36t
		0x25311101, // 1101 - 35t
		0x27311101,
		0x29311101,
		0x2B311101
*/
};

struct spic_table_flash_type {
	uint8	cmd[12];
	uint8 	strlr2;
	uint8 	fbaud;
	uint8 	addrlen;
	uint8 	fsize;
	uint32	contrl;
	uint16 	validcmd[3];
	uint8	manufacturerid;
	uint8	memorytype;
};

//PSPIC_INIT_PARA pSpicInitPara;

struct spic_table_flash_type spic_table_flash = {
	{	// for FLASH MX25L8006E/1606E
	 	FLASH_CMD_FREAD,	// REG_SPIC_READ_FAST_SINGLE 		0x400060E0 0x0B
		FLASH_CMD_DREAD,	// REG_SPIC_READ_DUAL_DATA 			0x400060E4 0x3B
		FLASH_CMD_DREAD,   	// REG_SPIC_READ_DUAL_ADDR_DATA 	0x400060E8 0x3B ?
		FLASH_CMD_QREAD,   	// REG_SPIC_READ_QUAD_DATA 			0x400060EC 0x6B
		FLASH_CMD_4READ,	// REG_SPIC_READ_QUAD_ADDR_DATA 	0x400060F0 0xEB ?
		FLASH_CMD_PP,   	// REG_SPIC_WRITE_SIGNLE 			0x400060F4 0x02
		FLASH_CMD_DPP,   	// REG_SPIC_WRITE_DUAL_DATA 		0x400060F8 0xA2
		FLASH_CMD_DPP,		// REG_SPIC_WRITE_DUAL_ADDR_DATA 	0x400060FC 0xA2 ?
		FLASH_CMD_QPP,   	// REG_SPIC_WRITE_QUAD_DATA 		0x40006100 0x32
		FLASH_CMD_4PP,   	// REG_SPIC_WRITE_QUAD_ADDR_DATA 	0x40006104 0x38
		FLASH_CMD_WREN,   	// REG_SPIC_WRITE_ENABLE 			0x40006108 0x06
		FLASH_CMD_RDSR   	// REG_SPIC_READ_STATUS 			0x4000610C 0x05
	},
		BIT_FIFO_ENTRY(5) | BIT_SO_DUM,	// REG_SPIC_CTRLR2 		0x40006110 0x51
		BIT_FSCKDV(1),   				// REG_SPIC_FBAUDR 		0x40006114 0x01
		BIT_ADDR_PHASE_LENGTH(3),		// REG_SPIC_ADDR_LENGTH 0x40006118 0x03
		BIT_FLASE_SIZE(0x0F),			// REG_SPIC_FLASE_SIZE	0x40006124 0x0E ?
	    BIT_CS_H_WR_DUM_LEN(2)| BIT_AUTO_ADDR__LENGTH(3) | BIT_RD_DUMMY_LENGTH(0x0), // REG_SPIC_AUTO_LENGTH 0x4000611C 0x20030001 ?
	{
		BIT_WR_BLOCKING, 	// REG_SPIC_VALID_CMD					0x40006120 0x200 SpicOneBitMode
		BIT_WR_BLOCKING | BIT_RD_DUAL_I, 	// REG_SPIC_VALID_CMD	0x40006120 0x202 SpicDualBitMode
		BIT_WR_BLOCKING | BIT_RD_DUAL_I, 	// REG_SPIC_VALID_CMD	0x40006120 0x202 SpicDualBitMode
//		BIT_WR_BLOCKING | BIT_RD_QUAD_O, 	// REG_SPIC_VALID_CMD	0x40006120 0x208 SpicQuadBitMode MXIC not QIO
	},
		0xC2, 0x20 // MX25L8006/MX25L1606
};

LOCAL int BOOT_RAM_TEXT_SECTION SetSpicBitMode(uint8 BitMode) {
	PSPIC_INIT_PARA pspic = &SpicInitParaAllClk[BitMode][((HAL_SYS_CTRL_READ32(REG_SYS_CLK_CTRL1) >> 4) & 7)];
	if(pspic->Mode.Valid) {
	    // Disable SPI_FLASH User Mode
	    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
		HAL_SPI_WRITE32(REG_SPIC_VALID_CMD, spic_table_flash.validcmd[pspic->Mode.BitMode]);
		HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH, (HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH) & 0xFFFF0000) | pspic->RdDummyCyle);
		HAL_SPI_WRITE32(REG_SPIC_BAUDR, pspic->BaudRate);
		FLASH_DDL_FCTRL(pspic->DelayLine);	 		// SPI_DLY_CTRL_ADDR [7:0]
		// Enable SPI_FLASH  User Mode
	    HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);
	}
	SPI_FLASH_PIN_FCTRL(ON);
    // Test Read Pattern
    if(!SpicCmpDataForCalibrationRtl8195A()) {
    	FLASH_DDL_FCTRL(0x31);	 		// SPI_DLY_CTRL_ADDR [7:0]
    	for(uint8 BaudRate = 1; BaudRate < 4; BaudRate++) {
    		for(uint8 RdDummyCyle = 0; RdDummyCyle < 63; RdDummyCyle++) {
    			// Disable SPI_FLASH User Mode
    		    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
    			HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH, (HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH) & 0xFFFF0000) | RdDummyCyle);
    			HAL_SPI_WRITE32(REG_SPIC_BAUDR, BaudRate);
    		    // Enable SPI_FLASH  User Mode
    		    HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);
//    	    	HAL_SPI_WRITE32(REG_SPIC_FLUSH_FIFO, 1);
    	    	if(SpicCmpDataForCalibrationRtl8195A()) {
    	    		DiagPrintf("Spic reinit %d:%d\n", BaudRate, RdDummyCyle);
    	    		pspic->BaudRate = BaudRate;
    	    		pspic->RdDummyCyle = RdDummyCyle;
    	    		pspic->DelayLine = 0x31;
    	    		pspic->Mode.Valid = 1;
    	    	    return 1;
    	    	};
   	    	};
		};
	    return 0;
	};
    return 1;
}

void BOOT_RAM_TEXT_SECTION InitSpicFlashType(struct spic_table_flash_type *ptable_flash) {
	u8 * ptrb = &ptable_flash->cmd;
	volatile u32 * ptrreg = (volatile u32 *)(SPI_FLASH_CTRL_BASE + REG_SPIC_READ_FAST_SINGLE);// 0x400060E0
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0); // Disable SPI_FLASH User Mode
	do	{
		*ptrreg++ = *ptrb++;
	} while(ptrb < (u8 *)(&ptable_flash->fsize));
	ptrreg[0] = ptable_flash->contrl;
	ptrreg[1] = ptable_flash->validcmd[SpicOneBitMode];
	ptrreg[2] = ptable_flash->fsize;
    HAL_SPI_WRITE32(REG_SPIC_SER, BIT_SER);
}

LOCAL int BOOT_RAM_TEXT_SECTION InitSpic(uint8 SpicBitMode) {
	_memset(SpicInitParaAllClk, 0, sizeof(SpicInitParaAllClk));
	uint32 * ptr = InitTabParaAllClk;
	uint8 x;
	for(x = 0; x < SpicMaxMode; x++) {
		*(uint32 *)&SpicInitParaAllClk[SpicOneBitMode][x].BaudRate = ptr[0];
		*(uint32 *)&SpicInitParaAllClk[SpicDualBitMode][x].BaudRate = ptr[CPU_CLK_TYPE_NO];
		*(uint32 *)&SpicInitParaAllClk[SpicQuadBitMode][x].BaudRate = ptr[CPU_CLK_TYPE_NO]; //  MXIC not QIO
		ptr++;
	}
	ACTCK_FLASH_CCTRL(1);
    SLPCK_FLASH_CCTRL(1);
    HalPinCtrlRtl8195A(SPI_FLASH, 0, 1);
    InitSpicFlashType(&spic_table_flash);
    return SetSpicBitMode(SpicBitMode);
}


/* SYSPlatformInit */
LOCAL void INFRA_START_SECTION SYSPlatformInit(void) {
	__asm__ __volatile__ ("cpsid f\n");
	JtagOn();
	SetDebugFlgs();
	//----- SYS Init
	HAL_SYS_CTRL_WRITE32(REG_SYS_EFUSE_SYSCFG0,
			(HAL_SYS_CTRL_READ32(REG_SYS_EFUSE_SYSCFG0) & (~(BIT_MASK_SYS_EEROM_LDO_PAR_07_04 << BIT_SHIFT_SYS_EEROM_LDO_PAR_07_04))) | BIT_SYS_EEROM_LDO_PAR_07_04(6)); // & 0xF0FFFFFF | 0x6000000
	HAL_SYS_CTRL_WRITE32(REG_SYS_XTAL_CTRL1,
			(HAL_SYS_CTRL_READ32(REG_SYS_XTAL_CTRL1) & (~(BIT_MASK_SYS_XTAL_DRV_RF1 << BIT_SHIFT_SYS_XTAL_DRV_RF1))) | BIT_SYS_XTAL_DRV_RF1(1)); //  & 0xFFFFFFE7 | 8;
	//----- SDIO_Device_Off
	HAL_PERI_ON_WRITE32(REG_PESOC_HCI_CLK_CTRL0,
			HAL_PERI_ON_READ32(REG_PESOC_HCI_CLK_CTRL0) & (~BIT_SOC_ACTCK_SDIO_DEV_EN));
	HAL_PERI_ON_WRITE32(REG_SOC_HCI_COM_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_HCI_COM_FUNC_EN) & (~(BIT_SOC_HCI_SDIOD_ON_EN | BIT_SOC_HCI_SDIOD_OFF_EN)));
	HAL_PERI_ON_WRITE32(REG_HCI_PINMUX_CTRL,
			HAL_PERI_ON_READ32(REG_HCI_PINMUX_CTRL) & (~(BIT_HCI_SDIOD_PIN_EN)));
	//----- GPIO Adapter
	extern HAL_GPIO_ADAPTER gBoot_Gpio_Adapter;
	_memset(&gBoot_Gpio_Adapter, 0, sizeof(gBoot_Gpio_Adapter));
	_pHAL_Gpio_Adapter = &gBoot_Gpio_Adapter;
#ifdef DEFAULT_BOOT_CLK_CPU
	//----- CLK CPU
	loguart_wait_tx_fifo_empty(); //	иначе глючит LogUART, если переключение CLK приходится на вывод символов !
#if	DEFAULT_BOOT_CPU_CLOCK_SEL_DIV5_3
	// 6 - 200000000 Hz, 7 - 10000000 Hz, 8 - 50000000 Hz, 9 - 25000000 Hz, 10 - 12500000 Hz, 11 - 4000000 Hz
	HalCpuClkConfig(DEFAULT_BOOT_CPU_CLOCK_SEL_VALUE);
	*((int *)(SYSTEM_CTRL_BASE+REG_SYS_SYSPLL_CTRL1)) |= (1<<17);// REG_SYS_SYSPLL_CTRL1 |= BIT_SYS_SYSPLL_DIV5_3
#else
	// 0 - 166666666 Hz, 1 - 83333333 Hz, 2 - 41666666 Hz, 3 - 20833333 Hz, 4 - 10416666 Hz, 5 - 4000000 Hz
	*((int *) (SYSTEM_CTRL_BASE + REG_SYS_SYSPLL_CTRL1)) &= ~(1 << 17); // REG_SYS_SYSPLL_CTRL1 &= ~BIT_SYS_SYSPLL_DIV5_3
	HalCpuClkConfig(DEFAULT_BOOT_CPU_CLOCK_SEL_VALUE);
#endif // CPU_CLOCK_SEL_DIV5_3
#endif // DEFAULT_CLK_CPU
	//----- System
	VectorTableInitRtl8195A(STACK_TOP);	// 0x1FFFFFFC
	HalInitPlatformLogUartV02(); // Show "<RTL8195A>"... :(
	HalInitPlatformTimerV02();
	__asm__ __volatile__ ("cpsie f\n");
}

/*-------------------------------------------------------------------------------------
 Копирует данные из области align(4) (flash, registers, ...) в область align(1) (ram)
 --------------------------------------------------------------------------------------*/
LOCAL unsigned int BOOT_RAM_TEXT_SECTION flashcpy(unsigned int faddr,
		void *dist, unsigned int size) {
	union {
		unsigned char uc[4];
		unsigned int ud;
	} tmp;
	if (faddr < SPI_FLASH_BASE)
		faddr += SPI_FLASH_BASE;
	unsigned char * pd = (unsigned char *) dist;
	unsigned int *p = (unsigned int *) ((unsigned int) faddr & (~3));
	unsigned int xlen = (unsigned int) faddr & 3;
	unsigned int len = size;

	if (xlen) {
		tmp.ud = *p++;
		while (len) {
			len--;
			*pd++ = tmp.uc[xlen++];
			if (xlen & 4)
				break;
		};
	};
	xlen = len >> 2;
	while (xlen) {
		tmp.ud = *p++;
		*pd++ = tmp.uc[0];
		*pd++ = tmp.uc[1];
		*pd++ = tmp.uc[2];
		*pd++ = tmp.uc[3];
		xlen--;
	};
	if (len & 3) {
		tmp.ud = *p;
		pd[0] = tmp.uc[0];
		if (len & 2) {
			pd[1] = tmp.uc[1];
			if (len & 1) {
				pd[2] = tmp.uc[2];
			};
		};
	};
	return size;
}

enum {
	SEG_ID_ERR,
	SEG_ID_SRAM,
	SEG_ID_TCM,
	SEG_ID_SDRAM,
	SEG_ID_SOC,
	SEG_ID_FLASH,
	SEG_ID_CPU,
	SEG_ID_ROM,
	SEG_ID_MAX
} SEG_ID;

LOCAL const char * const txt_tab_seg[] = {
		"UNK",		// 0
		"SRAM",		// 1
		"TCM",		// 2
		"SDRAM",	// 3
		"SOC",		// 4
		"FLASH",	// 5
		"CPU",		// 6
		"ROM"		// 7
		};

LOCAL const uint32 tab_seg_def[] = { 0x10000000, 0x10070000, 0x1fff0000,
		0x20000000, 0x30000000, 0x30200000, 0x40000000, 0x40800000, 0x98000000,
		0xA0000000, 0xE0000000, 0xE0010000, 0x00000000, 0x00050000 };

LOCAL uint32 BOOT_RAM_TEXT_SECTION get_seg_id(uint32 addr, int32 size) {
	uint32 ret = SEG_ID_ERR;
	uint32 * ptr = &tab_seg_def;
	if (size > 0) {
		do {
			ret++;
			if (addr >= ptr[0] && addr + size <= ptr[1]) {
				return ret;
			};
			ptr += 2;
		} while (ret < SEG_ID_MAX);
	};
	return 0;
}

LOCAL uint32 BOOT_RAM_TEXT_SECTION load_img2_head(uint32 faddr, PIMG2HEAD hdr) {
	flashcpy(faddr, hdr, sizeof(IMG2HEAD));
	uint32 ret = get_seg_id(hdr->seg.ldaddr, hdr->seg.size);
	if (hdr->sign[1] == IMG_SIGN2_RUN) {
		if (hdr->sign[0] == IMG_SIGN1_RUN) {
			ret |= 1 << 9;
		} else if (hdr->sign[0] == IMG_SIGN1_SWP) {
			ret |= 1 << 8;
		};
	}
	if (*(u32 *) (&hdr->rtkwin) == IMG2_SIGN_DW1_TXT) {
		ret |= 1 << 10;
	};
	return ret;
}

LOCAL uint32 BOOT_RAM_TEXT_SECTION load_segs(uint32 faddr, PIMG2HEAD hdr,
		uint8 flgload) {
	uint32 fnextaddr = faddr;
	uint8 segnum = 0;
	while (1) {
		uint32 seg_id = get_seg_id(hdr->seg.ldaddr, hdr->seg.size);
		if (flgload
				&& (seg_id == SEG_ID_SRAM || seg_id == SEG_ID_TCM
						|| seg_id == SEG_ID_SDRAM)) {
#if CONFIG_DEBUG_LOG > 1
			DBG_8195A("Load Flash seg%d: 0x%08x -> %s: 0x%08x, size: %d\n",
					segnum, faddr, txt_tab_seg[seg_id], hdr->seg.ldaddr,
					hdr->seg.size);
#endif
			fnextaddr += flashcpy(fnextaddr, hdr->seg.ldaddr, hdr->seg.size);
		} else if (seg_id) {
#if CONFIG_DEBUG_LOG > 2
			DBG_8195A("Skip Flash seg%d: 0x%08x -> %s: 0x%08x, size: %d\n", segnum,
					faddr, txt_tab_seg[seg_id], hdr->seg.ldaddr, hdr->seg.size);
#endif
			fnextaddr += hdr->seg.size;
		} else {
			break;
		}
		fnextaddr += flashcpy(fnextaddr, &hdr->seg, sizeof(IMGSEGHEAD));
		segnum++;
	}
	return fnextaddr;
}

/*-------------------------------------------------------------------------------------
 * 0 - default image (config data + 0), 1 - image N1, 2 - image N2, ...
 --------------------------------------------------------------------------------------*/
LOCAL int BOOT_RAM_TEXT_SECTION loadUserImges(int imgnum) {
	IMG2HEAD hdr;
	int imagenum = 1;
	uint32 faddr = 0xb000; // start image2 in flash
	DBG_8195A("Selected Image %d.\n", imgnum);

	while (1) {
		faddr = (faddr + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1));
		uint32 img_id = load_img2_head(faddr, &hdr);
		if ((img_id >> 8) > 4 || (uint8) img_id != 0) {
			faddr = load_segs(faddr + 0x10, &hdr.seg, imagenum == imgnum);
			if (imagenum == imgnum) {
//				DBG_8195A("Image%d: %s\n", imgnum, hdr.name);
				break;
			}
			imagenum++;
		} else if (imagenum) {
			DBG_8195A("No Image%d! Trying Image0...\n", imgnum);
			// пробуем загрузить image по умолчанию, по записи в секторе установок
			flashcpy(FLASH_SYSTEM_DATA_ADDR, &faddr, sizeof(faddr));
			if (faddr < 0x8000000)
				faddr += SPI_FLASH_BASE;
			if (get_seg_id(faddr, 0x100) == SEG_ID_FLASH) {
				imagenum = 0;
				imgnum = 0;
			} else {
				DBG_8195A("No Image0!\n");
				imagenum = -1;
				break;
			};
		} else {
			imagenum = -1;
			break;
		}
	};
	return imagenum;
}

//----- IsForceLoadDefaultImg2
LOCAL uint8 BOOT_RAM_TEXT_SECTION IsForceLoadDefaultImg2(void) {
	uint8 gpio_pin[4];
	HAL_GPIO_PIN GPIO_Pin;
	HAL_GPIO_PIN_STATE flg;
	int result = 0;
	flashcpy(FLASH_SYSTEM_DATA_ADDR + 0x08, &gpio_pin, sizeof(gpio_pin)); // config data + 8
//	_pHAL_Gpio_Adapter = &gBoot_Gpio_Adapter;
	for (int i = 1; i; i--) {
		uint8 x = gpio_pin[i];
		result <<= 1;
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
			if (HAL_GPIO_ReadPin_8195a(&GPIO_Pin) == flg) {
				result |= 1;
			}
			HAL_GPIO_DeInit_8195a(&GPIO_Pin);
		}
	}
//	_pHAL_Gpio_Adapter->IrqHandle.IrqFun = NULL;
	return result;
}

/* RTL Console ROM */
LOCAL void BOOT_RAM_TEXT_SECTION RtlConsolRam(void) {
//	DiagPrintf("\r\nRTL Console ROM\r\n");
//	RtlConsolInit(ROM_STAGE, (u32) 6, (void*) &UartLogRomCmdTable);
	pUartLogCtl->RevdNo = UART_LOG_HISTORY_LEN;
	pUartLogCtl->BootRdy = 1;
	pUartLogCtl->pTmpLogBuf->UARTLogBuf[0] = '?';
	pUartLogCtl->pTmpLogBuf->BufCount = 1;
	pUartLogCtl->ExecuteCmd = 1;
	RtlConsolTaskRom(pUartLogCtl);
}

/* Enter Image 1.5 */
LOCAL void BOOT_RAM_TEXT_SECTION EnterImage15(int flg) {

	if (flg)
		_memset(&__rom_bss_start__, 0, &__rom_bss_end__ - &__rom_bss_start__);

	SYSPlatformInit();

	if (!flg)
		DBG_8195A("\r===== Enter FLASH-Boot ====\n");
	else
		DBG_8195A("\r===== Enter SRAM-Boot %d ====\n", flg);
#if CONFIG_DEBUG_LOG > 1
	DBG_8195A("CPU CLK: %d Hz, SOC FUNC EN: %p\r\n", HalGetCpuClk(),
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN));
#endif
	uint8 ChipId = _Get_ChipId();
	if (ChipId < CHIP_ID_8195AM) {
		//----- SDRAM Off
		SDR_PIN_FCTRL(OFF);
		LDO25M_CTRL(OFF);
		HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT(21)); // Flag SDRAM Off
	} else {
		//----- SDRAM On
		LDO25M_CTRL(ON);
		HAL_SYS_CTRL_WRITE32(REG_SYS_REGU_CTRL0,
				(HAL_SYS_CTRL_READ32(REG_SYS_REGU_CTRL0) & 0xfffff) | BIT_SYS_REGU_LDO25M_ADJ(0x0e));
		SDR_PIN_FCTRL(ON);
	};
	if (!InitSpic(SpicDualBitMode)) {
		DBG_8195A("Spic Init Error!\n");
		RtlConsolRam();
	};
	if ((HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & BIT(21)) == 0) { // уже загружена?
//		extern DRAM_DEVICE_INFO SdrDramInfo_rom;  // 50 MHz
		if (!SdrControllerInit_rom(&SdrDramInfo)) { // 100 MHz
			DBG_8195A("SDR Controller Init fail!\n");
			RtlConsolRam();
		}
#if 0 // Test SDRAM
		else {
			uint32 *ptr = SDR_SDRAM_BASE;
			uint32 tt = 0x55AA55AA;
			for (int i = 0; i < 512 * 1024; i++) {
				ptr[i] = tt++;
			};
			tt = 0x55AA55AA;
			for (int i = 0; i < 512 * 1024; i++) {
				if (ptr[i] != tt) {
					DBG_8195A("SDR err %p %p != %p!\n", &ptr[i], ptr[i], tt);
					RtlConsolRam();
				}
				tt++;
			};
			DBG_8195A("SDR tst end\n");
		};
#endif // test
		HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT(21));
	};

	if (!flg)
		loadUserImges(IsForceLoadDefaultImg2() + 1);

	if (_strcmp((const char *) &__image2_validate_code__, IMG2_SIGN_TXT)) {
		DBG_8195A("Invalid Image Signature!\n");
		RtlConsolRam();
	}

	DBG_8195A("Img Sign: %s, Go @ 0x%08x\r\n", &__image2_validate_code__,
			__image2_entry_func__);
	__image2_entry_func__();
}

/* RtlBootToSram */
LOCAL void BOOT_RAM_TEXT_SECTION RtlBoot1ToSram(void) {
	EnterImage15(1);
}
/* RtlBootToSram */
LOCAL void BOOT_RAM_TEXT_SECTION RtlBoot2ToSram(void) {
	EnterImage15(2);
}
/* RtlBootToSram */
LOCAL void BOOT_RAM_TEXT_SECTION RtlBoot3ToSram(void) {
	EnterImage15(3);
}
/* RtlBootToSram */
LOCAL void BOOT_RAM_TEXT_SECTION RtlBoot4ToSram(void) {
	EnterImage15(4);
}

LOCAL void BOOT_RAM_TEXT_SECTION RtlBootToFlash(void) {
	EnterImage15(0);
}

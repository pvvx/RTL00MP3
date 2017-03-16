/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

#include "objects.h"
#include "PinNames.h"

#include "pinmap.h"

#include "rtl8195a.h"
#include "hal_spi_flash.h"
#include "hal_platform.h"
#include "rtl8195a_spi_flash.h"
#include "hal_api.h"
#include "flash_api.h"

extern u32 ConfigDebugInfo;
extern SPIC_INIT_PARA SpicInitParaAllClk[SpicMaxMode][CPU_CLK_TYPE_NO]; // SpicMaxMode = 3, CPU_CLK_TYPE_NO = 6 !

_LONG_CALL_
extern VOID SpicWaitBusyDoneRtl8195A(VOID);

bool fspic_isinit = 0;
flash_t flashobj;

/**
 * global data structure
 */
//flash_t	        flash;
/**
 * @brief  Control the flash chip write protect enable/disable
 * @param  protect: 1/0: protect/unprotect
 * @retval none
 */
void flash_write_protect(flash_t *obj, uint32_t protect) {
	flash_turnon();

	if (fspic_isinit == 0)
		flash_init(&flashobj);
	SpicWriteProtectFlashRtl8195A(protect);
	SpicDisableRtl8195A();
}

/**
 * @brief  Init Flash
 * @param  obj: address of the flash object
 * @retval   none
 */
void flash_init(flash_t *obj) {
	//SPIC_INIT_PARA spic_init_para;

	// Init SPI Flash Controller
#if CONFIG_DEBUG_LOG > 4
	DBG_8195A("Initial Spi Flash Controller\n");
#endif
	u8 flmode;
	SPI_FLASH_PIN_FCTRL(ON);
#if 0
	if (SpicFlashInitRtl8195A(SpicQuadBitMode)) {
		flmode = SpicQuadBitMode;
		fspic_isinit = 1;
	}
	else
#endif
	if (SpicFlashInitRtl8195A(SpicDualBitMode)) { // SpicFlashInitRtl8195A(SpicQuadBitMode)
		flmode = SpicDualBitMode;
		fspic_isinit = 1;
	} else if (SpicFlashInitRtl8195A(SpicOneBitMode)) {
		flmode = SpicOneBitMode;
		fspic_isinit = 1;
	} else {
		DBG_8195A("SPI Init Fail!\n"); // DBG_SPIF_ERR?
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSTBY_INFO3,
				HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSTBY_INFO3) | 0xf);
//		flmode = SpicOneBitMode;
		return;
	}
	u8 curclk = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1)
			>> BIT_SHIFT_PESOC_OCP_CPU_CK_SEL) & BIT_MASK_PESOC_OCP_CPU_CK_SEL;
	flashobj.SpicInitPara = SpicInitParaAllClk[flmode][curclk];
	flashobj.SpicInitPara.Mode.CpuClk = curclk;
	flashobj.SpicInitPara.Mode.BitMode = flmode;

#if CONFIG_DEBUG_LOG > 3
	DBG_SPIF_INFO("Flash ID: %02x%02x%02x\n", flashobj.SpicInitPara.id[0], flashobj.SpicInitPara.id[1], flashobj.SpicInitPara.id[2]);
	DBG_SPIF_INFO("Flash Mode: 0x%02x; BaudRate:%d; RdDummyCyle:%d; DelayLine:%d\n",
			flashobj.SpicInitPara.Mode.BitMode, flashobj.SpicInitPara.BaudRate, flashobj.SpicInitPara.RdDummyCyle, flashobj.SpicInitPara.DelayLine);
	DBG_SPIF_INFO("Flash Size: %d bytes\n", flash_get_size(obj));
#endif
}

void flash_turnon(void) {
	SPI_FLASH_PIN_FCTRL(ON);
	SpicWaitBusyDoneRtl8195A();
}

/**
 * @brief  Erase flash sector
 * @param  address: Specifies the starting address to be erased.
 * @retval   none
 */
void flash_erase_sector(flash_t *obj, uint32_t address) {
	flash_turnon();

	if (fspic_isinit == 0)
		flash_init(&flashobj);

	SpicSectorEraseFlashRtl8195A(SPI_FLASH_BASE + address);
	SpicDisableRtl8195A();
}

void flash_erase_block(flash_t *obj, uint32_t address) {
	flash_turnon();

	if (fspic_isinit == 0)
		flash_init(&flashobj);

	SpicBlockEraseFlashRtl8195A(SPI_FLASH_BASE + address);
	SpicDisableRtl8195A();
}

/**
 * @brief  Read a word from specified address
 * @param  obj: Specifies the parameter of flash object.
 * @param  address: Specifies the address to be read.
 * @param  data: Specified the address to save the readback data.
 * @retval   status: Success:1 or Failure: Others.
 */
int flash_read_word(flash_t *obj, uint32_t address, uint32_t * data) {

	flash_turnon();
	if (fspic_isinit == 0)
		flash_init(&flashobj);
	// Wait flash busy done (wip=0)
	SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);

	*data = HAL_READ32(SPI_FLASH_BASE, address);
	SpicDisableRtl8195A();

	return 1;
}

/**
 * @brief  Write a word to specified address
 * @param  obj: Specifies the parameter of flash object.
 * @param  address: Specifies the address to be programmed.
 * @param  data: Specified the data to be programmed.
 * @retval   status: Success:1 or Failure: Others.
 */
int flash_write_word(flash_t *obj, uint32_t address, uint32_t data) {
	u8 flashtype = 0;

	flash_turnon();
	if (fspic_isinit == 0)
		flash_init(&flashobj);

	flashtype = flashobj.SpicInitPara.flashtype;

	//Write word
	HAL_WRITE32(SPI_FLASH_BASE, address, data);

	// Wait spic busy done
	SpicWaitBusyDoneRtl8195A();

	// Wait flash busy done (wip=0)
	if (flashtype == FLASH_MICRON) {
		SpicWaitOperationDoneRtl8195A(flashobj.SpicInitPara);
	} else
		SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);

	SpicDisableRtl8195A();
	return 1;
}

/**
 * @brief  Read a stream of data from specified address
 * @param  obj: Specifies the parameter of flash object.
 * @param  address: Specifies the address to be read.
 * @param  len: Specifies the length of the data to read.
 * @param  data: Specified the address to save the readback data.
 * @retval   status: Success:1 or Failure: Others.
 */
int flash_stream_read(flash_t *obj, uint32_t address, uint32_t len,
		uint8_t * data) {
	u32 offset_to_align;
	u32 i;
	u32 read_word;
	uint8_t *ptr;
	uint8_t *pbuf;

	flash_turnon();

	if (fspic_isinit == 0)
		flash_init(&flashobj);

	// Wait flash busy done (wip=0)
	SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);

	offset_to_align = address & 0x03;
	pbuf = data;
	if (offset_to_align != 0) {
		// the start address is not 4-bytes aligned
		read_word = HAL_READ32(SPI_FLASH_BASE, (address - offset_to_align));
		ptr = (uint8_t*) &read_word + offset_to_align;
		offset_to_align = 4 - offset_to_align;
		for (i = 0; i < offset_to_align; i++) {
			*pbuf = *(ptr + i);
			pbuf++;
			len--;
			if (len == 0) {
				break;
			}
		}
	}

	address = (((address - 1) >> 2) + 1) << 2; // address = next 4-bytes aligned

	ptr = (uint8_t*) &read_word;
	if ((u32) pbuf & 0x03) {
		while (len >= 4) {
			read_word = HAL_READ32(SPI_FLASH_BASE, address);
			for (i = 0; i < 4; i++) {
				*pbuf = *(ptr + i);
				pbuf++;
			}
			address += 4;
			len -= 4;
		}
	} else {
		while (len >= 4) {
			*((u32 *) pbuf) = HAL_READ32(SPI_FLASH_BASE, address);
			pbuf += 4;
			address += 4;
			len -= 4;
		}
	}

	if (len > 0) {
		read_word = HAL_READ32(SPI_FLASH_BASE, address);
		for (i = 0; i < len; i++) {
			*pbuf = *(ptr + i);
			pbuf++;
		}
	}

	SpicDisableRtl8195A();
	return 1;
}

/**
 * @brief  Write a stream of data to specified address
 * @param  obj: Specifies the parameter of flash object.
 * @param  address: Specifies the address to be read.
 * @param  len: Specifies the length of the data to write.
 * @param  data: Specified the pointer of the data to be written.
 * @retval   status: Success:1 or Failure: Others.
 */
int flash_stream_write(flash_t *obj, uint32_t address, uint32_t len,
		uint8_t * data) {
	u32 offset_to_align;
	u32 align_addr;
	u32 i;
	u32 write_word;
	uint8_t *ptr;
	uint8_t *pbuf;
	u8 flashtype = 0;

	flash_turnon();

	if (fspic_isinit == 0)
		flash_init(&flashobj);

	flashtype = flashobj.SpicInitPara.flashtype;
	offset_to_align = address & 0x03;
	pbuf = data;
	if (offset_to_align != 0) {
		// the start address is not 4-bytes aligned
		align_addr = (address - offset_to_align);
		write_word = HAL_READ32(SPI_FLASH_BASE, align_addr);
		ptr = (uint8_t*) &write_word + offset_to_align;
		offset_to_align = 4 - offset_to_align;
		for (i = 0; i < offset_to_align; i++) {
			*(ptr + i) = *pbuf;
			pbuf++;
			len--;
			if (len == 0) {
				break;
			}
		}
		//Write word
		HAL_WRITE32(SPI_FLASH_BASE, align_addr, write_word);
		// Wait spic busy done
		SpicWaitBusyDoneRtl8195A();
		// Wait flash busy done (wip=0)
		if (flashtype == FLASH_MICRON) {
			SpicWaitOperationDoneRtl8195A(flashobj.SpicInitPara);
		} else
			SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);
	}

	address = (((address - 1) >> 2) + 1) << 2; // address = next 4-bytes aligned

	if ((u32) pbuf & 0x03) {
		while (len >= 4) {
			write_word = (u32) (*pbuf) | ((u32) (*(pbuf + 1)) << 8)
					| ((u32) (*(pbuf + 2)) << 16) | ((u32) (*(pbuf + 3)) << 24);
			//Write word
			HAL_WRITE32(SPI_FLASH_BASE, address, write_word);
			// Wait spic busy done
			SpicWaitBusyDoneRtl8195A();
			// Wait flash busy done (wip=0)
			if (flashtype == FLASH_MICRON) {
				SpicWaitOperationDoneRtl8195A(flashobj.SpicInitPara);
			} else
				SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);

			pbuf += 4;
			address += 4;
			len -= 4;
		}
	} else {
		while (len >= 4) {
			//Write word
			HAL_WRITE32(SPI_FLASH_BASE, address, (u32)*((u32 *)pbuf));
			// Wait spic busy done
			SpicWaitBusyDoneRtl8195A();
			// Wait flash busy done (wip=0)
			if (flashtype == FLASH_MICRON) {
				SpicWaitOperationDoneRtl8195A(flashobj.SpicInitPara);
			} else
				SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);

			pbuf += 4;
			address += 4;
			len -= 4;
		}
	}

	if (len > 0) {
		write_word = HAL_READ32(SPI_FLASH_BASE, address);
		ptr = (uint8_t*) &write_word;
		for (i = 0; i < len; i++) {
			*(ptr + i) = *pbuf;
			pbuf++;
		}
		//Write word
		HAL_WRITE32(SPI_FLASH_BASE, address, write_word);
		// Wait spic busy done
		SpicWaitBusyDoneRtl8195A();
		// Wait flash busy done (wip=0)
		if (flashtype == FLASH_MICRON) {
			SpicWaitOperationDoneRtl8195A(flashobj.SpicInitPara);
		} else
			SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);

	}

	SpicDisableRtl8195A();
	return 1;
}

/*
 Function Description:
 This function performans the same functionality as the function flash_stream_write.
 It enhances write performance by reducing overheads.
 Users can use either of functions depending on their needs.

 * @brief  Write a stream of data to specified address
 * @param  obj: Specifies the parameter of flash object.
 * @param  address: Specifies the address to be read.
 * @param  Length: Specifies the length of the data to write.
 * @param  data: Specified the pointer of the data to be written.
 * @retval   status: Success:1 or Failure: Others.

 */

int flash_burst_write(flash_t *obj, uint32_t address, uint32_t Length,
		uint8_t * data) {

	u32 OccuSize;
	u32 ProgramSize;
	u32 PageSize;
	u8 flashtype = 0;

	PageSize = 256;

	flash_turnon();

	if (fspic_isinit == 0)
		flash_init(&flashobj);

	flashtype = flashobj.SpicInitPara.flashtype;

	OccuSize = address & 0xFF;
	if ((Length >= PageSize) || ((Length + OccuSize) >= PageSize))
		ProgramSize = PageSize - OccuSize;
	else
		ProgramSize = Length;

	flashobj.Length = Length;
	while (Length > 0) {
		if (OccuSize) {
			SpicUserProgramRtl8195A(data, flashobj.SpicInitPara, address,
					&(flashobj.Length));
			// Wait spic busy done
			SpicWaitBusyDoneRtl8195A();
			// Wait flash busy done (wip=0)
			if (flashtype == FLASH_MICRON) {
				SpicWaitOperationDoneRtl8195A(flashobj.SpicInitPara);
			} else
				SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);

			address += ProgramSize;
			data += ProgramSize;
			Length -= ProgramSize;
			OccuSize = 0;
		} else {
			while ((flashobj.Length) >= PageSize) {
				SpicUserProgramRtl8195A(data, flashobj.SpicInitPara, address,
						&(flashobj.Length));
				// Wait spic busy done
				SpicWaitBusyDoneRtl8195A();
				// Wait flash busy done (wip=0)
				if (flashtype == FLASH_MICRON) {
					SpicWaitOperationDoneRtl8195A(flashobj.SpicInitPara);
				} else
					SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);

				address += PageSize;
				data += PageSize;
				Length -= PageSize;
			}
			flashobj.Length = Length;
			if ((flashobj.Length) > 0) {
				SpicUserProgramRtl8195A(data, flashobj.SpicInitPara, address,
						&(flashobj.Length));
				// Wait spic busy done
				SpicWaitBusyDoneRtl8195A();
				// Wait flash busy done (wip=0)
				if (flashtype == FLASH_MICRON) {
					SpicWaitOperationDoneRtl8195A(flashobj.SpicInitPara);
				} else
					SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);

				break;
			}
		}
		flashobj.Length = Length;
	}

	SpicDisableRtl8195A();
	return 1;

}
/**
 * @brief  Read a stream of data from specified address
 * @param  obj: Specifies the parameter of flash object.
 * @param  address: Specifies the address to be read.
 * @param  len: Specifies the length of the data to read.
 * @param  data: Specified the address to save the readback data.
 * @retval   status: Success:1 or Failure: Others.
 */

int flash_burst_read(flash_t *obj, uint32_t address, uint32_t Length,
		uint8_t * data) {
	flash_turnon();

	if (fspic_isinit == 0)
		flash_init(&flashobj);

	// Wait flash busy done (wip=0)
	SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);
	SpicUserReadRtl8195A(Length, address, data,
			flashobj.SpicInitPara.Mode.BitMode);
	SpicDisableRtl8195A();
	return 1;
}

int flash_get_status(flash_t *obj) {
	u8 Status = 0;

	flash_turnon();

	if (fspic_isinit == 0)
		flash_init(&flashobj);

	Status = SpicGetFlashStatusRefinedRtl8195A(flashobj.SpicInitPara);

	SpicDisableRtl8195A();
	return Status;
}

/*
 Function Description:
 Please refer to the datatsheet of flash for more details of the content of status register.
 The block protected area and the corresponding control bits are provided in the flash datasheet.

 * @brief  Set Status register to enable desired operation
 * @param  obj: Specifies the parameter of flash object.
 * @param  data: Specifies which bit users like to set
 ex: if users want to set the third bit, data = 0x8.

 */
int flash_set_status(flash_t *obj, uint32_t data) {
	flash_turnon();

	if (fspic_isinit == 0)
		flash_init(&flashobj);

	SpicSetFlashStatusRefinedRtl8195A(data, flashobj.SpicInitPara);
	SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);
	DBG_8195A("Status Register After Setting= %x\n",
			flash_get_status(&flashobj));
	SpicDisableRtl8195A();
	return 1;
}

/*
 Function Description:
 This function aims to reset the status register, please make sure the operation is appropriate.
 */
void flash_reset_status(flash_t *obj) {
	flash_turnon();

	if (fspic_isinit == 0)
		flash_init(&flashobj);

	SpicSetFlashStatusRefinedRtl8195A(0, flashobj.SpicInitPara);
	SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);
	SpicDisableRtl8195A();
}
/*
 Function Description:
 This function is only for Micron 512Mbit flash to access beyond 128Mbit by switching between four 128 Mbit area.
 Please refer to flash datasheet for more information about memory mapping.
 */

int flash_set_extend_addr(flash_t *obj, uint32_t data) {
	flash_turnon();

	if (fspic_isinit == 0)
		flash_init(&flashobj);

	SpicSetExtendAddrRtl8195A(data, flashobj.SpicInitPara);
	SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);
	DBG_8195A("Extended Address Register After Setting = %x\n",
			flash_get_extend_addr(&flashobj));
	SpicDisableRtl8195A();
	return 1;
}

int flash_get_extend_addr(flash_t *obj) {
	u8 Status = 0;

	flash_turnon();
	if (fspic_isinit == 0)
		flash_init(&flashobj);
	Status = SpicGetExtendAddrRtl8195A(flashobj.SpicInitPara);

	SpicDisableRtl8195A();
	return Status;

}

/*
 * Return Flash size in bytes
 * = 0 - unknown flash
 */
unsigned int flash_get_size(flash_t *obj) {
	unsigned int flashchip_size = 0;
	if (fspic_isinit == 0) {
		flash_turnon();
		flash_init(&flashobj);
		SpicDisableRtl8195A();
		if (fspic_isinit == 0)
			return flashchip_size;
	}
	if (flashobj.SpicInitPara.id[2] > 0x11
			&& flashobj.SpicInitPara.id[2] < 0x20) {
		flashchip_size = 1 << (flashobj.SpicInitPara.id[2]); // Flash size in bytes
	} else
		flashchip_size = 1024 * 1024; // default 1 mbytes?
	return flashchip_size;
}

/*
 * Read Flash OTP data
 */
int flash_otp_read(flash_t *obj, uint32_t address, uint32_t Length,
		uint8_t * data) {
	int ret = 1;
	flash_turnon();
	if (fspic_isinit == 0)
		flash_init(&flashobj);
	SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);
	switch (flashobj.SpicInitPara.flashtype) {
	case FLASH_MXIC_4IO:
	case FLASH_MXIC: // Only 512 bits
#if	CONFIG_DEBUG_LOG > 4
		DBG_SPIF_INFO("MXIC: Only 512 bits!\n");
#endif
		SpicTxCmdWithDataRtl8195A(FLASH_CMD_ENSO, 0, 0, flashobj.SpicInitPara); // enter secured OTP
		SpicUserReadRtl8195A(Length, address, data,
				flashobj.SpicInitPara.Mode.BitMode);
		SpicTxCmdWithDataRtl8195A(FLASH_CMD_EXSO, 0, 0, flashobj.SpicInitPara); // exit secured OTP
		break;
	case FLASH_EON: //  OTP sector is mapping to sector 1023
#if	CONFIG_DEBUG_LOG > 4
		DBG_SPIF_INFO("EON: OTP sector is mapping to sector 1023!\n");
#endif
		SpicTxCmdWithDataRtl8195A(FLASH_CMD_EOTPM, 0, 0, flashobj.SpicInitPara); // enter secured OTP
		SpicUserReadRtl8195A(Length, address, data,
				flashobj.SpicInitPara.Mode.BitMode);
		SpicTxCmdWithDataRtl8195A(FLASH_CMD_WRDI, 0, 0, flashobj.SpicInitPara); // exit secured OTP
		break;
	case FLASH_MICRON: // (4Bh) READ OTP ARRAY
#if	CONFIG_DEBUG_LOG > 4
		DBG_SPIF_INFO("MICRON: @TODO !\n");
#endif
		// FLASH_CMD_ROTPA
		ret = 0;
		break;
	default:
		DBG_8195A("Flash type?");
		ret = 0;
	}
	SpicDisableRtl8195A();
	return ret;
}

/* 
 * sdio_host.c (disasm sdio_host.o)
 *
 * RTL8710/11 pvvx 12/2016
 */

#include "rtl8195a.h"
#ifdef CONFIG_SDIO_HOST_EN
#include "sd.h"
#include "sdio_host.h"
#include "hal_sdio_host.h"
#include "rtl8195a_sdio_host.h"
//#include "hal_sdio.h"
//#include "PinNames.h"
//#include "pinmap.h"

#define MAX_BUF_ADMA 64
#define SIZE_BLOCK_ADMA 512
//-------------------------------------------------------------------------
// Function declarations
//-------------------------------------------------------------------------
// Data declarations
sdio_sd_irq_handler xfer_done_irq_handler;
uint32_t xfer_err_irq_data;
uint32_t xfer_done_irq_data;
sdio_sd_irq_handler xfer_err_irq_handler;
_sdio_init_s sdio_status = SDIO_INIT_NONE;
sdio_sd_irq_handler card_remove_irq_handler;
uint32_t card_remove_irq_data;
sdio_sd_irq_handler card_insert_irq_handler;
uint32_t card_insert_irq_data;
HAL_SDIO_HOST_OP HalSdioHostOp;
s8 sd_protected = -1;
HAL_SDIO_HOST_ADAPTER SdioHostAdapter;
SRAM_BF_DATA_SECTION ADMA2_DESC_FMT gAdmaTbls[MAX_BUF_ADMA];
//-------------------------------------------------------------------------

void xfer_done_callback(void *param) {
	if (xfer_done_irq_handler)
		xfer_done_irq_handler((void *) xfer_done_irq_data);
}

void xfer_err_callback(void *param) {
	if (xfer_err_irq_handler)
		xfer_err_irq_handler((void *) xfer_err_irq_data);
}

void card_insert_callback(void *param) {
#if CONFIG_DEBUG_LOG > 1 
	rtl_printf("SD card insert\n");
#endif
	if (card_insert_irq_handler)
		card_insert_irq_handler((void *) card_insert_irq_data);
}

void card_remove_callback(void *param) {
#if CONFIG_DEBUG_LOG > 1 
	rtl_printf("SD card removed\n");
#endif
	sdio_status = SDIO_SD_NONE;
	if (card_remove_irq_handler)
		card_remove_irq_handler((void *) card_remove_irq_data);
}

//----- sdio_init_host
s8 sdio_init_host() {
	s8 result;
	HAL_Status stat;

	DBG_SDIO_INFO("SDIO Init Host Begin...\n");
	if (sdio_status > SDIO_INIT_FAIL) {
		DBG_SDIO_INFO("SDIO Host init already.\n");
		result = 0;
	} else {
		rtl_memset(&SdioHostAdapter, 0, sizeof(SdioHostAdapter));
		HalSdioHostOpInit(&HalSdioHostOp);
		stat = HalSdioHostOp.HalSdioHostInitHost(&SdioHostAdapter);
		SdioHostAdapter.CardInsertCbPara = &SdioHostAdapter;
		SdioHostAdapter.CardInsertCallBack =
				(void (*)(void *)) card_insert_callback;
		SdioHostAdapter.CardRemoveCbPara = &SdioHostAdapter;
		SdioHostAdapter.CardRemoveCallBack =
				(void (*)(void *)) card_remove_callback;
		SdioHostAdapter.XferCompCbPara = &SdioHostAdapter;
		SdioHostAdapter.XferCompCallback =
				(void (*)(void *)) xfer_done_callback;
		SdioHostAdapter.ErrorCbPara = &SdioHostAdapter;
		SdioHostAdapter.ErrorCallback = (void (*)(void *)) xfer_err_callback;
		if (stat == HAL_OK) {
			sdio_status = SDIO_INIT_OK;
			DBG_SDIO_INFO("SDIO Host init Success.\n");
			result = 0;
		} else {
			sdio_status = SDIO_INIT_FAIL;
			DBG_SDIO_ERR("SDIO Host init Fail.\n");
			result = -1;
		}
	}
	return result;
}

//-----
void sdio_deinit_host(void) {
	if (sdio_status > 0) {
		HAL_Status hs = HalSdioHostOp.HalSdioHostDeInit(&SdioHostAdapter);
		if (hs == HAL_OK) {
			SdioHostAdapter.CardInsertCallBack = NULL;
			SdioHostAdapter.CardRemoveCallBack = NULL;
			sdio_status = SDIO_INIT_NONE;
		}
	}
}

//-----
s8 sdio_read_blocks(u32 sector, u8 *buffer, u32 count) {

	if (!count) {
		DBG_SDIO_ERR("Parameter error, try to read %d count\n");
		return -1;
	}
	if (sdio_status <= SDIO_SD_NONE) {
		DBG_SDIO_ERR("SD card is not ready\n");
		return -1;
	}
	if (count > MAX_BUF_ADMA) {
		count = MAX_BUF_ADMA;
		DBG_SDIO_ERR("Not enough ADMA table(maximum %d), reduce blocks count\n",
				count);
	}
	rtl_memset(gAdmaTbls, 0, sizeof(ADMA2_DESC_FMT) * count);
	if (1) {
		ADMA2_DESC_FMT *p = gAdmaTbls;
		u8 * pbuf = buffer;
		int i = 0;
		while (i < count) {
			i++;
			p->Addr1 = (u32) pbuf;
			p->Len1 = SIZE_BLOCK_ADMA;
			if (i == count) {
				p->Attrib1.Valid = 1; // 0x23
				p->Attrib1.End = 1;
				p->Attrib1.Act2 = 1;
			} else {
				p->Attrib1.Valid = 1; // 0x21
				p->Attrib1.Act2 = 1;
				p->Attrib2.Valid = 1; // 0x31
				p->Attrib2.Act1 = 1;
				p->Attrib2.Act2 = 1;
				p->Len2 = 0;
				p->Addr2 = (u32) (&p[1]);
			}
			pbuf += SIZE_BLOCK_ADMA;
			p++;
		}
		SdioHostAdapter.AdmaDescTbl = gAdmaTbls;
	}
	HAL_Status result = HalSdioHostOp.HalSdioHostReadBlocksDma(&SdioHostAdapter,
			(unsigned long long) sector * SIZE_BLOCK_ADMA, count);
	if (result) {
		DBG_SDIO_ERR("sdio_read_blocks fail(0x%02x)\n", result);
		return -1;
	}
	return 0;
}

//-----
s8 sdio_write_blocks(uint32_t sector, const uint8_t *buffer, uint32_t count) {

	if (!count) {
		DBG_SDIO_ERR("Parameter error, try to read %d count\n");
		return -1;
	}
	if (sdio_status <= SDIO_SD_NONE) {
		DBG_SDIO_ERR("SD card is not ready\n");
		return -1;
	}
	if (count > MAX_BUF_ADMA) {
		count = MAX_BUF_ADMA;
		DBG_SDIO_ERR("Not enough ADMA table(maximum %d), reduce blocks count\n",
				count);
	}
	if (sd_protected) {
		DBG_SDIO_ERR("SD card is write protected\n");
		return -1;
	}
	rtl_memset(gAdmaTbls, 0, sizeof(ADMA2_DESC_FMT) * count);
	if (1) {
		ADMA2_DESC_FMT *p = gAdmaTbls;
		u8 * pbuf = buffer;
		int i = 0;
		while (i < count) {
			i++;
			p->Addr1 = (u32) pbuf;
			p->Len1 = SIZE_BLOCK_ADMA;
			if (i == count) {
				p->Attrib1.Valid = 1; // 0x23
				p->Attrib1.End = 1;
				p->Attrib1.Act2 = 1;
			} else {
				p->Attrib1.Valid = 1; // 0x21
				p->Attrib1.Act2 = 1;
				p->Attrib2.Valid = 1; // 0x31
				p->Attrib2.Act1 = 1;
				p->Attrib2.Act2 = 1;
				p->Len2 = 0;
				p->Addr2 = (u32) (&p[1]);
			}
			pbuf += SIZE_BLOCK_ADMA;
			p++;
		}
		SdioHostAdapter.AdmaDescTbl = gAdmaTbls;
	}
	HAL_Status result = HalSdioHostOp.HalSdioHostWriteBlocksDma(
			&SdioHostAdapter, (unsigned long long) sector * SIZE_BLOCK_ADMA,
			count);
	if (result != HAL_OK) {
		DBG_SDIO_ERR("write fail(0x%02x)\n", result);
		return -1;
	}
	return 0;
}

//-----
s8 sdio_sd_init(void) {
	if (sdio_status != SDIO_SD_OK) {
		SdioHostAdapter.AdmaDescTbl = gAdmaTbls;
		if (sdio_status <= SDIO_INIT_FAIL)
			sdio_init_host();
		if (sdio_status != SDIO_INIT_OK) {
			return -1;
		}
		DBG_SDIO_INFO("Init sd card.\n");
		if (HalSdioHostOp.HalSdioHostInitCard(&SdioHostAdapter)) {
			return -1;
		}
		sdio_status = SDIO_SD_OK;
		if (HalSdioHostOp.HalSdioHostChangeSdClock(&SdioHostAdapter,
				SD_CLK_41_6MHZ) != HAL_OK)
			DBG_SDIO_INFO("SD card does not support high speed.\n");
	}
	return 0;
}

//-----
void sdio_sd_deinit() {
	if (sdio_status > SDIO_SD_NONE)
		sdio_status = SDIO_INIT_OK;
	sdio_deinit_host(); // add pvvx (fix SD_DeInit())
}

//-----
s8 sdio_sd_setClock(SD_CLK_FREQUENCY SDCLK) {
	HAL_Status sta;
	if (sdio_status <= SDIO_SD_NONE) {
		return -1;
	}
	ADMA2_DESC_FMT * padma = rtw_zmalloc(sizeof(ADMA2_DESC_FMT));
	if (!padma) {
		DBG_SDIO_ERR("Malloc ADMA2 table fail.\n");
		return -1;
	}
	DBG_SDIO_INFO("SD card set CLK %d Hz\n", PLATFORM_CLOCK/(4<<(8-SDCLK)));
	sta = HalSdioHostOp.HalSdioHostChangeSdClock(&SdioHostAdapter, SDCLK);
	rtw_mfree(padma, sizeof(ADMA2_DESC_FMT));
	if (sta)
		return -1;
	return 0;
}

//-----
s8 sdio_sd_setProtection(bool protection) {
	s8 result;
	ADMA2_DESC_FMT *padma;
	HAL_Status hls;
	if (sdio_status > SDIO_SD_NONE) {
		padma = (ADMA2_DESC_FMT *) rtw_zmalloc(sizeof(ADMA2_DESC_FMT));
		if (!padma) {
			DBG_SDIO_ERR("Malloc ADMA2 table fail.\n");
			return -1;
		}
		SdioHostAdapter.AdmaDescTbl = padma;
		if (protection) {
			if (sd_protected == 1)
				goto LABEL_8;
			hls = HalSdioHostOp.HalSdioHostSetWriteProtect(&SdioHostAdapter, 1);
		} else {
			if (sd_protected == 0)
				goto LABEL_8;
			hls = HalSdioHostOp.HalSdioHostSetWriteProtect(&SdioHostAdapter, 0);
		}
		if (hls) {
			DBG_SDIO_ERR("Set SD card Protection fail.\n");
			result = -1;
			goto LABEL_17;
		}
LABEL_8:
		sd_protected = protection;
LABEL_7:
		DBG_SDIO_INFO("Set SD card Protection done.\n");
		result = 0;
LABEL_17:
		rtw_mfree(padma, sizeof(ADMA2_DESC_FMT));
		return result;
	}
	return -1;
}

//----- sdio_sd_getProtection
s8 sdio_sd_getProtection() {
	s8 result;

	result = sd_protected;
	if (sd_protected != -1)
		return result;
	if (sdio_status <= SDIO_SD_NONE) {
		result = -1;
		return result;
	}
	if (HalSdioHostOp.HalSdioHostGetWriteProtect(&SdioHostAdapter)) {
		DBG_SDIO_ERR("Get SD card Protection fail.\n");
		result = -1;
		return result;
	}
	if (SdioHostAdapter.IsWriteProtect)
		DBG_SDIO_INFO("SD card is Write Protected.\n");
	result = (s8) SdioHostAdapter.IsWriteProtect;
	sd_protected = SdioHostAdapter.IsWriteProtect;
	return result;
}

//----- sdio_sd_status
s8 sdio_sd_status() {
	s8 result;

	DBG_SDIO_INFO("sdio_get_sdcard_status.\n");
	if (sdio_status <= SDIO_SD_NONE
			|| HalSdioHostOp.HalSdioHostGetCardStatus(&SdioHostAdapter))
		result = -1;
	else
		result = SdioHostAdapter.CardCurState;
	return result;
}

//----- sdio_sd_getCSD
s8 sdio_sd_getCSD(u8* CSD) {
	s8 result;

	if (sdio_status <= SDIO_SD_NONE)
		result = -1;
	else {
		rtl_memcpy(CSD, SdioHostAdapter.Csd, CSD_REG_LEN);
		result = 0;
	}
	return result;
}

//----- sdio_sd_isReady
s8 sdio_sd_isReady() {
	s8 result = sdio_status - SDIO_SD_OK;
	if (sdio_status != SDIO_SD_OK)
		result = -1;
	return result;
}

//----- sdio_sd_getCapacity
u32 sdio_sd_getCapacity(void) {
	u32 result;
	uint8_t csd[CSD_REG_LEN];
	if (sdio_status <= SDIO_SD_NONE)
		result = 0; // -1;
	else {
		rtl_memcpy(csd, SdioHostAdapter.Csd, CSD_REG_LEN);
		if ((csd[0] & 0xC0) == 64)
			result = (u16) (csd[9] + 1 + (csd[8] << 8)) << 9;
		else
			result = (4 * csd[7] + ((u32) csd[8] >> 6) + 1
					+ ((csd[6] & 3) << 10))
					<< ((csd[5] & 0xF) + (csd[10] >> 7) + 2 * (csd[9] & 3) - 8);
		result *= 2;
	}
	return result;
}

s8 sdio_sd_hook_insert_cb(sdio_sd_irq_handler CallbackFun, void *param) {
	card_insert_irq_handler = CallbackFun;
	card_insert_irq_data = (uint32_t) param;
	return 0;
}

s8 sdio_sd_hook_remove_cb(sdio_sd_irq_handler CallbackFun, void *param) {
	card_remove_irq_handler = CallbackFun;
	card_remove_irq_data = (uint32_t) param;
	return 0;
}

s8 sdio_sd_hook_xfer_cmp_cb(sdio_sd_irq_handler CallbackFun, void *param) {
	xfer_done_irq_handler = CallbackFun;
	xfer_done_irq_data = (uint32_t) param;
	return 0;
}

s8 sdio_sd_hook_xfer_err_cb(sdio_sd_irq_handler CallbackFun, void *param) {
	xfer_err_irq_handler = CallbackFun;
	xfer_err_irq_data = (uint32_t) param;
	return 0;
}

#endif // CONFIG_SDIO_HOST_EN

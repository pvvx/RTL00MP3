/*
 * sd.c (disasm ds.o)
 *
 * RTL8710/11 pvvx 12/2016
 */

#include "rtl8195a.h"
#ifdef CONFIG_SDIO_HOST_EN
#include "sd.h"
#include "sdio_host.h"

#define SIZE_BLOCK_ADMA 512

SemaphoreHandle_t sdWSema;

void sd_xfer_done_callback(void *obj) {
	RtlUpSema(&sdWSema);
}

void sd_xfer_err_callback(void *obj) {
	DBG_SDIO_ERR("sd_xfer_err_callback \r\n");
}

//----- SD_WaitReady
SD_RESULT SD_WaitReady() {
	return SD_OK;
}

//----- SD_Init
SD_RESULT SD_Init() {
	SD_RESULT result;

	if (sdio_sd_init() != 0)
		result = SD_INITERR;
	else {
		if (sdio_sd_getProtection() != 0)
			result = SD_PROTECTED;
		RtlInitSema(&sdWSema, 0);
		sdio_sd_hook_xfer_cmp_cb(sd_xfer_done_callback, 0);
		sdio_sd_hook_xfer_err_cb(sd_xfer_err_callback, 0);
	}
	return result;
}

//----- SD_DeInit
SD_RESULT SD_DeInit() {
	sdio_sd_deinit();
	RtlFreeSema(&sdWSema);
	return SD_OK;
}

//----- SD_SetCLK
SD_RESULT SD_SetCLK(SD_CLK CLK) {
	SD_RESULT result;
	switch (CLK) {
	case SD_CLK_HIGH:
		result = sdio_sd_setClock(SD_CLK_41_6MHZ);
		break;
	case SD_CLK_MID:
		result = sdio_sd_setClock(SD_CLK_20_8MHZ);
		break;
	case SD_CLK_LOW:
		result = sdio_sd_setClock(SD_CLK_10_4MHZ);
		break;
	case SD_CLK_RSV:
		result = sdio_sd_setClock(SD_CLK_5_2MHZ);
		break;
	default:
//		DBG_SDIO_INFO("clk = %d ?\n", CLK);
		return SD_ERROR;
	}
	if (result)
		return SD_ERROR;
	return SD_OK;
}

//----- SD_Status
SD_RESULT SD_Status() {
	if (sdio_sd_isReady())
		return SD_NODISK;
	else
		return sdio_sd_getProtection() != 0;
}

//----- SD_GetCID
SD_RESULT SD_GetCID(u8 *cid_data) {
	return SD_OK;
}

//----- SD_GetCSD
SD_RESULT SD_GetCSD(u8 *csd_data) {
	if (sdio_sd_getCSD(csd_data))
		return SD_ERROR;
	else
		return SD_OK;
}

//----- SD_GetCapacity
SD_RESULT SD_GetCapacity(uint32_t *sector_count) {

	u32 sc = sdio_sd_getCapacity();
	*sector_count = sc;
	if (sc != 0)
		return SD_OK;
	else
		return SD_ERROR;
}

//----- SD_ReadBlocks
SD_RESULT SD_ReadBlocks(u32 sector, u8 *data, u32 count) {
	int rd_count;
	unsigned char * buf;

	if ((u32) data & 3) {
		buf = pvPortMalloc(SIZE_BLOCK_ADMA);
		if (buf == NULL)
			DBG_SDIO_ERR("Fail to malloc cache for SDIO host!\n");
		u32 end_sector = count + sector;
		while (sector < end_sector) {
			rd_count = sdio_read_blocks(sector, buf, 1);
//			rtl_printf("rd_counts = %d\n", rd_count);
			if (rd_count == 0 && RtlDownSemaWithTimeout(&sdWSema, 1000) != 1) {
				DBG_SDIO_ERR("SD_ReadBlocks timeout\n");
				return SD_ERROR;
			}
			rtl_memcpy(data, buf, SIZE_BLOCK_ADMA);
			sector++;
			data += SIZE_BLOCK_ADMA;
		}
		vPortFree(buf);
		if (rd_count)
			return SD_ERROR;
		return SD_OK;
	} else {
		if (sdio_read_blocks(sector, data, count) == 0) {
			if (RtlDownSemaWithTimeout(&sdWSema, 1000) == 1)
				return SD_OK;
			DBG_SDIO_ERR("SD_ReadBlocks timeout\n");
		}
	}

	return SD_ERROR;
}

//----- SD_WriteBlocks
SD_RESULT SD_WriteBlocks(u32 sector, const u8 *data, u32 count) {
	int wr_count;
	unsigned char * buf;

	if ((u32) data & 3) {
		buf = pvPortMalloc(SIZE_BLOCK_ADMA);
		if (buf == NULL)
			DBG_SDIO_ERR("Fail to malloc cache for SDIO host!\n");
		u32 end_sector = count + sector;
		while (sector != end_sector) {
			rtl_memcpy(buf, data, SIZE_BLOCK_ADMA);
			wr_count = sdio_write_blocks(sector, buf, 1);
			if (wr_count == 0 && RtlDownSemaWithTimeout(&sdWSema, 1000) != 1) {
				DBG_SDIO_ERR("SD_WriteBlocks timeout\n");
				return SD_ERROR;
			}
			sector++;
			data += SIZE_BLOCK_ADMA;
		}
		vPortFree(buf);
		if (wr_count == 0)
			return SD_OK;
	} else if (sdio_write_blocks(sector, data, count) == 0) {
		if (RtlDownSemaWithTimeout(&sdWSema, 1000) == 1)
			return SD_OK;
		DBG_SDIO_ERR("SD_WriteBlocks timeout\n");
	}
	return SD_ERROR;

}
#endif // CONFIG_SDIO_HOST_EN

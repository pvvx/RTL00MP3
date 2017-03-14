/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 */
#ifndef _HAL_8195A_H_
#define _HAL_8195A_H_

#include "platform_autoconf.h"
#include "basic_types.h"
#include "section_config.h"
#include "rtl8195a_sys_on.h"
#include "rtl8195a_peri_on.h"
#include "hal_platform.h"
#include "hal_pinmux.h"
#include "hal_api.h"
#include "hal_peri_on.h"
#include "hal_misc.h"
#include "hal_irqn.h"
#include "hal_vector_table.h"
#include "hal_diag.h"
#include "hal_spi_flash.h"
#include "rtl8195a_spi_flash.h"
#include "hal_timer.h"
#include "hal_util.h"
#include "hal_efuse.h"
#include "hal_soc_ps_monitor.h"
#include "diag.h"
#include "hal_common.h"
#include "hal_soc_ps_monitor.h"

/* ----------------------------------------------------------------------------
   -- Cortex M3 Core Configuration
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Cortex_Core_Configuration Cortex M0 Core Configuration
 * @{
 */

#define __CM3_REV                      0x0200    /**< Core revision r0p0 */
#define __MPU_PRESENT                  1         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               4         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         1         /**< Vendor specific implementation of SysTickConfig is defined */

#include "core_cm3.h"

#ifdef CONFIG_TIMER_EN
#include "hal_timer.h"
#endif

#ifdef CONFIG_GDMA_EN
#include "hal_gdma.h"
#include "rtl8195a_gdma.h"
#endif

#ifdef CONFIG_GPIO_EN
#include "hal_gpio.h"
#include "rtl8195a_gpio.h"
#endif

#ifdef CONFIG_SPI_COM_EN
#include "hal_ssi.h"
#include "rtl8195a_ssi.h"
#endif

#ifdef CONFIG_UART_EN
#include "hal_uart.h"
#include "rtl8195a_uart.h"
#endif

#ifdef CONFIG_I2C_EN
#include "hal_i2c.h"
#include "rtl8195a_i2c.h"
#endif

#ifdef CONFIG_PCM_EN
#include "hal_pcm.h"
#include "rtl8195a_pcm.h"
#endif

#ifdef CONFIG_PWM_EN
#include "hal_pwm.h"
#include "rtl8195a_pwm.h"
#endif

#ifdef CONFIG_I2S_EN
#include "hal_i2s.h"
#include "rtl8195a_i2s.h"
#endif

#ifdef CONFIG_DAC_EN
#include "hal_dac.h"
#include "rtl8195a_dac.h"
#endif

#ifdef CONFIG_ADC_EN
#include "hal_adc.h"
#include "rtl8195a_adc.h"
#endif

#ifdef CONFIG_SDR_EN
#endif

#ifdef CONFIG_SPIC_EN
#endif

#ifdef CONFIG_SDIO_DEVICE_EN
#include "hal_sdio.h"
#endif

#ifdef CONFIG_NFC_EN
#include "hal_nfc.h"
#include "rtl8195a_nfc.h"
#endif

#ifdef CONFIG_WDG
#include "rtl8195a_wdt.h"
#endif

#ifdef CONFIG_USB_EN
#include "hal_usb.h"
#include "rtl8195a_usb.h"
#endif

#include "hal_log_uart.h"

#ifdef CONFIG_MII_EN
#include "hal_mii.h"
#include "rtl8195a_mii.h"
#endif

// firmware information, located at the header of Image2
#define FW_VERSION          (0x0100)
#define FW_SUBVERSION       (0x0001)
#define FW_CHIP_ID          (0x8195)
#define FW_CHIP_VER         (0x01)
#define FW_BUS_TYPE         (0x01)          // the iNIC firmware type: USB/SDIO
#define FW_INFO_RSV1        (0x00)          // the firmware information reserved
#define FW_INFO_RSV2        (0x00)          // the firmware information reserved
#define FW_INFO_RSV3        (0x00)          // the firmware information reserved
#define FW_INFO_RSV4        (0x00)          // the firmware information reserved

#define FLASH_RESERVED_DATA_BASE		0x8000  // reserve 32K for Image1
#define FLASH_SYSTEM_DATA_ADDR			0x9000  // reserve 32K+4K for Image1 + Reserved data
// Flash Map for Calibration data
#define FLASH_CAL_DATA_BASE				0xA000
#define FLASH_CAL_DATA_ADDR(_offset)	(FLASH_CAL_DATA_BASE + _offset)
#define FLASH_CAL_DATA_SIZE				0x1000
#define FLASH_SECTOR_SIZE				0x1000
// SPIC Calibration Data
#define FLASH_SPIC_PARA_OFFSET          0x80
#define FLASH_SPIC_PARA_BASE            (FLASH_SYSTEM_DATA_ADDR+FLASH_SPIC_PARA_OFFSET)
// SDRC Calibration Data
#define FLASH_SDRC_PARA_OFFSET          0x180
#define FLASH_SDRC_PARA_BASE            (FLASH_SYSTEM_DATA_ADDR+FLASH_SDRC_PARA_OFFSET)
// ADC Calibration Data
#define FLASH_ADC_PARA_OFFSET           0x200
#define FLASH_ADC_PARA_BASE             (FLASH_SYSTEM_DATA_ADDR+FLASH_ADC_PARA_OFFSET)

#define	IMG_SIGN_RUN	"81958711"
#define	IMG_SIGN_SWP 	"01958711"
#define	IMG_SIGN1_RUN	0x35393138	// "8195"
#define	IMG_SIGN1_SWP	0x35393130	// "0195"
#define	IMG_SIGN2_RUN	0x31313738	// "8711"
#define	IMG_SIGN2_SWP	IMG_SIGN2_RUN	// "8711"
#define IMG2_SIGN_TXT	"RTKWin"
#define IMG2_SIGN_DW1_TXT	0x574b5452 	// "RTKW"
#define IMG2_SIGN_SW2_TXT	0x6e69 		// "in"

typedef struct _RAM_FUNCTION_START_TABLE_ {
    VOID (*RamStartFun) (VOID);		// Run for Init console, Run if ( v40000210 & 0x4000000 )
    VOID (*RamWakeupFun) (VOID);	// Run if ( v40000210 & 0x20000000 )
    VOID (*RamPatchFun0) (VOID);	// Run if ( v40000210 & 0x10000000 )
    VOID (*RamPatchFun1) (VOID);	// Run if ( v400001F4 & 0x8000000 ) && ( v40000210 & 0x8000000 )
    VOID (*RamPatchFun2) (VOID);	// Run for Init console, if ( v40000210 & 0x4000000 )
}RAM_FUNCTION_START_TABLE, *PRAM_FUNCTION_START_TABLE;
// START_RAM_FUN_SECTION RAM_FUNCTION_START_TABLE __ram_start_table_start__ =
// {RamStartFun + 1, RamWakeupFun + 1, RamPatchFun0 + 1, RamPatchFun1 + 1, RamPatchFun2 + 1 };

#define IMG1_VALID_PATTEN_INIT() { 0x23, 0x79, 0x16, 0x88, 0xff, 0xff, 0xff, 0xff }
// IMAGE1_VALID_PATTEN_SECTION uint8 RAM_IMG1_VALID_PATTEN[8] = IMG1_VALID_PATTEN_INIT();

typedef struct _RAM_START_FUNCTION_ {
    VOID (*RamStartFun) (VOID);
}RAM_START_FUNCTION, *PRAM_START_FUNCTION;
// IMAGE2_START_RAM_FUN_SECTION RAM_START_FUNCTION gImage2EntryFun0 = { InfraStart + 1 };

typedef struct __RAM_IMG2_VALID_PATTEN__ {
	char rtkwin[7];
	u8 x[13];
} _RAM_IMG2_VALID_PATTEN, *_PRAM_IMG2_VALID_PATTEN;

// IMAGE2_VALID_PATTEN_SECTION _RAM_IMG2_VALID_PATTEN RAM_IMG2_VALID_PATTEN = RAM_IMG2_VALID_PATTEN_INIT();
#define RAM_IMG2_VALID_PATTEN_INIT() { \
	{ IMG2_SIGN_TXT }, { 0xff, \
    (FW_VERSION&0xff), ((FW_VERSION >> 8)&0xff),	\
    (FW_SUBVERSION&0xff), ((FW_SUBVERSION >> 8)&0xff), \
    (FW_CHIP_ID&0xff), ((FW_CHIP_ID >> 8)&0xff),	\
    (FW_CHIP_VER), (FW_BUS_TYPE), \
    (FW_INFO_RSV1), (FW_INFO_RSV2), (FW_INFO_RSV3), (FW_INFO_RSV4)}}


#endif //_HAL_8195A_H_

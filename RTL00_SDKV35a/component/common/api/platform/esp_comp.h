#ifndef _INCLUDE_ESP_COMP_H_
#define _INCLUDE_ESP_COMP_H_

#include "platform_autoconf.h"
#include "flash_api.h"

#define ICACHE_FLASH_ATTR
#define ICACHE_RODATA_ATTR
#define DATA_IRAM_ATTR
#define ICACHE_RAM_ATTR


#define os_printf rtl_printf
#define os_printf_plus rtl_printf
#define os_sprintf_fd rtl_sprintf
#define ets_sprintf rtl_sprintf

//#ifndef os_malloc
#undef os_malloc
extern void *pvPortMalloc(size_t xWantedSize);
#define os_malloc   pvPortMalloc
#undef os_zalloc
extern void *pvPortZalloc(size_t xWantedSize);
#define os_zalloc   pvPortZalloc
//#undef os_calloc
//#define os_calloc   pvPortCalloc
#undef os_realloc
extern void *pvPortReAlloc(void *pv, size_t xWantedSize);
#define os_realloc  pvPortReAlloc
#undef os_free
extern void vPortFree(void *pv);
#define os_free     vPortFree
//#endif
extern size_t xPortGetFreeHeapSize(void);
#define system_get_free_heap_size xPortGetFreeHeapSize

#define os_bzero rtl_bzero
#define os_delay_us wait_us // HalDelayUs
//#define os_install_putc1 rtl_install_putc1
//#define os_install_putc2 rtl_install_putc2
//#define os_intr_lock rtl_intr_lock
//#define os_intr_unlock rtl_intr_unlock
//#define os_isr_attach rtl_isr_attach
//#define os_isr_mask rtl_isr_mask
//#define os_isr_unmask rtl_isr_unmask
#define os_memcmp rtl_memcmp
#define os_memcpy rtl_memcpy
#define ets_memcpy rtl_memcpy
#define os_memmove rtl_memmove
#define os_memset rtl_memset
#define os_putc rtl_putc
//#define os_str2macaddr rtl_str2macaddr
//#define os_strcat strcat
#define os_strchr rtl_strchr
#define os_strrchr rtl_strrchr
#define os_strcmp rtl_strcmp
#define os_strcpy rtl_strcpy
#define os_strlen rtl_strlen
#define os_strncmp rtl_strncmp
#define os_strncpy rtl_strncpy
#define os_strstr rtl_strstr
#define os_random Rand

//extern uint32 phy_get_rand(void);
#define system_get_os_print() 1

#ifdef USE_US_TIMER
#define os_timer_arm_us(a, b, c) rtl_timer_arm_new(a, b, c, 0)
#endif


//#define os_timer_arm(a, b, c) rtl_timer_arm_new(a, b, c, 1)
//#define os_timer_disarm rtl_timer_disarm
//#define os_timer_init rtl_timer_init
//#define os_timer_setfn rtl_timer_setfn

//#define os_timer_done rtl_timer_done
//#define os_timer_handler_isr rtl_timer_handler_isr

//#define os_update_cpu_frequency rtl_update_cpu_frequency

//#define os_sprintf ets_sprintf
/*
typedef struct{
	uint32_t	deviceId;		//+00
	uint32_t	chip_size;    	//+04 chip size in byte
	uint32_t	block_size;		//+08
	uint32_t	sector_size;	//+0c
	uint32_t	page_size;		//+10
	uint32_t	status_mask;	//+14
} SpiFlashChip;

typedef enum {
    SPI_FLASH_RESULT_OK,
    SPI_FLASH_RESULT_ERR,
    SPI_FLASH_RESULT_TIMEOUT
} SpiFlashOpResult;

extern SpiFlashChip * flashchip; // in RAM-BIOS: 0x3fffc714
*/

// include "flash_api.h"
#define spi_flash_real_size() flash_get_size(&flashobj)
#define Cache_Read_Disable() SpicDisableRtl8195A()
#define Cache_Read_Enable(a, b, c) flash_turnon()

#define spi_flash_get_id() ((flashobj.SpicInitPara.id[0] << 16) | (flashobj.SpicInitPara.id[1]<<8) | flashobj.SpicInitPara.id[2])
//SpiFlashOpResult spi_flash_read_status(uint32_t * sta);
//SpiFlashOpResult spi_flash_write_status(uint32_t sta);
#define spi_flash_erase_sector(sec) flash_erase_sector(&flashobj, (sec)<<12)
#define spi_flash_write(faddr, pbuf, size) flash_stream_write(&flashobj, faddr, size, (uint8_t *)pbuf)
#define spi_flash_read(faddr, pbuf, size) flash_stream_read(&flashobj, faddr, size, (uint8_t *)pbuf)
#define spi_flash_erase_block(blk) flash_erase_block(&flashobj, (blk)<<16);

#ifndef ip4_addr1
#define ip4_addr1(ipaddr) (((u8_t*)(ipaddr))[0])
#define ip4_addr2(ipaddr) (((u8_t*)(ipaddr))[1])
#define ip4_addr3(ipaddr) (((u8_t*)(ipaddr))[2])
#define ip4_addr4(ipaddr) (((u8_t*)(ipaddr))[3])
#endif
/* These are cast to u16_t, with the intent that they are often arguments
 * to printf using the U16_F format from cc.h. */
#ifndef ip4_addr1_16
#define ip4_addr1_16(ipaddr) ((u16_t)ip4_addr1(ipaddr))
#define ip4_addr2_16(ipaddr) ((u16_t)ip4_addr2(ipaddr))
#define ip4_addr3_16(ipaddr) ((u16_t)ip4_addr3(ipaddr))
#define ip4_addr4_16(ipaddr) ((u16_t)ip4_addr4(ipaddr))
#endif

#undef IP2STR
#define IP2STR(ipaddr) ip4_addr1_16(ipaddr), \
    ip4_addr2_16(ipaddr), \
    ip4_addr3_16(ipaddr), \
    ip4_addr4_16(ipaddr)

#undef IPSTR
#define IPSTR "%d.%d.%d.%d"

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif


#ifndef DEBUGSOO
/* CONFIG_DEBUG_LOG:
=0 Off all diag/debug msg,
=1 Only errors,
=2 errors + warning, (default)
=3 errors + warning + info,
=4 errors + warning + info + debug,
=5 full */
#if CONFIG_DEBUG_LOG > 3
#define DEBUGSOO (CONFIG_DEBUG_LOG - 1)
#elif CONFIG_DEBUG_LOG > 1
#define DEBUGSOO 2
#else
#define DEBUGSOO CONFIG_DEBUG_LOG
#endif
#endif //#ifndef DEBUGSOO

//#define system_get_sdk_version() "3.5.3"

//#define system_get_time xTaskGetTickCount
//#define ets_get_cpu_frequency HalGetCpuClk


#endif // _INCLUDE_ESP_COMP_H_

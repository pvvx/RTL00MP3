/*
*
*	
*/
#ifndef _INCLUDE_ESP_COMP_H_
#define _INCLUDE_ESP_COMP_H_

#include "platform_autoconf.h"

#define ICACHE_FLASH_ATTR
#define ICACHE_RODATA_ATTR
#define DATA_IRAM_ATTR

#define os_printf(...) rtl_printf(__VA_ARGS__)
#define os_printf_plus(...) rtl_printf(__VA_ARGS__)
#define os_sprintf_fd(...) rtl_sprintf(__VA_ARGS__)
#define ets_sprintf(...) rtl_sprintf(__VA_ARGS__)
/*
#define os_malloc   pvPortMalloc
#define os_zalloc   pvPortZalloc
#define os_calloc   pvPortCalloc
#define os_realloc  pvPortRealloc
*/
#undef os_free
#define os_free     vPortFree
#define system_get_free_heap_size xPortGetFreeHeapSize
#undef os_realloc
#define os_realloc  pvPortReAlloc


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
#define os_timer_arm_us(a, b, c) rtl_timer_arm_new(a, b, c)
#endif


//#define os_timer_arm(a, b, c) rtl_timer_arm_new(a, b, c)
//#define os_timer_disarm rtl_timer_disarm
//#define os_timer_init rtl_timer_init
//#define os_timer_setfn rtl_timer_setfn

//#define os_timer_done rtl_timer_done
//#define os_timer_handler_isr rtl_timer_handler_isr

//#define os_update_cpu_frequency rtl_update_cpu_frequency

//#define os_sprintf ets_sprintf

#define spi_flash_real_size() (1<<(flashobj.SpicInitPara.id[2]-1))



#define ip4_addr1(ipaddr) (((u8_t*)(ipaddr))[0])
#define ip4_addr2(ipaddr) (((u8_t*)(ipaddr))[1])
#define ip4_addr3(ipaddr) (((u8_t*)(ipaddr))[2])
#define ip4_addr4(ipaddr) (((u8_t*)(ipaddr))[3])
/* These are cast to u16_t, with the intent that they are often arguments
 * to printf using the U16_F format from cc.h. */
#define ip4_addr1_16(ipaddr) ((u16_t)ip4_addr1(ipaddr))
#define ip4_addr2_16(ipaddr) ((u16_t)ip4_addr2(ipaddr))
#define ip4_addr3_16(ipaddr) ((u16_t)ip4_addr3(ipaddr))
#define ip4_addr4_16(ipaddr) ((u16_t)ip4_addr4(ipaddr))

#define IP2STR(ipaddr) ip4_addr1_16(ipaddr), \
    ip4_addr2_16(ipaddr), \
    ip4_addr3_16(ipaddr), \
    ip4_addr4_16(ipaddr)

#define IPSTR "%d.%d.%d.%d"

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

#endif // _INCLUDE_ESP_COMP_H_

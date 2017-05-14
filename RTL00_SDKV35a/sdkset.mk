#=============================================
# USER CONFIG (in project.mk)
#=============================================
#USE_AT = 1
#USE_FATFS = 1
#USE_SDIOH = 1
#USE_POLARSSL = 1
#USE_P2P_WPS = 1
#ifndef USE_AT
#USE_NEWCONSOLE = 1
#USE_WIFI_API = 1
#endif
#USE_MBED = 1
#USE_GCC_LIB = 1
#RTOSDIR=freertos_v8.1.2
#RTOSDIR=freertos_v9.0.0
#LWIPDIR=lwip_v1.4.1
# -------------------------------------------------------------------
# FLAGS
# -------------------------------------------------------------------
CFLAGS = -DM3 -DCONFIG_PLATFORM_8195A -DGCC_ARMCM3 -DARDUINO_SDK -DF_CPU=166666666L -DNDEBUG
CFLAGS += -mcpu=cortex-m3 -mthumb -g2 -Os -std=gnu99 -Wall -Werror
CFLAGS += -fno-common -fmessage-length=0 -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-short-enums -fsigned-char 
CFLAGS += -w -Wno-pointer-sign    
ifdef USE_GCC_LIB 
LFLAGS = -mcpu=cortex-m3 -mthumb -g -Os -nostartfiles --specs=nano.specs
else
LFLAGS = -mcpu=cortex-m3 -mthumb -g -Os -nostartfiles -nostdlib
endif
LFLAGS += -Wl,--gc-sections -Wl,--cref -Wl,--entry=Reset_Handler -Wl,--no-enum-size-warning -Wl,--no-wchar-size-warning -Wl,-nostdlib

# LIBS
# -------------------------------------------------------------------
LIBS =
ifdef USE_GCC_LIB 
all: LIBS +=_platform_new _wlan _p2p _wps _websocket _sdcard _xmodem _mdns gcc m c nosys 
mp: LIBS +=_platform_new _wlan_mp _wps _p2p _websocket _sdcard _xmodem _mdns gcc m c nosys 
else
all: LIBS +=_platform_new _wlan _p2p _wps _websocket _sdcard _xmodem _mdns
mp: LIBS +=_platform_new _wlan_mp _wps _p2p _websocket _sdcard _xmodem _mdns
endif
# m c nosys gcc
PATHLIBS = sdk/component/soc/realtek/8195a/misc/bsp/lib/common/GCC
LDFILE = rlx8195A-symbol-v04-img2.ld
BOOTS = sdk/component/soc/realtek/8195a/misc/bsp/image

# Include folder list
# -------------------------------------------------------------------
INCLUDES = ../inc
INCLUDES += project/inc
INCLUDES += sdk/component/soc/realtek/common/bsp
INCLUDES += sdk/component/os/freertos
INCLUDES += sdk/component/os/freertos/$(RTOSDIR)/Source/include
INCLUDES += sdk/component/os/freertos/$(RTOSDIR)/Source/portable/GCC/ARM_CM3
INCLUDES += sdk/component/os/os_dep/include sdk/component/soc/realtek/8195a/misc/driver
INCLUDES += sdk/component/common/api/network/include
INCLUDES += sdk/component/common/api 
INCLUDES += sdk/component/common/api/platform
INCLUDES += sdk/component/common/api/wifi
INCLUDES += sdk/component/common/api/wifi/rtw_wpa_supplicant/src
INCLUDES += sdk/component/common/network
INCLUDES += sdk/component/common/network/lwip/$(LWIPDIR)/port/realtek/freertos
INCLUDES += sdk/component/common/network/lwip/$(LWIPDIR)/src/include
INCLUDES += sdk/component/common/network/lwip/$(LWIPDIR)/src/include/lwip
INCLUDES += sdk/component/common/network/lwip/$(LWIPDIR)/src/include/ipv4
INCLUDES += sdk/component/common/network/lwip/$(LWIPDIR)/port/realtek
INCLUDES += sdk/component/common/test
INCLUDES += sdk/component/soc/realtek/8195a/cmsis
INCLUDES += sdk/component/soc/realtek/8195a/cmsis/device
INCLUDES += sdk/component/soc/realtek/8195a/fwlib
INCLUDES += sdk/component/soc/realtek/8195a/fwlib/rtl8195a
INCLUDES += sdk/component/soc/realtek/8195a/misc/rtl_std_lib/
INCLUDES += sdk/component/soc/realtek/8195a/misc/rtl_std_lib/include
INCLUDES += sdk/component/common/drivers
INCLUDES += sdk/component/common/drivers/i2s
INCLUDES += sdk/component/common/drivers/wlan/realtek/include
INCLUDES += sdk/component/common/drivers/wlan/realtek/src/osdep
INCLUDES += sdk/component/common/drivers/wlan/realtek/src/hci
INCLUDES += sdk/component/common/drivers/wlan/realtek/src/hal
INCLUDES += sdk/component/common/drivers/wlan/realtek/src/hal/OUTSRC
INCLUDES += sdk/component/common/drivers/sdio/realtek/sdio_host/inc
INCLUDES += sdk/component/soc/realtek/8195a/fwlib/ram_lib/wlan/realtek/wlan_ram_map/rom
INCLUDES += sdk/component/common/network/ssl/ssl_ram_map/rom
#INCLUDES += sdk/component/common/media/codec
#INCLUDES += sdk/component/common/drivers/usb_class/host/uvc/inc
#INCLUDES += sdk/component/common/drivers/usb_class/device
#INCLUDES += sdk/component/common/drivers/usb_class/device/class 
#INCLUDES += sdk/component/soc/realtek/8195a/fwlib/ram_lib/usb_otg/include

# Source file list
# -------------------------------------------------------------------
SRC_C =
DRAM_C =
BOOT_C =

#bootloader
SRC_C += sdk/component/soc/realtek/8195a/fwlib/ram_lib/rtl_bios_data.c
BOOT_C += sdk/component/soc/realtek/8195a/fwlib/ram_lib/rtl_boot.c 

#cmsis
SRC_C += sdk/component/soc/realtek/8195a/cmsis/device/system_8195a.c

#console new/old
ifdef USE_NEWCONSOLE
SRC_C += sdk/component/soc/realtek/8195a/misc/driver/rtl_console_new.c
else
SRC_C += sdk/component/common/api/at_cmd/log_service.c
SRC_C += sdk/component/soc/realtek/8195a/misc/driver/rtl_consol.c
endif
ifdef USE_AT
DRAM_C += sdk/component/common/api/at_cmd/atcmd_ethernet.c
DRAM_C += sdk/component/common/api/at_cmd/atcmd_lwip.c
DRAM_C += sdk/component/common/api/at_cmd/atcmd_sys.c
DRAM_C += sdk/component/common/api/at_cmd/atcmd_wifi.c
endif
#SRC_C += sdk/component/soc/realtek/8195a/misc/driver/low_level_io.c

#network - api
ifdef USE_P2P_WPS
SRC_C += sdk/component/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_p2p_config.c
SRC_C += sdk/component/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_wps_config.c
endif
SRC_C += sdk/component/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_eap_config.c
SRC_C += sdk/component/common/api/wifi/wifi_conf.c
SRC_C += sdk/component/common/api/wifi/wifi_ind.c
SRC_C += sdk/component/common/api/wifi/wifi_promisc.c
SRC_C += sdk/component/common/api/wifi/wifi_simple_config.c
SRC_C += sdk/component/common/api/wifi/wifi_util.c
SRC_C += sdk/component/common/api/lwip_netconf.c
ifdef USE_WIFI_API
SRC_C += sdk/component/common/api/wifi_api.c 
SRC_C += sdk/component/common/api/wifi_api_scan.c 
endif

#network - lwip
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/api/api_lib.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/api/api_msg.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/api/err.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/api/netbuf.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/api/netdb.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/api/netifapi.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/api/sockets.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/api/tcpip.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/ipv4/autoip.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/ipv4/icmp.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/ipv4/igmp.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/ipv4/inet.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/ipv4/inet_chksum.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/ipv4/ip.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/ipv4/ip_addr.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/ipv4/ip_frag.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/def.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/dhcp.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/dns.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/init.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/lwip_timers.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/mem.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/memp.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/netif.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/pbuf.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/raw.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/stats.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/sys.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/tcp.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/tcp_in.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/tcp_out.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/core/udp.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/src/netif/etharp.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/port/realtek/freertos/ethernetif.c
SRC_C += sdk/component/common/drivers/wlan/realtek/src/osdep/lwip_intf.c
SRC_C += sdk/component/common/network/lwip/$(LWIPDIR)/port/realtek/freertos/sys_arch.c
SRC_C += sdk/component/common/network/dhcp/dhcps.c
SRC_C += sdk/component/common/network/sntp/sntp.c
SRC_C += sdk/component/common/network/netbios/netbios.c 

#network - mdns
#SRC_C += sdk/component/common/network/mDNS/mDNSPlatform.c

#os - freertos
SRC_C += sdk/component/os/freertos/$(RTOSDIR)/Source/portable/MemMang/heap_5.c
SRC_C += sdk/component/os/freertos/$(RTOSDIR)/Source/portable/GCC/ARM_CM3/port.c
SRC_C += sdk/component/os/freertos/cmsis_os.c
SRC_C += sdk/component/os/freertos/$(RTOSDIR)/Source/croutine.c
SRC_C += sdk/component/os/freertos/$(RTOSDIR)/Source/event_groups.c
SRC_C += sdk/component/os/freertos/$(RTOSDIR)/Source/list.c
SRC_C += sdk/component/os/freertos/$(RTOSDIR)/Source/queue.c
SRC_C += sdk/component/os/freertos/$(RTOSDIR)/Source/tasks.c
SRC_C += sdk/component/os/freertos/$(RTOSDIR)/Source/timers.c

#os - osdep
SRC_C += sdk/component/os/os_dep/device_lock.c
SRC_C += sdk/component/os/freertos/freertos_service.c
SRC_C += sdk/component/os/os_dep/mailbox.c
SRC_C += sdk/component/os/os_dep/osdep_api.c
SRC_C += sdk/component/os/os_dep/osdep_service.c
SRC_C += sdk/component/os/os_dep/tcm_heap.c

ifdef USE_MBED
#peripheral - api
INCLUDES += sdk/component/common/mbed/api
INCLUDES += sdk/component/common/mbed/hal
INCLUDES += sdk/component/common/mbed/hal_ext
INCLUDES += sdk/component/common/mbed/targets/hal/rtl8195a
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/analogin_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/dma_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/efuse_api.c
#SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/ethernet_api.c
#SRC_C += sdk/component/common/drivers/ethernet_mii/ethernet_mii.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/flash_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/gpio_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/gpio_irq_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/i2c_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/i2s_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/log_uart_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/nfc_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/pinmap.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/pinmap_common.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/port_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/pwmout_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/rtc_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/serial_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/sleep.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/spdio_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/spi_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/sys_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/timer_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/us_ticker.c
SRC_C += sdk/component/common/mbed/common/us_ticker_api.c
SRC_C += sdk/component/common/mbed/common/wait_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/wdt_api.c
SRC_C += sdk/component/common/mbed/targets/hal/rtl8195a/flash_eep.c 
endif

#peripheral - hal
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_32k.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_adc.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_gdma.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_gpio.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_i2c.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_i2s.c
#SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_mii.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_nfc.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_pcm.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_pwm.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_sdr_controller.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_ssi.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_timer.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_uart.c
ifdef USE_SDIOH
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_sdio_host.c
SRC_C += sdk/component/common/drivers/sdio/realtek/sdio_host/src/sd.c 
SRC_C += sdk/component/common/drivers/sdio/realtek/sdio_host/src/sdio_host.c 
endif

#peripheral - osdep
SRC_C += sdk/component/os/freertos/freertos_pmu.c

#peripheral - rtl8195a
SRC_C += sdk/component/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_adc.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_gdma.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_gpio.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_i2c.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_i2s.c
#SRC_C += sdk/component/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_mii.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_nfc.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_pwm.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_ssi.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_timer.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_uart.c

#peripheral - wlan
#SRC_C += sdk/component/common/drivers/wlan/realtek/src/core/option/rtw_opt_skbuf.c

#SDRAM
#DRAM_C += sdk/component/common/api/platform/stdlib_patch.c
#SDRAM - polarssl
ifdef USE_POLARSSL
INCLUDES += sdk/component/common/network/ssl/polarssl-1.3.8/include

SRC_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/bignum.c

DRAM_C += sdk/component/common/network/ssl/ssl_ram_map/rom/rom_ssl_ram_map.c
DRAM_C += sdk/component/common/network/ssl/ssl_ram_map/ssl_ram_map.c

DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/aes.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/aesni.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/arc4.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/asn1parse.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/asn1write.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/base64.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/blowfish.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/camellia.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/ccm.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/certs.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/cipher.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/cipher_wrap.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/ctr_drbg.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/debug.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/des.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/dhm.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/ecp.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/ecp_curves.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/ecdh.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/ecdsa.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/entropy.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/entropy_poll.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/error.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/gcm.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/havege.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/hmac_drbg.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/md.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/md_wrap.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/md2.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/md4.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/md5.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/memory_buffer_alloc.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/net.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/oid.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/padlock.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/pbkdf2.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/pem.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/pkcs5.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/pkcs11.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/pkcs12.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/pk.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/pk_wrap.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/pkparse.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/pkwrite.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/platform.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/ripemd160.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/rsa.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/sha1.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/sha256.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/sha512.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/ssl_cache.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/ssl_ciphersuites.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/ssl_cli.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/ssl_srv.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/ssl_tls.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/threading.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/timing.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/version.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/version_features.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/x509.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/x509_crt.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/x509_crl.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/x509_csr.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/x509_create.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/x509write_crt.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/x509write_csr.c
DRAM_C += sdk/component/common/network/ssl/polarssl-1.3.8/library/xtea.c
endif

#utilities - FatFS
ifdef USE_FATFS
INCLUDES += sdk/component/common/file_system/fatfs
INCLUDES += sdk/component/common/file_system/fatfs/r0.10c/include
SRC_C += sdk/component/common/file_system/fatfs/fatfs_ext/src/ff_driver.c
SRC_C += sdk/component/common/file_system/fatfs/r0.10c/src/diskio.c
SRC_C += sdk/component/common/file_system/fatfs/r0.10c/src/ff.c
SRC_C += sdk/component/common/file_system/fatfs/r0.10c/src/option/ccsbcs.c
ifdef USE_SDIOH
SRC_C += sdk/component/common/file_system/fatfs/disk_if/src/sdcard.c
endif
endif

# Reversed SDK component
#ADD_SRC_C += sdk/component/soc/realtek/8195a/cmsis/device/app_start.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_dac.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_common.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_soc_ps_monitor.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_efuse.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_log_uart.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_pinmux.c
SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_misc.c
#ADD_SRC_C += sdk/component/soc/realtek/8195a/fwlib/src/hal_spi_flash_ram.c
# Component clib, ...
SRC_C += sdk/component/soc/realtek/8195a/fwlib/ram_lib/startup.c 
SRC_C += sdk/component/soc/realtek/8195a/misc/rtl_std_lib/lib_rtlstd/ram_libc.c 
SRC_C += sdk/component/soc/realtek/8195a/misc/rtl_std_lib/lib_rtlstd/ram_libgloss_retarget.c
SRC_C += sdk/component/soc/realtek/8195a/misc/rtl_std_lib/lib_rtlstd/rtl_eabi_cast_ram.c
SRC_C += sdk/component/soc/realtek/8195a/misc/rtl_std_lib/lib_rtlstd/rtl_math_ram.c
#if +- nostdlib..
ifndef USE_GCC_LIB 
SRC_C += sdk/component/soc/realtek/8195a/misc/rtl_std_lib/lib_rtlstd/ram_pvvx_libc.c 
endif
#if c_printf() float
SRC_C += sdk/component/soc/realtek/8195a/misc/rtl_std_lib/lib_rtlstd/c_stdio.c
# -------------------------------------------------------------------
# Add Source file list
# -------------------------------------------------------------------
ADD_SRC_C = 
# -------------------------------------------------------------------
# SAMPLES
# -------------------------------------------------------------------
ifdef USE_AT
INCLUDES += sdk/component/common/utilities
ADD_SRC_C += sdk/component/common/api/network/src/wlan_network.c
ADD_SRC_C += sdk/component/common/api/wifi_interactive_mode.c
ADD_SRC_C += sdk/component/common/api/network/src/ping_test.c
ADD_SRC_C += sdk/component/common/utilities/webserver.c
ADD_SRC_C += sdk/component/common/utilities/tcptest.c
ADD_SRC_C += sdk/component/common/utilities/update.c
INCLUDES += sdk/component/common/example
INCLUDES += sdk/component/common/example/wlan_fast_connect
ADD_SRC_C += sdk/component/common/example/wlan_fast_connect/example_wlan_fast_connect.c
ADD_SRC_C += sdk/component/common/example/uart_atcmd/example_uart_atcmd.c
ADD_SRC_C += sdk/component/common/example/example_entry.c
ADD_SRC_C += sdk/component/common/application/xmodem/uart_fw_update.c
endif
#application
#INCLUDES += sdk/component/common/application/apple/WACServer/External/Curve25519
#INCLUDES += sdk/component/common/application/apple/WACServer/External/GladmanAES
#INCLUDES += sdk/component/common/audio 
#INCLUDES += sdk/component/common/application/xmodem
#INCLUDES += sdk/component/common/application
#INCLUDES += sdk/component/common/application/iotdemokit
#INCLUDES += sdk/component/common/application/google
#INCLUDES += sdk/component/common/media/framework
#SDRAM - wigadget
#DRAM_C += sdk/component/common/application/wigadget/cloud_link.c
#DRAM_C += sdk/component/common/application/wigadget/shtc1.c
#DRAM_C += sdk/component/common/application/wigadget/wigadget.c

#utilities
#ADD_SRC_C += sdk/component/common/utilities/cJSON.c
#ADD_SRC_C += sdk/component/common/utilities/http_client.c
#ADD_SRC_C += sdk/component/common/utilities/uart_socket.c
#ADD_SRC_C += sdk/component/common/utilities/xml.c
#ADD_SRC_C += sdk/component/common/utilities/ssl_client.c
#ADD_SRC_C += sdk/component/common/utilities/ssl_client_ext.c
#ADD_SRC_C += sdk/component/common/utilities/uart_ymodem.c
#ADD_SRC_C += sdk/component/common/application/uart_adapter/uart_adapter.c
#ADD_SRC_C += sdk/component/common/example/cJSON/cJSON_example.c 
#ADD_SRC_C += sdk/component/common/example/googlenest/example_google.c  
#ADD_SRC_C += sdk/component/common/example/mdns/example_mdns.c
#ADD_SRC_C += sdk/component/common/example/socket_select/example_socket_select.c
#ADD_SRC_C += sdk/component/common/example/xml/example_xml.c

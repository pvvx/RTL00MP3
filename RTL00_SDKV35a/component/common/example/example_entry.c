/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include <platform_opts.h>
#include "main.h"
#if CONFIG_EXAMPLE_UART_ATCMD
#include "uart_atcmd/example_uart_atcmd.h"
#endif
#ifdef CONFIG_EXAMPLE_MDNS
#include <mdns/example_mdns.h>
#endif

/*
	Preprocessor of example
*/
void pre_example_entry(void)
{
	//
#if	defined(CONFIG_EXAMPLE_WLAN_FAST_CONNECT) && CONFIG_EXAMPLE_WLAN_FAST_CONNECT && !CONFIG_EXAMPLE_UART_ATCMD
	example_wlan_fast_connect();
#endif
#ifdef CONFIG_EXAMPLE_MDNS
	example_mdns();
#endif
}

/*
  	All of the examples are disabled by default for code size consideration
   	The configuration is enabled in platform_opts.h
*/
void example_entry(void)
{
#if CONFIG_EXAMPLE_UART_ATCMD
	example_uart_atcmd();
#endif
}

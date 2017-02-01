#ifndef __EXAMPLE_UART_ATCMD_H__
#define __EXAMPLE_UART_ATCMD_H__

/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#if CONFIG_EXAMPLE_UART_ATCMD
#include "FreeRTOS.h"
#include "semphr.h"
#include "osdep_api.h"


#if defined(RTL8710AF)
// RTL8710AF
#define UART_TX   		PA_4 // PC_3
#define UART_RX   		PA_0 // PC_0
#define UART_RTS		PA_2 // PC_2
#define UART_CTS		PA_1 // PC_1

#elif 0 // defined(RTL8711AM)
// RTL8711AM
#define UART_TX   		PA_7
#define UART_RX   		PA_6 // no Interrupt!
#define UART_RTS		PA_3
#define UART_CTS		PA_5

#elif 0 // else
// RTL8711AM + RTL8710AF
#define UART_TX   		PC_3
#define UART_RX   		PC_0 // no Interrupt!
#define UART_RTS		PC_2
#define UART_CTS		PC_1

#elif defined(RTL8711AM)
// RTL8711AM + RTL8710AF
#define UART_TX   		PE_0
#define UART_RX   		PE_3
#define UART_RTS		PE_1
#define UART_CTS		PE_2

#endif

#define KEY_ENTER 		0xd
#define KEY_BS    		0x8
#define KEY_ESC    		0x1B
#define KEY_LBRKT  		0x5B

void uart_at_lock(void);
void uart_at_unlock(void);
void uart_at_send_string(char *str);
void uart_at_send_buf(u8 *buf, u32 len);
void example_uart_atcmd(void);

#include "at_cmd/atcmd_wifi.h"

void uart_atcmd_reinit(UART_LOG_CONF* uartconf);
int write_uart_atcmd_setting_to_system_data(UART_LOG_CONF* uartconf);

extern u8 key_2char2num(u8 hch, u8 lch);
static void at_hex2str(const u8 *start, u32 size, u8 *out, u32 out_size)
{
	int index, index2;
	u8 *buf, *line;

	if(!start ||(size==0)||(!out)||(out_size==0))
		return;

	buf = (u8*)start;
	line = (u8*)out;
	for (index = 0, index2=0; (index < size)&&(index2<out_size); index++, index2+=2) 
	{
		sprintf((char *)line+index2, "%02x", (u8) buf[index]); 
	}

	return;
}
static void at_str2hex(const u8 *start, u32 size, u8 *out, u32 out_size)
{
	int index, index2;
	u8 *buf, *line;

	if(!start ||(size==0))
		return;

	buf = (u8*)start;
	line = (u8*)out;

	for (index=0, index2=0; index<size; index+=2, index2++){
		line[index2] = key_2char2num(buf[index], buf[index+1]);
	}

	return;
}

#endif //#if CONFIG_EXAMPLE_UART_ATCMD
#endif //#ifndef __EXAMPLE_UART_ATCMD_H__

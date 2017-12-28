/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"
//#include <stdarg.h>
#include "rtl_bios_data.h"
//#include "rtl_consol.h"
#include "osdep_api.h"
#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
#include "freertos_pmu.h"
#endif
#include "tcm_heap.h"
//#include "log_service.h"

//MON_RAM_BSS_SECTION   UART_LOG_CTL    UartLogCtl;
//MON_RAM_BSS_SECTION   UART_LOG_CTL    *pUartLogCtl;
//MON_RAM_BSS_SECTION   u8                       *ArgvArray[MAX_ARGV];
//MON_RAM_BSS_SECTION   UART_LOG_BUF             UartLogBuf;


#ifdef CONFIG_UART_LOG_HISTORY
//MON_RAM_BSS_SECTION    u8  UartLogHistoryBuf[UART_LOG_HISTORY_LEN][UART_LOG_CMD_BUFLEN];
#endif

_LONG_CALL_
extern u8
UartLogCmdChk(
    IN  u8  RevData,
    IN  UART_LOG_CTL    *prvUartLogCtl,
    IN  u8  EchoFlag
);

_LONG_CALL_
extern VOID
ArrayInitialize(
    IN  u8  *pArrayToInit,
    IN  u8  ArrayLen,
    IN  u8  InitValue
);

_LONG_CALL_
extern VOID
UartLogHistoryCmd(
    IN  u8  RevData,
    IN  UART_LOG_CTL    *prvUartLogCtl,
    IN  u8  EchoFlag
);

_LONG_CALL_
extern VOID
UartLogCmdExecute(
    IN  PUART_LOG_CTL   pUartLogCtlExe
);



//=================================================


/* Minimum and maximum values a `signed long int' can hold.
   (Same as `int').  */
#ifndef __LONG_MAX__
#if defined (__alpha__) || (defined (__sparc__) && defined(__arch64__)) || defined (__sparcv9) || defined (__s390x__)
#define __LONG_MAX__ 9223372036854775807L
#else
#define __LONG_MAX__ 2147483647L
#endif /* __alpha__ || sparc64 */
#endif
#undef LONG_MIN
#define LONG_MIN (-LONG_MAX-1)
#undef LONG_MAX
#define LONG_MAX __LONG_MAX__

/* Maximum value an `unsigned long int' can hold.  (Minimum is 0).  */
#undef ULONG_MAX
#define ULONG_MAX (LONG_MAX * 2UL + 1)

#ifndef __LONG_LONG_MAX__
#define __LONG_LONG_MAX__ 9223372036854775807LL
#endif




//======================================================
//<Function>:  UartLogIrqHandleRam
//<Usage   >:  To deal with Uart-Log RX IRQ
//<Argus    >:  VOID
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
MON_RAM_TEXT_SECTION
VOID
UartLogIrqHandleRam
(
    VOID * Data
)
{
    u8      UartReceiveData = 0;
    //For Test
    BOOL    PullMode = _FALSE;

    u32 IrqEn = DiagGetIsrEnReg();

    DiagSetIsrEnReg(0);

    UartReceiveData = DiagGetChar(PullMode);
    if (UartReceiveData == 0) {
        goto exit;
    }

    //KB_ESC chk is for cmd history, it's a special case here.
    if (UartReceiveData == KB_ASCII_ESC) {
        //4 Esc detection is only valid in the first stage of boot sequence (few seconds)
        if (pUartLogCtl->ExecuteEsc != _TRUE)
        {
            pUartLogCtl->ExecuteEsc = _TRUE;
            (*pUartLogCtl).EscSTS = 0;
        }
        else
        {
            //4 the input commands are valid only when the task is ready to execute commands
            if ((pUartLogCtl->BootRdy == 1)  
#ifdef CONFIG_KERNEL 
                ||(pUartLogCtl->TaskRdy == 1)
#endif          
            )
            {
                if ((*pUartLogCtl).EscSTS==0)
                {
                    (*pUartLogCtl).EscSTS = 1;
                }
            }
            else
            {
                (*pUartLogCtl).EscSTS = 0;
            }            
        }
    }
    else if ((*pUartLogCtl).EscSTS==1){
        if (UartReceiveData != KB_ASCII_LBRKT){
            (*pUartLogCtl).EscSTS = 0;
        }
        else{
            (*pUartLogCtl).EscSTS = 2;
        }
    }

    else{
        if ((*pUartLogCtl).EscSTS==2){
            (*pUartLogCtl).EscSTS = 0;
#ifdef CONFIG_UART_LOG_HISTORY
            if ((UartReceiveData=='A')|| UartReceiveData=='B'){
                UartLogHistoryCmd(UartReceiveData,(UART_LOG_CTL *)pUartLogCtl,1);
            }
#endif
        }
        else{
            if (UartLogCmdChk(UartReceiveData,(UART_LOG_CTL *)pUartLogCtl,1)==2)
            {
                //4 check UartLog buffer to prevent from incorrect access
                if (pUartLogCtl->pTmpLogBuf != NULL)
                {
                    pUartLogCtl->ExecuteCmd = _TRUE;
#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
    				if (pUartLogCtl->TaskRdy)
    					RtlUpSemaFromISR((_Sema *)&pUartLogCtl->Sema);
#endif
                }
                else
                {
                    ArrayInitialize((u8 *)pUartLogCtl->pTmpLogBuf->UARTLogBuf, UART_LOG_CMD_BUFLEN, '\0');
                }
            }
        }
    }
exit:
    DiagSetIsrEnReg(IrqEn);

}



MON_RAM_TEXT_SECTION
VOID
RtlConsolInitRam(
    IN  u32     Boot,
    IN  u32     TBLSz,
    IN  VOID    *pTBL    
)
{
    UartLogBuf.BufCount = 0;
    ArrayInitialize(&UartLogBuf.UARTLogBuf[0],UART_LOG_CMD_BUFLEN,'\0');
    pUartLogCtl = &UartLogCtl;
    
    pUartLogCtl->NewIdx = 0;
    pUartLogCtl->SeeIdx = 0;
    pUartLogCtl->RevdNo = 0;
    pUartLogCtl->EscSTS = 0;
    pUartLogCtl->BootRdy = 0;
    pUartLogCtl->pTmpLogBuf = &UartLogBuf;
#ifdef CONFIG_UART_LOG_HISTORY
    pUartLogCtl->CRSTS = 0;
    pUartLogCtl->pHistoryBuf = &UartLogHistoryBuf[0];
#endif
    pUartLogCtl->pfINPUT = (VOID*)&DiagPrintf;
    pUartLogCtl->pCmdTbl = (PCOMMAND_TABLE) pTBL;
    pUartLogCtl->CmdTblSz = TBLSz;
#ifdef CONFIG_KERNEL    
    pUartLogCtl->TaskRdy = 0;
#endif
    //executing boot sequence
    if (Boot == ROM_STAGE)
    {
        pUartLogCtl->ExecuteCmd = _FALSE;
        pUartLogCtl->ExecuteEsc = _FALSE;
    }
    else
    {
        pUartLogCtl->ExecuteCmd = _FALSE;
        pUartLogCtl->ExecuteEsc= _TRUE;		//don't check Esc anymore
#if defined(CONFIG_KERNEL)
        /* Create a Semaphone */
        RtlInitSema((_Sema*)&(pUartLogCtl->Sema), 0);
        pUartLogCtl->TaskRdy = 0;
#ifdef PLATFORM_FREERTOS
#define	LOGUART_STACK_SIZE	200 //USE_MIN_STACK_SIZE modify from 512 to 128
#if 0 //CONFIG_USE_TCM_HEAP
	{
		int ret = 0;
		void *stack_addr = tcm_heap_malloc(LOGUART_STACK_SIZE*sizeof(int));
		//void *stack_addr = rtw_malloc(stack_size*sizeof(int));
		if(stack_addr == NULL){
			DiagPrintf("Out of TCM heap in \"LOGUART_TASK\" ");
		}
		ret = xTaskGenericCreate(
				RtlConsolTaskRam,
				(const char *)"log_uart",
				LOGUART_STACK_SIZE,
				NULL,
				tskIDLE_PRIORITY + 5 + PRIORITIE_OFFSET, // +5
				NULL,
				stack_addr,
				NULL);
		if (pdTRUE != ret)
		{
			DiagPrintf("Create Log UART Task Err!\n");
		}
	}
#else							 
	if (pdTRUE != xTaskCreate( RtlConsolTaskRam, (const signed char * const)"log_uart", LOGUART_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5 + PRIORITIE_OFFSET, NULL))
	{
		DiagPrintf("Create Log UART Task Err!\n");
	}
#endif

#endif

#endif    
    }	

    CONSOLE_8195A();
}

#if SUPPORT_LOG_SERVICE
extern char log_buf[LOG_SERVICE_BUFLEN];
extern xSemaphoreHandle	log_rx_interrupt_sema;
#endif
//======================================================
void console_cmd_exec(PUART_LOG_CTL   pUartLogCtlExe)
{
//    u8  CmdCnt = 0;
    u8  argc = 0;
    u8  **argv;
    //u32  CmdNum;
    PUART_LOG_BUF   pUartLogBuf = pUartLogCtlExe->pTmpLogBuf;
#if SUPPORT_LOG_SERVICE
    strncpy(log_buf, (const u8*)&(*pUartLogBuf).UARTLogBuf[0], LOG_SERVICE_BUFLEN-1);
#endif
    argc = GetArgc((const u8*)&((*pUartLogBuf).UARTLogBuf[0]));
    argv = GetArgv((const u8*)&((*pUartLogBuf).UARTLogBuf[0]));

	if(argc > 0){
#if SUPPORT_LOG_SERVICE
//		if(log_handler(argv[0]) == NULL)
//			legency_interactive_handler(argc, argv);
              RtlUpSema((_Sema *)&log_rx_interrupt_sema);
#endif
              ArrayInitialize(argv[0], sizeof(argv[0]) ,0);
	}else{
#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
		acquire_wakelock(WAKELOCK_LOGUART);
#endif
		CONSOLE_8195A(); // for null command
	}

    (*pUartLogBuf).BufCount = 0;
    ArrayInitialize(&(*pUartLogBuf).UARTLogBuf[0], UART_LOG_CMD_BUFLEN, '\0');
}
//======================================================
// overload original RtlConsolTaskRam
MON_RAM_TEXT_SECTION
VOID
RtlConsolTaskRam(
    VOID *Data
)
{
#if SUPPORT_LOG_SERVICE
	extern void log_service_init(void);
	log_service_init();
#else
#ifdef CONFIG_AT_USR
	at_user_init();
#endif
#endif
    //4 Set this for UartLog check cmd history
#ifdef CONFIG_KERNEL
	pUartLogCtl->TaskRdy = 1;
#else
    pUartLogCtl->BootRdy = 1;
#endif
    do{
#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
		RtlDownSema((_Sema *)&pUartLogCtl->Sema);
#endif
        if (pUartLogCtl->ExecuteCmd) {            
			// Add command handler here
			console_cmd_exec((PUART_LOG_CTL)pUartLogCtl);
            //UartLogCmdExecute((PUART_LOG_CTL)pUartLogCtl);
            pUartLogCtl->ExecuteCmd = _FALSE;
        }
    }while(1);
}
extern void * UartLogRomCmdTable;
//======================================================
void console_init(void)
{
    IRQ_HANDLE          UartIrqHandle;
    
    //4 Register Log Uart Callback function
	UartIrqHandle.Data = 0;//(u32)&UartAdapter;
    UartIrqHandle.IrqNum = UART_LOG_IRQ;
    UartIrqHandle.IrqFun = (IRQ_FUN) UartLogIrqHandleRam;
    UartIrqHandle.Priority = 0; // ??

    
    //4 Register Isr handle
    InterruptUnRegister(&UartIrqHandle); 
    InterruptRegister(&UartIrqHandle); 
#if !TASK_SCHEDULER_DISABLED    
//    RtlConsolInitRam((u32)ROM_STAGE,(u32)0,(VOID*)NULL);
    RtlConsolInitRam((u32)RAM_STAGE,(u32)6,(VOID*)UartLogRomCmdTable); // ); NULL);
#else
    RtlConsolInitRam((u32)RAM_STAGE,(u32)6,(VOID*)UartLogRomCmdTable); // ); NULL);
//    RtlConsolInitRam((u32)ROM_STAGE,(u32)0,(VOID*)NULL);
#endif
}





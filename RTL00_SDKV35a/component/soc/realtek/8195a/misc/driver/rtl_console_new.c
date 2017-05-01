/*
 * console_api.c
 *
 *  Created on: 24/02/17
 *      Author: pvvx
 */
//======================================================
#ifndef LOGUART_STACK_SIZE
#define	LOGUART_STACK_SIZE	400 // USE_MIN_STACK_SIZE modify from 512 to 128
#endif
#define	CONSOLE_PRIORITY 0
//======================================================
#include "rtl8195a.h"
#include "rtl_bios_data.h"
#include "osdep_api.h"
#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
#include "freertos_pmu.h"
#else
#error "Define configUSE_WAKELOCK_PMU  = 1 & configUSE_WAKELOCK_PMU = 1!"
#endif
#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
#else
#error "Define CONFIG_KERNEL & TASK_SCHEDULER_DISABLED = 0!"
#endif
#ifndef CONFIG_UART_LOG_HISTORY
#error "Define CONFIG_UART_LOG_HISTORY!"
#endif
//======================================================
// #define USE_ROM_CONSOLE
//======================================================
_LONG_CALL_ extern u8 UartLogCmdChk(
IN u8 RevData, IN UART_LOG_CTL *prvUartLogCtl,
IN u8 EchoFlag);

_LONG_CALL_ extern void ArrayInitialize(
IN u8 *pArrayToInit,
IN u8 ArrayLen,
IN u8 InitValue);

_LONG_CALL_ extern void UartLogHistoryCmd(
IN u8 RevData, IN UART_LOG_CTL *prvUartLogCtl,
IN u8 EchoFlag);

//_LONG_CALL_ extern void UartLogCmdExecute(IN PUART_LOG_CTL pUartLogCtlExe);
//======================================================
extern PCOMMAND_TABLE UartLogRamCmdTable[];
extern UartLogRamCmdTableSize;
//======================================================
//<Function>:  UartLogIrqHandleRam
//<Usage   >:  To deal with Uart-Log RX IRQ
//<Argus   >:  void
//<Return  >:  void
//<Notes   >:  NA
//======================================================
// overload original UartLogIrqHandle
MON_RAM_TEXT_SECTION
void UartLogIrqHandleRam(void * Data) {
	uint32 IrqEn = DiagGetIsrEnReg(); // HAL_UART_READ32(UART_INTERRUPT_EN_REG_OFF)
	DiagSetIsrEnReg(0); // HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, 0)
	uint8 UartReceiveData = DiagGetChar(_FALSE); // if(flg) while(!(HAL_UART_READ32(UART_LINE_STATUS_REG_OFF)&1)); return HAL_UART_READ32(UART_REV_BUF_OFF);
	if (UartReceiveData == 0) {
		goto exit;
	}
	PUART_LOG_CTL p = pUartLogCtl;
	//KB_ESC chk is for cmd history, it's a special case here.
	if (UartReceiveData == KB_ASCII_ESC) {
		// Esc detection is only valid in the first stage of boot sequence (few seconds)
		if (p->ExecuteEsc != _TRUE) {
			p->ExecuteEsc = _TRUE;
			p->EscSTS = 0;
		} else {
			//4 the input commands are valid only when the task is ready to execute commands
			if (p->BootRdy == 1 || p->TaskRdy == 1) {
				if (p->EscSTS == 0) {
					p->EscSTS = 1;
				}
			} else {
				p->EscSTS = 0;
			}
		}
	} else if (p->EscSTS == 1) {
		if (UartReceiveData != KB_ASCII_LBRKT) { // '['
			p->EscSTS = 0;
		} else {
			p->EscSTS = 2;
		}
	} else {
		if (p->EscSTS == 2) {
			p->EscSTS = 0;
			if (UartReceiveData == 'A' || UartReceiveData == 'B') {
				// if(UartReceiveData == ...) set pUartLogCtl->SeeIdx ...
				// prvStrCpy(pUartLogCtl->pTmpLogBuf->UARTLogBuf, pUartLogCtl->pHistoryBuf[pUartLogCtl->SeeIdx]);
				// pUartLogCtl->pTmpLogBuf->BufCount = prvStrLen(pUartLogCtl->pTmpLogBuf->UARTLogBuf);
				// if(EchoFlag) pUartLogCtl->pfINPUT(pUartLogCtl->pTmpLogBuf->UARTLogBuf);
				UartLogHistoryCmd(UartReceiveData, (UART_LOG_CTL *) pUartLogCtl,
						1);
			}
		} else {
			if (UartLogCmdChk(UartReceiveData, (UART_LOG_CTL *) pUartLogCtl, 1)
					== 2) {
				// check UartLog buffer to prevent from incorrect access
				if (p->pTmpLogBuf != NULL) {
					p->ExecuteCmd = _TRUE;
					if (p->TaskRdy) {
						RtlUpSemaFromISR((_Sema *) &pUartLogCtl->Sema);
					}
				} else {
					ArrayInitialize((u8 *) pUartLogCtl->pTmpLogBuf->UARTLogBuf,
					UART_LOG_CMD_BUFLEN, '\0');
				}
			}
		}
	}
	exit:
	DiagSetIsrEnReg(IrqEn); // HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, IrqEn)
}
//======================================================
//<Function>:  GetArgvRam
//<Usage   >:  парсигн аргументов строки
//<Argus   >:  pstr - указатель на строку
//<Return  >:  кол-во аргументов
//<Notes   >:  2 формата:
// 1) cmd=arg1,arg2,...
// 2) cmd arg1 arg2
// arg может быть обрамлен '"' или '\''
// для передачи ' ' или ','.
// Начальные пробелы cmd или arg удаляются.
//======================================================
int GetArgvRam(IN u8 *pstr, u8** argv) {
	int arvc = 0;
//	u8** argv = ArgvArray;
	u8* p = pstr;
	u8 t, n = ' ';
	int m = 0;
	while(*p != 0
		&& *p != '\r'
		&& *p != '\n'
		&& arvc < MAX_ARGV
		&& p < &pstr[UART_LOG_CMD_BUFLEN-1]) {
		switch(m) {
		case 0: // wait cmd
			if(*p == ' ') {
//				*p = 0;
				break;
			}
			*argv++ = p;
			arvc++;
			m++;
			break;
		case 1: // test end cmd, type format parm
			if(*p == ' ') { // format cmd arg1 arg2 ...
				m++;
				*p = 0;
			} else if(*p == '=') { // "at" format cmd=arg1,arg2,...
				n = ',';
				m++;
				*p = 0;
		 	}
			break;
		case 2:	// wait start arg
			if(*p == ' ') {
				*p = 0;
				break;
			}
			if(*p == '"' || *p == '\'') {
				t = *p;
				m = 4;
				*p = 0;
				break;
			}
			*argv++ = p;
			arvc++;
			m++;
		case 3:	// end arg
			if(*p == n) { // ' ' or ','
				m = 2;
				*p = 0;
			}
			break;
		case 4:
			*argv++ = p;
			arvc++;
			m++;
		case 5:
			if(*p == t) { // '\'' or '"'
				m = 3;
				*p = 0;
			}
			break;
		}
		p++;
	}
	return arvc;
}
//======================================================
//<Function>:  RtlConsolTaskRam
//<Usage   >:  overload original RtlConsolTaskRam
//<Argus   >:  Data - указатель PUART_LOG_CTL
//<Return  >:  none
//<Notes   >:
//======================================================
MON_RAM_TEXT_SECTION void RtlConsolTaskRam(void *Data) {
	PUART_LOG_CTL p = pUartLogCtl;
#ifdef USE_ROM_CONSOLE // show Help
	p->pTmpLogBuf->UARTLogBuf[0] = '?';
	p->pTmpLogBuf->BufCount = 1;
	p->ExecuteCmd = _TRUE;
#endif
	do {
		p->TaskRdy = _TRUE;
		RtlDownSema(&p->Sema);
		if (p->ExecuteCmd) {
			//	UartLogCmdExecute(pUartLogCtl);
			int argc = GetArgvRam(p->pTmpLogBuf->UARTLogBuf, ArgvArray);
			if(argc) {
				StrUpr(ArgvArray[0]);
				PCOMMAND_TABLE pcmd = p->pCmdTbl;
				int flg = 1;
#ifdef USE_ROM_CONSOLE
				for(int i = 0; i < p->CmdTblSz; i++) {
#else
				while(pcmd->cmd) {
#endif
					if(prvStrCmp(ArgvArray[0], pcmd->cmd) == 0) {
						flg = 0;
						if(pcmd->ArgvCnt < argc) {
#ifdef USE_ROM_CONSOLE
							pcmd->func(argc-1, &ArgvArray[1]);
#else
							pcmd->func(argc, &ArgvArray);
#endif
						} else {
#ifdef USE_ROM_CONSOLE
							DiagPrintf(pcmd->msg);
#else
							DiagPrintf("%s%s\n", pcmd->cmd, pcmd->msg);
#endif
						}
					}
					pcmd++;
				}
				if(flg) DiagPrintf("cmd: %s - nothing!\n", ArgvArray[0]);
#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
				release_wakelock(WAKELOCK_LOGUART);
#endif
			}
#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
			else acquire_wakelock(WAKELOCK_LOGUART);
#endif
			p->pTmpLogBuf->BufCount = 0;
			p->pTmpLogBuf->UARTLogBuf[0] = 0;
			HalSerialPutcRtl8195a('\r');
			HalSerialPutcRtl8195a('>');
			p->ExecuteCmd = _FALSE;
		}
	} while (1);
}
//======================================================
//<Function>:  console_init
//<Usage   >:  Initialize rtl console
//<Argus   >:  none
//<Return  >:  none
//<Notes   >:  delete rtl_concole.h from project
//======================================================
MON_RAM_TEXT_SECTION void console_init(void) {
	IRQ_HANDLE UartIrqHandle;
	// Register Log Uart Callback function
	UartIrqHandle.Data = 0; // (u32)&UartAdapter;
	UartIrqHandle.IrqNum = UART_LOG_IRQ;
	UartIrqHandle.IrqFun = (IRQ_FUN) UartLogIrqHandleRam;
	UartIrqHandle.Priority = 0; // ??
	// Register Isr handle
	InterruptUnRegister(&UartIrqHandle);
#ifdef USE_ROM_CONSOLE // use ROM Consol init & printf "<RTL8195A>"
	RtlConsolInit(RAM_STAGE, (u32) 6, (void*) UartLogRomCmdTable);
#else
	UartLogBuf.BufCount = 0;
	ArrayInitialize(&UartLogBuf.UARTLogBuf[0], UART_LOG_CMD_BUFLEN, '\0');
	pUartLogCtl = &UartLogCtl;
	pUartLogCtl->NewIdx = 0;
	pUartLogCtl->SeeIdx = 0;
	pUartLogCtl->EscSTS = 0;
	pUartLogCtl->BootRdy = 0;
	pUartLogCtl->pTmpLogBuf = &UartLogBuf;
	pUartLogCtl->CRSTS = 0;
	pUartLogCtl->pHistoryBuf = UartLogHistoryBuf;
	pUartLogCtl->pfINPUT = (void*) &DiagPrintf;
	pUartLogCtl->pCmdTbl = (PCOMMAND_TABLE) UartLogRamCmdTable;
	pUartLogCtl->CmdTblSz = UartLogRamCmdTableSize/16; //6; // GetRomCmdNum()
	pUartLogCtl->TaskRdy = 0;
#endif
	pUartLogCtl->RevdNo = UART_LOG_HISTORY_LEN;
	// Create a Semaphone
	RtlInitSema(&pUartLogCtl->Sema, 1);
	// executing boot sequence
	pUartLogCtl->ExecuteCmd = _FALSE;
	pUartLogCtl->ExecuteEsc = _TRUE;		//don't check Esc anymore
	InterruptRegister(&UartIrqHandle);
	if (pdTRUE
			!= xTaskCreate(RtlConsolTaskRam,
					(const signed char * const )"loguart", LOGUART_STACK_SIZE,
					NULL, tskIDLE_PRIORITY + CONSOLE_PRIORITY + PRIORITIE_OFFSET, NULL)) {
		DiagPrintf("Create Log UART Task Err!!\n");
	}
}

#ifndef USE_ROM_CONSOLE
//======================================================
//<Function>:  console_help
//<Usage   >:  Initialize rtl console
//<Argus   >:  argc - кол-во аргуметов, argv - список аргументов
//<Return  >:  none
//<Notes   >:
//======================================================
extern char str_rom_57ch3Dch0A[]; // "=========================================================\n" 57 шт
_WEAK void console_help(int argc, char *argv[]) { 	// Help
	DiagPrintf("CONSOLE COMMAND SET:\n");
	DiagPrintf(&str_rom_57ch3Dch0A[25]); //	DiagPrintf("==============================\n");
	PCOMMAND_TABLE pcmdtab = UartLogRamCmdTable;
	while(pcmdtab->cmd) {
#ifdef USE_ROM_CONSOLE
		DiagPrintf(pcmdtab->msg);
#else
		DiagPrintf("%s%s\n", pcmdtab->cmd, pcmdtab->msg);
#endif
		pcmdtab++;
	}
	DiagPrintf(&str_rom_57ch3Dch0A[25]); //	DiagPrintf("==============================\n");
}
LOCAL void print_on(int argc, char *argv[])
{
	print_off = argv[1][0]!='1';
}
// (!) размещается в специальном сегменте '.mon.tab*' (см. *.ld файл)
MON_RAM_TAB_SECTION COMMAND_TABLE console_commands[] = {
		{"PR", 1, print_on, "=<1/0>: Printf on/off"},	// Help
		{"?", 0, console_help, ": This Help"}	// Help
//		{"HELP", 0, console_help, ": Help"}	// Help
};
#endif

/*
 * hal_log_uart.c
 *
 *  Created on: 08/10/2016
 *      Author: pvvx
 */
#include "rtl8195a.h"

#ifdef CONFIG_LOG_UART_EN

#include "hal_log_uart.h"

//-------------------------------------------------------------------------
// Function declarations
/*
VOID HalLogUartIrqHandle(VOID * Data);
VOID HalLogUartSetBaudRate(HAL_LOG_UART_ADAPTER *pUartAdapter);
VOID HalLogUartSetLineCtrl(HAL_LOG_UART_ADAPTER *pUartAdapter);
VOID HalLogUartSetIntEn(HAL_LOG_UART_ADAPTER *pUartAdapter);
u32 HalLogUartInitSetting(HAL_LOG_UART_ADAPTER *pUartAdapter);
u32 HalLogUartRecv(HAL_LOG_UART_ADAPTER *pUartAdapter,
                    u8  *pRxData, u32 Length, u32 TimeoutMS);
u32 HalLogUartSend(HAL_LOG_UART_ADAPTER *pUartAdapter,
        u8 *pTxData, u32 Length, u32 TimeoutMS);
HAL_Status HalLogUartIntSend(HAL_LOG_UART_ADAPTER *pUartAdapter,
        u8 *pTxData, u32 Length);
HAL_Status HalLogUartIntRecv(HAL_LOG_UART_ADAPTER *pUartAdapter,
        u8  *pRxData, u32 Length);
VOID HalLogUartAbortIntSend(HAL_LOG_UART_ADAPTER *pUartAdapter);
VOID HalLogUartAbortIntRecv(HAL_LOG_UART_ADAPTER *pUartAdapter);
HAL_Status HalLogUartRstFIFO(HAL_LOG_UART_ADAPTER *pUartAdapter, u8 RstCtrl);
VOID HalLogUartEnable(HAL_LOG_UART_ADAPTER *pUartAdapter);
VOID HalLogUartDisable(HAL_LOG_UART_ADAPTER *pUartAdapter);
 */
extern VOID UartLogIrqHandleRam(VOID * Data);
// extern DiagPrintf();
// extern HalGetCpuClk(void);
// extern VectorIrqUnRegisterRtl8195A();
// extern VectorIrqRegisterRtl8195A();
// extern VectorIrqEnRtl8195A();
// extern RuartIsTimeout();
// extern HalDelayUs();
// extern HalPinCtrlRtl8195A();
// extern HalLogUartInit();
//-------------------------------------------------------------------------
// Data declarations
// extern ConfigDebugWarn;
// extern ConfigDebugErr;
// extern HalTimerOp;
// extern ConfigDebugInfo;
// extern UartLogIrqHandleRam;
//-------------------------------------------------------------------------
/*
 *  16 bytes FIFO ... 16*11/38400 = 0.004583 sec
 *  (0.005/5)*166666666 = 166666.666
 */
VOID HalLogUartWaitTxFifoEmpty(VOID) {
	int x = 16384;
	while((!(HAL_READ8(LOG_UART_REG_BASE, 0x14) & BIT6)) && x--);
}

//----- HalLogUartIrqRxRdyHandle
void HalLogUartIrqRxRdyHandle(HAL_LOG_UART_ADAPTER *pUartAdapter) {
	volatile uint8_t *pRxBuf = pUartAdapter->pRxBuf;
	if (pRxBuf != NULL) {
		while (pUartAdapter->RxCount) {
			if (!(HAL_UART_READ32(UART_INTERRUPT_EN_REG_OFF) & 1)) // v40003014
			{
				if (pUartAdapter->RxCount <= 7) {
					pUartAdapter->FIFOControl = pUartAdapter->FIFOControl
							& (~(FCR_RX_TRIG_MASK)); // & 0xFFFFFF3F;
					HAL_UART_WRITE32(UART_FIFO_CTL_REG_OFF,
							pUartAdapter->FIFOControl); // 40003008
				}
				break;
			}
			*pRxBuf++ = HAL_UART_READ32(UART_REV_BUF_OFF); // 40003000;
			--pUartAdapter->RxCount;
		}
		if (!pUartAdapter->RxCount) {
			pUartAdapter->IntEnReg = pUartAdapter->IntEnReg
					& (~(IER_ERBFI | IER_ELSI)); // & 0xFFFFFFFA;
			HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg); // 40003004
			if (pUartAdapter->pRxBuf != pRxBuf) {
				if (pUartAdapter->RxCompCallback)
					pUartAdapter->RxCompCallback(pUartAdapter->RxCompCbPara);
			}
			if (pUartAdapter->FIFOControl & FCR_RX_TRIG_MASK) // 0xC0
					{
				pUartAdapter->FIFOControl = pUartAdapter->FIFOControl
						& (~(FCR_RX_TRIG_MASK)); // & 0xFFFFFF3F;
				HAL_UART_WRITE32(UART_FIFO_CTL_REG_OFF,
						pUartAdapter->FIFOControl); // 40003008
			}
		}
		pUartAdapter->pRxBuf = pRxBuf;
	} else
		DBG_UART_WARN("HalLogUartIrqRxDataHandle: No RX Buffer\n");
}

//----- HalLogUartIrqHandle
void HalLogUartIrqHandle(VOID * Data) {
  PHAL_LOG_UART_ADAPTER pUartAdapter = (PHAL_LOG_UART_ADAPTER) Data;
  u32 iir = HAL_UART_READ32(UART_INTERRUPT_IDEN_REG_OFF) & 0x0F; // v40003008 & 0xF;
  switch(iir) {
	case IIR_MODEM_STATUS: // Clear to send or data set ready or ring indicator or data carrier detect.
		break;
	case IIR_NO_PENDING:
		return;
	case IIR_THR_EMPTY: // TX FIFO level lower than threshold or FIFO empty
	{
		volatile u8 * pTxBuf = pUartAdapter->pTxBuf;
		if (pTxBuf != NULL) {
			while (pUartAdapter->TxCount) {
				if (!(HAL_UART_READ32(UART_LINE_STATUS_REG_OFF) & LSR_THRE)) { // v40003014 & 0x20 // Transmit Holding Register Empty bit(IER_PTIME=0)
					HAL_UART_WRITE32(UART_REV_BUF_OFF, *pTxBuf++);
					pUartAdapter->TxCount--; // *((_DWORD *)v1 + 4);
				}
			}
			pUartAdapter->pTxBuf = pTxBuf;
			if (!(pUartAdapter->TxCount)) {
				pUartAdapter->IntEnReg = pUartAdapter->IntEnReg & (~IER_PTIME); // & 0xFFFFFF7F
				HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF,
						pUartAdapter->IntEnReg); // 40003004
				if (HAL_UART_READ32(UART_LINE_STATUS_REG_OFF) & LSR_THRE) { // 40003014 & 0x20 )
					pUartAdapter->IntEnReg = pUartAdapter->IntEnReg
							& (~IER_ETBEI); // & 0xFFFFFFFD
					HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF,
							pUartAdapter->IntEnReg); // 40003004
					if (pUartAdapter->TxCompCallback != NULL)
						pUartAdapter->TxCompCallback(
								pUartAdapter->TxCompCbPara);
				}
			}
		} else {
			pUartAdapter->IntEnReg = pUartAdapter->IntEnReg & (~IER_ETBEI); // & 0xFFFFFFFD
			HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg); // 40003004
		}
	}
		break;
	case IIR_RX_RDY:     // RX data ready
	case IIR_CHAR_TIMEOUT:  // timeout: Rx dara ready but no read
		HalLogUartIrqRxRdyHandle(pUartAdapter); // (HAL_LOG_UART_ADAPTER *)
		break;
	case IIR_RX_LINE_STATUS: // Overrun/parity/framing errors or break interrupt
		pUartAdapter->LineStatus = HAL_UART_READ32(UART_LINE_STATUS_REG_OFF); //  *((_BYTE *)v1 + 15) = v40003014; // LineStatusCallback
		if (pUartAdapter->LineStatusCallback != NULL)
			pUartAdapter->LineStatusCallback(pUartAdapter->LineStatusCbPara, pUartAdapter->LineStatus); // v3(*((_DWORD *)v1 + 17)); RxCompCallback
		break;
	case IIR_BUSY:
		break;
	default:
		DBG_UART_WARN("HalLogUartIrqHandle: UnKnown Interrupt ID!\n");
		break;
	}
	if (pUartAdapter->api_irq_handler)
		pUartAdapter->api_irq_handler(pUartAdapter->api_irq_id, iir);
}

//----- HalLogUartSetBaudRate
void HalLogUartSetBaudRate(HAL_LOG_UART_ADAPTER *pUartAdapter) {
	u32 clk4 = HalGetCpuClk() >> 2; // PLATFORM_CLOCK/2; // (unsigned int) HalGetCpuClk() >> 2; // div 4
	if (pUartAdapter->BaudRate == 0)
		pUartAdapter->BaudRate = DEFAULT_BAUDRATE;
	u32 br16 = pUartAdapter->BaudRate << 4; // * 16
	if ((br16 != 0) && (br16 <= clk4)) {
		unsigned int dll = clk4 / br16;
		if ((((10 * clk4) / br16) - (10 * dll)) > 4)
			dll++;
		HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF,
				HAL_UART_READ32(UART_LINE_CTL_REG_OFF) | LCR_DLAB);
		HAL_UART_WRITE32(UART_DLL_OFF, dll);
		HAL_UART_WRITE32(UART_DLH_OFF, dll >> 8);
		HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF,
				HAL_UART_READ32(UART_LINE_CTL_REG_OFF) & (~LCR_DLAB));
	} else
		DBG_UART_ERR("Cannot support Baud Sample Rate which bigger than Serial Clk!\n");
}

//----- HalLogUartSetLineCtrl
void HalLogUartSetLineCtrl(HAL_LOG_UART_ADAPTER *pUartAdapter) {
	HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF,
			pUartAdapter->Stop | pUartAdapter->Parity
					| pUartAdapter->DataLength);
}

//----- HalLogUartSetIntEn
void HalLogUartSetIntEn(HAL_LOG_UART_ADAPTER *pUartAdapter) {
	HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg);
}

//----- HalLogUartInitSetting
u32 HalLogUartInitSetting(HAL_LOG_UART_ADAPTER *pUartAdapter) {
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & (~BIT_SOC_LOG_UART_EN)); // 40000210 &= 0xFFFFEFFF;
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT_SOC_LOG_UART_EN); // 40000210 |= 0x1000;
	HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL,
			HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_ACTCK_LOG_UART_EN); // 40000230 |= 0x1000;
//  HalPinCtrlRtl8195A(LOG_UART, 0, 1); ????
	u32 clk4 = HalGetCpuClk() >> 2; // PLATFORM_CLOCK/2; // (unsigned int) HalGetCpuClk() >> 2; // div 4
	if (pUartAdapter->BaudRate == 0)
		pUartAdapter->BaudRate = DEFAULT_BAUDRATE;
	u32 br16 = pUartAdapter->BaudRate << 4; // * 16
	HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, 0); // 40003004 = 0;
	if (br16 <= clk4) {
		u32 dll = clk4 / br16;
		if ((((10 * clk4) / br16) - (10 * dll)) > 4)
			dll++;
		HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF, LCR_DLAB); // 4000300C = 128;
		HAL_UART_WRITE32(UART_DLL_OFF, dll); // v40003000 =
		HAL_UART_WRITE32(UART_DLH_OFF, dll >> 8); // v40003004 =
		HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF, 0);
	}
	HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF,
			pUartAdapter->Parity | pUartAdapter->Stop
					| pUartAdapter->DataLength); // 4000300C =
	HAL_UART_WRITE32(UART_FIFO_CTL_REG_OFF, pUartAdapter->FIFOControl);
	HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg);

	pUartAdapter->IrqHandle.IrqNum = UART_LOG_IRQ;
	pUartAdapter->IrqHandle.Data = (u32)pUartAdapter;
	pUartAdapter->IrqHandle.IrqFun = HalLogUartIrqHandle;
	pUartAdapter->IrqHandle.Priority = 14;
	VectorIrqUnRegisterRtl8195A(&pUartAdapter->IrqHandle);
	VectorIrqRegisterRtl8195A(&pUartAdapter->IrqHandle);
	VectorIrqEnRtl8195A(&pUartAdapter->IrqHandle);
	return 0;
}

//----- HalLogUartRecv
u32 HalLogUartRecv(HAL_LOG_UART_ADAPTER *pUartAdapter, u8 *pRxData, u32 Length, u32 TimeoutMS) {
	u32 result, i, timecnt, timeout; // v4 , v10
	volatile u8 LineStatus;

	if ((pRxData != NULL) && Length) {
		if (TimeoutMS - 1 > 0xFFFFFFFD)
			timeout = 0;
		else {
			timeout = 1000 * TimeoutMS / 0x1F;
			timecnt = HalTimerOp.HalTimerReadCount(1);
		}
		i = 0;
		while (i < Length) {
			LineStatus = HAL_UART_READ32(UART_LINE_STATUS_REG_OFF);
			if (LineStatus & LSR_DR) {
				pRxData[i++] = HAL_UART_READ32(UART_REV_BUF_OFF);
			} else if (timeout) {
				if (RuartIsTimeout(timecnt, timeout) == HAL_TIMEOUT) {
					DBG_UART_INFO("HalLogUartRecv: Rx Timeout, RxCount=%d\n",
							timecnt, timeout);
					break;
				}
			} else if (!TimeoutMS)
				break;
		}
		result = i;
		pUartAdapter->LineStatus = LineStatus;
	} else {
		DBG_UART_ERR("HalLogUartRecv: Err: pRxData=0x%x,  Length=%d!\n",
				pRxData, Length);
		result = 0;
	}
	return result;
}

//----- HalLogUartSend
u32 HalLogUartSend(HAL_LOG_UART_ADAPTER *pUartAdapter,
        u8 *pTxData, u32 Length, u32 TimeoutMS) {
	u32 i, result, timeout, timecnt;

	if ((pTxData != NULL) && Length) {
		if (TimeoutMS - 1 > 0xFFFFFFFD) {
			timeout = 0;
		} else {
			timeout = 1000 * TimeoutMS / 0x1F;
			timecnt = HalTimerOp.HalTimerReadCount(1); //  v4 = (*((int (__fastcall **)(_DWORD))&HalTimerOp + 2))(1);
		}
		i = 0;
		while (i < Length) {
			if (HAL_UART_READ32(UART_LINE_STATUS_REG_OFF) & LSR_THRE) { //  v40003014 & 0x20 )
				HAL_UART_WRITE32(UART_REV_BUF_OFF, pTxData[i++]); // 40003000 = pTxData[i++];
			} else if (timeout) {
				if (RuartIsTimeout(timecnt, timeout) == HAL_TIMEOUT) {
					DBG_UART_INFO("HalLogUartSend: Tx Timeout, TxCount=%d\n",
							timecnt, timeout);
					break;
				}
			} else if (!TimeoutMS)
				break;
		}
		if (i == Length)
			while (!(HAL_UART_READ32(UART_LINE_STATUS_REG_OFF) & LSR_TEMT)
					&& (!timeout
							|| RuartIsTimeout(timecnt, timeout) != HAL_TIMEOUT))
				; // 40003014 & 0x40
		result = i;
	} else {
		DBG_UART_ERR("HalLogUartSend: Err: pTxData=0x%x,  Length=%d!\n",
				pTxData, Length);
		result = 0;
	}
	return result;
}

//----- HalLogUartIntSend
HAL_Status HalLogUartIntSend(HAL_LOG_UART_ADAPTER *pUartAdapter,
        u8 *pTxData, u32 Length) {
	HAL_Status result;
	if (pTxData && Length) {
		pUartAdapter->TxCount = Length;
		pUartAdapter->pTxBuf = pTxData;
		pUartAdapter->pTxStartAddr = pTxData;
		pUartAdapter->IntEnReg = pUartAdapter->IntEnReg
				| (IER_PTIME | IER_ETBEI); // | 0x82;
		HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg); // 40003004
		result = HAL_OK; // 0;
	} else {
		DBG_UART_ERR("HalLogUartIntSend: Err: pTxData=0x%x, Length=%d!\n",
				pTxData, Length);
		result = HAL_ERR_PARA; //3;
	}
	return result;
}

//----- HalLogUartIntRecv
HAL_Status HalLogUartIntRecv(HAL_LOG_UART_ADAPTER *pUartAdapter,
        u8  *pRxData, u32 Length) {
	HAL_Status result;

	if (pRxData && Length) {
		pUartAdapter->pRxBuf = pRxData;
		pUartAdapter->pRxStartAddr = pRxData;
		pUartAdapter->RxCount = Length;
		if (Length > 8) {
			pUartAdapter->FIFOControl = pUartAdapter->FIFOControl
					| FCR_RX_TRIG_HF; // | 0x80 RCVR Trigger: FIFO 1/2 full
			HAL_UART_WRITE32(UART_FIFO_CTL_REG_OFF, pUartAdapter->FIFOControl); // 40003008
		}
		pUartAdapter->IntEnReg = pUartAdapter->IntEnReg
				| (IER_ERBFI | IER_ELSI); // | 5
		HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg); // 40003004
		result = HAL_OK;
	} else {
		DBG_UART_ERR("HalLogUartIntRecv: Err: pRxData=0x%x, Length=%d\n",
				pRxData, Length);
		result = HAL_ERR_PARA; // 3;
	}
	return result;
}

//----- HalLogUartAbortIntSend
void HalLogUartAbortIntSend(HAL_LOG_UART_ADAPTER *pUartAdapter) {
	pUartAdapter->IntEnReg = pUartAdapter->IntEnReg
			& (~(IER_PTIME | IER_ETBEI)); // & 0xFFFFFF7D
	HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg); // 40003004
}

//----- HalLogUartAbortIntRecv
void HalLogUartAbortIntRecv(HAL_LOG_UART_ADAPTER *pUartAdapter) {
	pUartAdapter->IntEnReg = pUartAdapter->IntEnReg & (~(IER_ERBFI | IER_ELSI)); // & 0xFFFFFFFA
	HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg); // 40003004
	while (pUartAdapter->RxCount
			&& (HAL_UART_READ32(UART_LINE_STATUS_REG_OFF) & LSR_DR)) {
		*pUartAdapter->pRxBuf++ = HAL_UART_READ32(UART_REV_BUF_OFF); // 40003000
		pUartAdapter->RxCount--;
	}
}

//----- HalLogUartRstFIFO
HAL_Status HalLogUartRstFIFO(HAL_LOG_UART_ADAPTER *pUartAdapter, u8 RstCtrl) {
	u32 RegValue = pUartAdapter->FIFOControl;
	if (RstCtrl & LOG_UART_RST_TX_FIFO)
		RegValue |= FCR_RST_TX;
	if (RstCtrl & LOG_UART_RST_RX_FIFO)
		RegValue |= FCR_RST_RX;
	HAL_UART_WRITE32(UART_FIFO_CTL_REG_OFF, RegValue); // 40003008 = RegValue;
	if (RstCtrl & LOG_UART_RST_TX_FIFO) {
		int i = 100;
		for (RegValue = HAL_UART_READ32(UART_LINE_STATUS_REG_OFF);
				!(RegValue & LSR_TEMT);
				HAL_UART_WRITE32(UART_LINE_STATUS_REG_OFF, RegValue)) {
			if (!(i--))
				break;
			HalDelayUs(100);
		}
	}
	return HAL_OK;
}

//----- HalLogUartEnable
void HalLogUartEnable(HAL_LOG_UART_ADAPTER *pUartAdapter) {
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT_SOC_LOG_UART_EN); // 40000210 |= 0x1000u;
	HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL,
			HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_ACTCK_LOG_UART_EN); // 40000230 |= 0x1000u;
	HalPinCtrlRtl8195A(LOG_UART, 0, 1);
}

//----- HalLogUartDisable
void HalLogUartDisable(HAL_LOG_UART_ADAPTER *pUartAdapter) {
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & (~BIT_SOC_LOG_UART_EN)); // 40000210 &= 0xFFFFEFFF;
	HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL,
			HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) & (~BIT_SOC_ACTCK_LOG_UART_EN)); // 40000230 &= 0xFFFFEFFF;
	HalPinCtrlRtl8195A(LOG_UART, 0, 0);
}

//----- HalInitLogUart
void HalInitLogUart(void) {
	IRQ_HANDLE UartIrqHandle;
	LOG_UART_ADAPTER UartAdapter;

	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & (~BIT_SOC_LOG_UART_EN)); // 40000210 &= 0xFFFFEFFF;
	HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN,
			HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT_SOC_LOG_UART_EN);
	HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL,
			HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_ACTCK_LOG_UART_EN); // 40000230 |= 0x1000u;
	HalPinCtrlRtl8195A(LOG_UART, 0, 1);
	UartAdapter.BaudRate = DEFAULT_BAUDRATE;
	UartAdapter.DataLength = UART_DATA_LEN_8BIT;
	UartAdapter.FIFOControl = FCR_RX_TRIG_MASK | FCR_FIFO_EN; // 0xC1;
	UartAdapter.IntEnReg = IER_ERBFI | IER_ELSI; // 5
	UartAdapter.Parity = LCR_PARITY_NONE;
	UartAdapter.Stop = LCR_STOP_1B;
	HalLogUartInit(UartAdapter);
	UartIrqHandle.IrqNum = UART_LOG_IRQ;
	UartIrqHandle.IrqFun = (IRQ_FUN) &UartLogIrqHandleRam;
	UartIrqHandle.Data = 0;
	UartIrqHandle.Priority = 0;
	VectorIrqUnRegisterRtl8195A(&UartIrqHandle);
	VectorIrqRegisterRtl8195A(&UartIrqHandle);
}

//----- HalDeinitLogUart
void HalDeinitLogUart(void) {
	HalLogUartWaitTxFifoEmpty();
	HalPinCtrlRtl8195A(LOG_UART, 0, 0);
}

#endif // CONFIG_LOG_UART_EN

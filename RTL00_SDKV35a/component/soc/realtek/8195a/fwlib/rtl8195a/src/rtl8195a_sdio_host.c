/*
 */
#include "rtl8195a.h"
#include "hal_sdio_host.h"
#include "rtl8195a_sdio_host.h"
//-------------------------------------------------------------------------
// Function declarations
/*
 signed int  SdioHostIsTimeout(uint32_t StartCount, uint32_t TimeoutCnt);
 void  SdioHostSendCmd(SDIO_HOST_CMD *Cmd);
 signed int  SdioHostGetResponse(void *Data, int RspType);
 void  SdioHostSdBusPwrCtrl(uint8_t En, int a2);
 int  SdioHostSdClkCtrl(void *Data, int En, int Divisor);
 int  SdioHostChkDataLineActive(uint32_t Timeout);
 int  SdioHostChkCmdInhibitCMD(void); //uint32_t Timeout);
 int  SdioHostChkCmdInhibitDAT(uint32_t Timeout);
 uint32_t  SdioHostIsrHandle(void *Data);
 int  HalSdioHostDeInitRtl8195a(void *Data);
 int  HalSdioHostEnableRtl8195a(void *Data);
 int  HalSdioHostDisableRtl8195a(void *Data);
 signed int  HalSdioHostIrqInitRtl8195a(void *Data);
 signed int  HalSdioHostInitHostRtl8195a(void *Data);
 int  HalSdioHostStopTransferRtl8195a(void *Data, uint32_t a2);
 signed int  SdioHostErrIntRecovery(void *Data, int a2, signed int a3);
 signed int  SdioHostChkXferComplete(void *Data, uint32_t Timeout, signed int a3);
 signed int  SdioHostChkCmdComplete(void *Data, uint32_t Timeout);
 int  SdioHostCardSelection(void *Data, int Select, int a3);
 int  SdioHostGetCSD(void *Data, uint32_t a2);
 int  HalSdioHostReadBlocksDmaRtl8195a(int result, uint32_t a2, int64 a3, uint32_t BlockCnta);
 int  HalSdioHostWriteBlocksDmaRtl8195a(int result, uint32_t a2, int64 a3, uint32_t BlockCnta);
 int  SdioHostSwitchFunction(void *Data, int Mode, int Fn2Sel, int Fn1Sel, uint8_t *StatusBuf);
 int  HalSdioHostGetCardStatusRtl8195a(void *Data, uint32_t a2, int a3);
 signed int  HalSdioHostInitCardRtl8195a(void *Data, int a2, int a3);
 int  HalSdioHostGetSdStatusRtl8195a(void *Data, uint32_t a2, int a3);
 signed int  HalSdioHostChangeSdClockRtl8195a(void *Data, int Frequency);
 int  HalSdioHostEraseRtl8195a(uint64_t EndAddr, int64 a2, int64 EndAddra);
 signed int  HalSdioHostGetWriteProtectRtl8195a(void *Data, uint32_t a2, int a3);
 int  HalSdioHostSetWriteProtectRtl8195a(void *Data, int Setting);
 */
// int DiagPrintf(const char *, ...); weak
// int  VectorIrqDisRtl8195A(u32); weak
// int  VectorIrqUnRegisterRtl8195A(u32); weak
// int  HalPinCtrlRtl8195A(u32, u32, u32); weak
// int VectorIrqRegisterRtl8195A(void); weak
// int  VectorIrqEnRtl8195A(u32); weak
// int  HalDelayUs(u32); weak
//-------------------------------------------------------------------------
// Data declarations
//-------------------------------------------------------------------------
#define HAL_SDIOH_REG32(a) (*(volatile unsigned int *)(SDIO_HOST_REG_BASE+a))
#define HAL_SDIOH_REG16(a) (*(volatile unsigned short *)(SDIO_HOST_REG_BASE+a))
#define HAL_SDIOH_REG8(a) (*(volatile unsigned char *)(SDIO_HOST_REG_BASE+a))


//-----SdioHostIsTimeout(StartCount, TimeoutCnt)
HAL_Status SdioHostIsTimeout(u32 StartCount, u32 TimeoutCnt) {
	u32 t1, t2;
	HAL_Status result;

	t1 = HalTimerOp.HalTimerReadCount(1);
	t2 = StartCount - t1;
	if (StartCount < t1)
		t2--;
	if (TimeoutCnt >= t2)
		result = HAL_OK;
	else
		result = HAL_TIMEOUT;
	return result;
}

//----- SdioHostSendCmd(PSDIO_HOST_CMD)
void SdioHostSendCmd(PSDIO_HOST_CMD Cmd) {
	HAL_SDIOH_REG32(REG_SDIO_HOST_ARG) = Cmd->Arg; // 40058008 = Cmd->Arg
	HAL_SDIOH_REG16(REG_SDIO_HOST_CMD) = *(u16 *) &Cmd->CmdFmt; // & 0x3FFB;
}

//-----
HAL_Status SdioHostGetResponse(void *Data, int RspType) {
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	HAL_Status result;

	if (psha) {
		psha->Response[0] = HAL_SDIOH_REG32(REG_SDIO_HOST_RSP0); // 40058010;
		psha->Response[1] = HAL_SDIOH_REG32(REG_SDIO_HOST_RSP2);
		if (RspType == 1) {
			psha->Response[2] = HAL_SDIOH_REG32(REG_SDIO_HOST_RSP4);
			psha->Response[3] = HAL_SDIOH_REG32(REG_SDIO_HOST_RSP6);
		}
		result = HAL_OK;
	} else
		result = HAL_ERR_PARA;
	return result;
}

//-----
//void SdioHostSdBusPwrCtrl(uint8_t En) {
void SdioHostSdBusPwrCtrl(void) {
	HAL_SDIOH_REG8(REG_SDIO_HOST_PWR_CTRL) &= ~ PWR_CTRL_SD_BUS_PWR;
	if (HAL_SDIOH_REG32(REG_SDIO_HOST_CAPABILITIES) & CAPA_VOLT_SUPPORT_18V) {
		DBG_SDIO_WARN("Supply SD bus voltage: 1.8V\n");
		HAL_SDIOH_REG8(REG_SDIO_HOST_PWR_CTRL) = VOLT_18V << 1;
	}
	else if (HAL_SDIOH_REG32(REG_SDIO_HOST_CAPABILITIES) & CAPA_VOLT_SUPPORT_30V) {
		DBG_SDIO_WARN("Supply SD bus voltage: 3.0V\n");
		HAL_SDIOH_REG8(REG_SDIO_HOST_PWR_CTRL) = VOLT_30V << 1;
	}
	else if (HAL_SDIOH_REG32(REG_SDIO_HOST_CAPABILITIES) & CAPA_VOLT_SUPPORT_33V) {
		DBG_SDIO_WARN("Supply SD bus voltage: 3.3V\n");
		HAL_SDIOH_REG8(REG_SDIO_HOST_PWR_CTRL) = VOLT_33V << 1;
	}
	else DBG_SDIO_ERR("No supported voltage\n");
	HAL_SDIOH_REG8(REG_SDIO_HOST_PWR_CTRL) |= PWR_CTRL_SD_BUS_PWR;
}

//-----
HAL_Status SdioHostSdClkCtrl(void *Data, int En, u8 Divisor) { // SD_CLK_DIVISOR
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	if (HAL_SDIOH_REG32(REG_SDIO_HOST_PRESENT_STATE)
			& (PRES_STATE_CMD_INHIBIT_CMD | PRES_STATE_CMD_INHIBIT_DAT)) {
		return HAL_BUSY;
	} else {
		if (!En) {
			HAL_SDIOH_REG16(REG_SDIO_HOST_CLK_CTRL) &= ~CLK_CTRL_SD_CLK_EN;
		}
		else {
			switch(Divisor) {
			case BASE_CLK:
				psha->CurrSdClk = SD_CLK_41_6MHZ;
				break;
			case BASE_CLK_DIVIDED_BY_2:
				psha->CurrSdClk = SD_CLK_20_8MHZ;
				break;
			case BASE_CLK_DIVIDED_BY_4:
				psha->CurrSdClk = SD_CLK_10_4MHZ;
				break;
			case BASE_CLK_DIVIDED_BY_8:
				psha->CurrSdClk = SD_CLK_5_2MHZ;
				break;
			case BASE_CLK_DIVIDED_BY_16:
				psha->CurrSdClk = SD_CLK_2_6MHZ;
				break;
			case BASE_CLK_DIVIDED_BY_32:
				psha->CurrSdClk = SD_CLK_1_3MHZ;
				break;
			case BASE_CLK_DIVIDED_BY_64:
				psha->CurrSdClk = SD_CLK_650KHZ;
				break;
			case BASE_CLK_DIVIDED_BY_128:
				psha->CurrSdClk = SD_CLK_325KHZ;
				break;
			case BASE_CLK_DIVIDED_BY_256:
				psha->CurrSdClk = SD_CLK_162KHZ;
				break;
			default:
				DBG_SDIO_ERR("Unsupported SDCLK divisor!\n");
				Divisor = 0;
				psha->CurrSdClk = SD_CLK_41_6MHZ;
			}
		}
		HAL_SDIOH_REG16(REG_SDIO_HOST_CLK_CTRL) = (Divisor << 8) | CLK_CTRL_SD_CLK_EN | CLK_CTRL_INTERAL_CLK_EN;
	}
	return HAL_OK;
}

#define SD_CHK_TIMEOUT 3225

//----- SdioHostChkDataLineActive
HAL_Status SdioHostChkDataLineActive(void) {
	HAL_Status result;
	u32 t1 = HalTimerOp.HalTimerReadCount(1);
	do {
		if ((HAL_SDIOH_REG32(REG_SDIO_HOST_PRESENT_STATE)
				& PRES_STATE_DAT_LINE_ACTIVE) == 0)
			break;
		result = SdioHostIsTimeout(t1, SD_CHK_TIMEOUT);
	} while (result != HAL_TIMEOUT);
	return result;
}

//----- SdioHostChkCmdInhibitCMD
HAL_Status SdioHostChkCmdInhibitCMD(void) {
	HAL_Status result;
	u32 t1 = HalTimerOp.HalTimerReadCount(1);
	do {
		if ((HAL_SDIOH_REG32(REG_SDIO_HOST_PRESENT_STATE)
				& PRES_STATE_CMD_INHIBIT_CMD) == 0)
			break;
		result = SdioHostIsTimeout(t1, SD_CHK_TIMEOUT);
	} while (result != HAL_TIMEOUT);
	return result;
}

//----- SdioHostChkCmdInhibitDAT
int SdioHostChkCmdInhibitDAT(void) {
	HAL_Status result;
	u32 t1 = HalTimerOp.HalTimerReadCount(1);
	do {
		if ((HAL_SDIOH_REG32(REG_SDIO_HOST_PRESENT_STATE)
				& PRES_STATE_CMD_INHIBIT_DAT) == 0)
			break;
		result = SdioHostIsTimeout(t1, SD_CHK_TIMEOUT);
	} while (result != HAL_TIMEOUT);
	return result;
}

//----- HalSdioHostDeInitRtl8195a
HAL_Status HalSdioHostDeInitRtl8195a(IN VOID *Data) {
	HAL_Status ret; // r4@1

	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	HAL_SDIOH_REG8(REG_SDIO_HOST_PWR_CTRL) &= ~PWR_CTRL_SD_BUS_PWR;
	ret = SdioHostSdClkCtrl(psha, 0, BASE_CLK);
	if (ret == HAL_OK) {
		if (psha) {
			VectorIrqDisRtl8195A(&psha->IrqHandle);
			VectorIrqUnRegisterRtl8195A(psha);
			HAL_SDIOH_REG16(REG_SDIO_HOST_CLK_CTRL) &= ~CLK_CTRL_INTERAL_CLK_EN;
			HAL_SDIOH_REG32(0x1000)  &= 0xFFFFFBFF; // v40059000
			HAL_WRITE32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN,
					HAL_READ32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN) & (~BIT_SOC_HCI_SDIOH_EN));
			HalPinCtrlRtl8195A(SDIOH, 0, 0);
			ACTCK_SDIOH_CCTRL(OFF);
			SLPCK_SDIOH_CCTRL(OFF);
		} else {
			ret = HAL_ERR_PARA;
		}
	}
	return ret;
}

//----- HalSdioHostEnableRtl8195a
HAL_Status HalSdioHostEnableRtl8195a(IN VOID *Data) // // PHAL_SDIO_HOST_ADAPTER Data
{
//	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	ACTCK_SDIOH_CCTRL(ON);
	SLPCK_SDIOH_CCTRL(ON);
	HAL_SDIOH_REG16(REG_SDIO_HOST_CLK_CTRL) |= CLK_CTRL_INTERAL_CLK_EN;
	while (!(HAL_SDIOH_REG16(REG_SDIO_HOST_CLK_CTRL) & CLK_CTRL_INTERAL_CLK_STABLE));
	return SdioHostSdClkCtrl(Data, 1, BASE_CLK_DIVIDED_BY_2);
}

//----- HalSdioHostDisableRtl8195a
HAL_Status HalSdioHostDisableRtl8195a(IN VOID *Data) {
	int result; // r0@1

	result = SdioHostSdClkCtrl(Data, 0, BASE_CLK);
	if (result == HAL_OK) {
		HAL_SDIOH_REG16(REG_SDIO_HOST_CLK_CTRL) &= ~CLK_CTRL_INTERAL_CLK_EN;
		ACTCK_SDIOH_CCTRL(OFF);
		SLPCK_SDIOH_CCTRL(OFF);
	}
	return result;
}

//----- HalSdioHostIrqInitRtl8195a
HAL_Status HalSdioHostIrqInitRtl8195a(IN VOID *Data) // PIRQ_HANDLE Data
{
	HAL_Status result;
	PIRQ_HANDLE pih = Data;
	if (pih) {
		pih->Data = Data;
		pih->IrqNum = SDIO_HOST_IRQ;
		pih->IrqFun = SdioHostIsrHandle;
		pih->Priority = 6;
		VectorIrqRegisterRtl8195A((PIRQ_HANDLE) pih);
		VectorIrqEnRtl8195A((PIRQ_HANDLE) pih);
		result = HAL_OK;
	} else
		result = HAL_ERR_PARA;
	return result;
}

//----- HalSdioHostInitHostRtl8195a
HAL_Status HalSdioHostInitHostRtl8195a(IN VOID *Data) {
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;

	HAL_WRITE32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0,
			HAL_READ32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0) & (~BIT_SOC_ACTCK_SDIO_DEV_EN));
	HAL_WRITE32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN,
			HAL_READ32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN) & (~BIT_SOC_HCI_SDIOD_ON_EN));

	HAL_WRITE32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN,
			HAL_READ32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN) & (~BIT_SOC_HCI_SDIOD_OFF_EN));

	HAL_WRITE32(PERI_ON_BASE, REG_HCI_PINMUX_CTRL,
			HAL_READ32(PERI_ON_BASE, REG_HCI_PINMUX_CTRL) & (~BIT_HCI_SDIOD_PIN_EN));

	HAL_WRITE32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0,
			HAL_READ32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0) | BIT_SOC_ACTCK_SDIO_HST_EN);
	HAL_WRITE32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0,
			HAL_READ32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0) | BIT_SOC_SLPCK_SDIO_HST_EN);
	HalPinCtrlRtl8195A(SDIOH, 0, 1);
	HAL_WRITE32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN,
			HAL_READ32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN) | BIT_SOC_HCI_SDIOH_EN);
	HAL_SDIOH_REG8(REG_SDIO_HOST_SW_RESET) |= SW_RESET_FOR_ALL; //4005802F |= 1;
	int x = 1000;
	while (HAL_SDIOH_REG8(REG_SDIO_HOST_SW_RESET) & SW_RESET_FOR_ALL) {
		if (x-- == 0) {
			DBG_SDIO_ERR("SD host initialization FAIL!\n");
			return HAL_TIMEOUT;
		}
	}
	HalSdioHostIrqInitRtl8195a(&psha->IrqHandle);
	HAL_SDIOH_REG16(REG_SDIO_HOST_NORMAL_INT_STATUS_EN)
			= NOR_INT_STAT_EN_CMD_COMP
			| NOR_INT_STAT_EN_XFER_COMP
			| NOR_INT_STAT_EN_CARD_REMOVAL
			| NOR_INT_STAT_EN_CARD_INT; // 0xC3;
	HAL_SDIOH_REG16(REG_SDIO_HOST_NORMAL_INT_SIG_EN)
			= NOR_INT_SIG_EN_CMD_COMP
			| NOR_INT_SIG_EN_XFER_COMP
			| NOR_INT_SIG_EN_CARD_REMOVAL
			| NOR_INT_SIG_EN_CARD_INT;	// 195;


	HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS_EN) = 0x17F;
	HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_SIG_EN) = 0x17F;
	HAL_SDIOH_REG16(REG_SDIO_HOST_CLK_CTRL) |= CLK_CTRL_INTERAL_CLK_EN;
	x = 1000;
	while (!(HAL_SDIOH_REG16(REG_SDIO_HOST_CLK_CTRL)
			& CLK_CTRL_INTERAL_CLK_STABLE)) {
		if (x-- == 0) {
			DBG_SDIO_ERR("SD host initialization FAIL!\n");
			return HAL_TIMEOUT;
		}
	}
	HAL_SDIOH_REG32(0x1000) |= 0x400; // 40059000 |= 0x400;
	if (HAL_SDIOH_REG32(REG_SDIO_HOST_CAPABILITIES) & CAPA_ADMA2_SUPPORT)
		HAL_SDIOH_REG16(REG_SDIO_HOST_HOST_CTRL) = 0x10; // 32-bit Address ADMA2 is selected
	HAL_SDIOH_REG8(REG_SDIO_HOST_TIMEOUT_CTRL) = 0x0E; // TMCLK x 2^27
	return HAL_OK;
}

//----- HalSdioHostStopTransferRtl8195a
HAL_Status HalSdioHostStopTransferRtl8195a(IN VOID *Data) {
	HAL_Status result;
	SDIO_HOST_CMD Cmd;
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;

	if (psha) {
		result = SdioHostChkCmdInhibitCMD(); //(uint32_t) Data);
		if (result == HAL_OK) {
			result = SdioHostChkCmdInhibitDAT();
			if (result == HAL_OK) {

				psha->CmdCompleteFlg = 0;
				psha->XferType = SDIO_XFER_NOR;
				psha->XferCompleteFlg = 0;

				Cmd.CmdFmt.RespType = RSP_LEN_48_CHK_BUSY;
				Cmd.CmdFmt.Rsvd0 = 0;
				Cmd.CmdFmt.CmdCrcChkEn = 1;
				Cmd.CmdFmt.CmdIdxChkEn = 0;
				Cmd.CmdFmt.DataPresent = NO_DATA;
				Cmd.CmdFmt.CmdType = ABORT;
				Cmd.CmdFmt.CmdIdx = CMD_STOP_TRANSMISSION;
				Cmd.CmdFmt.Rsvd1 = 0;
				Cmd.Arg = 0;

				SdioHostSendCmd(&Cmd);
			}
		}
	} else {
		result = HAL_ERR_PARA;
	}
	return result;
}

//----- SdioHostErrIntRecovery
HAL_Status SdioHostErrIntRecovery(void *Data) {
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	int t;

	if (!psha)	return HAL_ERR_PARA;
	DBG_SDIO_ERR("Recovering error interrupt...\n");
	u16 ierr = HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS); // v40058032;

	if (HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS)
			& (   ERR_INT_STAT_CMD_TIMEOUT
				| ERR_INT_STAT_CMD_CRC
				| ERR_INT_STAT_CMD_END_BIT
				| ERR_INT_STAT_CMD_IDX)) {
		HAL_SDIOH_REG8(REG_SDIO_HOST_SW_RESET) |= SW_RESET_FOR_CMD;
		int t = 0;
		while((HAL_SDIOH_REG8(REG_SDIO_HOST_SW_RESET) & SW_RESET_FOR_CMD)) {
			if(++t > 1000) {
				DBG_SDIO_ERR("CMD line reset timeout!\n");
				return HAL_TIMEOUT;
			}
		}
	}
	if (HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS)
			& (	  ERR_INT_STAT_DATA_TIMEOUT
				| ERR_INT_STAT_DATA_CRC
				| ERR_INT_STAT_DATA_END_BIT)) {
		HAL_SDIOH_REG8(REG_SDIO_HOST_SW_RESET) |= SW_RESET_FOR_DAT;
		t = 0;
		while((HAL_SDIOH_REG8(REG_SDIO_HOST_SW_RESET) & SW_RESET_FOR_DAT)) {
			if(++t > 1000) {
				DBG_SDIO_ERR("DAT line reset timeout!\n");
				return HAL_TIMEOUT;
			}
		}
	}

	DBG_SDIO_ERR("Error interrupt status: 0x%04X\n", HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS));

	HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS) = ierr;
	psha->ErrIntFlg = 0;
	int result = HalSdioHostStopTransferRtl8195a(psha);
	if (result == HAL_OK) {
		t = 0;
		while(HAL_SDIOH_REG32(REG_SDIO_HOST_PRESENT_STATE)
				& (PRES_STATE_CMD_INHIBIT_CMD | PRES_STATE_CMD_INHIBIT_DAT)) {
			if(++t > 1000) break;
		}
		if(HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS)
				& (   ERR_INT_STAT_CMD_TIMEOUT
					| ERR_INT_STAT_CMD_CRC
					| ERR_INT_STAT_CMD_END_BIT
					| ERR_INT_STAT_CMD_IDX)) {
			DBG_SDIO_ERR("Non-recoverable error(1)!\n");
			return HAL_ERR_UNKNOWN;
		}
		if(HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS)
				&  ERR_INT_STAT_DATA_TIMEOUT) {
			DBG_SDIO_ERR("Non-recoverable error(2)!\n");
			return HAL_ERR_UNKNOWN;
		}
	    HalDelayUs(50);
	    if((HAL_SDIOH_REG32(REG_SDIO_HOST_PRESENT_STATE) & 0xF00000) != 0xF00000) {
			DBG_SDIO_ERR("Non-recoverable error(3)!\n");
			return HAL_ERR_UNKNOWN;
	    }
		DBG_SDIO_ERR("Recoverable error...\n");
		HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_SIG_EN) = 0x17F;
		return 16;
	}
	DBG_SDIO_ERR("Stop transmission error!\n");
	return HAL_ERR_UNKNOWN;
}

//----- (0000028C) --------------------------------------------------------
void SdioHostIsrHandle(void *Data) {
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	u16 status = HAL_SDIOH_REG16(REG_SDIO_HOST_NORMAL_INT_STATUS);// v40058030;

	HAL_SDIOH_REG16(REG_SDIO_HOST_NORMAL_INT_SIG_EN) = 0;
	if (status) {
		if (status & NOR_INT_STAT_CMD_COMP)
			psha->CmdCompleteFlg = 1;
		if (status & NOR_INT_STAT_XFER_COMP) {
			psha->XferCompleteFlg = 1;
			if ((status & NOR_INT_STAT_ERR_INT) == 0) {
				if (psha->XferCompCallback)
					psha->XferCompCallback(psha->XferCompCbPara);

			} else if (HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS) &
					( ERR_INT_STAT_DATA_TIMEOUT
					| ERR_INT_STAT_DATA_CRC
					| ERR_INT_STAT_DATA_END_BIT)) {
				DBG_SDIO_ERR("\r[SDIO Err]XFER CP with ErrIntVal: 0x%04X /0x%04X -- TYPE 0x%02X\n",
					status,
					HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS),
					psha->XferType);
				psha->errType = SDIO_ERR_DAT_CRC;
				if (psha->ErrorCallback)
					psha->ErrorCallback(psha->ErrorCbPara);
			}

		}
		if (status & NOR_INT_STAT_CARD_INSERT) // 0x40
		{
			SdioHostSdClkCtrl(psha, 1, BASE_CLK_DIVIDED_BY_128); // BASE_CLK_DIVIDED_BY_128
			SdioHostSdBusPwrCtrl();
			if (psha->CardInsertCallBack)
				psha->CardInsertCallBack(psha->CardInsertCbPara);
		}
		if (status & NOR_INT_STAT_CARD_REMOVAL) // 0x80
		{
			HAL_SDIOH_REG8(REG_SDIO_HOST_PWR_CTRL) &= ~PWR_CTRL_SD_BUS_PWR;
			SdioHostSdClkCtrl(psha, 0, BASE_CLK); // BASE_CLK
			if (psha->CardRemoveCallBack)
				psha->CardRemoveCallBack(psha->CardRemoveCbPara);
		}
		if (status & NOR_INT_STAT_CARD_INT) // 0x100 )
		{
			u16 val = HAL_SDIOH_REG16(REG_SDIO_HOST_NORMAL_INT_STATUS_EN);
			HAL_SDIOH_REG16(REG_SDIO_HOST_NORMAL_INT_STATUS_EN) = val & (~NOR_INT_STAT_EN_CARD_INT);
			DBG_SDIO_ERR("CARD INT: 0x%04X\n", status);
			HAL_SDIOH_REG16(REG_SDIO_HOST_NORMAL_INT_STATUS_EN) = val;

		}
		if (status & NOR_INT_STAT_ERR_INT) // 0x8000 )
		{
			HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_SIG_EN) = 0;
			u16 err = HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS);

			DBG_SDIO_ERR("\r[SDIO Err]XFER CP with ErrIntVal: 0x%04X /0x%04X -- TYPE 0x%02X\n",
				status,
				err,
				psha->CmdCompleteFlg);

			if (psha->CmdCompleteFlg) {
				SdioHostErrIntRecovery(psha);
				goto ir_end;
			}
	        DiagPrintf("\r[SDIO Err]Read/Write command Error\n");

			psha->ErrIntFlg = 1;
		}
	}
ir_end:
	HAL_SDIOH_REG16(REG_SDIO_HOST_NORMAL_INT_SIG_EN)
			= NOR_INT_SIG_EN_CMD_COMP
			| NOR_INT_SIG_EN_XFER_COMP
			| NOR_INT_SIG_EN_CARD_REMOVAL
			| NOR_INT_SIG_EN_CARD_INT;	// 195;
}

//----- SdioHostChkCmdComplete
HAL_Status SdioHostChkCmdComplete(void *Data, uint32_t Timeout) {
{
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	uint32_t TimeoutCnt;
	uint32_t StartCount;
	HAL_Status result;


	if (psha) {
		if (Timeout == SDIO_HOST_WAIT_FOREVER) {
			StartCount = 0;
			TimeoutCnt = 0;
		} else {
			TimeoutCnt = 1000 * Timeout / 31;
			StartCount = HalTimerOp.HalTimerReadCount(1); //	v6 = (*((int (**)(u32)) &HalTimerOp + 2))(1);
		}
		while(psha->CmdCompleteFlg == 0) {
			if(psha->ErrIntFlg)
				return SdioHostErrIntRecovery(psha);
			if(TimeoutCnt) {
				result = SdioHostIsTimeout(StartCount, TimeoutCnt);
				if (result == HAL_TIMEOUT)
					return result;
			} else if(Timeout == 0) {
				result = HAL_BUSY;
				return result;
			}
		}
		result = HAL_OK;
	} else
		result = HAL_ERR_PARA;
	return result;
}

//----- SdioHostGetCSD
HAL_Status SdioHostGetCSD(void *Data)
{
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	HAL_Status result;
	SDIO_HOST_CMD Cmd;

	if (psha) {
		result = SdioHostChkCmdInhibitCMD();
		if (result == HAL_OK) {

			Cmd.CmdFmt.RespType = RSP_LEN_136;
			Cmd.CmdFmt.Rsvd0 = 0;
			Cmd.CmdFmt.CmdCrcChkEn = 1;
			Cmd.CmdFmt.CmdIdxChkEn = 0;
			Cmd.CmdFmt.DataPresent = NO_DATA;
			Cmd.CmdFmt.CmdType = NORMAL;
			Cmd.CmdFmt.CmdIdx = CMD_SEND_CSD;
			Cmd.CmdFmt.Rsvd1 = 0;
			Cmd.Arg = psha->RCA << 16;

			psha->CmdCompleteFlg = 0;
			psha->XferType = 0;

			SdioHostSendCmd(&Cmd);
			result = SdioHostChkCmdComplete(psha, 50);
			if(result == HAL_OK) {
				SdioHostGetResponse(psha, Cmd.CmdFmt.RespType);
				psha->Csd[15] = 1;
				uint32 x = psha->Response[3];
				psha->Csd[0] = x >> 16;
				psha->Csd[1] = x >> 8;
				psha->Csd[2] = x;
				x = psha->Response[2];
				psha->Csd[3] = x >> 24;
				psha->Csd[4] = x >> 16;
				psha->Csd[5] = x >> 8;
				psha->Csd[6] = x;
				x = psha->Response[1];
				psha->Csd[7] = x >> 24;
				psha->Csd[8] = x >> 16;
				psha->Csd[9] = x >> 8;
				psha->Csd[10] = x;
				x = psha->Response[0];
				psha->Csd[11] = x >> 24;
				psha->Csd[12] = x >> 16;
				psha->Csd[13] = x >> 8;
				psha->Csd[14] = x;
			}
		}
	} else
		result = HAL_ERR_PARA;
	return result;
}

//----- SdioHostChkXferComplete
HAL_Status SdioHostChkXferComplete(void *Data)
{
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	uint32_t StartCount;
	HAL_Status result;
	if (psha) {
		StartCount = HalTimerOp.HalTimerReadCount(1);
		while(psha->XferCompleteFlg == 0
				&& (HAL_SDIOH_REG32(REG_SDIO_HOST_PRESENT_STATE)
						& PRES_STATE_DAT0_SIGNAL_LEVEL) == 0) {
			if(psha->ErrIntFlg)
				return SdioHostErrIntRecovery(psha);
			result = SdioHostIsTimeout(StartCount, 161290);
			if (result == HAL_TIMEOUT)
				return result;
		}
		result = HAL_OK;
	} else {
		result = HAL_ERR_PARA;
	}
	return result;
}

//----- SdioHostCardSelection
HAL_Status SdioHostCardSelection(void *Data, int Select) {
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	HAL_Status result;
	SDIO_HOST_CMD Cmd; // [sp+0h] [bp-18h]@1

	if (psha) {
		result = SdioHostChkCmdInhibitCMD();
		if (result == HAL_OK) {
			psha->CmdCompleteFlg = 0;
			psha->XferType = SDIO_XFER_NOR;
			psha->XferCompleteFlg = 0;

			if (Select == 1) {
				result = SdioHostChkCmdInhibitDAT(0);
				if (result == HAL_OK) {

					Cmd.CmdFmt.RespType = RSP_LEN_48_CHK_BUSY;
					Cmd.CmdFmt.Rsvd0 = 0;
					Cmd.CmdFmt.CmdCrcChkEn = 1;
					Cmd.CmdFmt.CmdIdxChkEn = 1;
					Cmd.CmdFmt.DataPresent = NO_DATA;
					Cmd.CmdFmt.CmdType = NORMAL;
					Cmd.CmdFmt.CmdIdx = CMD_SELECT_DESELECT_CARD;
					Cmd.CmdFmt.Rsvd1 = 0;
					Cmd.Arg = psha->RCA << 16;


					SdioHostSendCmd(&Cmd);
					result = SdioHostChkCmdComplete(psha, 50);
					if(result == HAL_OK) {
						result = SdioHostChkXferComplete(psha);
						if(result != HAL_OK) return result;
						result = SdioHostGetResponse(psha, Cmd.CmdFmt.RespType);
						if(result != HAL_OK) return result;
						if((uint8)psha->Response[1] != 7) return result;
						else {
							DBG_SDIO_ERR("Command index error!\n");
							result = HAL_ERR_UNKNOWN;
						}
					}
				}
			} else {

				Cmd.CmdFmt.RespType = NO_RSP;
				Cmd.CmdFmt.Rsvd0 = 0;
				Cmd.CmdFmt.CmdCrcChkEn = 0;
				Cmd.CmdFmt.CmdIdxChkEn = 0;
				Cmd.CmdFmt.DataPresent = NO_DATA;
				Cmd.CmdFmt.CmdType = NORMAL;
				Cmd.CmdFmt.CmdIdx = CMD_SELECT_DESELECT_CARD;
				Cmd.CmdFmt.Rsvd1 = 0;
				Cmd.Arg = psha->RCA << 16;


				SdioHostSendCmd(&Cmd);
				result = SdioHostChkCmdComplete(psha, 50);
			}
		}
	} else result = HAL_ERR_PARA;
	return result;
}





















//----- (00000D34) --------------------------------------------------------
HAL_Status SdioHostSwitchFunction(void *Data, int Mode, int Fn2Sel, int Fn1Sel,
		uint8_t *StatusBuf)
{
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	HAL_Status result; // r0@3
	SDIO_HOST_CMD Cmd; // [sp+0h] [bp-20h]@1

	u8 *v5; // r4@1
	uint32_t v6; // r0@1
	int v7; // r5@1
	int v8; // r6@1
	char v9; // r7@3
	char v11; // r3@5
	uint32_t v12; // r1@5
	signed int v13; // r2@6
	uint32_t v14; // r1@6


	int v16; // [sp+8h] [bp-18h]@1

	*(u32 *) &Cmd.CmdFmt = Data;
	Cmd.Arg = Mode;
	v16 = Fn2Sel;
	v5 = Data;
	v6 = *((u32 *) Data + 4);
	v7 = Mode;
	v8 = Fn2Sel;
	if (!v6 || ((u8) Fn1Sel | (u8) v6) & 3) {
		result = 3;
	} else {
		HAL_SDIOH_REG32(REG_SDIO_HOST_ADMA_SYS_ADDR) = (uint32)psha->AdmaDescTbl; // v40058058 = v6;
		HAL_SDIOH_REG16(REG_SDIO_HOST_BLK_SIZE) = 64; // v40058004 = 64;
		HAL_SDIOH_REG32(REG_SDIO_HOST_XFER_MODE) = XFER_MODE_DATA_XFER_DIR | XFER_MODE_DMA_EN; //	v4005800C = 17;

		v9 = *(u8 *) v6;
		*(u16 *) (v6 + 2) = 64;
		*(u8 *) v6 = ((v9 | 3) & 0xFB | 4 * ((Fn1Sel | v6) & 1)) & 0xEF
				| 16 * ((Fn1Sel | v6) & 1) | 0x20;
		*(u32 *) (v6 + 4) = Fn1Sel;
		result = SdioHostChkCmdInhibitCMD();//v6);
		if (!result) {
			result = SdioHostChkDataLineActive();
			if (result == HAL_OK) {
				Cmd.CmdFmt = (SDIO_HOST_CMD_FMT) ((*(u8 *) &Cmd.CmdFmt & 0xF4
						| 0x3A) & 0x3F);
				v11 = *((u8 *) &Cmd.CmdFmt + 1);
				v5[128] = 0;
				*((u8 *) &Cmd.CmdFmt + 1) = v11 & 0xC0 | 6;
				v5[129] = 0;
				Cmd.Arg = v8 | 0xFFFFF0 | (v7 << 31);
				SdioHostSendCmd(&Cmd);
				result = SdioHostChkCmdComplete(v5);//, v12);
				if (!result) {
					SdioHostGetResponse(v5, *(u8 *) &Cmd.CmdFmt & 3);
					result = SdioHostChkXferComplete(v5, 0x1388u, v13);
					if (result) {
						if (result != 16) {
							if (HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS) & ERR_INT_STAT_ADMA) {
								HAL_SDIOH_REG16(REG_SDIO_HOST_ERROR_INT_STATUS) = ERR_INT_STAT_ADMA;
								if (HalSdioHostStopTransferRtl8195a(psha)) {
									DBG_SDIO_ERR("Stop transmission error!\n");
								}
							}
						}
						result = 238;
					}
				}
			}
		}
	}
	return result;
}





//----- (000009CC) --------------------------------------------------------
HAL_Status HalSdioHostReadBlocksDmaRtl8195a(IN VOID *Data, IN u64 ReadAddr,
IN u32 BlockCnt) {
	int64 v4; // r4@1
	int v5; // r6@1
	int v6; // r7@3
	char v7; // r3@12
	uint32_t v8; // r1@12
	signed int v9; // r2@13
	int v10; // r1@16
	const char *v11; // r0@17
	char v12; // r3@20
	uint32_t v13; // r1@20
	signed int v14; // r2@23
	uint32_t v15; // r1@23
	SDIO_HOST_CMD Cmd; // [sp+0h] [bp-20h]@1

	HAL_Status result;

	*(u32 *) &Cmd.CmdFmt = result;
	Cmd.Arg = a2;
	v4 = a3;
	v5 = result;
	if (!result)
		return 3;
	if (BlockCnt >= 0x10000)
		return HAL_ERR_PARA;
	v6 = *(u32 *) (result + 16);
	if (v6 << 30 || *(u32 *) (*(u32 *) (result + 16) + 4) << 30)
		return 3;
	if (*(u8 *) (result + 132))
		LODWORD (v4) = a3 >> 9;
	while (1) {
		while (1) {
			HAL_SDIO_HOST_WRITE32(REG_SDIO_HOST_ADMA_SYS_ADDR, v6); // 40058058 = v6;
			v40058004 = DATA_BLK_LEN;
			if (BlockCnt != 1)
				break;
			v4005800C = 17;
			LABEL_18: result = SdioHostChkCmdInhibitCMD();//result);
			if (result)
				return result;
			result = SdioHostChkDataLineActive(0);
			if (result)
				return result;
			Cmd.CmdFmt =
					(SDIO_HOST_CMD_FMT) ((*(u8 *) &Cmd.CmdFmt & 0xF4 | 0x3A)
							& 0x3F);
			v12 = *((u8 *) &Cmd.CmdFmt + 1);
			*(u8 *) (v5 + 128) = 0;
			*(u8 *) (v5 + 129) = 0;
			*((u8 *) &Cmd.CmdFmt + 1) = v12 & 0xC0 | 0x11;
			Cmd.Arg = v4;
			SdioHostSendCmd(&Cmd);
			result = SdioHostChkCmdComplete((void *) v5);//, v13);
			if (result)
				goto LABEL_21;
			SdioHostGetResponse((void *) v5, *(u8 *) &Cmd.CmdFmt & 3);
			result = SdioHostChkXferComplete((void *) v5, 0x1388u, v14);
			if (!result)
				return 0;
			if (result != 16) {
				if (v40058032 & 0x200) {
					v40058032 = 512;
					if (HalSdioHostStopTransferRtl8195a((void *) v5, v15)) {
						if (ConfigDebugErr & 0x400) {
							v11 = "\r[SDIO Err]Stop transmission error!\n";
							goto LABEL_29;
						}
					}
				}
				return 238;
			}
		}
		v40058006 = BlockCnta;
		v4005800C = 55;
		if (BlockCnta <= 1)
			goto LABEL_18;
		result = SdioHostChkCmdInhibitCMD();//result);
		if (result)
			return result;
		result = SdioHostChkDataLineActive(0);
		if (result)
			return result;
		Cmd.CmdFmt = (SDIO_HOST_CMD_FMT) ((*(u8 *) &Cmd.CmdFmt & 0xF4 | 0x3A)
				& 0x3F);
		v7 = *((u8 *) &Cmd.CmdFmt + 1);
		*(u8 *) (v5 + 128) = 0;
		*(u8 *) (v5 + 129) = 0;
		*(u8 *) (v5 + 130) = 0;
		*((u8 *) &Cmd.CmdFmt + 1) = v7 & 0xC0 | 0x12;
		Cmd.Arg = v4;
		SdioHostSendCmd(&Cmd);
		result = SdioHostChkCmdComplete((void *) v5);//, v8);
		if (!result) {
			SdioHostGetResponse((void *) v5, *(u8 *) &Cmd.CmdFmt & 3);
			result = SdioHostChkXferComplete((void *) v5, 0x1388u, v9);
			if (!result)
				break;
		}
		LABEL_21: if (result != 16)
			return result;
	}
	if (!(v40058032 & 0x20))
		return 0;
	v10 = ConfigDebugErr << 21;
	if (ConfigDebugErr & 0x400) {
		v11 = "\r[SDIO Err]Data CRC error!\n";
		LABEL_29: DiagPrintf(v11, v10);
	}
	return 238;
}
// 23D4: using guessed type int DiagPrintf(const char *, ...);

//----- (00000B78) --------------------------------------------------------
HAL_Status HalSdioHostWriteBlocksDmaRtl8195a(IN VOID *Data,
IN u64 WriteAddr,
IN u32 BlockCnt) {
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	PHAL_SDIO_HOST_ADAPTER pSdioHostAdapter = (PHAL_SDIO_HOST_ADAPTER) Data; //int v5; // r6@1
	uint32_t sec_count; // v4; // r4@1

//int v6; // r8@3
	char v7; // r3@12
	uint32_t v8; // r1@12
	signed int v9; // r2@13
	char v10; // r3@25
	uint32_t v11; // r1@25
	signed int v12; // r2@28
	uint32_t v13; // r1@29
	SDIO_HOST_CMD Cmd; // [sp+0h] [bp-20h]@1
	HAL_Status result;

	*(u32 *) &Cmd.CmdFmt = ?;
	Cmd.Arg = a2;
	v4 = WriteAddr;
//v5 = result; //pSdioHostAdapter
	if (BlockCnt == 0)
		return HAL_ERR_PARA;
	if (BlockCnt >= 0x10000)
		return HAL_ERR_PARA;
	ADMA2_DESC_FMT AdmaDescTbl = pSdioHostAdapter->AdmaDescTbl;
	if (((u32) AdmaDescTbl & 3) || (AdmaDescTbl.Addr1 & 3))
		return HAL_ERR_PARA;
	if (*(u8 *) (Data + 132))
		sec_count = WriteAddr >> 9; //
	while (!(HAL_SDIO_HOST_READ32(REG_SDIO_HOST_PRESENT_STATE) & 0x100000)) {
		result = SdioHostIsTimeout(v8, 0x3F01u);
		if (result == HAL_TIMEOUT) {
			DBG_SDIO_ERR("card busy TIMEOUT\n");
			return result;
		}
	}

	while (1) {

		while (1) {
			HAL_SDIO_HOST_WRITE32(REG_SDIO_HOST_ADMA_SYS_ADDR,
					(u32)AdmaDescTbl); //40058058 = v6;
			HAL_SDIO_HOST_WRITE16(REG_SDIO_HOST_BLK_SIZE, DATA_BLK_LEN); // 40058004 = 512;
			if (BlockCnt == 1) {
				HAL_SDIO_HOST_WRITE16(REG_SDIO_HOST_BLK_CNT, 1); // 4005800C = 1;
				goto LABEL_23;
			}
			HAL_SDIO_HOST_WRITE16(REG_SDIO_HOST_BLK_CNT, BlockCnt); //  40058006 = BlockCnta;
			HAL_SDIO_HOST_WRITE16(REG_SDIO_HOST_XFER_MODE,
					XFER_MODE_DMA_EN | XFER_MODE_BLK_CNT_EN | XFER_MODE_AUTO_CMD12_EN | XFER_MODE_MULT_SINGLE_BLK); //4005800C = 0x27;
			if (BlockCnt > 1)
				break;

			LABEL_23: result = SdioHostChkCmdInhibitCMD();//result);
			if (result != HAL_OK)
				return result;
			result = SdioHostChkDataLineActive(0);
			if (result != HAL_OK)
				return result;
			Cmd.CmdFmt =
					(SDIO_HOST_CMD_FMT) ((*(u8 *) &Cmd.CmdFmt & 0xF4 | 0x3A)
							& 0x3F);
			v10 = *((u8 *) &Cmd.CmdFmt + 1);
			*(u8 *) (pSdioHostAdapter + 128) = 0;
			*(u8 *) (pSdioHostAdapter + 129) = 0;
			*((u8 *) &Cmd.CmdFmt + 1) = v10 & 0xC0 | 0x18;
			Cmd.Arg = sec_count;
			SdioHostSendCmd(&Cmd);
			result = SdioHostChkCmdComplete((void *) v5);//, v11);
			if (result)
				goto LABEL_26;
			SdioHostGetResponse((void *) pSdioHostAdapter,
					*(u8 *) &Cmd.CmdFmt & 3);
			if (*(u32 *) (pSdioHostAdapter + 20) & 0x4000000) {
				LABEL_14:
				DBG_SDIO_ERR("Write protect violation!\n");
				return HAL_ERR_PARA;
			}
			result = SdioHostChkXferComplete((void *) pSdioHostAdapter, 0x1388u,
					v12);
			if (result != HAL_OK)
				return result;
			if (result != 16) {
				if (HAL_SDIO_HOST_READ16(
						REG_SDIO_HOST_ERROR_INT_STATUS) & ERR_INT_STAT_ADMA) {
					HAL_SDIO_HOST_WRITE16(REG_SDIO_HOST_ERROR_INT_STATUS,
							ERR_INT_STAT_ADMA); // 40058032 = 512;
					if (HalSdioHostStopTransferRtl8195a(
							(void *) pSdioHostAdapter, v13)) {
						DBG_SDIO_ERR("Stop transmission error!\n");
					}
				}
				return 238;
			}
		}
		result = SdioHostChkCmdInhibitCMD();//result);
		if (result != HAL_OK)
			return result;
		result = SdioHostChkDataLineActive(0);
		if (result != HAL_OK)
			return result;
		Cmd.CmdFmt = (SDIO_HOST_CMD_FMT) ((*(u8 *) &Cmd.CmdFmt & 0xF4 | 0x3A)
				& 0x3F);
		v7 = *((u8 *) &Cmd.CmdFmt + 1);
		*(u8 *) (pSdioHostAdapter + 128) = 0;
		*(u8 *) (pSdioHostAdapter + 129) = 0;
		*(u8 *) (pSdioHostAdapter + 130) = 0;
		*((u8 *) &Cmd.CmdFmt + 1) = v7 & 0xC0 | 0x19;
		Cmd.Arg = sec_count;
		SdioHostSendCmd(&Cmd);
		result = SdioHostChkCmdComplete((void *) v5);//, v8);
		if (!result) {
			SdioHostGetResponse((void *) pSdioHostAdapter,
					*(u8 *) &Cmd.CmdFmt & 3);
			if (*(u32 *) (pSdioHostAdapter + 20) & 0x4000000)
				goto LABEL_14;
			result = SdioHostChkXferComplete((void *) pSdioHostAdapter, 0x1F40u,
					v9);
			if (result != HAL_OK)
				break;
		}
		LABEL_26: if (result != 16)
			return result;
	}
	if (HAL_SDIO_HOST_READ16(
			REG_SDIO_HOST_ERROR_INT_STATUS) & ERR_INT_STAT_DATA_CRC) // 40058032 & 0x20)
		return HAL_ERR_UNKNOWN;
	if (HAL_SDIO_HOST_READ16(
			REG_SDIO_HOST_ERROR_INT_STATUS) & ERR_INT_STAT_DATA_TIMEOUT)
		result = HAL_TIMEOUT;
	else
		result = HAL_OK;
	return result;
}
// 23D4: using guessed type int DiagPrintf(const char *, ...);

// 23D4: using guessed type int DiagPrintf(const char *, ...);

//----- (00000E34) --------------------------------------------------------
HAL_Status HalSdioHostGetCardStatusRtl8195a(IN VOID *Data) {
	void *v3; // r4@1
	int result; // r0@2
	char v5; // r3@3
	uint32_t v6; // r1@3
	signed int v7; // r5@3
	unsigned int v8; // r3@7
	SDIO_HOST_CMD Cmd; // [sp+0h] [bp-18h]@1
	int v10; // [sp+8h] [bp-10h]@1

	*(u32 *) &Cmd.CmdFmt = Data;
	Cmd.Arg = a2;
	v10 = a3;
	v3 = Data;
	if (!Data)
		return 3;
	result = SdioHostChkCmdInhibitCMD();//(uint32_t) Data);
	if (result)
		return result;
	Cmd.CmdFmt =
			(SDIO_HOST_CMD_FMT) ((*(u8 *) &Cmd.CmdFmt & 0xF4 | 0x1A) & 0x1F);
	v5 = *((u8 *) &Cmd.CmdFmt + 1);
	*((u8 *) v3 + 128) = 0;
	*((u8 *) &Cmd.CmdFmt + 1) = v5 & 0xC0 | 0xD;
	Cmd.Arg = *((u16 *) v3 + 67) << 16;
	SdioHostSendCmd(&Cmd);
	v7 = SdioHostChkCmdComplete(v3);//, v6);
	if (v7)
		return v7;
	SdioHostGetResponse(v3, *(u8 *) &Cmd.CmdFmt & 3);
	if (*((u8 *) v3 + 24) == 13) {
		v8 = *((u32 *) v3 + 5);
		*((u32 *) v3 + 10) = v8;
		*((u8 *) v3 + 131) = (v8 >> 9) & 0xF;
		return v7;
	}
	DBG_SDIO_ERR("Command index error!\n");
	return 238;
}
// 23D4: using guessed type int DiagPrintf(const char *, ...);

//----- (00000ED0) --------------------------------------------------------
HAL_Status HalSdioHostInitCardRtl8195a(IN VOID *Data) {
	u8 *v3; // r5@1
	int v4; // r0@2
	char v5; // r3@3
	uint32_t v6; // r1@3
	signed int v7; // r4@4
	const char *v8; // r0@6
	int v9; // r0@7
	uint32_t v10; // r1@8
	const char *v11; // r0@11
	int v12; // r3@12
	int v13; // r0@24
	char v14; // r3@25
	uint32_t v15; // r1@25
	signed int v16; // r0@26
	const char *v17; // r0@28
	char v18; // r3@34
	uint32_t v19; // r1@34
	signed int v20; // r0@35
	signed int v21; // r6@35
	u8 v22; // cf@36
	char v23; // r3@39
	uint32_t v24; // r1@39
	signed int v25; // r0@40
	const char *v26; // r0@42
	char v27; // r3@48
	uint32_t v28; // r1@48
	const char *v29; // r0@53
	uint32_t v30; // r0@64
	int v31; // r0@64
	char v32; // r3@65
	uint32_t v33; // r1@65
	int v34; // r0@70
	char v35; // r3@71
	uint32_t v36; // r1@71
	uint32_t v37; // r1@81
	int v38; // r2@81
	int v39; // r6@81
	int v40; // r1@82
	const char *v41; // r0@83
	int v42; // r0@87
	uint32_t v43; // r1@87
	int v44; // r2@87
	int v45; // r3@88
	uint32_t v46; // r1@88
	signed int v47; // r0@89
	const char *v48; // r0@91
	char v49; // r3@98
	uint32_t v50; // r1@98
	void *v52; // [sp+0h] [bp-20h]@1
	int v53; // [sp+4h] [bp-1Ch]@1
	int v54; // [sp+8h] [bp-18h]@1

	v52 = Data;
	v53 = a2;
	v54 = a3;
	v3 = Data;
	if (!Data) {
		v7 = 3;
		goto LABEL_115;
	};
	v4 = SdioHostChkCmdInhibitCMD();//(uint32_t) Data);
	if (!v4) {
		v5 = (char) v52;
		v3[128] = 0;
		LOBYTE (v52) = v5 & 4;
		v53 = 0;
		BYTE1(v52) &= 0xC0u;
		SdioHostSendCmd((SDIO_HOST_CMD *) &v52);
		v4 = SdioHostChkCmdComplete(v3);//, v6);
	};
	v7 = v4;
	if (v4) {
		DBG_SDIO_ERR("Reset sd card fail!\n");
		goto LABEL_104;
	};
	goto LABEL_115;

	v9 = SdioHostChkCmdInhibitCMD();//0);
	if (v9
			|| (LOBYTE(v52) = (((u8) v52 & 0xF4 | 0x1A) & 0xDF | 32 * (v7 & 1))
					& 0x3F | ((v7 & 3) << 6), BYTE1(v52) = BYTE1(v52) & 0xC0
					| 8, v3[128] = v7, v53 = 426, SdioHostSendCmd(
					(SDIO_HOST_CMD *) &v52), (v9 = SdioHostChkCmdComplete(v3);//,
					v10)) != 0)) {
		v7 = v9;
		if (v9)
			goto LABEL_22;
	} else {
		SdioHostGetResponse(v3, (u8) v52 & 3);
		if (v3[24] != 8) {
			if (ConfigDebugErr & 0x400) {
				v11 = "\r[SDIO Err]Command index error!\n"
				); // DBG_SDIO_ERR("
				LABEL_18: DiagPrintf(v11);
				goto LABEL_21;
			}
			goto LABEL_21;
		}
		v12 = *((u32 *) v3 + 5);
		if ((u8) v12 != 170) {
			DBG_SDIO_ERR("Echo-back of check pattern: %X\n");
			goto LABEL_21;
		}
		v9 = v12 << 23;
		if (!(v12 & 0x100)) {
			if (ConfigDebugErr & 0x400) {
				v11 = "\r[SDIO Err]Voltage accepted error!\n";
				goto LABEL_18;
			}
			LABEL_21: v7 = 238;
			LABEL_22: if (ConfigDebugErr & 0x400) {
				v8 = "\r[SDIO Err]Voltage check fail!\n";
				goto LABEL_104;
			}
			goto LABEL_115;
		}
	}
	v13 = SdioHostChkCmdInhibitCMD();//v9);
	if (v13)
		goto LABEL_63;
	LOBYTE (v52) = ((u8) v52 & 0xFC | 0x1A) & 0x1F;
	v14 = BYTE1(v52) & 0xC0 | 0x37;
	v3[128] = v7;
	BYTE1 (v52) = v14;
	v53 = v7;
	SdioHostSendCmd((SDIO_HOST_CMD *) &v52);
	v13 = SdioHostChkCmdComplete(v3);//, v15);
	if (v13)
		goto LABEL_63;
	v16 = SdioHostGetResponse(v3, (u8) v52 & 3);
	if (v3[24] != 55) {
		if (ConfigDebugErr & 0x400) // DBG_SDIO_ERR("
				{
			v17 = "\r[SDIO Err]Command index error!\n";
			LABEL_32: DiagPrintf(v17);
			goto LABEL_57;
		}
		goto LABEL_57;
	};
	if (!(*((u32 *) v3 + 5) & 0x20)) {
		if (ConfigDebugErr & 0x400) {
			v17 = "\r[SDIO Err]ACMD isn't expected!\n";
			goto LABEL_32;
		}
		LABEL_57: v7 = 238;
		goto LABEL_60;
	};
	v13 = SdioHostChkCmdInhibitCMD();//v16);
	if (v13
			|| (LOBYTE(v52) = ((u8) v52 & 0xFC | 2) & 7, v18 = BYTE1(v52) & 0xC0
					| 0x29, v3[128] = v7, BYTE1(v52) = v18, v53 = v7, SdioHostSendCmd(
					(SDIO_HOST_CMD *) &v52), (v13 = SdioHostChkCmdComplete(v3)) != 0)) {
		LABEL_63: v7 = v13;
		if (!v13)
			goto LABEL_64;
		LABEL_60: if (ConfigDebugErr & 0x400) {
			v8 = "\r[SDIO Err]Get OCR fail!\n";
			goto LABEL_104;
		}
		goto LABEL_115;
	}
	v20 = SdioHostGetResponse(v3, (u8) v52 & 3);
	v21 = 100;
	*((u32 *) v3 + 9) = *((u32 *) v3 + 5) & 0xFFFFFF;
	while (1) {
		v22 = __CFADD__(v21--, -1);
		if (!v22)
			goto LABEL_51;
		v13 = SdioHostChkCmdInhibitCMD();//v20);
		if (v13)
			goto LABEL_63;
		LOBYTE (v52) = ((u8) v52 & 0xFC | 0x1A) & 0x1F;
		v23 = BYTE1(v52);
		v3[128] = 0;
		v53 = 0;
		BYTE1 (v52) = v23 & 0xC0 | 0x37;
		SdioHostSendCmd((SDIO_HOST_CMD *) &v52);
		v13 = SdioHostChkCmdComplete(v3);//, v24);
		if (v13)
			goto LABEL_63;
		v25 = SdioHostGetResponse(v3, (u8) v52 & 3);
		if (v3[24] != 55) {
			if (ConfigDebugErr & 0x400) {
				v26 = "\r[SDIO Err]Command index error!\n";
				LABEL_46: DiagPrintf(v26);
				goto LABEL_62;
			}
			goto LABEL_62;
		}
		if (!(*((u32 *) v3 + 5) & 0x20)) {
			if (ConfigDebugErr & 0x400) {
				v26 = "\r[SDIO Err]ACMD isn't expected!\n";
				goto LABEL_46;
			}
			LABEL_62: v13 = 238;
			goto LABEL_63;
		}
		v13 = SdioHostChkCmdInhibitCMD();//v25);
		if (v13)
			goto LABEL_63;
		LOBYTE (v52) = ((u8) v52 & 0xFC | 2) & 7;
		v27 = BYTE1(v52);
		v3[128] = 0;
		BYTE1 (v52) = v27 & 0xC0 | 0x29;
		v53 = 1077673984;
		SdioHostSendCmd((SDIO_HOST_CMD *) &v52);
		v13 = SdioHostChkCmdComplete(v3);//, v28);
		if (v13)
			goto LABEL_63;
		SdioHostGetResponse(v3, (u8) v52 & 3);
		if (*((u32 *) v3 + 5) < 0)
			break;
		v20 = HalDelayUs(10000);
	};
	if (!v21) {
		v7 = 2;
		goto LABEL_60;
	};
	LABEL_51: if (*((u32 *) v3 + 5) & 0x40000000) {
		v3[132] = 1;
		if (!(ConfigDebugInfo & 0x400))
			goto LABEL_64;
		v29 = "\r[SDIO Inf]This is a SDHC card\n";
	} else {
		v3[132] = 0;
		if (!(ConfigDebugInfo & 0x400))
			goto LABEL_64;
		v29 = "\r[SDIO Inf]This is a SDSC card\n";
	}
	DiagPrintf(v29);
	LABEL_64: v30 = HalDelayUs(20);
	v31 = SdioHostChkCmdInhibitCMD();//v30);
	if (v31
			|| (LOBYTE(v52) = ((u8) v52 & 0xF4 | 9) & 0xF, v32 = BYTE1(v52)
					& 0xC0 | 2, v3[128] = v7, BYTE1(v52) = v32, v53 = v7, SdioHostSendCmd(
					(SDIO_HOST_CMD *) &v52), (v31 = SdioHostChkCmdComplete(v3)) != 0)) {
		v7 = v31;
		if (!v31)
			goto LABEL_70;
		if (ConfigDebugErr & 0x400) {
			v8 = "\r[SDIO Err]Get CID fail!\n";
			goto LABEL_104;
		}
		goto LABEL_115;
	}
	v31 = SdioHostGetResponse(v3, (u8) v52 & 3);
	LABEL_70: v34 = SdioHostChkCmdInhibitCMD();//v31);
	if (v34
			|| (LOBYTE(v52) = ((u8) v52 & 0xF4 | 0x1A) & 0x1F, v35 = BYTE1(v52)
					& 0xC0 | 3, v3[128] = v7, BYTE1(v52) = v35, v53 = v7, SdioHostSendCmd(
					(SDIO_HOST_CMD *) &v52), (v34 = SdioHostChkCmdComplete(v3)) != 0)) {
		v7 = v34;
		if (v34)
			goto LABEL_79;
	} else {
		SdioHostGetResponse(v3, (u8) v52 & 3);
		if (v3[24] != 3) {
			if (ConfigDebugErr & 0x400)
				DiagPrintf("\r[SDIO Err]Command index error!\n");
			v7 = 238;
			LABEL_79: if (ConfigDebugErr & 0x400) {
				v8 = "\r[SDIO Err]Get RCA fail!\n";
				goto LABEL_104;
			}
			goto LABEL_115;
		}
		*((u16 *) v3 + 67) = *((u16 *) v3 + 11);
	}
	SdioHostSdClkCtrl(v3, 1, BASE_CLK_DIVIDED_BY_2);
	v39 = SdioHostGetCSD(v3, v37);
	if (v39) {
		v40 = ConfigDebugErr << 21;
		if (ConfigDebugErr & 0x400) {
			v41 = "\r[SDIO Err]Get CSD fail!\n";
			goto LABEL_108;
		}
		LABEL_113: v7 = v39;
		goto LABEL_115;
	}
	v39 = SdioHostCardSelection(v3, 1, v38);
	if (v39) {
		if (!(ConfigDebugErr & 0x400))
			goto LABEL_113;
		v41 = "\r[SDIO Err]Select sd card fail!\n";
		LABEL_108: DiagPrintf(v41, v40);
		goto LABEL_113;
	}
	v42 = SdioHostChkCmdInhibitCMD();//0);
	if (v42)
		goto LABEL_120;
	LOBYTE (v52) = ((u8) v52 & 0xFC | 0x1A) & 0x1F;
	BYTE1 (v52) = BYTE1(v52) & 0xC0 | 0x37;
	v45 = *((u16 *) v3 + 67) << 16;
	v3[128] = v7;
	v53 = v45;
	SdioHostSendCmd((SDIO_HOST_CMD *) &v52);
	v42 = SdioHostChkCmdComplete(v3);//, v46);
	if (v42)
		goto LABEL_120;
	v47 = SdioHostGetResponse(v3, (u8) v52 & 3);
	if (v3[24] != 55)
		goto LABEL_90;
	if (!(*((u32 *) v3 + 5) & 0x20)) {
		if (ConfigDebugErr & 0x400) {
			v48 = "\r[SDIO Err]ACMD isn't expected!\n";
			LABEL_95: DiagPrintf(v48);
		}
		goto LABEL_96;
	}
	v42 = SdioHostChkCmdInhibitCMD();//v47);
	if (v42
			|| (LOBYTE(v52) = ((u8) v52 & 0xFC | 0x1A) & 0x1F, v49 = BYTE1(v52)
					& 0xC0 | 6, v3[128] = v7, BYTE1(v52) = v49, v53 = 2, SdioHostSendCmd(
					(SDIO_HOST_CMD *) &v52), (v42 = SdioHostChkCmdComplete(v3)) != 0)) {
		LABEL_120: v7 = v42;
		if (v42)
			goto LABEL_102;
		LABEL_105: v39 = HalSdioHostGetCardStatusRtl8195a(v3, v43, v44);
		if (v39) {
			v40 = ConfigDebugErr << 21;
			if (!(ConfigDebugErr & 0x400))
				goto LABEL_113;
			v41 = "\r[SDIO Err]Get sd card current state fail!\n";
			goto LABEL_108;
		}
		if (v3[131] != 4) {
			DBG_SDIO_ERR(
					"The card isn't in TRANSFER state! (Current state: %d)\n",
					v3[131], ConfigDebugErr << 21);
			v7 = 238;
			goto LABEL_115;
		}
	} else {
		SdioHostGetResponse(v3, (u8) v52 & 3);
		if (v3[24] == 6) {
			v44 = v40058028 | 2;
			v40058028 |= 2u;
			goto LABEL_105;
		}
		LABEL_90: if (ConfigDebugErr & 0x400) {
			v48 = "\r[SDIO Err]Command index error!\n";
			goto LABEL_95;
		}
		LABEL_96: v7 = 238;
		LABEL_102: if (ConfigDebugErr & 0x400) {
			v8 = "\r[SDIO Err]Set bus width fail!\n";
			LABEL_104: DiagPrintf(v8);
		}
		LABEL_115:
		DBG_SDIO_ERR("SD card initialization FAIL!\n");
	}
	return v7;
}
// 23D4: using guessed type int DiagPrintf(const char *, ...);
// 23F0: using guessed type int  HalDelayUs(u32);

//----- (000014E8) --------------------------------------------------------
HAL_Status HalSdioHostGetSdStatusRtl8195a(IN VOID *Data) {
	int v3; // r3@1
	char *v4; // r4@1
	char v5; // r0@3
	int v6; // r0@3
	int result; // r0@3
	int v8; // r3@4
	uint32_t v9; // r1@4
	const char *v10; // r0@7
	int v11; // r3@8
	char v12; // r3@13
	uint32_t v13; // r1@13
	signed int v14; // r2@14
	signed int v15; // r0@14
	uint32_t v16; // r1@14
	SDIO_HOST_CMD Cmd; // [sp+0h] [bp-18h]@1
	int v18; // [sp+8h] [bp-10h]@1

	v3 = *((u32 *) Data + 4);
	*(u32 *) &Cmd.CmdFmt = Data;
	Cmd.Arg = a2;
	v18 = a3;
	v4 = (char *) Data;
	if (!v3 || v3 & 3) {
		result = 3;
	} else {
		v40058058 = v3;
		v40058004 = 64;
		v4005800C = 17;
		v5 = *(u8 *) v3;
		*(u16 *) (v3 + 2) = 64;
		v6 = (u8) ((v5 | 3) & 0xEB) | 0x20;
		*(u8 *) v3 = v6;
		*(u32 *) (v3 + 4) = v4 + 48;
		result = SdioHostChkCmdInhibitCMD();//v6);
		if (!result) {
			Cmd.CmdFmt =
					(SDIO_HOST_CMD_FMT) ((*(u8 *) &Cmd.CmdFmt & 0xFC | 0x1A)
							& 0x1F);
			*((u8 *) &Cmd.CmdFmt + 1) = *((u8 *) &Cmd.CmdFmt + 1) & 0xC0 | 0x37;
			v8 = *((u16 *) v4 + 67);
			v4[128] = 0;
			Cmd.Arg = v8 << 16;
			SdioHostSendCmd(&Cmd);
			result = SdioHostChkCmdComplete(v4);
			if (!result) {
				SdioHostGetResponse(v4, *(u8 *) &Cmd.CmdFmt & 3);
				if (v4[24] != 55) {
					if (ConfigDebugErr & 0x400) {
						v10 = "\r[SDIO Err]Command index error!\n";
						LABEL_20: DiagPrintf(v10);
						return 238;
					}
					return 238;
				}
				v11 = *((u32 *) v4 + 5);
				if (!(v11 & 0x20)) {
					if (ConfigDebugErr & 0x400) {
						v10 = "\r[SDIO Err]ACMD isn't expected!\n";
						goto LABEL_20;
					}
					return 238;
				}
				result = SdioHostChkCmdInhibitCMD();//*((u32 *) v4 + 5) << 26);
				if (!result) {
					result = SdioHostChkDataLineActive(0);
					if (!result) {
						Cmd.CmdFmt = (SDIO_HOST_CMD_FMT) ((*(u8 *) &Cmd.CmdFmt
								& 0xFC | 0x3A) & 0x3F);
						v12 = *((u8 *) &Cmd.CmdFmt + 1);
						v4[128] = 0;
						v4[129] = 0;
						Cmd.Arg = 0;
						*((u8 *) &Cmd.CmdFmt + 1) = v12 & 0xC0 | 0xD;
						SdioHostSendCmd(&Cmd);
						result = SdioHostChkCmdComplete(v4);//, v13);
						if (!result) {
							SdioHostGetResponse(v4, *(u8 *) &Cmd.CmdFmt & 3);
							v15 = SdioHostChkXferComplete(v4, 0x1388u, v14);
							if (v15) {
								if (v15 == 16)
									return 238;
								if (!(v40058032 & 0x200))
									return 238;
								v40058032 = 512;
								if (!HalSdioHostStopTransferRtl8195a(v4, v16)
										|| !(ConfigDebugErr & 0x400))
									return 238;
								v10 = "\r[SDIO Err]Stop transmission error!\n";
								goto LABEL_20;
							}
							result = 0;
						}
					}
				}
			}
		}
	}
	return result;
}
// 23D4: using guessed type int DiagPrintf(const char *, ...);

//----- (00001668) --------------------------------------------------------
// SD_CLK_FREQUENCY Frequency
HAL_Status HalSdioHostChangeSdClockRtl8195a(IN VOID *Data, IN u8 Frequency) {
	PHAL_SDIO_HOST_ADAPTER pSdioHostAdapter = (PHAL_SDIO_HOST_ADAPTER) Data; //	v2 = Data; u8 * v2; // r5@1
	PHAL_SDIO_HOST_ADAPTER psha = (PHAL_SDIO_HOST_ADAPTER)Data;
	HAL_Status ret = HAL_ERR_PARA; // int v3; // r4@6
	int v4; // r2@8
	int v5; // r0@10
	int v6; // r0@10
	char v7; // r3@11
	uint32_t v8; // r1@11
	signed int v9; // r0@12
	const char *v10; // r0@14
	char v11; // r3@22
	uint32_t v12; // r1@22
	signed int v13; // r2@23
	signed int v14; // r0@23
	uint32_t v15; // r1@25
	int v16; // r0@32
	int v17; // r0@34
	int v18; // r0@36
	const char *v19; // r0@40
	u8 Divisor; // int v20; // r2@52
	char v22; // SDIO_HOST_CMD [sp+0h] [bp-58h]@11
	char v23; // [sp+1h] [bp-57h]@11
	int v24; // [sp+4h] [bp-54h]@11

	uint8_t StatusData[64]; // [sp+8h] [bp-50h]@8


	if (!psha || Frequency > SD_CLK_41_6MHZ)
		return ret;
	if (psha->CurrSdClk == Frequency) {
		DBG_SDIO_WARN("Current SDCLK frequency is already the specified value...\n");
		ret = HAL_OK;
		return ret;
	}
	if (Frequency != SD_CLK_41_6MHZ) {
		if (Frequency == SD_CLK_10_4MHZ)
			Divisor = BASE_CLK_DIVIDED_BY_4;	// 10.4 MHz
		else if (Frequency == SD_CLK_20_8MHZ)
			Divisor = BASE_CLK_DIVIDED_BY_2;	// 20.8 MHz
		else if (Frequency != SD_CLK_5_2MHZ) {
			DBG_SDIO_ERR("Unsupported SDCLK frequency!\n");
			return ret;
		}
		Divisor = BASE_CLK_DIVIDED_BY_8; // 5.2 MHZ
	}
	ret = SdioHostSdClkCtrl(psha, 1, Divisor);
	if (ret == HAL_OK) return ret;
	DBG_SDIO_ERR("Host changes clock fail!\n");
	return ret;

	*(u32 *) StatusData = 0;
	*(u32 *) &StatusData[4] = 0;

	PADMA2_DESC_FMT	pAdmaDescTbl = psha->AdmaDescTbl;
	if (pAdmaDescTbl == NULL || ((uint32)(pAdmaDescTbl) & 3)) {
		ret = HAL_ERR_PARA;
		return ret;
	}
	HAL_SDIOH_REG32(REG_SDIO_HOST_ADMA_SYS_ADDR) = psha->AdmaDescTbl; // v40058058 = v4;
	HAL_SDIOH_REG16(REG_SDIO_HOST_BLK_SIZE) = 8; //	v40058004 = 8;
	HAL_SDIOH_REG16(REG_SDIO_HOST_XFER_MODE) = XFER_MODE_DMA_EN | XFER_MODE_DATA_XFER_DIR; //	v4005800C = 17;
	//	v5 = (u8) ((*(u8 *) v4 | 3) & 0xEB) | 0x20;
	//	*(u8 *) v4 = v5;
	pAdmaDescTbl->Attrib1.Int = 0;
	pAdmaDescTbl->Attrib1.Act1 = 1;
	pAdmaDescTbl->Len1 = 8; // *(u16 *) (v4 + 2) = 8;
	pAdmaDescTbl->Addr2 = &StatusData; // *(u32 *) (v4 + 4) = StatusData;
	ret = SdioHostChkCmdInhibitCMD();//v5);
	if (ret != HAL_OK) return ret;
	SDIO_HOST_CMD sdhcmd;
	sdhcmd.CmdFmt.CmdCrcChkEn = 1; //	v22 = (v22 & 0xF4 | 0x1A) & 0x1F;
	sdhcmd.CmdFmt.CmdIdx = 0x37; //	v23 = v7 & 0xC0 | 0x37;
	pSdioHostAdapter->CmdCompleteFlg = 0; //	v2[128] = 0;
	sdhcmd.Arg = pSdioHostAdapter->Csd[?] << 16; //	v24 = *((u16 *) v2 + 67) << 16;
	SdioHostSendCmd((SDIO_HOST_CMD *) &sdhcmd);
	ret = SdioHostChkCmdComplete(psha);//, v8);
	if (ret != HAL_OK) return ret;
	v9 = SdioHostGetResponse(psha, v22 & 3);
	if (v2[24] != 55) {
		if (!(ConfigDebugErr & 0x400))
			return 238;
		v10 = "\r[SDIO Err]Command index error!\n";
		LABEL_15: DiagPrintf(v10);
		return 238;
	}
	if (!(*((u32 *) v2 + 5) & 0x20)) {
		if (!(ConfigDebugErr & 0x400))
			return 238;
		v10 = "\r[SDIO Err]ACMD isn't expected!\n";
		goto LABEL_15;
	}
	v6 = SdioHostChkCmdInhibitCMD();//v9);
	if (v6 || (v6 = SdioHostChkDataLineActive(0)) != 0
			|| (v22 = (v22 & 0xF4 | 0x3A) & 0x3F, v11 = v23, psha v2[128] = 0, v2[129] =
					0, v24 = 0, v23 = v11 & 0xC0 | 0x33, SdioHostSendCmd(
					(SDIO_HOST_CMD *) &v22), (v6 = SdioHostChkCmdComplete(psha)) != 0)) {
		LABEL_70: v3 = v6;
		if (v6)
			return v3;
	} else {
		SdioHostGetResponse(v2, v22 & 3);
		v14 = SdioHostChkXferComplete(psha, 0x1388u, v13);
		if (v14) {
			if (v14 == 16)
				return 238;
			v15 = v40058032 << 22;
			if (!(v40058032 & 0x200))
				return 238;
			v40058032 = 512;
			if (!HalSdioHostStopTransferRtl8195a(psha, v15)
					|| !(ConfigDebugErr & 0x400))
				return 238;
			v10 = "\r[SDIO Err]Stop transmission error!\n";
			goto LABEL_15;
		}
		v3 = 0;
		*((u16 *) v2 + 68) = StatusData[0] & 0xF;
	}
	if (*((u16 *) v2 + 68)) {
		v16 = SdioHostSwitchFunction(psha, 0, 15, (int) StatusData,
				*(uint8_t **) &v22);
		if (v16)
			return v16;
		if (StatusData[13] & 2) {
			v17 = SdioHostSwitchFunction(psha, v16, 1, (int) StatusData,
					*(uint8_t **) &v22);
			if (v17)
				return v17;
			if ((StatusData[16] & 0xF) != 1) {
				if (!(ConfigDebugErr & 0x400))
					return 238;
				v10 = "\r[SDIO Err]\"High-Speed\" can't be switched!\n";
				goto LABEL_15;
			}
			v18 = SdioHostSwitchFunction(v2, 1, 1, (int) StatusData,
					*(uint8_t **) &v22);
			if (v18)
				return v18;
			if ((StatusData[16] & 0xF) != 1) {
				if (!(ConfigDebugErr & 0x400))
					return 238;
				v10 = "\r[SDIO Err]Card changes to High-Speed fail!\n";
				goto LABEL_15;
			}
			v3 = SdioHostSdClkCtrl(psha, 1, v18);
			if (v3) {
				if (!(ConfigDebugErr & 0x400))
					return v3;
				v19 = "\r[SDIO Err]Host changes to High-Speed fail!\n";
				LABEL_62: DiagPrintf(v19);
				return v3;
			}
		} else if (ConfigDebugInfo & 0x400) {
// DBG_SDIO_WARN("
			v19 =
					"\r[SDIO Inf]This card doesn't support \"High-Speed Function\" and can't change to high-speed...\n";
			goto LABEL_62;
		}
	} else if (ConfigDebugInfo & 0x400) {
		v19 =
				"\r[SDIO Inf]This card doesn't support CMD6 and can't change to high-speed...\n";
		goto LABEL_62;
	}
	return 0;
}
// 23D4: using guessed type int DiagPrintf(const char *, ...);

//----- (0000194C) --------------------------------------------------------
HAL_Status HalSdioHostEraseRtl8195a(IN VOID *Data, IN u64 StartAddr,
IN u64 EndAddr) {
	uint32_t v3; // r4@1
	uint32_t v4; // r6@1
	int v5; // r8@1
	int result; // r0@4
	char v7; // r3@5
	uint32_t v8; // r1@5
	signed int v9; // r0@6
	char v10; // r3@11
	uint32_t v11; // r1@11
	signed int v12; // r0@12
	char v13; // r3@15
	uint32_t v14; // r1@15
	signed int v15; // r2@15
	SDIO_HOST_CMD v16; // [sp+0h] [bp-20h]@1

	v16 = (SDIO_HOST_CMD) EndAddr;
	v3 = EndAddra;
	v4 = a2;
	v5 = EndAddr;
	if (!(u32) EndAddr)
		return 3;
	if (*(u8 *) (EndAddr + 132)) {
		v4 = a2 >> 9;
		v3 = EndAddra >> 9;
	}
	result = SdioHostChkCmdInhibitCMD();//EndAddr);
	if (!result) {
		v16.CmdFmt = (SDIO_HOST_CMD_FMT) ((*(u8 *) &v16.CmdFmt & 0xFC | 0x1A)
				& 0x1F);
		v7 = *((u8 *) &v16.CmdFmt + 1);
		*(u8 *) (v5 + 128) = 0;
		*((u8 *) &v16.CmdFmt + 1) = v7 & 0xC0 | 0x20;
		v16.Arg = v4;
		SdioHostSendCmd(&v16);
		result = SdioHostChkCmdComplete((void *) v5);
		if (!result) {
			v9 = SdioHostGetResponse((void *) v5, *(u8 *) &v16.CmdFmt & 3);
			if (*(u8 *) (v5 + 24) != 32)
				goto LABEL_20;
			result = SdioHostChkCmdInhibitCMD();//v9);
			if (result)
				return result;
			v16.CmdFmt =
					(SDIO_HOST_CMD_FMT) ((*(u8 *) &v16.CmdFmt & 0xFC | 0x1A)
							& 0x1F);
			v10 = *((u8 *) &v16.CmdFmt + 1);
			*(u8 *) (v5 + 128) = 0;
			*((u8 *) &v16.CmdFmt + 1) = v10 & 0xC0 | 0x21;
			v16.Arg = v3;
			SdioHostSendCmd(&v16);
			result = SdioHostChkCmdComplete((void *) v5);
			if (result)
				return result;
			v12 = SdioHostGetResponse((void *) v5, *(u8 *) &v16.CmdFmt & 3);
			if (*(u8 *) (v5 + 24) != 33) {
				LABEL_20:
				DBG_SDIO_ERR("Command index error!\n");
				result = 238;
			} else {
				result = SdioHostChkCmdInhibitCMD();//v12);
				if (!result) {
					result = SdioHostChkCmdInhibitDAT(0);
					if (!result) {
						v16.CmdFmt = (SDIO_HOST_CMD_FMT) ((*(u8 *) &v16.CmdFmt
								| 0x1B) & 0x1F);
						v13 = *((u8 *) &v16.CmdFmt + 1);
						*(u8 *) (v5 + 128) = 0;
						*(u8 *) (v5 + 129) = 0;
						v16.Arg = 0;
						*((u8 *) &v16.CmdFmt + 1) = v13 & 0xC0 | 0x26;
						SdioHostSendCmd(&v16);
						result = SdioHostChkCmdComplete((void *) v5);
						if (!result)
							result = SdioHostChkXferComplete((void *) v5,
									0x1388u, v15);
					}
				}
			}
		}
	}
	return result;
}
// 23D4: using guessed type int DiagPrintf(const char *, ...);

//----- (00001AA8) --------------------------------------------------------
HAL_Status HalSdioHostGetWriteProtectRtl8195a(IN VOID *Data) {
	void *v3; // r4@1
	uint32_t v4; // r1@2
	int v5; // r2@2
	int v6; // r5@2
	signed int result; // r0@6
	int v8; // r2@10
	const char *v9; // r0@13

	v3 = Data;
	if (!Data)
		return 3;
	v6 = HalSdioHostGetCardStatusRtl8195a(Data, a2, a3);
	if (v6) {
		if (!(ConfigDebugErr & 0x400))
			return v6;
		v9 = "\r[SDIO Err]Get card status fail!\n";
		LABEL_16: DiagPrintf(v9);
		return v6;
	}
	if (*((u8 *) v3 + 131) == 3) {
		LABEL_10: v6 = SdioHostGetCSD(v3, v4);
		if (!v6) {
			*((u32 *) v3 + 11) = ((unsigned int) *((u8 *) v3 + 126) >> 4) & 1;
			return SdioHostCardSelection(v3, 1, v8);
		}
		if (!(ConfigDebugErr & 0x400))
			return v6;
		v9 = "\r[SDIO Err]Get CSD fail!\n";
		goto LABEL_16;
	}
	if (*((u8 *) v3 + 131) == 4 || *((u8 *) v3 + 131) == 5) {
		result = SdioHostCardSelection(v3, 0, v5);
		if (result)
			return result;
		goto LABEL_10;
	}
	if (ConfigDebugErr & 0x400)
		DiagPrintf("\r[SDIO Err]Wrong card state!\n", ConfigDebugErr << 21);
	return 238;
}
// 23D4: using guessed type int DiagPrintf(const char *, ...);

//----- (00001B48) --------------------------------------------------------
HAL_Status HalSdioHostSetWriteProtectRtl8195a(IN VOID *Data, IN u8 Setting) {
	int v2; // r3@1
	u8 *v3; // r4@1
	int v4; // r2@2
	uint8_t v5; // r2@5
	int v6; // r1@7
	unsigned int v7; // r2@7
	signed int v8; // r0@8
	u8 v9; // cf@11
	char v10; // r1@13
	int result; // r0@13
	char v12; // r3@15
	uint32_t v13; // r1@15
	signed int v14; // r2@16
	uint32_t v15; // r1@20
	char v16; // [sp+0h] [bp-30h]@15
	char v17; // [sp+1h] [bp-2Fh]@15
	HAL_Status ret; // [sp+4h] [bp-2Ch]@15
	uint8_t tmp[16]; // [sp+8h] [bp-28h]@3

	v2 = *((u32 *) Data + 4);
	v3 = Data;
	if (!v2)
		return 3;
	v4 = *((u32 *) Data + 4) & 3;
	if (v2 & 3)
		return 3;
	do {
		tmp[v4] = *((u8 *) Data + v4 + 112);
		++v4;
	} while (v4 != 15);
	if (Setting == 1)
		v5 = tmp[14] | 0x10;
	else
		v5 = tmp[14] & 0xEF;
	v6 = 0;
	tmp[14] = v5;
	LOBYTE (v7) = 0;
	do {
		v8 = 7;
		do {
			v7 = (u8) (2 * v7);
			if ((((signed int) tmp[v6] >> v8) & 1) != v7 >> 7)
				LOBYTE (v7) = v7 ^ 0x89;
			v9 = __CFADD__(v8--, -1);
		} while (v9);
		++v6;
	} while (v6 != 15);
	tmp[15] = 2 * v7 | 1;
	v40058058 = v2;
	v40058004 = 16;
	v4005800C = 1;
	v10 = *(u8 *) v2;
	*(u16 *) (v2 + 2) = 16;
	*(u8 *) v2 = v10 & 0xC8 | 0x23;
	*(u32 *) (v2 + 4) = tmp;
	result = SdioHostChkCmdInhibitCMD();//1u);
	if (!result) {
		result = SdioHostChkDataLineActive(0);
		if (!result) {
			v16 = (v16 & 0xF4 | 0x3A) & 0x3F;
			v12 = v17;
			v3[128] = 0;
			v3[129] = 0;
			ret = 0;
			v17 = v12 & 0xC0 | 0x1B;
			SdioHostSendCmd((SDIO_HOST_CMD *) &v16);
			result = SdioHostChkCmdComplete(v3);
			if (!result) {
				SdioHostGetResponse(v3, v16 & 3);
				if (*((u32 *) v3 + 5) & 0x4000000) {
					DBG_SDIO_ERR("Write protect violation!\n",
							ConfigDebugErr << 21);
					return 3;
				}
				result = SdioHostChkXferComplete(v3, 0x1388u, v14);
				if (result) {
					if (result != 16) {
						if (v40058032 & 0x200) {
							v40058032 = 512;
							if (HalSdioHostStopTransferRtl8195a(v3, v15)) {
								DBG_SDIO_ERR("Stop transmission error!\n");
							}
						}
					}
					result = 238;
				}
			}
		}
	}
	return result;
}


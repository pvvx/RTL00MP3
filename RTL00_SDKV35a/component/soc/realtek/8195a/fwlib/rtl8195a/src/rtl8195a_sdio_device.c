/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h" 
#include "hal_sdio.h"
#include "mailbox.h"

#if CONFIG_INIC_EN
#include "freertos_pmu.h"
extern struct sk_buff *rltk_wlan_alloc_skb(unsigned int total_len);
extern unsigned char *skb_put(struct sk_buff * skb, unsigned int len);
extern void inic_sdio_free_data(unsigned char *data);
#if (CONFIG_INIC_SKB_TX == 0) //pre-allocated memory for SDIO TX BD
ALIGNMTO(4) char inic_TX_Buf[SDIO_TX_BD_NUM][SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT];
#endif
#endif

extern PHAL_SDIO_ADAPTER pgSDIODev;

#ifdef CONFIG_SOC_PS_MODULE    
//extern RAM_START_FUNCTION gRamWakeupFun;
extern u8 __ram_start_table_start__[];
extern _LONG_CALL_ VOID HalCpuClkConfig(u8  CpuType);
extern _LONG_CALL_ VOID VectorTableInitRtl8195A(u32 StackP);
extern _LONG_CALL_ VOID HalReInitPlatformLogUartV02(VOID);
extern _LONG_CALL_ VOID HalInitPlatformTimerV02(VOID);
extern VOID InfraStart(VOID);
extern VOID SleepPG(u8  Option, u32 SDuration);
extern VOID PSHalInitPlatformLogUart(VOID);
extern VOID HalReInitPlatformTimer(VOID);
extern VOID DeepStandby(u8  Option, u32 SDuration, u8  GpioOption);
extern VOID QueryRegPwrState(u8  FuncIdx, u8* RegState, u8* HwState);
#endif

/******************************************************************************
 * Function Prototype Declaration
 ******************************************************************************/
BOOL SDIO_Device_Init(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_Device_DeInit(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_IRQ_Handler(
	IN VOID *pData
);

VOID SDIO_Interrupt_Init(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_Interrupt_DeInit(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_Enable_Interrupt(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN u32 IntMask
);

VOID SDIO_Disable_Interrupt(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN u32 IntMask
);

VOID SDIO_Clear_ISR(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN u32 IntMask
);

VOID SDIO_TxTask(
	IN VOID *pData
);

VOID SDIO_RxTask(
	IN VOID *pData
);

static __inline VOID SDIO_Wakeup_Task(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

static VOID SDIO_SetEvent(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u32 Event
);

static VOID SDIO_ClearEvent(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u32 Event
);

static BOOL SDIO_IsEventPending(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u32 Event
);

VOID SDIO_IRQ_Handler_BH(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_RX_IRQ_Handler_BH(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_TX_BD_Buf_Refill( 
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_TX_FIFO_DataReady( 
	IN PHAL_SDIO_ADAPTER pSDIODev
);

PSDIO_RX_PACKET SDIO_Alloc_Rx_Pkt(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_Free_Rx_Pkt(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN PSDIO_RX_PACKET pPkt
);

VOID SDIO_Recycle_Rx_BD (
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_Process_H2C_IOMsg(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_Send_C2H_IOMsg(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u32 *C2HMsg
);

VOID SDIO_Process_H2C_PktMsg(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN u8 *H2CMsg
);

u8 SDIO_Send_C2H_PktMsg(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u8 *C2HMsg, 
	IN u16 MsgLen
);

u8 SDIO_Process_RPWM(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

u8 SDIO_Process_RPWM2(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_Reset_Cmd(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_Return_Rx_Data(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_Register_Tx_Callback(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN s8 (*CallbackFun)(VOID *pAdapter, u8 *pPkt, u16 Offset, u16 PktSize),
	IN VOID *pAdapter	
);

s8 SDIO_Rx_Callback(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN VOID *pData,
	IN u16 Offset,
	IN u16 Length,
	IN u8 CmdType	
);

s8 SDIO_Handle_MsgBlk(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN MSG_BLK *pMblk
);

#if SDIO_MP_MODE
VOID SDIO_PeriodicalTimerCallback(
    void *pContex
);

u8 SDIO_MapMPCmd(
	IN char *CmdStr,
	IN u16 *Offset
);

VOID SDIO_DumpMPStatus(
	IN PHAL_SDIO_ADAPTER pSDIODev
	);

VOID SDIO_StatisticDump(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

s8 SDIO_MP_Loopback(
	IN VOID *pAdapter,
	IN u8 *pData,
	IN u16 Offset,
	IN u16 PktSize
);

s8 SDIO_MP_ContinueTx(
	IN VOID *pAdapter,
	IN u8 *pData,
	IN u16 Offset,
	IN u16 PktSize
);

VOID SDIO_MP_ContinueRx(
	IN PHAL_SDIO_ADAPTER pSDIODev
);

VOID SDIO_DeviceMPApp(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN u16 argc, 
    IN u8  *argv[]
);
#endif	/* endof '#if SDIO_MP_MODE' */

/******************************************************************************
 * Global Variable Declaration
 ******************************************************************************/

#if SDIO_MP_MODE
const SDIO_MP_CMD SDIO_MPCmdTable[] = {
	{"mp_start", SDIO_MP_START},
	{"mp_stop", SDIO_MP_STOP},
	{"mp_loopback", SDIO_MP_LOOPBACK},
	{"status", SDIO_MP_STATUS},
	{"read_reg8", SDIO_MP_READ_REG8},
	{"read_reg16", SDIO_MP_READ_REG16},
	{"read_reg32", SDIO_MP_READ_REG32},
	{"write_reg8", SDIO_MP_WRITE_REG8},
	{"write_reg16", SDIO_MP_WRITE_REG16},
	{"write_reg32", SDIO_MP_WRITE_REG32},
	{"wakeup", SDIO_MP_WAKEUP},
	{"dump", SDIO_MP_DUMP},
	{"ctx", SDIO_MP_CTX},
	{"crx", SDIO_MP_CRX},
    {"crx_da", SDIO_MP_CRX_DA},
	{"crx_stop", SDIO_MP_CRX_STOP},
	{"dbg_msg", SDIO_MP_DBG_MSG}
};

const u8 MP_WlanHdr[]={
    0x88,0x01,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,
    0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,
    0x00,0x01,0x10,0x00,0x06,0x00};
#endif

/******************************************************************************
 * External Function & Variable Declaration
 ******************************************************************************/
extern PHAL_SDIO_ADAPTER pgSDIODev;

extern u32 Strtoul(
    IN  const u8 *nptr, 
    IN  u8 **endptr, 
    IN  u32 base
);

/******************************************************************************
 * Function: SDIO_Device_Init
 * Desc: SDIO device driver initialization.
 *		1. Allocate SDIO TX FIFO buffer and initial TX related register.
 *		2. Allocate SDIO RX Buffer Descriptor and RX Buffer. Initial RX related
 *			register.
 *		3. Register the Interrupt function.
 *		4. Create the SDIO Task and allocate resource(Semaphore).
 *
 ******************************************************************************/
BOOL SDIO_Device_Init(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	int i;
	SDIO_TX_PACKET *pTxPkt;
	SDIO_RX_PACKET *pPkt;
	SDIO_TX_BD_HANDLE *pTxBdHdl;
	SDIO_RX_BD_HANDLE *pRxBdHdl;
	int ret;
	u32 reg_value;
	
	DBG_SDIO_INFO("SDIO_Device_Init==>\n");

    // Clean boot from wakeup bit
    reg_value = HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN);
    reg_value &= ~(BIT(29));
    HAL_WRITE32(PERI_ON_BASE, REG_SOC_FUNC_EN, reg_value);

	/* SDIO Function Enable */
    SDIOD_ON_FCTRL(ON);
    SDIOD_OFF_FCTRL(ON);

	/* Enable Clock for SDIO function */
    ACTCK_SDIOD_CCTRL(ON);
    SLPCK_SDIOD_CCTRL(ON);

	// Reset SDIO DMA
	HAL_SDIO_WRITE8(REG_SPDIO_CPU_RST_DMA, BIT_CPU_RST_SDIO_DMA);
	
	/* Initial SDIO TX BD */
	DBG_SDIO_INFO("Tx BD Init==>\n");	

// TODO: initial TX BD
	pSDIODev->pTXBDAddr = RtlZmalloc((SDIO_TX_BD_NUM * sizeof(SDIO_TX_BD))+3);
	if (NULL == pSDIODev->pTXBDAddr) {
		DBG_SDIO_ERR("SDIO_Device_Init: Malloc for TX_BD Err!!\n");	
		goto SDIO_INIT_ERR;
	}
	pSDIODev->pTXBDAddrAligned = (PSDIO_TX_BD)(((((u32)pSDIODev->pTXBDAddr - 1) >> 2) + 1) << 2);	// Make it 4-bytes aligned
	HAL_SDIO_WRITE32(REG_SPDIO_TXBD_ADDR, pSDIODev->pTXBDAddrAligned);
	HAL_SDIO_WRITE16(REG_SPDIO_TXBD_SIZE, SDIO_TX_BD_NUM);
	/* Set TX_BUFF_UNIT_SIZE */
#if 0
	reg = HAL_SDIO_READ32(REG_SPDIO_RXBD_CNT);
    reg &= ~((0xff)<<8);
    reg |= (SDIO_TX_BD_BUF_USIZE<<8);
	HAL_SDIO_WRITE32(REG_SPDIO_RXBD_CNT, reg);
#endif    
	HAL_SDIO_WRITE8(REG_SPDIO_TX_BUF_UNIT_SZ, SDIO_TX_BD_BUF_USIZE);

	DBG_SDIO_INFO("Tx BD Buf Unit Size(%d), Reg=0x%x\n", SDIO_TX_BD_BUF_USIZE, HAL_SDIO_READ8(REG_SPDIO_TX_BUF_UNIT_SZ));	

	/* Set DISPATCH_TXAGG_PKT */
	HAL_SDIO_WRITE32(REG_SPDIO_AHB_DMA_CTRL, HAL_SDIO_READ32(REG_SPDIO_AHB_DMA_CTRL)|BIT31);
    // Reset HW TX BD pointer
    pSDIODev->TXBDWPtr = HAL_SDIO_READ32(REG_SPDIO_TXBD_WPTR);
    pSDIODev->TXBDRPtr = pSDIODev->TXBDWPtr;
    pSDIODev->TXBDRPtrReg = pSDIODev->TXBDWPtr;
    HAL_SDIO_WRITE32(REG_SPDIO_TXBD_RPTR, pSDIODev->TXBDRPtrReg);

	DBG_SDIO_INFO("TXBDWPtr=0x%x TXBDRPtr=0x%x\n", pSDIODev->TXBDWPtr, pSDIODev->TXBDRPtr);	
    
	pSDIODev->pTXBDHdl = (PSDIO_TX_BD_HANDLE)RtlZmalloc(SDIO_TX_BD_NUM * sizeof(SDIO_TX_BD_HANDLE));
	if (NULL == pSDIODev->pTXBDHdl) {
		DBG_SDIO_ERR("SDIO_Device_Init: Malloc for TX_BD Handle Err!!\n");	
		goto SDIO_INIT_ERR;
	}

	for (i=0;i<SDIO_TX_BD_NUM;i++)
	{
		pTxBdHdl = pSDIODev->pTXBDHdl + i;
		pTxBdHdl->pTXBD = pSDIODev->pTXBDAddrAligned + i;
#if CONFIG_INIC_EN
#if CONFIG_INIC_SKB_TX
//allocate wlan skb here
        pTxBdHdl->skb = rltk_wlan_alloc_skb(SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT);
        DBG_SDIO_INFO("SDIO_Device_Init: pTxBdHdl->pkt @ 0x%x\n", pTxBdHdl->skb);
	if(pTxBdHdl->skb)
		pTxBdHdl->pTXBD->Address = (u32)pTxBdHdl->skb->tail;
	else
		DBG_SDIO_ERR("SDIO_Device_Init: rltk_wlan_alloc_skb (%d) failed!!\n", SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT);
#else
		pTxBdHdl->pTXBD->Address = (u32)(&inic_TX_Buf[i][0]);
#endif
#else
        // Allocate buffer for each TX BD
        pTxBdHdl->pTXBD->Address = (u32)RtlMalloc(SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT);
#if SDIO_DEBUG
        pSDIODev->MemAllocCnt++;
#endif
#endif
        if (NULL == (u32*)(pTxBdHdl->pTXBD->Address)) {
            // Memory Allocate Failed
            int j;
            
            for (j=0;j<i;j++) {
                pTxBdHdl = pSDIODev->pTXBDHdl + j;
                pTxBdHdl->pTXBD = pSDIODev->pTXBDAddrAligned + j;
                if (pTxBdHdl->pTXBD->Address) {
#if CONFIG_INIC_EN
#if CONFIG_INIC_SKB_TX
			//free wlan skb here
			dev_kfree_skb_any(pTxBdHdl->skb);
#endif
			pTxBdHdl->pTXBD->Address =(u32)NULL;
#else
                    RtlMfree((u8 *)pTxBdHdl->pTXBD->Address, (SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT));
#endif
                }
            }
		    goto SDIO_INIT_ERR;            
        }
		pTxBdHdl->isFree = 1;
		DBG_SDIO_INFO("TX_BD%d @ 0x%x 0x%x\n", i, pTxBdHdl, pTxBdHdl->pTXBD);	
	}

#if (CONFIG_INIC_EN == 0)
	RtlInitListhead(&pSDIODev->FreeTxPktList);	// Init the list for free packet handler
	/* Allocate memory for TX Packets handler */
	pSDIODev->pTxPktHandler = (SDIO_TX_PACKET *)(RtlZmalloc(sizeof(SDIO_TX_PACKET)*SDIO_TX_PKT_NUM));
	if (NULL == pSDIODev->pTxPktHandler) {
		DBG_SDIO_ERR("SDIO_Device_Init: Malloc for TX PKT Handler Err!!\n");	
		goto SDIO_INIT_ERR;
	}
	/* Add all TX packet handler into the Free Queue(list) */
	for (i=0;i<SDIO_TX_PKT_NUM;i++)
	{
		pTxPkt = pSDIODev->pTxPktHandler + i;
		RtlListInsertTail(&pTxPkt->list, &pSDIODev->FreeTxPktList);
	}
#endif
	/* Init RX BD and RX Buffer */
	pSDIODev->pRXBDAddr = RtlZmalloc((SDIO_RX_BD_NUM * sizeof(SDIO_RX_BD))+7);
	if (NULL == pSDIODev->pRXBDAddr) {
		DBG_SDIO_ERR("SDIO_Device_Init: Malloc for RX_BD Err!!\n");	
		goto SDIO_INIT_ERR;
	}
	pSDIODev->pRXBDAddrAligned = (PSDIO_RX_BD)(((((u32)pSDIODev->pRXBDAddr - 1) >> 3) + 1) << 3);	// Make it 8-bytes aligned
	HAL_SDIO_WRITE32(REG_SPDIO_RXBD_ADDR, pSDIODev->pRXBDAddrAligned);
	HAL_SDIO_WRITE16(REG_SPDIO_RXBD_SIZE, SDIO_RX_BD_NUM);

    // Set the threshold of free RX BD count to trigger interrupt
	HAL_SDIO_WRITE16(REG_SPDIO_RX_BD_FREE_CNT, RX_BD_FREE_TH);
	DBG_SDIO_INFO("Rx BD Free Cnt(%d), Reg=0x%x\n", RX_BD_FREE_TH, HAL_SDIO_READ16(REG_SPDIO_RX_BD_FREE_CNT));	

	pSDIODev->pRXBDHdl = (PSDIO_RX_BD_HANDLE)RtlZmalloc(SDIO_RX_BD_NUM * sizeof(SDIO_RX_BD_HANDLE));
	if (NULL == pSDIODev->pRXBDHdl) {
		DBG_SDIO_ERR("SDIO_Device_Init: Malloc for RX_BD Handle Err!!\n");	
		goto SDIO_INIT_ERR;
	}

	for (i=0;i<SDIO_RX_BD_NUM;i++)
	{
		pRxBdHdl = pSDIODev->pRXBDHdl + i;
		pRxBdHdl->pRXBD = pSDIODev->pRXBDAddrAligned + i;
		pRxBdHdl->isFree = 1;
		DBG_SDIO_INFO("RX_BD%d @ 0x%x 0x%x\n", i, pRxBdHdl, pRxBdHdl->pRXBD);	
	}


	RtlInitListhead(&pSDIODev->FreeRxPktList);	// Init the list for free packet handler
	/* Allocate memory for RX Packets handler */
	pSDIODev->pRxPktHandler = (SDIO_RX_PACKET *)(RtlZmalloc(sizeof(SDIO_RX_PACKET)*SDIO_RX_PKT_NUM));
	if (NULL == pSDIODev->pRxPktHandler) {
		DBG_SDIO_ERR("SDIO_Device_Init: Malloc for RX PKT Handler Err!!\n");	
		goto SDIO_INIT_ERR;
	}
	/* Add all RX packet handler into the Free Queue(list) */
	for (i=0;i<SDIO_RX_PKT_NUM;i++)
	{
		pPkt = pSDIODev->pRxPktHandler + i;
		RtlListInsertTail(&pPkt->list, &pSDIODev->FreeRxPktList);
	}
	RtlInitListhead(&pSDIODev->RxPktList);	// Init the list for RX packet to be send to the SDIO bus
//	RtlInitListhead(&pSDIODev->RecyclePktList);	// Init the list for packet to be recycled after the SDIO RX DMA is done
	
	RtlMutexInit(&pSDIODev->RxMutex);
#if SDIO_DEBUG
	RtlMutexInit(&pSDIODev->StatisticMutex);
#endif
	/* Create a Semaphone for SDIO Sync control */
#if !TASK_SCHEDULER_DISABLED
	RtlInitSema(&(pSDIODev->TxSema), 0);
	if (NULL == pSDIODev->TxSema){
		DBG_SDIO_ERR("SDIO_Device_Init Create Semaphore Err!!\n");	
		goto SDIO_INIT_ERR;
	}

	RtlInitSema(&(pSDIODev->RxSema), 0);
	if (NULL == pSDIODev->RxSema){
		DBG_SDIO_ERR("SDIO_Device_Init Create RX Semaphore Err!!\n");	
		goto SDIO_INIT_ERR;
	}

	/* create a Mailbox for other driver module to send message to SDIO driver */
	pSDIODev->pMBox = RtlMailboxCreate(MBOX_ID_SDIO, SDIO_MAILBOX_SIZE, &(pSDIODev->RxSema));
	if (NULL == pSDIODev->pMBox) {
		DBG_SDIO_ERR("SDIO_Device_Init Create Mailbox Err!!\n");	
		goto SDIO_INIT_ERR;
	}
#if SDIO_MP_MODE
    pSDIODev->pPeriodTimer = RtlTimerCreate("SDIO_Periodical", SDIO_PERIODICAL_TIMER_INTERVAL, SDIO_PeriodicalTimerCallback, pSDIODev, 1);
#endif
	/* Create the SDIO task */
#ifdef PLATFORM_FREERTOS
	ret = xTaskCreate( SDIO_TxTask, "SDIO_TX_TASK", ((1024*2)/sizeof(portBASE_TYPE)), (void *)pSDIODev, SDIO_TASK_PRIORITY + PRIORITIE_OFFSET, &pSDIODev->xSDIOTxTaskHandle);
	if (pdTRUE != ret )
	{
		DBG_SDIO_ERR("SDIO_Device_Init: Create Task Err(%d)!!\n", ret);
		goto SDIO_INIT_ERR;
	}

	ret = xTaskCreate( SDIO_RxTask, "SDIO_RX_TASK", ((1024*1)/sizeof(portBASE_TYPE)), (void *)pSDIODev, SDIO_TASK_PRIORITY + PRIORITIE_OFFSET, &pSDIODev->xSDIORxTaskHandle);
	if (pdTRUE != ret )
	{
		DBG_SDIO_ERR("SDIO_Device_Init: Create RX Task Err(%d)!!\n", ret);
		goto SDIO_INIT_ERR;
	}
    
#endif
#endif	// end of "#if !TASK_SCHEDULER_DISABLED"
#if SDIO_MP_MODE
//1 for MP mode test only
	pSDIODev->MP_ModeEn = 1;
//    SDIO_Register_Tx_Callback(pSDIODev, (VOID *)SDIO_MP_Loopback, (VOID *) pSDIODev);
//	pSDIODev->MP_LoopBackEn = 1;
//End
#endif
#if TASK_SCHEDULER_DISABLED
	/* enable the interrupt */
    SDIO_Interrupt_Init(pSDIODev);

	/* Indicate the Host system that the TX/RX is ready */
	HAL_SDIO_WRITE8(REG_SPDIO_CPU_IND, \
		HAL_SDIO_READ8(REG_SPDIO_CPU_IND)|BIT_SYSTEM_TRX_RDY_IND);
#endif
	pSDIODev->CRPWM = HAL_SDIO_READ8(REG_SPDIO_CRPWM);
	pSDIODev->CRPWM2 = HAL_SDIO_READ16(REG_SPDIO_CRPWM2);

    // Indicate Host this is a iNIC FW
    pSDIODev->CCPWM2 |= CPWM2_INIC_FW_RDY_BIT;
    pSDIODev->CCPWM2 ^= CPWM2_TOGGLE_BIT;
    HAL_SDIO_WRITE16(REG_SPDIO_CCPWM2, pSDIODev->CCPWM2);

#if !PURE_SDIO_INIC
#ifdef CONFIG_SOC_PS_MODULE
    {
        REG_POWER_STATE SDIOPwrState;
        
        // To register a new peripheral device power state
        SDIOPwrState.FuncIdx = SDIOD;
        SDIOPwrState.PwrState = ACT;
        RegPowerState(SDIOPwrState);
    }
#endif    
#endif

	DBG_SDIO_INFO("<==SDIO_Device_Init\n");

	return SUCCESS;
	
	SDIO_INIT_ERR:
#if !TASK_SCHEDULER_DISABLED
	if (pSDIODev->TxSema) {
		RtlFreeSema(&pSDIODev->TxSema);
		pSDIODev->TxSema = NULL;
	}

    if (pSDIODev->RxSema) {
        RtlFreeSema(&pSDIODev->RxSema);
        pSDIODev->RxSema = NULL;
    }
#endif

	if (pSDIODev->RxMutex) {
		RtlMutexFree(&pSDIODev->RxMutex);
	}
#if SDIO_DEBUG
	if (pSDIODev->StatisticMutex) {
		RtlMutexFree(&pSDIODev->StatisticMutex);
	}
#endif
	if (pSDIODev->pRxPktHandler) {
		RtlMfree((u8*)pSDIODev->pRxPktHandler, sizeof(SDIO_RX_PACKET)*SDIO_RX_PKT_NUM);
		pSDIODev->pRxPktHandler = NULL;
	}

	if (pSDIODev->pRXBDHdl) {
		RtlMfree((u8 *)pSDIODev->pRXBDHdl, SDIO_RX_BD_NUM * sizeof(SDIO_RX_BD_HANDLE));
		pSDIODev->pRXBDHdl = NULL;
	}

	if (pSDIODev->pRXBDAddr) {
		RtlMfree((u8 *)pSDIODev->pRXBDAddr, (SDIO_RX_BD_NUM * sizeof(SDIO_RX_BD))+7);
		pSDIODev->pRXBDAddr = NULL;
	}
#if (CONFIG_INIC_EN == 0)
    if (pSDIODev->pTxPktHandler) {
        RtlMfree((u8 *)pSDIODev->pTxPktHandler, (sizeof(SDIO_TX_PACKET)*SDIO_TX_PKT_NUM));
        pSDIODev->pTxPktHandler = NULL;
    }
#endif
    if ((pSDIODev->pTXBDHdl) && (pSDIODev->pTXBDAddr)) {
        for (i=0;i<SDIO_TX_BD_NUM;i++)
        {
            pTxBdHdl = pSDIODev->pTXBDHdl + i;
            if (pTxBdHdl->pTXBD->Address) {

#if CONFIG_INIC_EN
#if CONFIG_INIC_SKB_TX
		//free wlan skb here
		dev_kfree_skb_any(pTxBdHdl->skb);
#endif
#else
                RtlMfree((u8 *)pTxBdHdl->pTXBD->Address, (SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT));
#endif
                pTxBdHdl->pTXBD->Address = (u32)NULL;
            }
        }
    }
    
    if (pSDIODev->pTXBDHdl) {
        RtlMfree((u8 *)pSDIODev->pTXBDHdl, (SDIO_TX_BD_NUM * sizeof(SDIO_TX_BD_HANDLE)));
        pSDIODev->pTXBDHdl = NULL;
    }
	
    if (pSDIODev->pTXBDAddr) {
        RtlMfree(pSDIODev->pTXBDAddr, ((SDIO_TX_BD_NUM * sizeof(SDIO_TX_BD))+3));
        pSDIODev->pTXBDAddr = NULL;
        pSDIODev->pTXBDAddrAligned = NULL;
    }
			
#if !TASK_SCHEDULER_DISABLED
	if (pSDIODev->pMBox) {
		RtlMailboxDel(pSDIODev->pMBox);
		pSDIODev->pMBox = NULL;
	}
#if SDIO_MP_MODE
    if (pSDIODev->pPeriodTimer) {
        RtlTimerDelete(pSDIODev->pPeriodTimer);
        pSDIODev->pPeriodTimer = NULL;
    }
#endif        
#endif        
	return FAIL;
}


/******************************************************************************
 * Function: SDIO_Device_DeInit
 * Desc: SDIO device driver free resource. This function should be called in 
 *			a task.
 *		1. Free TX FIFO buffer
 *
 * Para:
 *		pSDIODev: The SDIO device data structor.
 ******************************************************************************/
//TODO: Call this function in a task

VOID SDIO_Device_DeInit(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
    int i=0;
	SDIO_TX_BD_HANDLE *pTxBdHdl;
    
	if (NULL == pSDIODev)
		return;

    // Indicate the Host that Ameba is InActived
    pSDIODev->CCPWM2 = HAL_SDIO_READ16(REG_SPDIO_CCPWM2);
    pSDIODev->CCPWM2 &= ~(CPWM2_ACT_BIT);
    pSDIODev->CCPWM2 ^= CPWM2_TOGGLE_BIT;
    HAL_SDIO_WRITE16(REG_SPDIO_CCPWM2, pSDIODev->CCPWM2);

#if !TASK_SCHEDULER_DISABLED
	/* Exit the SDIO task */
#if 0
    // if this function is called by TX Task, then the TX task cannot be stopped
	while (1) {
        SDIO_SetEvent(pSDIODev, SDIO_EVENT_EXIT);
        SDIO_Wakeup_Task(pSDIODev);
		if (SDIO_IsEventPending(pSDIODev, SDIO_EVENT_TX_STOPPED) && 
            SDIO_IsEventPending(pSDIODev, (u32)SDIO_EVENT_RX_STOPPED)) {
            SDIO_ClearEvent(pSDIODev, SDIO_EVENT_EXIT);
			break;  // break the while loop
        }
        RtlMsleepOS(10);
        i++;
        if (i> 100) {
            DBG_SDIO_ERR("SDIO_Device_DeInit: Delete SDIO Task Failed with Timeout\n");
            break;
        }
	}
#endif    
#if SDIO_MP_MODE
    if (pSDIODev->pPeriodTimer) {
        RtlTimerDelete(pSDIODev->pPeriodTimer);
        pSDIODev->pPeriodTimer = NULL;
    }
#endif
	/* Delete the Mailbox */
	if (pSDIODev->pMBox) {
		RtlMailboxDel(pSDIODev->pMBox);
		pSDIODev->pMBox = NULL;
	}

	/* Delete the Semaphore */
	if (pSDIODev->TxSema) {
		RtlFreeSema(&pSDIODev->TxSema);
		pSDIODev->TxSema = NULL;
	}

    if (pSDIODev->RxSema) {
        RtlFreeSema(&pSDIODev->RxSema);
        pSDIODev->RxSema = NULL;
    }
#endif

	if (pSDIODev->RxMutex) {
		RtlMutexFree(&pSDIODev->RxMutex);
	}
#if SDIO_DEBUG
    if (pSDIODev->StatisticMutex) {
        RtlMutexFree(&pSDIODev->StatisticMutex);
    }
#endif
	if (pSDIODev->pRxPktHandler) {
		RtlMfree((u8*)pSDIODev->pRxPktHandler, sizeof(SDIO_RX_PACKET)*SDIO_RX_PKT_NUM);
		pSDIODev->pRxPktHandler = NULL;
	}
	
	if (pSDIODev->pRXBDHdl) {
		RtlMfree((u8 *)pSDIODev->pRXBDHdl, SDIO_RX_BD_NUM * sizeof(SDIO_RX_BD_HANDLE));
		pSDIODev->pRXBDHdl = NULL;
	}
	
	/* Free RX BD */
	if (pSDIODev->pRXBDAddr) {
		RtlMfree((u8 *)pSDIODev->pRXBDAddr, (SDIO_RX_BD_NUM * sizeof(SDIO_RX_BD))+7);
		pSDIODev->pRXBDAddr = NULL;
	}
	
	/* Free TX FIFO Buffer */
    for (i=0;i<SDIO_TX_BD_NUM;i++)
    {
        pTxBdHdl = pSDIODev->pTXBDHdl + i;
        if (pTxBdHdl->pTXBD->Address) {
#if CONFIG_INIC_EN
#if CONFIG_INIC_SKB_TX
		//free wlan skb here
		dev_kfree_skb_any(pTxBdHdl->skb);
#endif
#else
            RtlMfree((u8 *)pTxBdHdl->pTXBD->Address, (SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT));
#endif
            pTxBdHdl->pTXBD->Address = (u32)NULL;
        }
    }
#if (CONFIG_INIC_EN == 0)
    if (pSDIODev->pTxPktHandler) {
        RtlMfree((u8 *)pSDIODev->pTxPktHandler, (sizeof(SDIO_TX_PACKET)*SDIO_TX_PKT_NUM));
        pSDIODev->pTxPktHandler = NULL;
    }
#endif
    if (pSDIODev->pTXBDHdl) {
        RtlMfree((u8 *)pSDIODev->pTXBDHdl, (SDIO_TX_BD_NUM * sizeof(SDIO_TX_BD_HANDLE)));
        pSDIODev->pTXBDHdl = NULL;
    }

    if (pSDIODev->pTXBDAddr) {
        RtlMfree(pSDIODev->pTXBDAddr, ((SDIO_TX_BD_NUM * sizeof(SDIO_TX_BD))+3));
        pSDIODev->pTXBDAddr = NULL;
        pSDIODev->pTXBDAddrAligned = NULL;
    }
    SDIO_Disable_Interrupt(pSDIODev, 0xffff);
    SDIO_Interrupt_DeInit(pSDIODev);

    // Reset SDIO DMA
    HAL_SDIO_WRITE8(REG_SPDIO_CPU_RST_DMA, BIT_CPU_RST_SDIO_DMA);

    /* Enable Clock for SDIO function */
//    ACTCK_SDIOD_CCTRL(OFF);

    /* SDIO Function Enable */
//    SDIOD_ON_FCTRL(OFF);
    SDIOD_OFF_FCTRL(OFF);    
}

#if TASK_SCHEDULER_DISABLED
/******************************************************************************
 * Function: SDIO_TaskUp
 * Desc: For the Task scheduler no running case, use this function to run the
 *			SDIO task main loop.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_TaskUp(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	u16 ISRStatus;
	
//	DiagPrintf("SDIO_TaskUp==>\n");
	pSDIODev->EventSema++;
    if (pSDIODev->EventSema > 1000) {
        pSDIODev->EventSema = 1000;
    }
	if (pSDIODev->EventSema == 1) {
		while (pSDIODev->EventSema > 0) {
            ISRStatus = HAL_SDIO_READ16(REG_SPDIO_CPU_INT_STAS);
            pSDIODev->IntStatus |= ISRStatus;
            HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_STAS, ISRStatus);    // clean the ISR
            SDIO_SetEvent(pSDIODev, SDIO_EVENT_IRQ|SDIO_EVENT_C2H_DMA_DONE);

			SDIO_TxTask(pSDIODev);
            SDIO_RxTask(pSDIODev);
            
			pSDIODev->EventSema--;
		}
	}
//	DiagPrintf("<==SDIO_TaskUp\n");
}
#endif

/******************************************************************************
 * Function: SDIO_IRQ_Handler
 * Desc: SDIO device interrupt service routine
 *		1. Read & clean the interrupt status
 *		2. Wake up the SDIO task to handle the IRQ event
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_IRQ_Handler(
	IN VOID *pData
)
{
	PHAL_SDIO_ADAPTER pSDIODev = pData;
	u16 ISRStatus;
	
	ISRStatus = HAL_SDIO_READ16(REG_SPDIO_CPU_INT_STAS);
    DBG_SDIO_INFO("%s:ISRStatus=0x%x\n", __FUNCTION__, ISRStatus);

	pSDIODev->IntStatus |= ISRStatus;
	HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_STAS, ISRStatus);	// clean the ISR
#if !TASK_SCHEDULER_DISABLED
    if (ISRStatus & BIT_C2H_DMA_OK) {
        SDIO_SetEvent(pSDIODev, SDIO_EVENT_C2H_DMA_DONE);
        RtlUpSemaFromISR(&pSDIODev->RxSema);
    }
    
    if (ISRStatus & ~BIT_C2H_DMA_OK) {
        SDIO_SetEvent(pSDIODev, SDIO_EVENT_IRQ);
	    RtlUpSemaFromISR(&pSDIODev->TxSema);
    }
#else
	SDIO_SetEvent(pSDIODev, SDIO_EVENT_IRQ|SDIO_EVENT_C2H_DMA_DONE);
	SDIO_TaskUp(pSDIODev);
#endif
}

/******************************************************************************
 * Function: SDIO_Interrupt_Init
 * Desc: SDIO device interrupt initialization.
 *		1. Register the ISR
 *		2. Initial the IMR register
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_Interrupt_Init(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	IRQ_HANDLE	SdioIrqHandle;

	pSDIODev->IntMask = BIT_H2C_DMA_OK | BIT_C2H_DMA_OK | BIT_H2C_MSG_INT | BIT_RPWM1_INT | \
						BIT_RPWM2_INT |BIT_H2C_BUS_RES_FAIL | BIT_RXBD_FLAG_ERR_INT;
	HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_STAS, pSDIODev->IntMask);	// Clean pending interrupt first
			
	SdioIrqHandle.Data = (u32) pSDIODev;
	SdioIrqHandle.IrqNum = SDIO_DEVICE_IRQ;
	SdioIrqHandle.IrqFun = (IRQ_FUN) SDIO_IRQ_Handler;
	SdioIrqHandle.Priority = SDIO_IRQ_PRIORITY;
			
	InterruptRegister(&SdioIrqHandle);
	InterruptEn(&SdioIrqHandle);

	HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_MASK, pSDIODev->IntMask);
}

/******************************************************************************
 * Function: SDIO_Interrupt_DeInit
 * Desc: SDIO device interrupt De-Initial.
 *		1. UnRegister the ISR
 *		2. Initial the IMR register with 0
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_Interrupt_DeInit(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	IRQ_HANDLE	SdioIrqHandle;

	HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_STAS, 0xffff);	// Clean pending interrupt first
			
	SdioIrqHandle.Data = (u32) pSDIODev;
	SdioIrqHandle.IrqNum = SDIO_DEVICE_IRQ;
	SdioIrqHandle.Priority = SDIO_IRQ_PRIORITY;
			
    InterruptDis(&SdioIrqHandle);
	InterruptUnRegister(&SdioIrqHandle);
}

/******************************************************************************
 * Function: SDIO_Enable_Interrupt
 * Desc: SDIO enable interrupt by modify the interrupt mask 
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 *	 IntMask: The bit map to enable the interrupt.
 ******************************************************************************/
__inline VOID SDIO_Enable_Interrupt(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN u32 IntMask
)
{
	RtlEnterCritical();
	pSDIODev->IntMask |= IntMask;
	HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_MASK, pSDIODev->IntMask);
	RtlExitCritical();
}

/******************************************************************************
 * Function: SDIO_Disable_Interrupt
 * Desc: SDIO disable interrupt by modify the interrupt mask 
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 *	 IntMask: The bit map to disable the interrupt.
 ******************************************************************************/
__inline VOID SDIO_Disable_Interrupt(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN u32 IntMask
)
{
	RtlEnterCritical();
	pSDIODev->IntMask &= ~IntMask;
	HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_MASK, pSDIODev->IntMask);
	RtlExitCritical();
}

/******************************************************************************
 * Function: SDIO_Clear_ISR
 * Desc: SDIO clear ISR bit map.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 *	 IntMask: The bit map to be clean.
 ******************************************************************************/
__inline VOID SDIO_Clear_ISR(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN u32 IntMask
)
{
	RtlEnterCritical();
	pSDIODev->IntStatus &= ~IntMask;
	RtlExitCritical();
}

/******************************************************************************
 * Function: SDIO_TxTask
 * Desc: The SDIO task handler. This is the main function of the SDIO device 
 * 			driver.
 *		1. Handle interrupt events.
 *			* SDIO TX data ready
 *			* Error handling
 *		2. 
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_TxTask(
	IN VOID *pData
)
{
	PHAL_SDIO_ADAPTER pSDIODev = pData;

	/* Initial resource */
#if !TASK_SCHEDULER_DISABLED
	/* enable the interrupt */
	SDIO_Interrupt_Init(pSDIODev);

    // Update the power state indication
    pSDIODev->CCPWM2 = HAL_SDIO_READ16(REG_SPDIO_CCPWM2);
    pSDIODev->CCPWM2 |= CPWM2_ACT_BIT;
    pSDIODev->CCPWM2 ^= CPWM2_TOGGLE_BIT;
    HAL_SDIO_WRITE16(REG_SPDIO_CCPWM2, pSDIODev->CCPWM2);
    
	/* Indicate the Host system that the TX/RX is ready */
	HAL_SDIO_WRITE8(REG_SPDIO_CPU_IND, \
		HAL_SDIO_READ8(REG_SPDIO_CPU_IND)|BIT_SYSTEM_TRX_RDY_IND);
#if SDIO_MP_MODE
    if (pSDIODev->pPeriodTimer) {
        RtlTimerStart(pSDIODev->pPeriodTimer, 0);
    }
#endif
#endif
	
#if !TASK_SCHEDULER_DISABLED
	for (;;)
#endif		
	{
		/* Task blocked and wait the semaphore(events) here */
#if !TASK_SCHEDULER_DISABLED
		RtlDownSema(&pSDIODev->TxSema);
#endif
		if (SDIO_IsEventPending(pSDIODev, SDIO_EVENT_IRQ)) {
			SDIO_ClearEvent(pSDIODev, SDIO_EVENT_IRQ);
			SDIO_IRQ_Handler_BH(pSDIODev);
		}

		if (SDIO_IsEventPending(pSDIODev, SDIO_EVENT_TXBD_REFILL)) {
			SDIO_ClearEvent(pSDIODev, SDIO_EVENT_TXBD_REFILL);
			SDIO_TX_BD_Buf_Refill(pSDIODev);
		}        

#if !TASK_SCHEDULER_DISABLED
        if (SDIO_IsEventPending(pSDIODev, SDIO_EVENT_EXIT)) {
            break;  // break the loop to exit the task
        }
#endif
	}

#if !TASK_SCHEDULER_DISABLED
#if SDIO_MP_MODE
    if (pSDIODev->pPeriodTimer) {
        RtlTimerStop(pSDIODev->pPeriodTimer, 0);
    }
#endif
	SDIO_SetEvent(pSDIODev, SDIO_EVENT_TX_STOPPED);
    DBG_SDIO_INFO("SDIO TX Task Stopped!\n");
#if ( INCLUDE_vTaskDelete == 1 )
	vTaskDelete(NULL);
#endif
#endif
}

/******************************************************************************
 * Function: SDIO_RxTask
 * Desc: The SDIO RX task handler. This is the main function of the SDIO device 
 * 			driver to handle SDIO RX.
 *		1. Handle interrupt events.
 *			* SDIO RX done
 *		2. Send RX data back to the host by fill RX_BD to hardware. 
 *      3. Handle messages from mailbox
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_RxTask(
	IN VOID *pData
)
{
    PHAL_SDIO_ADAPTER pSDIODev = pData;
    MSG_BLK Mblk;
    
#if !TASK_SCHEDULER_DISABLED
    for (;;)
#endif		
    {
        /* Task blocked and wait the semaphore(events) here */
#if !TASK_SCHEDULER_DISABLED
        RtlDownSema(&pSDIODev->RxSema);
#endif
        if (SDIO_IsEventPending(pSDIODev, SDIO_EVENT_C2H_DMA_DONE)) {
            SDIO_ClearEvent(pSDIODev, SDIO_EVENT_C2H_DMA_DONE);
            SDIO_RX_IRQ_Handler_BH(pSDIODev);
        }

#if SDIO_MP_MODE
        if (pSDIODev->MP_ContinueRx) {
            SDIO_MP_ContinueRx(pSDIODev);
        }
#endif  // end of "#if SDIO_MP_MODE"

        if (SDIO_IsEventPending(pSDIODev, SDIO_EVENT_RX_PKT_RDY)) {
            SDIO_ClearEvent(pSDIODev, SDIO_EVENT_RX_PKT_RDY);
            SDIO_Return_Rx_Data(pSDIODev);
        }
        
#if !TASK_SCHEDULER_DISABLED
		/* handle message block in the mailbox */
		do {
            if (_SUCCESS == RtlMailboxReceive(MBOX_ID_SDIO, &Mblk, MBOX_WAIT_NONE, 0)) {
				SDIO_Handle_MsgBlk(pSDIODev, &Mblk);
			}
            else {
                break;  // no more message pending, break the while loop
            }
		} while (1);

        if (SDIO_IsEventPending(pSDIODev, SDIO_EVENT_EXIT)) {
            break;  // break the loop to exit the task
        }
#endif
    }

#if !TASK_SCHEDULER_DISABLED
    SDIO_SetEvent(pSDIODev, (u32)SDIO_EVENT_RX_STOPPED);
    DBG_SDIO_INFO("SDIO RX Task Stopped!\n");
#if ( INCLUDE_vTaskDelete == 1 )
    vTaskDelete(NULL);
#endif
#endif
}


/******************************************************************************
 * Function: SDIO_Wakeup_Task
 * Desc: Send a semaphore to wake up the task.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
static __inline VOID SDIO_Wakeup_Task(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
#if !TASK_SCHEDULER_DISABLED
	RtlUpSema(&pSDIODev->TxSema);
    RtlUpSema(&pSDIODev->RxSema);
#else
	SDIO_TaskUp(pSDIODev);
#endif	
}

/******************************************************************************
 * Function: SDIO_SetEvent
 * Desc: Set an event and wake up SDIO task to handle it.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 *	 Event: The event to be set.
 ******************************************************************************/
static VOID SDIO_SetEvent(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u32 Event
)
{
	RtlEnterCritical();
	pSDIODev->Events |= Event;
	RtlExitCritical();
}

/******************************************************************************
 * Function: SDIO_ClearEvent
 * Desc: Clean a SDIO event.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 *	 Event: The event to be cleaned.
 ******************************************************************************/
static VOID SDIO_ClearEvent(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u32 Event
)
{
	RtlEnterCritical();
	pSDIODev->Events &= ~Event;
	RtlExitCritical();
}

/******************************************************************************
 * Function: SDIO_IsEventPending
 * Desc: To check is a event pending.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 *	 Event: The event to check.
 ******************************************************************************/
static BOOL SDIO_IsEventPending(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u32 Event
)
{
	BOOL ret;
	
	RtlEnterCritical();	
	ret = (pSDIODev->Events & Event) ? 1:0;
	RtlExitCritical();

	return ret;
}

/******************************************************************************
 * Function: SDIO_IRQ_Handler_BH
 * Desc: Process the SDIO IRQ, the button helf.
 *		1. SDIO TX data ready.
 *		2. H2C command ready.
 *		3. Host driver RPWM status updated.
 *		4. SDIO RX data transfer done.
 *		5. SDIO HW/BUS errors.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_IRQ_Handler_BH(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	u32 IntStatus;

    DBG_SDIO_INFO("%s @1 IntStatus=0x%x\n", __FUNCTION__, pSDIODev->IntStatus);

	RtlEnterCritical();
	IntStatus = pSDIODev->IntStatus;
	RtlExitCritical();
	
	if (IntStatus & BIT_H2C_DMA_OK) {
		SDIO_Clear_ISR(pSDIODev, BIT_H2C_DMA_OK);
		SDIO_Disable_Interrupt(pSDIODev, BIT_H2C_DMA_OK);
		SDIO_TX_FIFO_DataReady(pSDIODev);
		SDIO_Enable_Interrupt(pSDIODev, BIT_H2C_DMA_OK);
	}

	if (IntStatus & BIT_H2C_MSG_INT) {
		SDIO_Clear_ISR(pSDIODev, BIT_H2C_MSG_INT);
		SDIO_Process_H2C_IOMsg(pSDIODev);
	}

	if (IntStatus & BIT_RPWM1_INT) {
		SDIO_Clear_ISR(pSDIODev, BIT_RPWM1_INT);
		SDIO_Process_RPWM(pSDIODev);
	}

	if (IntStatus & BIT_RPWM2_INT) {
		SDIO_Clear_ISR(pSDIODev, BIT_RPWM2_INT);
		SDIO_Process_RPWM2(pSDIODev);
	}
	
	if (IntStatus & BIT_SDIO_RST_CMD_INT) {
		SDIO_Clear_ISR(pSDIODev, BIT_SDIO_RST_CMD_INT);
		SDIO_Reset_Cmd(pSDIODev);
	}

    DBG_SDIO_INFO("%s @2 IntStatus=0x%x\n", __FUNCTION__, pSDIODev->IntStatus);
}

/******************************************************************************
 * Function: SDIO_RX_IRQ_Handler_BH
 * Desc: Process the SDIO RX IRQ, the button helf.
 *		1. SDIO RX data transfer done.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_RX_IRQ_Handler_BH(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	u32 IntStatus;

	RtlEnterCritical();
	IntStatus = pSDIODev->IntStatus;
	RtlExitCritical();
	
	if (IntStatus & BIT_C2H_DMA_OK) {
		SDIO_Clear_ISR(pSDIODev, BIT_C2H_DMA_OK);
        RtlDownMutex(&pSDIODev->RxMutex);
        pSDIODev->RxFifoBusy = 0;
        RtlUpMutex(&pSDIODev->RxMutex);
		SDIO_Recycle_Rx_BD(pSDIODev);
//		SDIO_Return_Rx_Data(pSDIODev);
	}
}


/******************************************************************************
 * Function: SDIO_TX_BD_Buf_Refill
 * Desc: To refill all TX BD buffer.
 *		1. Check all TX BD buffer
 *		2. Allocate a new buffer for TX BD buffer is invalid
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_TX_BD_Buf_Refill( 
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
    u32 i,j;
    PSDIO_TX_BD_HANDLE pTxBdHdl;
    #define WAIT_TIMEOUT    100

    for (i=0;i<SDIO_TX_BD_NUM;i++) {
		pTxBdHdl = pSDIODev->pTXBDHdl + pSDIODev->TXBDRPtrReg;
        if (NULL == (u32*)(pTxBdHdl->pTXBD->Address)) {
            for (j=0;j<WAIT_TIMEOUT;j++) {
#if CONFIG_INIC_EN
#if CONFIG_INIC_SKB_TX
//allocate wlan skb here
        pTxBdHdl->skb = rltk_wlan_alloc_skb(SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT);
	if(pTxBdHdl->skb)
		pTxBdHdl->pTXBD->Address = (u32)pTxBdHdl->skb->tail;
#endif
#else
                pTxBdHdl->pTXBD->Address = (u32)RtlMalloc(SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT);
#endif
                if (NULL == (u32*)(pTxBdHdl->pTXBD->Address)) {
                    DBG_SDIO_WARN("%s Alloc Mem(size=%d) Failed\n", __FUNCTION__, SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT);   
                    RtlMsleepOS(20);                    
                }
                else {
#if SDIO_DEBUG
                    pSDIODev->MemAllocCnt++;
#endif
                    pSDIODev->TXBDRPtrReg++;
                    if (pSDIODev->TXBDRPtrReg >= SDIO_TX_BD_NUM) {
                        pSDIODev->TXBDRPtrReg = 0;
                    }
                    HAL_SDIO_WRITE16(REG_SPDIO_TXBD_RPTR, pSDIODev->TXBDRPtrReg);
                    break;  // break the for loop
                }
            }
            if (j == WAIT_TIMEOUT) {
                break;  // break the for loop
            }
        }
        else {
            break;  // break the for loop
        }        
    }

    if (pSDIODev->TXBDRPtrReg != pSDIODev->TXBDRPtr) {
        DBG_SDIO_ERR("SDIO_TX_BD_Buf_Refill Err: TXBDRPtrReg=%d TXBDRPtr=%d\n", pSDIODev->TXBDRPtrReg, pSDIODev->TXBDRPtr);   
    }
}


/******************************************************************************
 * Function: SDIO_TX_FIFO_DataReady
 * Desc: Handle the SDIO FIFO data ready interrupt.
 *		1. Send those data to the target driver via callback fun., like WLan.
 *		2. Allocate a buffer for the TX BD
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_TX_FIFO_DataReady( 
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
    PSDIO_TX_BD_HANDLE pTxBdHdl;
	PSDIO_TX_DESC pTxDesc;
	volatile u16 TxBDWPtr=0;
	u32 processed_pkt_cnt=0;
	u8 isForceBreak=0;
	s8 ret=FAIL;
    u32 mem_alloc_failed=0;
    u32 reg;


//	DBG_SDIO_INFO("SDIO_TX_FIFO_DataReady==>\n");	

	TxBDWPtr = HAL_SDIO_READ16(REG_SPDIO_TXBD_WPTR);
	if (TxBDWPtr == pSDIODev->TXBDRPtr) {
        if ((pSDIODev->IntStatus & BIT_TXFIFO_H2C_OVF) == 0) {
    		DBG_SDIO_WARN("SDIO TX Data Read False Triggered!!, TXBDWPtr=0x%x\n", TxBDWPtr);	
    		return;
        }
        else {
            reg = HAL_SDIO_READ32(REG_SPDIO_AHB_DMA_CTRL);
    		DBG_SDIO_WARN("SDIO TX Overflow Case: Reg DMA_CTRL==0x%x %x %x %x\n", (reg>> 24)&0xff , (reg>>16)&0xff, (reg>>8)&0xff, (reg)&0xff);	
        }
	}
	
	do {
		DBG_SDIO_INFO("SDIO_TX_DataReady: TxBDWPtr=%d TxBDRPtr=%d\n", TxBDWPtr, pSDIODev->TXBDRPtr);
		pTxBdHdl = pSDIODev->pTXBDHdl + pSDIODev->TXBDRPtr;
        pTxDesc = (PSDIO_TX_DESC)(pTxBdHdl->pTXBD->Address);

        DBG_SDIO_INFO("SDIO_TX_DataReady: PktSz=%d Offset=%d\n", pTxDesc->txpktsize, pTxDesc->offset);
        if ((pTxDesc->txpktsize + pTxDesc->offset) <= (SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT)) {
            // use the callback function to fordward this packet to target(WLan) driver
            if (pSDIODev->Tx_Callback) {
#if CONFIG_INIC_EN
#if CONFIG_INIC_SKB_TX
		ret = pSDIODev->Tx_Callback(pSDIODev->pTxCb_Adapter, (u8*)pTxBdHdl->skb, pTxDesc->offset, pTxDesc->txpktsize);  // includes TX Desc
#else
		ret = pSDIODev->Tx_Callback(pSDIODev->pTxCb_Adapter, (u8*)pTxBdHdl->pTXBD->Address, pTxDesc->offset, pTxDesc->txpktsize);  // includes TX Desc
#endif
#else
                ret = pSDIODev->Tx_Callback(pSDIODev->pTxCb_Adapter, (u8*)pTxBdHdl->pTXBD->Address, pTxDesc->offset, pTxDesc->txpktsize);  // includes TX Desc
#endif
#if 0           
                ret = pSDIODev->Tx_Callback(pSDIODev->pTxCb_Adapter,                            // doesn't include TX Desc
                                            (u8*)(pTxBdHdl->pTXBD->Address+pTxDesc->offset), 
                                            pTxDesc->txpktsize);
#endif
            }
            else {
                ret = FAIL;
                DBG_SDIO_ERR("SDIO TX_Callback is Null!\n");
            }
        } 
        else {
            // Invalid packet, Just drop it
            DBG_SDIO_WARN("SDIO_TX_DataReady Err: Incorrect TxDesc, PktSz=%d Offset=%d BufSize=%d\n", pTxDesc->txpktsize, pTxDesc->offset, \
                (SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT));
            ret = SUCCESS;  // pretend we call the TX callback OK
        }
        processed_pkt_cnt++;
        if (SUCCESS != ret) {
            // may be is caused by TX queue is full, so we skip it and try again later
            isForceBreak = 1;
            break;  // break the while loop
        }
        else {            
#if SDIO_MP_MODE					
            pSDIODev->MP_TxPktCnt++;
            pSDIODev->MP_TxByteCnt += pTxDesc->txpktsize;

            pSDIODev->MP_TxPktCntInPeriod++;
            pSDIODev->MP_TxByteCntInPeriod += pTxDesc->txpktsize;
#endif			
            pSDIODev->TXBDRPtr++;
            if (pSDIODev->TXBDRPtr >= SDIO_TX_BD_NUM) {
                pSDIODev->TXBDRPtr = 0;
            }
            
            // allocate a new buffer for this TX BD
            // buf once if the memory allocation failed, we will try it later
            if (mem_alloc_failed == 0) {
#if CONFIG_INIC_EN
#if CONFIG_INIC_SKB_TX//or use bss for tx bd, not need to re-allocate memory
	//allocate wlan skb for each TX BD
        pTxBdHdl->skb = rltk_wlan_alloc_skb(SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT);
	if(pTxBdHdl->skb)
		pTxBdHdl->pTXBD->Address = (u32)pTxBdHdl->skb->tail;
	else
		pTxBdHdl->pTXBD->Address = (u32)NULL;
#endif
#else
       // Allocate buffer for each TX BD
                pTxBdHdl->pTXBD->Address = (u32)RtlMalloc(SDIO_TX_BD_BUF_USIZE*SDIO_TX_BUF_SZ_UNIT);
#endif
                if (NULL == (u32*)(pTxBdHdl->pTXBD->Address)) {
                    // memory allocate error
                    // once memory allocate failed, stop to allocate new buffer for TX BD buffer
                    //, we refill TX BD buffer later
                    DBG_SDIO_WARN("%s: Alloc new TX BD Buf Failed\n", __FUNCTION__);  
                    mem_alloc_failed++;
                    SDIO_SetEvent(pSDIODev, SDIO_EVENT_TXBD_REFILL);
                }
                else {
#if SDIO_DEBUG
                    pSDIODev->MemAllocCnt++;
#endif
                    pSDIODev->TXBDRPtrReg = pSDIODev->TXBDRPtr;
                    HAL_SDIO_WRITE16(REG_SPDIO_TXBD_RPTR, pSDIODev->TXBDRPtrReg);
                }
            }
            else {
                pTxBdHdl->pTXBD->Address = (u32)NULL;
            }
        }

		TxBDWPtr = HAL_SDIO_READ16(REG_SPDIO_TXBD_WPTR);
		if (isForceBreak) {
			break;	// break the TX FIFO DMA Done processing
		}
	} while (pSDIODev->TXBDRPtr != TxBDWPtr);

	// if not all TX data were processed, set an event to trigger SDIO_Task to process them later
	if (isForceBreak) {
		DBG_SDIO_WARN("SDIO_TX Force Break: TXBDWP=0x%x TXBDRP=0x%x\n", TxBDWPtr, pSDIODev->TXBDRPtr);	
        RtlEnterCritical();
        if ((pSDIODev->IntStatus & BIT_TXFIFO_H2C_OVF) != 0) {
            if (pSDIODev->TXBDRPtr != TxBDWPtr) {
                pSDIODev->IntStatus &= ~BIT_TXFIFO_H2C_OVF;
            }
        }
		pSDIODev->IntStatus |= BIT_H2C_DMA_OK;
        RtlExitCritical();
		SDIO_SetEvent(pSDIODev, SDIO_EVENT_IRQ);
#if !TASK_SCHEDULER_DISABLED
		RtlUpSema(&pSDIODev->TxSema);
#else
		SDIO_TaskUp(pSDIODev);
#endif	
	}
    else {
        if ((pSDIODev->IntStatus & BIT_TXFIFO_H2C_OVF) != 0) {
            SDIO_Clear_ISR(pSDIODev, BIT_TXFIFO_H2C_OVF);
        }
    }
}

/******************************************************************************
 * Function: SDIO_Alloc_Rx_Pkt
 * Desc: Allocate a RX Packet Handle from the queue.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 *
 *	Return:
 *		The allocated RX packet handler.
 ******************************************************************************/
PSDIO_RX_PACKET SDIO_Alloc_Rx_Pkt(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	_LIST *plist;
	SDIO_RX_PACKET *pPkt;
    u32 loop_cnt;
	
	RtlDownMutex(&pSDIODev->RxMutex);
	if (RtlIsListEmpty(&pSDIODev->FreeRxPktList)) {
		RtlUpMutex(&pSDIODev->RxMutex);
        loop_cnt = 0;
        do {
            pPkt =(SDIO_RX_PACKET *)RtlZmalloc(sizeof(SDIO_RX_PACKET));
            if (NULL != pPkt) {
                pPkt->isDyna = 1;   // this packet handler is dynamic allocated
                DBG_SDIO_WARN("Warn! No Free RX PKT, Use Dyna Alloc\n");
            }
            else {
                RtlMsleepOS(10);
                loop_cnt++;
                if (loop_cnt > 100) {
                    DBG_SDIO_ERR("SDIO_Alloc_Rx_Pkt: Err!! Allocate RX PKT Failed!!\n");
                    break;
                }
            }
        }while (NULL == pPkt);
        return pPkt;
	}
	
	plist = RtlListGetNext(&pSDIODev->FreeRxPktList);	
	pPkt = CONTAINER_OF(plist, SDIO_RX_PACKET, list);
	
	RtlListDelete(&pPkt->list);
	RtlUpMutex(&pSDIODev->RxMutex);
	return pPkt;
}

/******************************************************************************
 * Function: SDIO_Free_Rx_Pkt
 * Desc: Put a RX Packet Handle back to the queue.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 * 	 pPkt: The packet handler to be free.
 *
 ******************************************************************************/
VOID SDIO_Free_Rx_Pkt(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN PSDIO_RX_PACKET pPkt
)
{
    if (pPkt->isDyna) {
        RtlMfree((u8 *)pPkt, sizeof(SDIO_RX_PACKET));
    }
    else {
    	RtlDownMutex(&pSDIODev->RxMutex);
    	RtlListInsertTail(&pPkt->list, &pSDIODev->FreeRxPktList);
    	RtlUpMutex(&pSDIODev->RxMutex);
    }
}

/******************************************************************************
 * Function: SDIO_Recycle_Rx_BD
 * Desc: To recycle some RX BD when C2H RX DMA done.
 *	1. Free the RX packet.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_Recycle_Rx_BD (
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	SDIO_RX_BD_HANDLE *pRxBdHdl;
	SDIO_RX_BD *pRXBD;
	u32 PktSize;
	u32 FreeCnt=0;		// for debugging

	DBG_SDIO_INFO("SDIO_Recycle_Rx_BD==> %d %d\n", HAL_SDIO_READ16(REG_SPDIO_RXBD_C2H_RPTR), pSDIODev->RXBDRPtr);
	SDIO_Disable_Interrupt(pSDIODev, BIT_C2H_DMA_OK);
	while (HAL_SDIO_READ16(REG_SPDIO_RXBD_C2H_RPTR) != pSDIODev->RXBDRPtr)
	{
		pRxBdHdl = pSDIODev->pRXBDHdl + pSDIODev->RXBDRPtr;
		pRXBD = pSDIODev->pRXBDAddrAligned + pSDIODev->RXBDRPtr;
		if (!pRxBdHdl->isFree) {
			if (pRxBdHdl->isPktEnd && (NULL != pRxBdHdl->pPkt)) {
				/* Free this packet */
				// TODO: The RX_DESC format may needs to be refined
				PktSize = pRxBdHdl->pPkt->RxDesc.pkt_len;
#if SDIO_MP_MODE
                if ((pSDIODev->MP_CRxPktPendingCnt > 0)) {
                    pSDIODev->MP_CRxPktPendingCnt--;
                }
                
                if (((pSDIODev->MP_CRxPktCnt == 0) && (pSDIODev->MP_CRxPktPendingCnt == 0)) ||
                    (SDIO_CRX_DYNA_BUF == pSDIODev->MP_ContinueRxMode))
#endif          
                {
#if CONFIG_INIC_EN
#if CONFIG_INIC_SKB_RX
                  	dev_kfree_skb_any(pRxBdHdl->pPkt->skb);
			pRxBdHdl->pPkt->skb = NULL;
#else
			inic_sdio_free_data((u8 *) (pRxBdHdl->pPkt->pData));
#endif
			pRxBdHdl->pPkt->pData = NULL;
#else
				    RtlMfree((u8 *) (pRxBdHdl->pPkt->pData), (pRxBdHdl->pPkt->Offset+PktSize));	// free packet buffer
#if SDIO_DEBUG
                    RtlDownMutex(&pSDIODev->StatisticMutex);
				    pSDIODev->MemAllocCnt--;
                    RtlUpMutex(&pSDIODev->StatisticMutex);
#endif
#endif
#if SDIO_MP_MODE
                    pSDIODev->pMP_CRxBuf = NULL;
#endif                    
                }

				_memset((void *)&(pRxBdHdl->pPkt->RxDesc), 0, sizeof(SDIO_RX_DESC));
                RtlDownMutex(&pSDIODev->RxMutex);
				RtlListInsertTail(&pRxBdHdl->pPkt->list, &pSDIODev->FreeRxPktList);	// Put packet handle to free queue
                RtlUpMutex(&pSDIODev->RxMutex);
				FreeCnt++;
				pRxBdHdl->isPktEnd = 0;
				pRxBdHdl->pPkt = NULL;
				DBG_SDIO_INFO("SDIO_Recycle_Rx_BD: Recycle Pkt, RXBDRPtr=%d\n", pSDIODev->RXBDRPtr);
#if SDIO_MP_MODE
				pSDIODev->MP_RxPktCnt++;
				pSDIODev->MP_RxByteCnt += PktSize;

                pSDIODev->MP_RxPktCntInPeriod++;
                pSDIODev->MP_RxByteCntInPeriod += PktSize;
#endif
			}
			_memset((void *)pRXBD , 0, sizeof(SDIO_RX_BD));	// clean this RX_BD
			pRxBdHdl->isFree = 1;
		}
		else {
			DBG_SDIO_WARN("SDIO_Recycle_Rx_BD: Warring, Recycle a Free RX_BD,RXBDRPtr=%d\n",pSDIODev->RXBDRPtr);
		}
		pSDIODev->RXBDRPtr++;
		if (pSDIODev->RXBDRPtr >= SDIO_RX_BD_NUM) {
			pSDIODev->RXBDRPtr -= SDIO_RX_BD_NUM;
		}
	}
	SDIO_Enable_Interrupt(pSDIODev, BIT_C2H_DMA_OK);
	DBG_SDIO_INFO("<==SDIO_Recycle_Rx_BD(%d)\n", FreeCnt);

}

/******************************************************************************
 * Function: SDIO_Process_H2C_IOMsg
 * Desc: Handle the interrupt for HC2 message ready. Read the H2C_MSG register
 *		and process the H2C message.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_Process_H2C_IOMsg(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{	
	u32 H2CMsg;

	// TODO: define H2C message type & format, currently we have  30 bits message only, may needs to extend the HW register
	H2CMsg = HAL_SDIO_READ32(REG_SPDIO_CPU_H2C_MSG);
	DBG_SDIO_INFO("H2C_MSG: 0x%x\n", H2CMsg);
	// TODO: May needs to handle endian free
	switch (H2CMsg)
	{
	default:
		break;
	}
	// TODO: Some H2C message needs to be fordward to WLan driver
}

/******************************************************************************
 * Function: SDIO_Send_C2H_IOMsg
 * Desc: Send C2H message to the Host.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_Send_C2H_IOMsg(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u32 *C2HMsg
)
{
	u32 TmpC2HMsg;

	// TODO: define C2H message type & format, currently we have  30 bits message only, may needs to extend the HW register
	
	// TODO: May needs to handle endian free
	TmpC2HMsg = HAL_SDIO_READ32(REG_SPDIO_CPU_C2H_MSG);
	TmpC2HMsg = ((TmpC2HMsg ^ (u32)BIT(31)) & (u32)BIT(31)) | *C2HMsg;
	HAL_SDIO_WRITE32(REG_SPDIO_CPU_C2H_MSG, TmpC2HMsg);
}

/******************************************************************************
 * Function: SDIO_Process_H2C_PktMsg
 * Desc: Handle the packet H2C message which from block write(CMD53).
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 *	 H2CMsg: point to the buffer of the H2C message received.
 ******************************************************************************/
VOID SDIO_Process_H2C_PktMsg(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN u8 *H2CMsg
)
{

	// TODO: define H2C message type & format
	DBG_SDIO_INFO("H2C_MSG: 0x%x\n", *H2CMsg);
	// TODO: May needs to handle endian free
	// TODO: Some H2C message needs to be fordward to WLan driver
}

/******************************************************************************
 * Function: SDIO_Send_C2H_PktMsg
 * Desc: To send a C2H message to the Host through the block read command.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 *	 H2CMsg: point to the buffer of the H2C message received.
 *	 MsgLen: The length of this message.
 ******************************************************************************/
u8 SDIO_Send_C2H_PktMsg(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u8 *C2HMsg, 
	IN u16 MsgLen
)
{
	u8 *MsgBuf;
	PSDIO_RX_DESC pRxDesc;
	SDIO_RX_PACKET *pPkt;
	
	// TODO: define H2C message type & format
	DBG_SDIO_INFO("C2H_MSG: 0x%x\n", *C2HMsg);
	// TODO: May needs to handle endian free

	MsgBuf = RtlZmalloc(MsgLen);
	if (NULL == MsgBuf) {
		DBG_SDIO_ERR("SDIO_Send_C2H_PktMsg: Malloc Err!!\n");
		return FAIL;
	}
	_memcpy((void *)(MsgBuf), (void *)C2HMsg, MsgLen);
	
	pPkt = SDIO_Alloc_Rx_Pkt(pSDIODev);
	if (pPkt == NULL) {
		DBG_SDIO_ERR("RX Callback Err!! No Free RX PKT!\n");
		return FAIL;
	}
	pRxDesc = &pPkt->RxDesc;
    pRxDesc->type = SDIO_CMD_C2H;
	pRxDesc->pkt_len = MsgLen;
	pRxDesc->offset = sizeof(SDIO_RX_DESC);
	pPkt->pData = MsgBuf;
	pPkt->Offset = 0;
	RtlDownMutex(&pSDIODev->RxMutex);
	RtlListInsertTail(&pPkt->list, &pSDIODev->RxPktList);
	pSDIODev->RxInQCnt++;
	RtlUpMutex(&pSDIODev->RxMutex);
	SDIO_SetEvent(pSDIODev, SDIO_EVENT_RX_PKT_RDY);
#if !TASK_SCHEDULER_DISABLED
    if (pSDIODev->RxInQCnt == 1) {
        RtlUpSema(&pSDIODev->RxSema);
    }
#else
	SDIO_TaskUp(pSDIODev);
#endif	

	return SUCCESS;
    
}

#ifdef CONFIG_SOC_PS_MODULE    
/******************************************************************************
 * Function: SDIO_Wakeup_From_PG
 * Desc: To handle the process of system wakeup from power gated.
 *
 * Para: None
 ******************************************************************************/
VOID SDIO_Wakeup_From_PG(VOID)
{
    ConfigDebugErr = 0xffffffff;
    ConfigDebugInfo = _DBG_SDIO_;
    ConfigDebugWarn = _DBG_SDIO_;

    HalCpuClkConfig(CLK_200M);
    VectorTableInitRtl8195A(0x1FFFFFFC);

#if CONFIG_CHIP_C_CUT
    HalReInitPlatformLogUartV02();    
#else
    PSHalInitPlatformLogUart();
#endif

#ifdef CONFIG_TIMER_MODULE
#if CONFIG_CHIP_C_CUT
    HalInitPlatformTimerV02();
#else
    HalReInitPlatformTimer();
#endif
//    HalDelayUs(1000);
#endif
    
    InfraStart();
}
#endif

#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
VOID SDIO_Pre_Sleep_Callback(u32 expected_idle_time){
	/* Indicate the Host system that the TX/RX is not ready */
	HAL_SDIO_WRITE8(REG_SPDIO_CPU_IND, \
		HAL_SDIO_READ8(REG_SPDIO_CPU_IND)&~BIT_SYSTEM_TRX_RDY_IND);
}
VOID SDIO_Post_Sleep_Callback(u32 expected_idle_time){
	/* Indicate the Host system that the TX/RX is ready */
	HAL_SDIO_WRITE8(REG_SPDIO_CPU_IND, \
		HAL_SDIO_READ8(REG_SPDIO_CPU_IND)|BIT_SYSTEM_TRX_RDY_IND);
}
#endif

/******************************************************************************
 * Function: SDIO_Process_RPWM
 * Desc: To handle RPWM interrupt.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
u8 SDIO_Process_RPWM(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	u8 rpwm;

	rpwm = HAL_SDIO_READ8(REG_SPDIO_CRPWM);

	DBG_SDIO_INFO ("RPWM1: 0x%x\n", rpwm);
	// TODO: forward this RPWM message to WLan	
	return 0;
}

/******************************************************************************
 * Function: SDIO_Process_RPWM
 * Desc: To handle RPWM interrupt.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
u8 SDIO_Process_RPWM2(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	u16 rpwm;
    u32 reg_value;;
    PRAM_FUNCTION_START_TABLE pRamStartFun = (PRAM_FUNCTION_START_TABLE) __ram_start_table_start__;

	rpwm = HAL_SDIO_READ16(REG_SPDIO_CRPWM2);

	DBG_SDIO_INFO ("RPWM2: 0x%x\n", rpwm);
    
#ifdef CONFIG_SOC_PS_MODULE    
	if ((rpwm&RPWM2_TOGGLE_BIT) != (pSDIODev->CRPWM2&RPWM2_TOGGLE_BIT)) {
        pSDIODev->CRPWM2 = rpwm;
        // Tgoole bit changed, means it's a new RPWM command
        if ((rpwm & RPWM2_ACT_BIT) == 0) {
            // request to enter sleep mode
            pSDIODev->CCPWM2 = HAL_SDIO_READ16(REG_SPDIO_CCPWM2);
            pSDIODev->CCPWM2 &= ~(CPWM2_ACT_BIT);

#if PURE_SDIO_INIC
            SDIO_Device_DeInit(pSDIODev);
#endif
            if ((rpwm & RPWM2_DSTANDBY_BIT) == 0) {
                pSDIODev->CCPWM2 &= ~(CPWM2_DSTANDBY_BIT);
                if((rpwm & RPWM2_CG_BIT)){
                    //enter clock gated state
                    pSDIODev->CCPWM2 ^= CPWM2_TOGGLE_BIT;
                    HAL_SDIO_WRITE16(REG_SPDIO_CCPWM2, pSDIODev->CCPWM2);
#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
                    add_wakeup_event(SLEEP_WAKEUP_BY_SDIO);
                    register_pre_sleep_callback(SDIO_Pre_Sleep_Callback);
                    register_post_sleep_callback(SDIO_Post_Sleep_Callback);
                    release_wakelock(WAKELOCK_SDIO_DEVICE);
#endif
                }
                else{
                // enter power gated state
                if ((rpwm & RPWM2_FBOOT_BIT)) {
                    pSDIODev->CCPWM2 |= CPWM2_FBOOT_BIT;
                    // setup the trap to call the wakeup callback when booting
                    reg_value = HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN);
                    reg_value |= BIT(29);
                    HAL_WRITE32(PERI_ON_BASE, REG_SOC_FUNC_EN, reg_value);
                    // Assign the RAM start address after boot from wakeup
                    pRamStartFun->RamWakeupFun = SDIO_Wakeup_From_PG;
                }
                pSDIODev->CCPWM2 ^= CPWM2_TOGGLE_BIT;
                HAL_SDIO_WRITE16(REG_SPDIO_CCPWM2, pSDIODev->CCPWM2);

#if PURE_SDIO_INIC
                SleepPG(SLP_SDIO, 0);
#endif    
                }
            } else {
                // enter Deep Standby state
                pSDIODev->CCPWM2 |= CPWM2_DSTANDBY_BIT;
                pSDIODev->CCPWM2 &= ~(CPWM2_FBOOT_BIT);
                pSDIODev->CCPWM2 ^= CPWM2_TOGGLE_BIT;
                HAL_SDIO_WRITE16(REG_SPDIO_CCPWM2, pSDIODev->CCPWM2);
#if PURE_SDIO_INIC
                {
                u8 gpio_option, i;
                u16 gpio_pin, gpio_en, gpio_act, gpio_lv;


                gpio_option = 0;
                gpio_pin = RPWM2_WKPIN_A5_BIT;
                gpio_lv = RPWM2_PIN_A5_LV_BIT;
                gpio_en = BIT0;
                gpio_act = BIT4;
                // Loop 4 to check 4 GPIO wake up event
                for (i=0;i<4;i++) {
                    if (rpwm & gpio_pin) {
                        gpio_option |= gpio_en;
                        if (rpwm &  gpio_lv) {
                            // Active High
                            gpio_option |= gpio_act;
                        }
                    }
                    gpio_pin = gpio_pin << 1;
                    gpio_lv = gpio_lv << 1;
                    gpio_en = gpio_en << 1;
                    gpio_act = gpio_act << 1;
                }
                    
                DeepStandby(DSTBY_GPIO, 0, gpio_option);
                }
#endif
            }
#if !PURE_SDIO_INIC
            {
                REG_POWER_STATE SDIOPwrState;
                u8 HwState;

                SDIOPwrState.FuncIdx = SDIOD;
                QueryRegPwrState(SDIOD, &(SDIOPwrState.PwrState), &HwState);

                if (SDIOPwrState.PwrState == ACT) {
                    SDIOPwrState.PwrState = INACT;
                    RegPowerState(SDIOPwrState);
                }    
            }
#endif
        } else {
#if !PURE_SDIO_INIC

#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
            acquire_wakelock(WAKELOCK_SDIO_DEVICE);
#endif

            // Request to Active SDIO iNIC
            REG_POWER_STATE SDIOPwrState;

            // Let the power management task know SDIO is in active
            SDIOPwrState.FuncIdx = SDIOD;
            SDIOPwrState.PwrState = ACT;
            RegPowerState(SDIOPwrState);

            pSDIODev->CCPWM2 |= CPWM2_ACT_BIT;
            pSDIODev->CCPWM2 ^= CPWM2_TOGGLE_BIT;
            HAL_SDIO_WRITE16(REG_SPDIO_CCPWM2, pSDIODev->CCPWM2);
#endif        
        }
    }
#endif  // #ifdef CONFIG_SOC_PS_MODULE
	return 0;
}

/******************************************************************************
 * Function: SDIO_Reset_Cmd
 * Desc: Handle the SDIO Reset Command interrupt. We did nothing currently. 
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_Reset_Cmd(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	// TODO:
	return;
}

/******************************************************************************
 * Function: SDIO_Return_Rx_Data
 * Desc: To send all packets in the RX packet list to the Host system via the 
 *		SDIO bus.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_Return_Rx_Data(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	SDIO_RX_PACKET *pPkt=NULL;
	SDIO_RX_DESC *pRxDesc;
	SDIO_RX_BD_HANDLE *pRxBdHdl;
	_LIST *plist;
	SDIO_RX_BD *pRXBD;
	u32 Offset=0;
	u16 RxBdWrite=0;	// to count how much RX_BD used in a Transaction
	u16 RxBdRdPtr=0;	// RX_BD read pointer
	u32 pkt_size;
	u8 isForceBreak=0;
    u8 isListEmpty;
#if SDIO_RX_PKT_SIZE_OVER_16K
	u8 needed_rxbd_num;
#endif

//	DBG_SDIO_INFO("SDIO_Return_Rx_Data==> RXBDWPtr=%d\n", pSDIODev->RXBDWPtr);
	RtlDownMutex(&pSDIODev->RxMutex);
	if (RtlIsListEmpty(&pSDIODev->RxPktList)) {
		RtlUpMutex(&pSDIODev->RxMutex);
//		DBG_SDIO_INFO("SDIO_Return_Rx_Data: Queue is empty\n");
		return;
	}

	if (pSDIODev->RxFifoBusy) {
		RtlUpMutex(&pSDIODev->RxMutex);
		SDIO_SetEvent(pSDIODev, SDIO_EVENT_RX_PKT_RDY);
		return;
	}
    RtlUpMutex(&pSDIODev->RxMutex);

	RxBdRdPtr = pSDIODev->RXBDRPtr;

    // since we always need to wait the HW to fetch RX BD done, 
    // so it seems no need to check the RX BD Read Pointer again
#if 0   
	/* Check if we shoule handle the RX_BD recycle ? */
	if (RxBdRdPtr != HAL_SDIO_READ16(REG_SPDIO_RXBD_C2H_RPTR)) {
		SDIO_Recycle_Rx_BD(pSDIODev);
		RxBdRdPtr = pSDIODev->RXBDRPtr;
	}
#endif
	do {
		/* check if RX_BD available */
        RtlDownMutex(&pSDIODev->RxMutex);
		plist = RtlListGetNext(&pSDIODev->RxPktList);	
        RtlUpMutex(&pSDIODev->RxMutex);
		pPkt = CONTAINER_OF(plist, SDIO_RX_PACKET, list);
		pRxDesc = &(pPkt->RxDesc);
#if SDIO_RX_PKT_SIZE_OVER_16K
		needed_rxbd_num = ((pRxDesc->pkt_len - 1)/MAX_RX_BD_BUF_SIZE) + MIN_RX_BD_SEND_PKT;
#endif
		if (RxBdRdPtr != pSDIODev->RXBDWPtr) {
			if (pSDIODev->RXBDWPtr > RxBdRdPtr) {
#if SDIO_RX_PKT_SIZE_OVER_16K
				if ((pSDIODev->RXBDWPtr - RxBdRdPtr) >= (SDIO_RX_BD_NUM - needed_rxbd_num)) 
#else
				if ((pSDIODev->RXBDWPtr - RxBdRdPtr) >= (SDIO_RX_BD_NUM - MIN_RX_BD_SEND_PKT)) 
#endif					
				{
					DBG_SDIO_WARN("SDIO_Return_Rx_Data: No Available RX_BD, ReadPtr=%d WritePtr=%d\n", \
						RxBdRdPtr, pSDIODev->RXBDWPtr);
					isForceBreak = 1;
					break;	// break the while loop
				}
			} 
			else {
#if SDIO_RX_PKT_SIZE_OVER_16K
				if ((RxBdRdPtr - pSDIODev->RXBDWPtr) <= needed_rxbd_num)
#else
				if ((RxBdRdPtr - pSDIODev->RXBDWPtr) <= MIN_RX_BD_SEND_PKT)
#endif
				{
					DBG_SDIO_WARN("SDIO_Return_Rx_Data: No Available RX_BD, ReadPtr=%d WritePtr=%d\n", RxBdRdPtr, pSDIODev->RXBDWPtr);
					isForceBreak = 1;
					break;	// break the while loop
				}
			}
		}

        RtlDownMutex(&pSDIODev->RxMutex);
		RtlListDelete(&pPkt->list);	// remove it from the SDIO RX packet Queue
		pSDIODev->RxInQCnt--;
        RtlUpMutex(&pSDIODev->RxMutex);

		// TODO: Add RX_DESC before the packet

		/* a SDIO RX packet will use at least 2 RX_BD, the 1st one is for RX_Desc, 
		   other RX_BDs are for packet payload */
		/* Use a RX_BD to transmit RX_Desc */		
		pRXBD = pSDIODev->pRXBDAddrAligned + pSDIODev->RXBDWPtr;	// get the RX_BD head
		pRxBdHdl = pSDIODev->pRXBDHdl + pSDIODev->RXBDWPtr;
		if (!pRxBdHdl->isFree) {
			DBG_SDIO_ERR("SDIO_Return_Rx_Data: Allocated a non-free RX_BD\n");
		}
		pRxBdHdl->isFree = 0;
		pRxBdHdl->pPkt = pPkt;
		pRXBD->FS = 1;
		pRXBD->PhyAddr = (u32)((u8 *)pRxDesc);
		pRXBD->BuffSize = sizeof(SDIO_RX_DESC);
		pRxBdHdl->isPktEnd = 0;
		pSDIODev->RXBDWPtr += 1;
		if (pSDIODev->RXBDWPtr >= SDIO_RX_BD_NUM) {
			pSDIODev->RXBDWPtr -= SDIO_RX_BD_NUM;
		}
        RxBdWrite++;

		/* Take RX_BD to transmit packet payload */
		pkt_size = pRxDesc->pkt_len;
		Offset = 0;
		do {
			pRXBD = pSDIODev->pRXBDAddrAligned + pSDIODev->RXBDWPtr;	// get the RX_BD head
			pRxBdHdl = pSDIODev->pRXBDHdl + pSDIODev->RXBDWPtr;
			pRxBdHdl->isFree = 0;
			pRxBdHdl->pPkt = pPkt;
			pRXBD->FS = 0;
			pRXBD->PhyAddr = (u32)(((u8 *)pPkt->pData)+pPkt->Offset);
#if SDIO_RX_PKT_SIZE_OVER_16K
			if ((pkt_size - Offset) <= MAX_RX_BD_BUF_SIZE)	{
				pRXBD->BuffSize = pkt_size - Offset;
				pRxBdHdl->isPktEnd = 1;
			}
			else {
				pRXBD->BuffSize = MAX_RX_BD_BUF_SIZE;
				pRxBdHdl->isPktEnd = 0;
				DBG_SDIO_INFO("SDIO_Return_Rx_Data: Split RX_BD, Offset=%d PktSize=%d\n", \
					Offset, pkt_size);
			}
#else
			if (pkt_size > MAX_RX_BD_BUF_SIZE)	{
				// if come to here, please enable "SDIO_RX_PKT_SIZE_OVER_16K"
				DBG_SDIO_ERR("SDIO_Return_Rx_Data: The Packet Size bigger than 16K\n");
				pkt_size = MAX_RX_BD_BUF_SIZE;
			}
			pRXBD->BuffSize = pkt_size;
			pRxBdHdl->isPktEnd = 1;
#endif
			Offset += pRXBD->BuffSize;
			// Move the RX_BD Write pointer forward
			RxBdWrite++;
			pSDIODev->RXBDWPtr += 1;
			if (pSDIODev->RXBDWPtr >= SDIO_RX_BD_NUM) {
				pSDIODev->RXBDWPtr -= SDIO_RX_BD_NUM;
			}

			if (Offset >= pkt_size) {
				pRXBD->LS = 1;
//				HAL_SDIO_WRITE16(REG_SPDIO_RXBD_C2H_WPTR, pSDIODev->RXBDWPtr);
//                HAL_SDIO_WRITE8(REG_SPDIO_HCI_RX_REQ, BIT_HCI_RX_REQ);
//				DBG_SDIO_INFO("SDIO_Return_Rx_Data:RXBDWPtr=%d\n", pSDIODev->RXBDWPtr);
			}
		} while (Offset < pkt_size);

        if (RxBdWrite >= (SDIO_RX_BD_NUM - MIN_RX_BD_SEND_PKT)) {
            isForceBreak = 1;
            break;  // break the while loop
        }
        
        RtlDownMutex(&pSDIODev->RxMutex);
        isListEmpty = RtlIsListEmpty(&pSDIODev->RxPktList);
        RtlUpMutex(&pSDIODev->RxMutex);
	} while(!isListEmpty);

	if (RxBdWrite > 0) {
        RtlDownMutex(&pSDIODev->RxMutex);
        HAL_SDIO_WRITE16(REG_SPDIO_RXBD_C2H_WPTR, pSDIODev->RXBDWPtr);
		HAL_SDIO_WRITE8(REG_SPDIO_HCI_RX_REQ, BIT_HCI_RX_REQ);
        pSDIODev->RxFifoBusy = 1;
        RtlUpMutex(&pSDIODev->RxMutex);
	}

	if (isForceBreak) {
//		SDIO_Recycle_Rx_BD(pSDIODev);
		// function end with insufficient resource, set event to try again later
		SDIO_SetEvent(pSDIODev, SDIO_EVENT_RX_PKT_RDY);
#if !TASK_SCHEDULER_DISABLED
        RtlMsleepOS(1);    // no resource, sleep a while
		RtlUpSema(&pSDIODev->RxSema);
#else
		SDIO_TaskUp(pSDIODev);
#endif	
	}
	DBG_SDIO_INFO("SDIO_Return_Rx_Data(%d)<==\n", RxBdWrite);
	
}

/******************************************************************************
 * Function: SDIO_Register_Tx_Callback
 * Desc: For a TX data target driver to register its TX callback function, so 
 *		the SDIO driver can use it to fordward a TX packet to the target driver.
 *
 * Para:
 *	pSDIODev: point to the SDIO device handler
 * 	CallbackFun: The function pointer of the callback
 *	pAdapter: a pointer will be use to call the registered CallBack function.
 *				It can be used to point to a handler of the caller, like WLan
 *				Adapter.
 ******************************************************************************/
VOID SDIO_Register_Tx_Callback(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN s8 (*CallbackFun)(VOID *pAdapter, u8 *pPkt, u16 Offset, u16 PktSize),
	IN VOID *pAdapter
)
{
	pSDIODev->Tx_Callback = CallbackFun;
	pSDIODev->pTxCb_Adapter = pAdapter;
}

/******************************************************************************
 * Function: SDIO_Rx_Callback
 * Desc: The callback function for an packet receiving, which be called from 
 *		the Target (WLan) driver to send a packet to the SDIO host.
 *
 * Para:
 *  pSDIODev: Point to the SDIO device data structer.
 *	pData: Point to the head of the data to be return to the Host system.
 *	Length: The length of the data to be send, in byte.
 *	
 * Return:
 * 	 The result, SUCCESS or FAIL.
 ******************************************************************************/
s8 SDIO_Rx_Callback(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN VOID *pData,
	IN u16 Offset,
	IN u16 PktSize,
	IN u8 CmdType
)
{
	PSDIO_RX_DESC pRxDesc;
	SDIO_RX_PACKET *pPkt;
#if CONFIG_INIC_EN
#if CONFIG_INIC_SKB_RX
	struct sk_buff *skb = (struct sk_buff *)pData;
#endif
#endif

	pPkt = SDIO_Alloc_Rx_Pkt(pSDIODev);
	if (pPkt == NULL) {
		DBG_SDIO_ERR("RX Callback Err!! No Free RX PKT!\n");
		return FAIL;
	}
	pRxDesc = &pPkt->RxDesc;
    pRxDesc->type = CmdType;
	pRxDesc->pkt_len = PktSize;
#if CONFIG_INIC_EN
#if CONFIG_INIC_SKB_RX
	pRxDesc->offset = sizeof(SDIO_RX_DESC)+Offset;//for data alignment reason
	pPkt->skb = skb;
	pPkt->pData = skb->data;
	pPkt->Offset = 0;
#else //CONFIG_INIC_SKB_RX
	pRxDesc->offset = sizeof(SDIO_RX_DESC);
	pPkt->pData = pData;
	pPkt->Offset = Offset;
#endif//CONFIG_INIC_SKB_RX
#else //CONFIG_INIC_EN
	pRxDesc->offset = sizeof(SDIO_RX_DESC);
	pPkt->pData = pData;
	pPkt->Offset = Offset;
#endif //CONFIG_INIC_EN
	RtlDownMutex(&pSDIODev->RxMutex);
	RtlListInsertTail(&pPkt->list, &pSDIODev->RxPktList);
	pSDIODev->RxInQCnt++;
	RtlUpMutex(&pSDIODev->RxMutex);
	SDIO_SetEvent(pSDIODev, SDIO_EVENT_RX_PKT_RDY);
#if !TASK_SCHEDULER_DISABLED
    if (pSDIODev->RxInQCnt == 1) {
        RtlUpSema(&pSDIODev->RxSema);
    }
#else
	SDIO_TaskUp(pSDIODev);
#endif	

	return SUCCESS;
}

/******************************************************************************
 * Function: SDIO_Handle_MsgBlk
 * Desc: Process a message block.
 *
 * Para:
 *  pSDIODev: Point to the SDIO device data structer.
 *	MSG_BLK: The message block to be processed.
 *	Length: The length of the data to be send, in byte.
 *	
 * Return:
 * 	 The result, SUCCESS or FAIL.
 ******************************************************************************/
s8 SDIO_Handle_MsgBlk(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN MSG_BLK *pMblk
)
{
	s8 ret=SUCCESS;

	DBG_SDIO_INFO("SDIO_Handle_MsgBlk==> MsgType=%d\n", pMblk->MsgType);	
	switch (pMblk->MsgType) {
	case MSG_SDIO_RX_PKT:
		ret = SDIO_Rx_Callback(pSDIODev, pMblk->pBuf, pMblk->Reserved, pMblk->DateLen, SDIO_CMD_RX_ETH); //pMblk->Reserved = Offset
        if (SUCCESS != ret) {
            // failed to send this packet to the host, drop it
            RtlMfree((u8 *) pMblk->pBuf, (pMblk->Reserved + pMblk->DateLen));   // pMblk->Reserved = Offset
#if SDIO_DEBUG
            RtlDownMutex(&pSDIODev->StatisticMutex);
            pSDIODev->MemAllocCnt--;
            RtlUpMutex(&pSDIODev->StatisticMutex);
#endif
#if SDIO_MP_MODE
            pSDIODev->MP_RxDropCnt++;
#endif
        }
		break;

	case MSG_SDIO_C2H:
		break;
		
	case MSG_SDIO_RPWM:
		break;
		
	default:
		DBG_SDIO_WARN("SDIO_Handle_MsgBlk: UnKnown MsgType %d\n", pMblk->MsgType);
		break;
	}

	return ret;
}

#if SDIO_MP_MODE

/******************************************************************************
 * Function: SDIO_PeriodicalTimerCallback
 * Desc: The callback function of the 1 Sec timer. It be used to statistic the
 *          throughput and update the status or something need to do periodically.
 *
 * Para:
 *  pContex: this pointer actually is the pointer of the SDIO device.
 *	
 * Return: None
 ******************************************************************************/
VOID SDIO_PeriodicalTimerCallback(
    void *pContex
)
{
    PHAL_SDIO_ADAPTER pSDIODev = pContex;

    if (SDIO_IsEventPending(pSDIODev, SDIO_EVENT_DUMP)) {
        SDIO_StatisticDump(pSDIODev);
}
}

#if !TASK_SCHEDULER_DISABLED
/******************************************************************************
 * Function: SDIO_MP_Task
 * Desc: The SDIO MP test task handler. This is the main function of the SDIO 
 *      device MP test mode.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SDIO_MP_Task(
	IN VOID *pData
)
{
    PHAL_SDIO_ADAPTER pSDIODev = pData;
    MSG_BLK Mblk_r;
    MSG_BLK Mblk_w;
    SDIO_MP_RX_PACKET *pRxPkt;
	_LIST *plist;
    int malloc_err_cnt=0;

    DiagPrintf("SDIO_MP_Task Started...\n");
	RtlInitListhead(&pSDIODev->MP_RxPktList);

    /* Initial resource */
    for (;;)
    {
        /* Task blocked and wait the semaphore(events) here */
        RtlDownSema(&pSDIODev->MP_EventSema);
        /* handle message block in the mailbox */
        do {
            if (_SUCCESS == RtlMailboxReceive(MBOX_ID_SDIO_MP, &Mblk_r, MBOX_WAIT_NONE, 0)) {
                switch (Mblk_r.MsgType) {
                    case MSG_SDIO_MP_LOOP_TXPKT:
                        pRxPkt = NULL;
                        malloc_err_cnt = 0;
                        do {
                            pRxPkt = (SDIO_MP_RX_PACKET *)RtlZmalloc(sizeof(SDIO_MP_RX_PACKET));
                            if (NULL != pRxPkt) {
                                pRxPkt->pData = Mblk_r.pBuf;
                                pRxPkt->Offset = Mblk_r.Reserved;
                                pRxPkt->DataLen = Mblk_r.DateLen; 
                                RtlListInsertTail(&pRxPkt->list, &pSDIODev->MP_RxPktList);
                            }
                            else {
                                RtlMsleepOS(10);
                                malloc_err_cnt++;
                                if (malloc_err_cnt > 100) {
                                    DBG_SDIO_ERR("SDIO_MP_Task: Malloc for Rx Pkt Failed\n");
                                    // no memory to handle this packet, drop it
                                    RtlMfree(Mblk_r.pBuf, (Mblk_r.Reserved+Mblk_r.DateLen));
                                    pSDIODev->MP_RxDropCnt++;
#if SDIO_DEBUG
                                    RtlDownMutex(&pSDIODev->StatisticMutex);
                                    pSDIODev->MemAllocCnt--;
                                    RtlUpMutex(&pSDIODev->StatisticMutex);
#endif
                                    break;  // break the while loop
                                }
                            }
                        }while (NULL == pRxPkt);
                        break;
                    
                    default:
                        DBG_SDIO_WARN("SDIO_MP_TASK: UnKnown MsgType %d\n", Mblk_r.MsgType);
                        break;
                }
            }
            else {
                break;  // no more message pending, break the while loop
            }
        } while (1);

        while (!RtlIsListEmpty(&pSDIODev->MP_RxPktList)) {
            plist = RtlListGetNext(&pSDIODev->MP_RxPktList); 
            pRxPkt = CONTAINER_OF(plist, SDIO_MP_RX_PACKET, list);            
            RtlListDelete(&pRxPkt->list);

            Mblk_w.MsgType = MSG_SDIO_RX_PKT;
            Mblk_w.pBuf = pRxPkt->pData;
            Mblk_w.Reserved = pRxPkt->Offset;
            Mblk_w.DateLen = pRxPkt->DataLen;
            if (_SUCCESS != RtlMailboxSendToBack(MBOX_ID_SDIO, &Mblk_w, 2000, 0)) {
                DBG_SDIO_ERR("SDIO_MP_Task: Send MSG_SDIO_RX_PKT FAILED\n");
                RtlListInsertHead(&pRxPkt->list, &pSDIODev->MP_RxPktList);
                break;
            }
            else {
                RtlMfree((u8 *)pRxPkt, sizeof(SDIO_MP_RX_PACKET));
            }
        }

        RtlEnterCritical();
        if (pSDIODev->MP_Events & SDIO_EVENT_EXIT) {
            pSDIODev->MP_Events &= ~SDIO_EVENT_EXIT;
            RtlExitCritical();
            DBG_SDIO_INFO("SDIO_MP_Task Exiting...\n");
            break;  // break the loop to exit the task
        }
        RtlExitCritical();
    }

    RtlEnterCritical();
    pSDIODev->MP_Events |= SDIO_EVENT_MP_STOPPED;
    RtlExitCritical();
    DBG_SDIO_INFO("SDIO_MP_Task Stoped!\n");
#if ( INCLUDE_vTaskDelete == 1 )
    vTaskDelete(NULL);
#endif
}
#endif  // end of "#if !TASK_SCHEDULER_DISABLED"

/******************************************************************************
 * Function: SDIO_MapMPCmd
 * Desc: Map a MP command string to a MP command type.
 *
 * Para:
 *	CmdStr: point to the command string buffer
 *	
 * return:
 *	The MP command type
 *
 ******************************************************************************/
u8 SDIO_MapMPCmd(
	IN char *CmdStr,
	IN u16 *Offset
)
{
	char cmd_str[16];
	u16 i;
	u16 str_len=0;
	u16 entry_num;
	u8 mp_cmd=0xff;

	for (i=0;i<16;i++) {
		if ((' ' != *(CmdStr+i)) && ('=' != *(CmdStr+i)) && (*(CmdStr+i))) {
			cmd_str[i] = *(CmdStr+i);
			str_len++;
		}
		else {
			break;
		}
	}

	*Offset = str_len+1;

	entry_num = sizeof(SDIO_MPCmdTable)/sizeof(SDIO_MP_CMD);

	for (i=0;i<entry_num;i++) {
		if (!_memcmp((void*)cmd_str, (void*)SDIO_MPCmdTable[i].cmd_name, str_len)) {
			mp_cmd = SDIO_MPCmdTable[i].cmd_type;
			break;
		}
	}

	return mp_cmd;
}

/******************************************************************************
 * Function: SDIO_DumpMPStatus
 * Desc: Dump SDIO MP mode status
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 *	
 * return: None
 *
 ******************************************************************************/
VOID SDIO_DumpMPStatus(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
	DiagPrintf("MP_Mode=%d\n", pSDIODev->MP_ModeEn);
	DiagPrintf("MP_Loopback=%d\n", pSDIODev->MP_LoopBackEn);
	DiagPrintf("TX: Packet Count=%d, Byte Count=%d\n", pSDIODev->MP_TxPktCnt, pSDIODev->MP_TxByteCnt);
	DiagPrintf("TX: TX_BD_WPTR=%d, TX_BD_RPTR=%d\n", HAL_SDIO_READ16(REG_SPDIO_TXBD_WPTR), HAL_SDIO_READ16(REG_SPDIO_TXBD_RPTR));
	DiagPrintf("RX: Packet Count=%d, Byte Count=%d\n", pSDIODev->MP_RxPktCnt, pSDIODev->MP_RxByteCnt);
	DiagPrintf("RX: RXBDWPtr=%d, RXBDRPtr=%d\n", pSDIODev->RXBDWPtr, pSDIODev->RXBDRPtr);
#if SDIO_DEBUG
	DiagPrintf("RX: InQueueCount=%d MemAllocatedCnt=%d\n", pSDIODev->RxInQCnt, pSDIODev->MemAllocCnt);
#endif
	DiagPrintf("TxDropPkt=%d RxDropPkt=%d\n", pSDIODev->MP_TxDropCnt, pSDIODev->MP_RxDropCnt);
}

/******************************************************************************
 * Function: SDIO_StatisticDump
 * Desc: Periodical dump SDIO throughput and other status.
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 *	
 * return: None
 *
 ******************************************************************************/
VOID SDIO_StatisticDump(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
    unsigned long tp;   // throughput
    u32 oldest_byte_cnt;
    u32 tx_avg_tp=0;  // in Kbps
    u32 rx_avg_tp=0;  // in Kbps

    // calculate the TX throughput
    if (pSDIODev->TxAvgWinCnt >= SDIO_AVG_TP_WIN_SIZE) {
        // flush the oldest one and add the newest one
        oldest_byte_cnt = pSDIODev->MP_TxAvgTPWin[pSDIODev->OldestTxAvgWinIdx];

        if (pSDIODev->MP_TxAvgTPWinSum >= oldest_byte_cnt) {
            pSDIODev->MP_TxAvgTPWinSum -= oldest_byte_cnt;
            pSDIODev->MP_TxAvgTPWin[pSDIODev->OldestTxAvgWinIdx] = pSDIODev->MP_TxByteCntInPeriod;
            pSDIODev->OldestTxAvgWinIdx++;
            if (SDIO_AVG_TP_WIN_SIZE <= pSDIODev->OldestTxAvgWinIdx) {
                pSDIODev->OldestTxAvgWinIdx = 0;
            }

            if (0 == pSDIODev->MP_TxAvgTPWinSum) {
                // reset the statistic
                pSDIODev->TxAvgWinCnt = 0;
                pSDIODev->OldestTxAvgWinIdx = 0;
            }
        }
        else {
            pSDIODev->MP_TxAvgTPWinSum = 0;
            // reset the statistic
            if (pSDIODev->MP_TxByteCntInPeriod > 0)
                pSDIODev->TxAvgWinCnt = 1;
            else
            pSDIODev->TxAvgWinCnt = 0;
            pSDIODev->OldestTxAvgWinIdx = 0;
        }
        pSDIODev->MP_TxAvgTPWinSum += pSDIODev->MP_TxByteCntInPeriod;
        tx_avg_tp = (pSDIODev->MP_TxAvgTPWinSum << 3) / (SDIO_PERIODICAL_TIMER_INTERVAL * SDIO_AVG_TP_WIN_SIZE);
    }
    else {
        if ((pSDIODev->MP_TxAvgTPWinSum > 0) || (pSDIODev->MP_TxByteCntInPeriod > 0)) {
            pSDIODev->MP_TxAvgTPWinSum += pSDIODev->MP_TxByteCntInPeriod;
            pSDIODev->MP_TxAvgTPWin[pSDIODev->TxAvgWinCnt] = pSDIODev->MP_TxByteCntInPeriod;
            pSDIODev->TxAvgWinCnt++;
            tx_avg_tp = (pSDIODev->MP_TxAvgTPWinSum << 3) / (SDIO_PERIODICAL_TIMER_INTERVAL * pSDIODev->TxAvgWinCnt);
        }
    }

    // calculate the RX throughput
    if (pSDIODev->RxAvgWinCnt >= SDIO_AVG_TP_WIN_SIZE) {
        // flush the oldest one and add the newest one
        oldest_byte_cnt = pSDIODev->MP_RxAvgTPWin[pSDIODev->OldestRxAvgWinIdx];
        if (pSDIODev->MP_RxAvgTPWinSum >= oldest_byte_cnt) {
            pSDIODev->MP_RxAvgTPWinSum -= oldest_byte_cnt;
            pSDIODev->MP_RxAvgTPWin[pSDIODev->OldestRxAvgWinIdx] = pSDIODev->MP_RxByteCntInPeriod;
            pSDIODev->OldestRxAvgWinIdx++;
            if (SDIO_AVG_TP_WIN_SIZE <= pSDIODev->OldestRxAvgWinIdx) {
                pSDIODev->OldestRxAvgWinIdx = 0;
            }

            if (0 == pSDIODev->MP_RxAvgTPWinSum) {
                // reset the statistic
                pSDIODev->RxAvgWinCnt = 0;
                pSDIODev->OldestRxAvgWinIdx = 0;
            }
        }
        else {
            pSDIODev->MP_RxAvgTPWinSum = 0;
            // reset the statistic
            if (pSDIODev->MP_RxByteCntInPeriod > 0)
            	pSDIODev->RxAvgWinCnt = 1;
            else
                pSDIODev->RxAvgWinCnt = 0;
                
            pSDIODev->OldestRxAvgWinIdx = 0;
        }
        pSDIODev->MP_RxAvgTPWinSum += pSDIODev->MP_RxByteCntInPeriod;

        rx_avg_tp = (pSDIODev->MP_RxAvgTPWinSum << 3) / (SDIO_PERIODICAL_TIMER_INTERVAL * SDIO_AVG_TP_WIN_SIZE);
    }
    else {
        if ((pSDIODev->MP_RxAvgTPWinSum > 0) || (pSDIODev->MP_RxByteCntInPeriod > 0)) {
            pSDIODev->MP_RxAvgTPWinSum += pSDIODev->MP_RxByteCntInPeriod;
            pSDIODev->MP_RxAvgTPWin[pSDIODev->RxAvgWinCnt] = pSDIODev->MP_RxByteCntInPeriod;
            pSDIODev->RxAvgWinCnt++;
            rx_avg_tp = (pSDIODev->MP_RxAvgTPWinSum << 3) / (SDIO_PERIODICAL_TIMER_INTERVAL * pSDIODev->RxAvgWinCnt);
        }
    }
    
    if ((pSDIODev->MP_TxByteCntInPeriod > 0) || (pSDIODev->MP_RxByteCntInPeriod > 0)) {
    	DiagPrintf("SDIO Dump:\n");
        tp = (pSDIODev->MP_TxByteCntInPeriod << 3)/(SDIO_PERIODICAL_TIMER_INTERVAL/1000);
        if (tp > 1000) {
    	    DiagPrintf("TX: Packet Count=%d, Byte Count=%d, TP=%d Kbps\n", pSDIODev->MP_TxPktCntInPeriod, pSDIODev->MP_TxByteCntInPeriod, tp/1000);
        }
        else {
    	    DiagPrintf("TX: Packet Count=%d, Byte Count=%d, TP=%d bps\n", pSDIODev->MP_TxPktCntInPeriod, pSDIODev->MP_TxByteCntInPeriod, tp);
        }
        tp = (pSDIODev->MP_RxByteCntInPeriod << 3)/(SDIO_PERIODICAL_TIMER_INTERVAL/1000);
        if (tp > 1000) {
            DiagPrintf("RX: Packet Count=%d, Byte Count=%d, TP=%d Kbps\n", pSDIODev->MP_RxPktCntInPeriod, pSDIODev->MP_RxByteCntInPeriod, tp/1000);
        }
        else {
            DiagPrintf("RX: Packet Count=%d, Byte Count=%d, TP=%d bps\n", pSDIODev->MP_RxPktCntInPeriod, pSDIODev->MP_RxByteCntInPeriod, tp);
        }

        pSDIODev->MP_TxPktCntInPeriod = 0;
        pSDIODev->MP_TxByteCntInPeriod = 0;
        pSDIODev->MP_RxPktCntInPeriod = 0;
        pSDIODev->MP_RxByteCntInPeriod = 0;
    }

    if ((tx_avg_tp > 0) || (rx_avg_tp > 0)) {
        DiagPrintf("TX Avg TP=%d Kbps, RX Avg TP=%d Kbps\n", tx_avg_tp, rx_avg_tp);
    }
}

/******************************************************************************
 * Function: SDIO_MP_Loopback
 * Desc: The loopback test function for MP mode.
 *
 * Para:
 *	pAdapter: a pointer which got from the callback function register, 
 *				here it point to the SDIO device itself.
 *	pData: The pointer of the SDIO TX data buffer.
 *	PktSize: The size (in byte) of this SDIO TX data. 
 *	
 * return:
 *	The result of this function, SUCCESS or FAILED.
 *
 ******************************************************************************/
s8 SDIO_MP_Loopback(
	IN VOID *pAdapter,
	IN u8 *pData,
	IN u16 Offset,
	IN u16 PktSize
)
{
#if TASK_SCHEDULER_DISABLED
	PHAL_SDIO_ADAPTER pSDIODev=(PHAL_SDIO_ADAPTER)pAdapter;
#endif
	s8 ret;

	MSG_BLK MBlk;
	
//	DBG_SDIO_INFO("SDIO_MP_Loopback==>\n");


#if TASK_SCHEDULER_DISABLED
	ret = SDIO_Rx_Callback(pSDIODev, pData, Offset, PktSize, SDIO_CMD_RX_ETH);
#else
	// Mailbox test, use message to replace call SDIO_Rx_Callback directly
	MBlk.MsgType = MSG_SDIO_MP_LOOP_TXPKT;
	MBlk.pBuf = pData;
	MBlk.Reserved = (u8)Offset;
	MBlk.DateLen = PktSize;
	if (_SUCCESS == RtlMailboxSendToBack(MBOX_ID_SDIO_MP, &MBlk, 2000, 0)) {
	    ret = SUCCESS;
    }
    else {
        DBG_SDIO_ERR("SDIO_MP_Loopback FAILED\n");
        ret = FAIL;
    }
#endif	
	return ret;
}

/******************************************************************************
 * Function: SDIO_MP_ContinueTx
 * Desc: The continue TX test function for MP mode. We just drop the TX packet.
 *
 * Para:
 *	pAdapter: a pointer which got from the callback function register, 
 *				here it point to the SDIO device itself.
 *	pData: The pointer of the SDIO TX data buffer.
 *	PktSize: The size (in byte) of this SDIO TX data. 
 *	
 * return:
 *	The result of this function, SUCCESS or FAILED.
 *
 ******************************************************************************/
s8 SDIO_MP_ContinueTx(
	IN VOID *pAdapter,
	IN u8 *pData,
	IN u16 Offset,
	IN u16 PktSize
)
{
	PHAL_SDIO_ADAPTER pSDIODev=(PHAL_SDIO_ADAPTER)pAdapter;

    RtlMfree(pData, (Offset+PktSize));
#if SDIO_DEBUG
    RtlDownMutex(&pSDIODev->StatisticMutex);
    pSDIODev->MemAllocCnt--;
    RtlUpMutex(&pSDIODev->StatisticMutex);
#endif
    return SUCCESS;
}

/******************************************************************************
 * Function: SDIO_MP_ContinueRx
 * Desc: Process Continue RX test.
 *
 * Para:
 *	pSDIODev: The SDIO device adapter.
 *	
 * return: None
 *
 ******************************************************************************/
VOID SDIO_MP_ContinueRx(
	IN PHAL_SDIO_ADAPTER pSDIODev
)
{
    u8 *pRxBuf=NULL;
    s8 ret;
    
    while (1) {
        if (pSDIODev->MP_CRxPktPendingCnt > 10) {
            break;
        }
        
        if (pSDIODev->MP_ContinueRxMode == SDIO_CRX_STATIC_BUF) {
            pRxBuf = pSDIODev->pMP_CRxBuf;
        }
        else if (pSDIODev->MP_ContinueRxMode == SDIO_CRX_DYNA_BUF) {
            pRxBuf = RtlMalloc(pSDIODev->MP_CRxSize+26);    // 26: Wlan header
#if SDIO_DEBUG
            pSDIODev->MemAllocCnt++;
#endif
            if (NULL != pRxBuf) {
                _memcpy(pRxBuf, MP_WlanHdr, 26);
                _memset((pRxBuf+26), 0x3E, pSDIODev->MP_CRxSize);
            }
        }
            
        if (NULL != pRxBuf) {
            ret = SDIO_Rx_Callback(pSDIODev, pRxBuf, 0, pSDIODev->MP_CRxSize+26, SDIO_CMD_RX_ETH);
            if (SUCCESS == ret) {
                if (pSDIODev->MP_CRxPktCnt > 0) {
                    pSDIODev->MP_CRxPktCnt--;
                    pSDIODev->MP_CRxPktPendingCnt++;
                    if (0 == pSDIODev->MP_CRxPktCnt) {
                        pSDIODev->MP_ContinueRx = 0;
                        break;  // break the while loop
                    }
                }
                if (pSDIODev->MP_CRxPktPendingCnt > 10) {
                    break;
                }
            }
            else {
                if (pSDIODev->MP_ContinueRxMode == SDIO_CRX_DYNA_BUF) {
                    RtlMfree(pRxBuf, pSDIODev->MP_CRxSize+26);
#if SDIO_DEBUG
                    pSDIODev->MemAllocCnt--;
#endif
                }
#if !TASK_SCHEDULER_DISABLED
                RtlMsleepOS(10);    // no resource, sleep a while
                RtlUpSema(&pSDIODev->RxSema);
#else
                SDIO_TaskUp(pSDIODev);
#endif	
                break;
            }
        }
        else {
#if !TASK_SCHEDULER_DISABLED
            RtlMsleepOS(10);    // no resource, sleep a while
            RtlUpSema(&pSDIODev->RxSema);
#else
            SDIO_TaskUp(pSDIODev);
#endif	
            break;
        }
    }
}

/******************************************************************************
 * Function: SDIO_DeviceMPApp
 * Desc: To handle SDIO MP command
 *
 * Para:
 *	pData: point to the command buffer
 *	
 ******************************************************************************/
VOID SDIO_DeviceMPApp(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN u16 argc, 
    IN u8  *argv[]
)
{
	u8 cmd_type;
	u16 offset=0;
	u32 arg1, arg2;
	int ret;
    int i;

	DBG_SDIO_INFO("==>MP_App: arg_num=%d cmd_str=%s\n", argc, (char *)argv[0]);	

	cmd_type = SDIO_MapMPCmd((char *)argv[0], &offset);
	DBG_SDIO_INFO("MP_App: MP_Cmdtype=%d\n", cmd_type);	

	switch (cmd_type)
	{
	case SDIO_MP_START:
		if (!pSDIODev->MP_ModeEn) {
			pSDIODev->MP_ModeEn = 1;
			pSDIODev->MP_TxPktCnt = 0;		/* SDIO TX packet count */
			pSDIODev->MP_RxPktCnt = 0;		/* SDIO RX packet count */
			pSDIODev->MP_TxByteCnt = 0;		/* SDIO TX Byte count */
			pSDIODev->MP_RxByteCnt = 0;		/* SDIO RX Byte count */
			DiagPrintf("SDIO MP Started!\n");
		}
		else {
			DiagPrintf("In SDIO MP Mode already!\n");
		}
		break;
		
	case SDIO_MP_STOP:
		pSDIODev->MP_ModeEn = 0;
		DiagPrintf("SDIO MP Stoped!\n");
		break;
		
	case SDIO_MP_LOOPBACK:
		DBG_SDIO_INFO("MP_App: argv[1]=%s\n", argv[1]);
		if (pSDIODev->MP_ModeEn == 0) {
			DiagPrintf("Not in MP mode!! Please start MP mode first.\n");
			break;
		}
		arg1 = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
		if (arg1) {
			if (pSDIODev->MP_LoopBackEn == 0) {
                // Create a Task for MP loopback test
#if !TASK_SCHEDULER_DISABLED
                RtlInitSema(&(pSDIODev->MP_EventSema), 0);
                if (NULL == pSDIODev->MP_EventSema){
                    DBG_SDIO_ERR("SDIO MP_Loopback Create Semaphore Err!!\n");  
                    break;  // break the switch case
                }
            
                /* create a Mailbox for other driver module to send message to SDIO driver */
                pSDIODev->pMP_MBox = RtlMailboxCreate(MBOX_ID_SDIO_MP, SDIO_MAILBOX_SIZE, &(pSDIODev->MP_EventSema));
                if (NULL == pSDIODev->pMBox) {
                    DBG_SDIO_ERR("SDIO MP_Loopback Create Mailbox Err!!\n");    
                    break;  // break the switch case
                }
            
                /* Create the SDIO task */
#ifdef PLATFORM_FREERTOS
                ret = xTaskCreate( SDIO_MP_Task, "SDIO_MP_TASK", ((256*4)/sizeof(portBASE_TYPE)), (void *)pSDIODev, SDIO_MP_TASK_PRIORITY, &pSDIODev->MP_TaskHandle);
                if (pdTRUE != ret )
                {
                    DBG_SDIO_ERR("SDIO MP Create Task Err(%d)!!\n", ret);
                    break;
                }
#endif
                DiagPrintf("SDIO MP Task Created\n");
#endif	// end of "#if !TASK_SCHEDULER_DISABLED"
                
				// Backup origional TX Callback function
				pSDIODev->pTxCallback_Backup = pSDIODev->Tx_Callback;
				pSDIODev->pTxCb_Adapter_Backup = (VOID *)pSDIODev->pTxCb_Adapter;			
				DiagPrintf("Register SDIO TX Callback with Loopback function\n");
				SDIO_Register_Tx_Callback(pSDIODev, &SDIO_MP_Loopback, (VOID *) pSDIODev);
				pSDIODev->MP_LoopBackEn = 1;
			}
			else {
				DiagPrintf("SDIO MP LoopBack is On already!\n");
			}
		}
		else {
			if (pSDIODev->MP_LoopBackEn) {
				// Restore origional TX Callback function
				DiagPrintf("Restore SDIO TX Callback...\n");
				SDIO_Register_Tx_Callback(pSDIODev, pSDIODev->pTxCallback_Backup, pSDIODev->pTxCb_Adapter_Backup);
				pSDIODev->MP_LoopBackEn = 0;
#if !TASK_SCHEDULER_DISABLED
                /* Exit the SDIO task */
                if (pSDIODev->MP_TaskHandle) {
                    RtlEnterCritical();
                    pSDIODev->MP_Events |= SDIO_EVENT_EXIT;
                    RtlExitCritical();
                    RtlUpSema(&pSDIODev->MP_EventSema);
                    i=0;
                    while (1) {
                        RtlEnterCritical();
                        if (pSDIODev->MP_Events & SDIO_EVENT_MP_STOPPED) {
                            RtlExitCritical();
                            break;
                        }
                        RtlExitCritical();
                        RtlMsleepOS(10);
                        i++;
                        if (i> 100) {
                            DBG_SDIO_ERR("Delete SDIO MP Task Failed with Timeout\n");
                            break;
                        }
                    }
                }
            
                /* Delete the Mailbox */
                if (pSDIODev->pMP_MBox) {
                    RtlMailboxDel(pSDIODev->pMP_MBox);
                    pSDIODev->pMP_MBox = NULL;
                }
            
                /* Delete the Semaphore */
                if (pSDIODev->MP_EventSema) {
                    RtlFreeSema(&pSDIODev->MP_EventSema);
                    pSDIODev->MP_EventSema = NULL;
                }
                DiagPrintf("SDIO MP Task Deleted\n");
#endif
                
			}
		}
		DiagPrintf("SDIO MP LoopBack=%d\n", pSDIODev->MP_LoopBackEn);
		break;
		
	case SDIO_MP_STATUS:
		SDIO_DumpMPStatus(pSDIODev);
		break;

	case SDIO_MP_READ_REG8:
		arg1 = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
		DiagPrintf("SDIO_Reg[%x]=%x\n", arg1, HAL_SDIO_READ8(arg1));
		break;

	case SDIO_MP_READ_REG16:
		arg1 = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
		DiagPrintf("SDIO_Reg[%x]=%x\n", arg1, HAL_SDIO_READ16(arg1));
		break;

	case SDIO_MP_READ_REG32:
		arg1 = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
		DiagPrintf("SDIO_Reg[%x]=%x\n", arg1, HAL_SDIO_READ32(arg1));
		break;

	case SDIO_MP_WRITE_REG8:
		arg1 = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
		arg2 = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
		HAL_SDIO_WRITE8(arg1, arg2);
		DiagPrintf("Write Reg[%x]=%x, Readback:%x\n", arg1, arg2, HAL_SDIO_READ8(arg1));
		break;

	case SDIO_MP_WRITE_REG16:
		arg1 = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
		arg2 = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
		HAL_SDIO_WRITE16(arg1, arg2);
		DiagPrintf("Write Reg[%x]=%x, Readback:%x\n", arg1, arg2, HAL_SDIO_READ16(arg1));
		break;

	case SDIO_MP_WRITE_REG32:
		arg1 = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
		arg2 = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
		HAL_SDIO_WRITE32(arg1, arg2);
		DiagPrintf("Write Reg[%x]=%x, Readback:%x\n", arg1, arg2, HAL_SDIO_READ32(arg1));
		break;

	case SDIO_MP_WAKEUP:
		SDIO_Wakeup_Task(pSDIODev);
		break;
		
	case SDIO_MP_DUMP:
		arg1 = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
        if (arg1) {
            if (!SDIO_IsEventPending(pSDIODev, SDIO_EVENT_DUMP)) {
                // reset statistic
                for (i=0;i<SDIO_AVG_TP_WIN_SIZE;i++) {
                    pSDIODev->MP_TxAvgTPWin[i] = 0;
                    pSDIODev->MP_RxAvgTPWin[i] = 0;
                }
                pSDIODev->MP_TxAvgTPWinSum = 0;
                pSDIODev->MP_RxAvgTPWinSum = 0;
                pSDIODev->OldestTxAvgWinIdx = 0;
                pSDIODev->OldestRxAvgWinIdx = 0;
                pSDIODev->TxAvgWinCnt = 0;
                pSDIODev->RxAvgWinCnt = 0;
            }
		    SDIO_SetEvent(pSDIODev, SDIO_EVENT_DUMP);
        }
        else {
		    SDIO_ClearEvent(pSDIODev, SDIO_EVENT_DUMP);
        }
        DiagPrintf("SDIO Dump %d\n", arg1);
		break;
        
    case SDIO_MP_CTX:
		arg1 = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
		if (arg1) {
            if (!pSDIODev->MP_ContinueTx) {
                // Backup origional TX Callback function
                pSDIODev->pTxCallback_Backup = pSDIODev->Tx_Callback;
                pSDIODev->pTxCb_Adapter_Backup = (VOID *)pSDIODev->pTxCb_Adapter;           
                DiagPrintf("Register SDIO TX Callback with Continue TX Test function\n");
                SDIO_Register_Tx_Callback(pSDIODev, &SDIO_MP_ContinueTx, (VOID *) pSDIODev);
                pSDIODev->MP_ContinueTx = 1;
            }
            DiagPrintf("SDIO Continue TX Test Enabled\n");
        }
        else {
            if (pSDIODev->MP_ContinueTx) {
				// Restore origional TX Callback function
				DiagPrintf("Restore SDIO TX Callback...\n");
				SDIO_Register_Tx_Callback(pSDIODev, pSDIODev->pTxCallback_Backup, pSDIODev->pTxCb_Adapter_Backup);
				pSDIODev->MP_ContinueTx = 0;
                DiagPrintf("SDIO Continue TX Test Disabled\n");
            }
            else {
                DiagPrintf("SDIO Continue TX Test Didn't Enabled\n");
            }
        }
		break;
        
    case SDIO_MP_CRX:
    case SDIO_MP_CRX_DA:
        if(SDIO_MP_CRX == cmd_type) {
            pSDIODev->MP_ContinueRxMode = SDIO_CRX_STATIC_BUF;
        }
        else if (SDIO_MP_CRX_DA == cmd_type) {
            pSDIODev->MP_ContinueRxMode = SDIO_CRX_DYNA_BUF;
        }
        
        if (pSDIODev->MP_ContinueRx) {
            DiagPrintf("SDIO Continue RX Test Running\n");
            break;
        }
        pSDIODev->MP_ContinueRx = 1;
		arg1 = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
        if (arg1 < 16) {
            DiagPrintf("SDIO RX Test Min Pkt Size is 16\n");
            arg1 = 16;
        }

        if (arg1 > 4096) {
            DiagPrintf("SDIO RX Test Max Pkt Size is 4096\n");
            arg1 = 4096;
        }
        pSDIODev->MP_CRxSize = arg1;
		arg2 = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
        pSDIODev->MP_CRxPktCnt = arg2;
        if (arg2 == 0) {
            pSDIODev->MP_CRxInfinite = 1;
        }

        if (SDIO_CRX_STATIC_BUF == pSDIODev->MP_ContinueRxMode) {
            if (NULL == pSDIODev->pMP_CRxBuf) {
                pSDIODev->pMP_CRxBuf = RtlMalloc(pSDIODev->MP_CRxSize+26);    // 26: Wlan header                
                DiagPrintf("SDIO RX Test: pBuf @ 0x%x\n", (u32)pSDIODev->pMP_CRxBuf);
                if (((u32)(pSDIODev->pMP_CRxBuf) & 0x03) != 0) {
                    DiagPrintf("SDIO RX Test: pBuf Not 4-bytes Aligned!!\n");
                }
#if SDIO_DEBUG
                pSDIODev->MemAllocCnt++;
#endif
                if (NULL != pSDIODev->pMP_CRxBuf) {
                    _memcpy(pSDIODev->pMP_CRxBuf, MP_WlanHdr, 26);
                    _memset((pSDIODev->pMP_CRxBuf+26), 0x3E, pSDIODev->MP_CRxSize);
                }
            }
            
            if (pSDIODev->pMP_CRxBuf) {
                DiagPrintf("SDIO RX Test(Static RX Buf): PktSize=%d, PktCount=%d\n", pSDIODev->MP_CRxSize, pSDIODev->MP_CRxPktCnt);
                SDIO_Wakeup_Task(pSDIODev);
            }
            else {
                pSDIODev->MP_ContinueRx = 0;
                pSDIODev->MP_CRxInfinite= 0;
                pSDIODev->MP_CRxPktCnt = 0;
                DiagPrintf("SDIO RX Test: Mem Allocate Failed\n");
            }
        }

        if (SDIO_CRX_DYNA_BUF == pSDIODev->MP_ContinueRxMode) {
            DiagPrintf("SDIO RX Test(Dyna-Allocate RX Buf): PktSize=%d, PktCount=%d\n", pSDIODev->MP_CRxSize, pSDIODev->MP_CRxPktCnt);
            SDIO_Wakeup_Task(pSDIODev);
        }
        
        break;

    case SDIO_MP_CRX_STOP:
        pSDIODev->MP_ContinueRx = 0;
        pSDIODev->MP_CRxPktCnt = 0;
        pSDIODev->MP_CRxInfinite= 0;
        DiagPrintf("SDIO RX Test Stopping...\n");
        break;

    case SDIO_MP_DBG_MSG:
        arg1 = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
        if (arg1) {
            ConfigDebugInfo |= _DBG_SDIO_;
            ConfigDebugWarn |= _DBG_SDIO_;
            DiagPrintf("SDIO Debug Message On.\n");
        }
        else {
            ConfigDebugInfo &= ~_DBG_SDIO_;
            ConfigDebugWarn &= ~_DBG_SDIO_;
            DiagPrintf("SDIO Debug Message Off.\n");
        }
        break;
        
	default:
		DiagPrintf("SDIO_DeviceMPApp: Unknown Cmd=%s\n", argv[0]);	
		DiagPrintf("==== SDIO Command Help ====\n");	
		DiagPrintf("SDIO mp_start                    : Enter MP mode\n");	
		DiagPrintf("SDIO mp_stop                     : Exit MP mode(Switch back to Normal mode)\n");	
		DiagPrintf("SDIO mp_loopback <1/0>           : enable/disable data path loopback test\n");	
		DiagPrintf("SDIO ctx <1/0>                   : Start/Stop SDIO continue TX test\n");	
		DiagPrintf("SDIO crx <PktSize> <PktCount>    : Start SDIO continue RX test with static RX Buffer\n");	
		DiagPrintf("SDIO crx_da <PktSize> <PktCount> : Start SDIO continue RX test with Dynamic Allocate RX Buffer\n");	
		DiagPrintf("SDIO crx_stop                    : Stop SDIO continue RX test\n");	
		DiagPrintf("SDIO status                      : Dump current SDIO driver status\n");	
		DiagPrintf("SDIO read_reg8 <Addr>            : Read SDIO register via 1-byte access\n");	
		DiagPrintf("SDIO read_reg16 <Addr>           : Read SDIO register via 2-bytes access\n");	
		DiagPrintf("SDIO read_reg32 <Addr>           : Read SDIO register via 4-bytes access\n");	
		DiagPrintf("SDIO write_reg8 <Addr> <Value>   : Write SDIO register via 1-byte access\n");	
		DiagPrintf("SDIO write_reg16 <Addr> <Value>  : Write SDIO register via 2-bytes access\n");	
		DiagPrintf("SDIO write_reg32 <Addr> <Value>  : Write SDIO register via 4-bytes access\n");	
		DiagPrintf("SDIO dump <1/0>                  : Start/Stop to dump SDIO throughput statistic periodically.\n");	
		break;
	}
}
#endif	/* endof '#if SDIO_MP_MODE' */


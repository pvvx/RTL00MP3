/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

//#include "mbed_assert.h"
#include "objects.h"
#include "PinNames.h"
//#include <osdep_api.h>
#include "hal_i2c.h"
#include "i2c_api.h"
#include "ex_api.h"


#if CONFIG_I2C_EN

//#include "cmsis.h"
#include "pinmap.h"


static const PinMap PinMap_I2C_SDA[] = {
    {PD_4,  RTL_PIN_PERI(I2C0, 0, S0), RTL_PIN_FUNC(I2C0, S0)},
    {PH_1,  RTL_PIN_PERI(I2C0, 0, S1), RTL_PIN_FUNC(I2C0, S1)},
    {PC_8,  RTL_PIN_PERI(I2C0, 0, S2), RTL_PIN_FUNC(I2C0, S2)},
    {PE_7,  RTL_PIN_PERI(I2C0, 0, S3), RTL_PIN_FUNC(I2C0, S3)},
    
    {PC_4,  RTL_PIN_PERI(I2C1, 1, S0), RTL_PIN_FUNC(I2C1, S0)},
    {PH_3,  RTL_PIN_PERI(I2C1, 1, S1), RTL_PIN_FUNC(I2C1, S1)},
    {PD_7,  RTL_PIN_PERI(I2C1, 1, S2), RTL_PIN_FUNC(I2C1, S2)},

    {PB_7,  RTL_PIN_PERI(I2C2, 2, S0), RTL_PIN_FUNC(I2C2, S0)},
    {PE_1,  RTL_PIN_PERI(I2C2, 2, S1), RTL_PIN_FUNC(I2C2, S1)},
    {PC_7,  RTL_PIN_PERI(I2C2, 2, S2), RTL_PIN_FUNC(I2C2, S2)},

    {PB_3,  RTL_PIN_PERI(I2C3, 3, S0), RTL_PIN_FUNC(I2C3, S0)},
    {PE_3,  RTL_PIN_PERI(I2C3, 3, S1), RTL_PIN_FUNC(I2C3, S1)},
    {PE_5,  RTL_PIN_PERI(I2C3, 3, S2), RTL_PIN_FUNC(I2C3, S2)},
    {PD_9,  RTL_PIN_PERI(I2C3, 3, S3), RTL_PIN_FUNC(I2C3, S3)},

    {NC,    NC,     0}
};

static const PinMap PinMap_I2C_SCL[] = {
    {PD_5,  RTL_PIN_PERI(I2C0, 0, S0), RTL_PIN_FUNC(I2C0, S0)},
    {PH_0,  RTL_PIN_PERI(I2C0, 0, S1), RTL_PIN_FUNC(I2C0, S1)},
    {PC_9,  RTL_PIN_PERI(I2C0, 0, S2), RTL_PIN_FUNC(I2C0, S2)},
    {PE_6,  RTL_PIN_PERI(I2C0, 0, S3), RTL_PIN_FUNC(I2C0, S3)},

    {PC_5,  RTL_PIN_PERI(I2C1, 1, S0), RTL_PIN_FUNC(I2C1, S0)},
    {PH_2,  RTL_PIN_PERI(I2C1, 1, S1), RTL_PIN_FUNC(I2C1, S1)},
    {PD_6,  RTL_PIN_PERI(I2C1, 1, S2), RTL_PIN_FUNC(I2C1, S2)},

    {PB_6,  RTL_PIN_PERI(I2C2, 2, S0), RTL_PIN_FUNC(I2C2, S0)},
    {PE_0,  RTL_PIN_PERI(I2C2, 2, S1), RTL_PIN_FUNC(I2C2, S1)},
    {PC_6,  RTL_PIN_PERI(I2C2, 2, S2), RTL_PIN_FUNC(I2C2, S2)},
    
    {PB_2,  RTL_PIN_PERI(I2C3, 3, S0), RTL_PIN_FUNC(I2C3, S0)},
    {PE_2,  RTL_PIN_PERI(I2C3, 3, S1), RTL_PIN_FUNC(I2C3, S1)},
    {PE_4,  RTL_PIN_PERI(I2C3, 3, S2), RTL_PIN_FUNC(I2C3, S2)},
    {PD_8,  RTL_PIN_PERI(I2C3, 3, S3), RTL_PIN_FUNC(I2C3, S3)},

    {NC,    NC,     0}
};

static uint16_t i2c_target_addr[4];
static SAL_I2C_TRANSFER_BUF    i2ctxtranbuf[4];
static SAL_I2C_TRANSFER_BUF    i2crxtranbuf[4];
extern u32 ConfigDebugErr;
extern u32 ConfigDebuginfo;
void i2c_init(i2c_t *obj, PinName sda, PinName scl) {
    
    uint32_t i2c_sel;
    uint32_t i2c_idx;
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_USERCB_ADPT    pSalI2CUserCBAdpt   = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    
    // Determine the I2C to use
    uint32_t i2c_sda = (uint32_t)pinmap_peripheral(sda, PinMap_I2C_SDA);
    uint32_t i2c_scl = (uint32_t)pinmap_peripheral(scl, PinMap_I2C_SCL);
	ConfigDebugErr &= (~(_DBG_I2C_|_DBG_GDMA_));
    ConfigDebugInfo&= (~(_DBG_I2C_|_DBG_GDMA_));
    i2c_sel = (uint32_t)pinmap_merge(i2c_sda, i2c_scl);
    i2c_idx = RTL_GET_PERI_IDX(i2c_sel);
    if (unlikely(i2c_idx == NC)) {
        DBG_8195A("%s: Cannot find matched port i2c\n", __FUNCTION__);
        return;
    }
    
    //DBG_8195A("i2c_sel:%x\n",i2c_sel);
    //DBG_8195A("i2c_idx:%x\n",i2c_idx);
    
    /* Get I2C device handler */
    pSalI2CMngtAdpt     = &(obj->SalI2CMngtAdpt);
    pSalI2CUserCBAdpt   = (PSAL_I2C_USERCB_ADPT)&(obj->SalI2CUserCBAdpt);
    
    

    /*To assign the rest pointers*/
    pSalI2CMngtAdpt->MstRDCmdCnt    = 0;
    pSalI2CMngtAdpt->InnerTimeOut   = 2000; // inner time-out count, 2000 ms
    pSalI2CMngtAdpt->pSalHndPriv    = &(obj->SalI2CHndPriv);
    pSalI2CMngtAdpt->pSalHndPriv->ppSalI2CHnd = (void**)&(pSalI2CMngtAdpt->pSalHndPriv);

    /* To assign the default (ROM) HAL OP initialization function */
#if  defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT) || defined(CONFIG_CHIP_C_CUT)
    pSalI2CMngtAdpt->pHalOpInit     = HalI2COpInit_Patch;
#elif defined(CONFIG_CHIP_E_CUT)
    pSalI2CMngtAdpt->pHalOpInit     = HalI2COpInit_V04;
#endif
    /* To assign the default (ROM) HAL GDMA OP initialization function */
    pSalI2CMngtAdpt->pHalGdmaOpInit = HalGdmaOpInit;

    /* To assign the default (ROM) SAL interrupt function */
#if defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT) || defined(CONFIG_CHIP_C_CUT)
    pSalI2CMngtAdpt->pSalIrqFunc    = I2CISRHandle_Patch;
#elif defined(CONFIG_CHIP_E_CUT)
    pSalI2CMngtAdpt->pSalIrqFunc    = I2CISRHandle_V04;
#endif

    /* To assign the default (ROM) SAL DMA TX interrupt function */
    pSalI2CMngtAdpt->pSalDMATxIrqFunc   = I2CTXGDMAISRHandle;

    /* To assign the default (ROM) SAL DMA RX interrupt function */
    pSalI2CMngtAdpt->pSalDMARxIrqFunc   = I2CRXGDMAISRHandle;

    pSalI2CMngtAdpt->pHalInitDat        = &(obj->HalI2CInitData);
    pSalI2CMngtAdpt->pHalOp             = &(obj->HalI2COp);
    pSalI2CMngtAdpt->pIrqHnd            = &(obj->I2CIrqHandleDat);
    pSalI2CMngtAdpt->pHalTxGdmaAdp      = &(obj->HalI2CTxGdmaAdpt);
    pSalI2CMngtAdpt->pHalRxGdmaAdp      = &(obj->HalI2CRxGdmaAdpt);
    pSalI2CMngtAdpt->pHalGdmaOp         = &(obj->HalI2CGdmaOp);
    pSalI2CMngtAdpt->pIrqTxGdmaHnd      = &(obj->I2CTxGdmaIrqHandleDat);
    pSalI2CMngtAdpt->pIrqRxGdmaHnd      = &(obj->I2CRxGdmaIrqHandleDat);
    pSalI2CMngtAdpt->pUserCB            = &(obj->SalI2CUserCB);
    pSalI2CMngtAdpt->pDMAConf           = &(obj->SalI2CDmaUserDef);
    
    /* Assign the private SAL handle to public SAL handle */
    pSalI2CHND      = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);

    /* Assign the internal HAL initial data pointer to the SAL handle */
    pSalI2CHND->pInitDat    = pSalI2CMngtAdpt->pHalInitDat;

    /* Assign the internal user callback pointer to the SAL handle */
    pSalI2CHND->pUserCB     = pSalI2CMngtAdpt->pUserCB;

    /* Assign the internal user define DMA configuration to the SAL handle */
    pSalI2CHND->pDMAConf    = pSalI2CMngtAdpt->pDMAConf;

    /*To assign user callback pointers*/
    pSalI2CMngtAdpt->pUserCB->pTXCB     = pSalI2CUserCBAdpt;
    pSalI2CMngtAdpt->pUserCB->pTXCCB    = (pSalI2CUserCBAdpt+1);
    pSalI2CMngtAdpt->pUserCB->pRXCB     = (pSalI2CUserCBAdpt+2);
    pSalI2CMngtAdpt->pUserCB->pRXCCB    = (pSalI2CUserCBAdpt+3);
    pSalI2CMngtAdpt->pUserCB->pRDREQCB  = (pSalI2CUserCBAdpt+4);
    pSalI2CMngtAdpt->pUserCB->pERRCB    = (pSalI2CUserCBAdpt+5);
    pSalI2CMngtAdpt->pUserCB->pDMATXCB  = (pSalI2CUserCBAdpt+6);
    pSalI2CMngtAdpt->pUserCB->pDMATXCCB = (pSalI2CUserCBAdpt+7);
    pSalI2CMngtAdpt->pUserCB->pDMARXCB  = (pSalI2CUserCBAdpt+8);
    pSalI2CMngtAdpt->pUserCB->pDMARXCCB = (pSalI2CUserCBAdpt+9);
    pSalI2CMngtAdpt->pUserCB->pGENCALLCB= (pSalI2CUserCBAdpt+10);
    
    /* Set I2C Device Number */
    pSalI2CHND->DevNum = i2c_idx;

    /* Load I2C default value */
    RtkI2CLoadDefault(pSalI2CHND);

    /* Assign I2C Pin Mux */
    pSalI2CHND->PinMux        = RTL_GET_PERI_SEL(i2c_sel);
    pSalI2CHND->OpType        = I2C_INTR_TYPE;
    pSalI2CHND->I2CMaster     = I2C_MASTER_MODE;
    pSalI2CHND->I2CSpdMod     = I2C_SS_MODE;
    pSalI2CHND->I2CClk        = 100;
    pSalI2CHND->I2CAckAddr    = 0;    
    pSalI2CHND->TimeOut       = 300;
    pSalI2CHND->AddRtyTimeOut = 3000;
    pSalI2CHND->I2CExd     	|= (I2C_EXD_MTR_ADDR_RTY);

    pSalI2CMngtAdpt->InnerTimeOut   = pSalI2CHND->TimeOut;

    /* Deinit I2C first */
    //i2c_reset(obj);

    /* Init I2C now */
    RtkI2CInitForPS(pSalI2CHND); 
}

void i2c_frequency(i2c_t *obj, int hz) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt     = &(obj->SalI2CMngtAdpt);
    pSalI2CHND          = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    uint16_t i2c_default_clk    = (uint16_t) pSalI2CHND->I2CClk;
    uint16_t i2c_user_clk       = (uint16_t) (hz/1000);
    
    if (i2c_default_clk != i2c_user_clk) {
        /* Deinit I2C first */
        i2c_reset(obj);
        if (i2c_user_clk <= 100) {
            pSalI2CHND->I2CSpdMod = I2C_SS_MODE;
        }
        else if ((i2c_user_clk > 100) && (i2c_user_clk <= 400)) {
            pSalI2CHND->I2CSpdMod = I2C_FS_MODE;
        }
        else if (i2c_user_clk > 400) {
            pSalI2CHND->I2CSpdMod = I2C_HS_MODE;
        }
        else {
            pSalI2CHND->I2CSpdMod = I2C_SS_MODE;
        }
        
        /* Load the user defined I2C clock */
        pSalI2CHND->I2CClk = i2c_user_clk;

        /* Init I2C now */
        RtkI2CInitForPS(pSalI2CHND);        
    }
}

inline int i2c_start(i2c_t *obj) {
    return 0;
}

inline int i2c_stop(i2c_t *obj) {
    return 0;
}

// extern u32 HalDelayUs(IN  u32 us);

int i2c_read(i2c_t *obj, int address, char *data, int length, int stop) {

    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    u32 I2CInTOTcnt     = 0;
    u32 InTimeoutCount  = 0;
    u32 InStartCount    = 0;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    if (i2c_target_addr[pSalI2CHND->DevNum] != address) {
        /* Deinit I2C first */
        i2c_reset(obj);

        /* Load the user defined I2C target slave address */
        i2c_target_addr[pSalI2CHND->DevNum] = address;
        pSalI2CHND->I2CAckAddr = address;

        /* Init I2C now */
        RtkI2CInitForPS(pSalI2CHND); 
    }

    /* Check if the it's the last byte or not */
    pSalI2CHND->I2CExd &= (~I2C_EXD_MTR_HOLD_BUS);
    if (!stop) {
        pSalI2CHND->I2CExd |= I2C_EXD_MTR_HOLD_BUS;
    }

    pSalI2CHND->pRXBuf            = &i2crxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pRXBuf->DataLen   = length;
    pSalI2CHND->pRXBuf->TargetAddr= pSalI2CHND->I2CAckAddr;
    pSalI2CHND->pRXBuf->RegAddr   = 0;
    pSalI2CHND->pRXBuf->pDataBuf  = (u8 *)data;

    if (RtkI2CReceive(pSalI2CHND) != HAL_OK) {
        length = length - pSalI2CHND->pRXBuf->DataLen;
        return ((int)length);
    }
    else {
        //DBG_8195A(">\n");
        /* Calculate user time out parameters */
        I2CInTOTcnt = 300;
        if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
            InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
            InStartCount = HalTimerOp.HalTimerReadCount(1);
        }
        while((pSalI2CHND->DevSts != I2C_STS_IDLE) && 
              (pSalI2CHND->DevSts != I2C_STS_ERROR) &&
              (pSalI2CHND->DevSts != I2C_STS_TIMEOUT)) {
             /* Time-Out check */
            if (InTimeoutCount > 0) {
                if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                    pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                    pSalI2CHND->ErrType = I2C_ERR_RX_ADD_TO;
                    
                    /* DeInit I2C, Init I2C */
                    //RtkI2CDeInit(pSalI2CHND);
                    //HalDelayUs(1000);
                    //RtkI2CInit(pSalI2CHND);
                    
                    return ((int)(length));
                }
            }
            else {
                if (I2CInTOTcnt == 0) {
                    pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                    pSalI2CHND->ErrType = I2C_ERR_RX_ADD_TO;
                    /* DeInit I2C, Init I2C */
                    //RtkI2CDeInit(pSalI2CHND);
                    
                    //RtkI2CInit(pSalI2CHND);
                    
                    return ((int)(length));
                }
            }  
        }
        //DBG_8195A("<\n");
        if (pSalI2CHND->DevSts != I2C_STS_TIMEOUT) 
            return ((int)(length - pSalI2CHND->pRXBuf->DataLen));
        else
            return ((int)(length));
    }
}

int i2c_write(i2c_t *obj, int address, const char *data, int length, int stop) {

    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    u32 I2CInTOTcnt     = 0;
    u32 InTimeoutCount  = 0;
    u32 InStartCount    = 0;
    
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    if (i2c_target_addr[pSalI2CHND->DevNum] != address) {
        /* Deinit I2C first */
        i2c_reset(obj);

        /* Load the user defined I2C target slave address */
        i2c_target_addr[pSalI2CHND->DevNum] = address;
        pSalI2CHND->I2CAckAddr = address;

        /* Init I2C now */
        RtkI2CInitForPS(pSalI2CHND); 
    }

    /* Check if the it's the last byte or not */
    pSalI2CHND->I2CExd &= (~I2C_EXD_MTR_HOLD_BUS);
    if (!stop) {
        pSalI2CHND->I2CExd |= I2C_EXD_MTR_HOLD_BUS;
    }
    
    pSalI2CHND->pTXBuf            = &i2ctxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pTXBuf->DataLen   = length;
    pSalI2CHND->pTXBuf->TargetAddr= pSalI2CHND->I2CAckAddr;
    pSalI2CHND->pTXBuf->RegAddr   = 0;
    pSalI2CHND->pTXBuf->pDataBuf  = (u8 *)data;

    if (RtkI2CSend(pSalI2CHND) != HAL_OK) {
        length = length - pSalI2CHND->pTXBuf->DataLen;
        return ((int)length);
    }
    else {
        //DBG_8195A("(\n");
        /* Calculate user time out parameters */
        I2CInTOTcnt = 300;
        if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
            InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
            InStartCount = HalTimerOp.HalTimerReadCount(1);
        }
        while((pSalI2CHND->DevSts != I2C_STS_IDLE) && 
              (pSalI2CHND->DevSts != I2C_STS_ERROR) &&
              (pSalI2CHND->DevSts != I2C_STS_TIMEOUT)) {
            /* Time-Out check */
            if (InTimeoutCount > 0) {
                if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                    pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                    pSalI2CHND->ErrType = I2C_ERR_TX_ADD_TO;
                    /* DeInit I2C, Init I2C */
                    //RtkI2CDeInit(pSalI2CHND);
                    
                    //RtkI2CInit(pSalI2CHND);
                    return ((int)(length));
                }
            }
            else {
                if (I2CInTOTcnt == 0) {
                    pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                    pSalI2CHND->ErrType = I2C_ERR_TX_ADD_TO;
                    /* DeInit I2C, Init I2C */
                    //RtkI2CDeInit(pSalI2CHND);
                    
                    //RtkI2CInit(pSalI2CHND);
                    return ((int)(length));
                }
            }         
        }

        if (pSalI2CHND->DevSts != I2C_STS_TIMEOUT)
            return ((int)(length - pSalI2CHND->pTXBuf->DataLen));
        else
            return ((int)(length));
    }
}

int i2c_byte_read(i2c_t *obj, int last) {
    uint8_t i2cdatlocal;
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    /* Check if the it's the last byte or not */
    pSalI2CHND->I2CExd &= (~I2C_EXD_MTR_HOLD_BUS);
    if (!last) {
        pSalI2CHND->I2CExd |= I2C_EXD_MTR_HOLD_BUS;
    }

    pSalI2CHND->pRXBuf            = &i2crxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pRXBuf->DataLen   = 1;
    pSalI2CHND->pRXBuf->TargetAddr= pSalI2CHND->I2CAckAddr;
    pSalI2CHND->pRXBuf->RegAddr   = 0;
    pSalI2CHND->pRXBuf->pDataBuf  = &i2cdatlocal;
    RtkI2CReceive(pSalI2CHND);

    return (int)i2cdatlocal;
}

int i2c_byte_write(i2c_t *obj, int data) {

    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    pSalI2CHND->I2CExd &= (~I2C_EXD_MTR_HOLD_BUS);
    pSalI2CHND->I2CExd |= I2C_EXD_MTR_HOLD_BUS;

    pSalI2CHND->pTXBuf            = &i2ctxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pTXBuf->DataLen   = 1;
    pSalI2CHND->pTXBuf->TargetAddr= pSalI2CHND->I2CAckAddr;
    pSalI2CHND->pTXBuf->RegAddr   = 0;
    pSalI2CHND->pTXBuf->pDataBuf  = (unsigned char*)&data;

    if (RtkI2CSend(pSalI2CHND) != HAL_OK) {
        return 0;
    }

    return 1;
}

void i2c_reset(i2c_t *obj) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    /* Deinit I2C directly */
    RtkI2CDeInitForPS(pSalI2CHND);
}

void i2c_restart_enable(i2c_t *obj) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    uint32_t                i2clocaltmp;
    uint8_t                 i2cen;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);

    i2cen = pSalI2CHND->pInitDat->I2CEn;

    if (i2cen == I2C_ENABLE) {
        pSalI2CHND->pInitDat->I2CEn     = I2C_DISABLE;
        pSalI2CMngtAdpt->pHalOp->HalI2CEnable(pSalI2CHND->pInitDat);
    }
    
    i2clocaltmp = HalI2CRead32(pSalI2CHND->DevNum, REG_DW_I2C_IC_CON);
    i2clocaltmp |= BIT_IC_CON_IC_RESTART_EN;
    HalI2CWrite32(pSalI2CHND->DevNum, REG_DW_I2C_IC_CON, i2clocaltmp);

    if (i2cen == I2C_ENABLE) {
        pSalI2CHND->pInitDat->I2CEn     = I2C_ENABLE;
        pSalI2CMngtAdpt->pHalOp->HalI2CEnable(pSalI2CHND->pInitDat);
    }
    
    pSalI2CHND->pInitDat->I2CReSTR  = I2C_ENABLE;
    
}

void i2c_restart_disable(i2c_t *obj) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    uint32_t                i2clocaltmp;
    uint8_t                 i2cen;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);

    i2cen = pSalI2CHND->pInitDat->I2CEn;

    if (i2cen == I2C_ENABLE) {
        pSalI2CHND->pInitDat->I2CEn     = I2C_DISABLE;
        pSalI2CMngtAdpt->pHalOp->HalI2CEnable(pSalI2CHND->pInitDat);
    }

    i2clocaltmp = HalI2CRead32(pSalI2CHND->DevNum, REG_DW_I2C_IC_CON);
    i2clocaltmp &= (~BIT_IC_CON_IC_RESTART_EN);
    HalI2CWrite32(pSalI2CHND->DevNum, REG_DW_I2C_IC_CON, i2clocaltmp);

    if (i2cen == I2C_ENABLE) {
        pSalI2CHND->pInitDat->I2CEn     = I2C_ENABLE;
        pSalI2CMngtAdpt->pHalOp->HalI2CEnable(pSalI2CHND->pInitDat);
    }

    pSalI2CHND->pInitDat->I2CReSTR  = I2C_DISABLE;
    
}

void i2c_set_user_callback(i2c_t *obj, I2CCallback i2ccb, void(*i2c_callback)(void *)) {

    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    if ((i2ccb >= I2C_TX_COMPLETE) && (i2ccb <= I2C_ERR_OCCURRED)) {
        switch (i2ccb) {
            case I2C_TX_COMPLETE:
                pSalI2CHND->pUserCB->pTXCCB->USERCB = i2c_callback;
                break;
            case I2C_RX_COMPLETE:
                pSalI2CHND->pUserCB->pRXCCB->USERCB = i2c_callback;
                break;
            case I2C_RD_REQ_COMMAND:
                pSalI2CHND->pUserCB->pRDREQCB->USERCB = i2c_callback;
                break;
            case I2C_ERR_OCCURRED:
                pSalI2CHND->pUserCB->pERRCB->USERCB = i2c_callback;
                break;
            default:
                break;
        }
    }
}


void i2c_clear_user_callback(i2c_t *obj, I2CCallback i2ccb) {

    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    if ((i2ccb >= I2C_TX_COMPLETE) && (i2ccb <= I2C_ERR_OCCURRED)) {
        switch (i2ccb) {
            case I2C_TX_COMPLETE:
                pSalI2CHND->pUserCB->pTXCCB = NULL;
                break;
            case I2C_RX_COMPLETE:
                pSalI2CHND->pUserCB->pRXCCB = NULL;
                break;
            case I2C_ERR_OCCURRED:
                pSalI2CHND->pUserCB->pERRCB = NULL;
                break;
            default:
                break;
        }
    }
}

int i2c_enable_control(i2c_t *obj, int enable) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);

    pSalI2CHND->pInitDat->I2CEn = enable;

    pSalI2CMngtAdpt->pHalOp->HalI2CEnable(pSalI2CHND->pInitDat);
}

#if DEVICE_I2CSLAVE

void i2c_slave_address(i2c_t *obj, int idx, uint32_t address, uint32_t mask) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    uint16_t i2c_default_addr   = (uint16_t) pSalI2CHND->I2CAckAddr;
    uint16_t i2c_user_addr      = (uint16_t) address;

    if (i2c_default_addr != i2c_user_addr) {
        /* Deinit I2C first */
        i2c_reset(obj);

        /* Load the user defined I2C clock */
        pSalI2CHND->I2CAckAddr    = i2c_user_addr;

        /* Init I2C now */
        RtkI2CInitForPS(pSalI2CHND);
    }
}

void i2c_slave_mode(i2c_t *obj, int enable_slave) {

    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    /* Deinit I2C first */
    i2c_reset(obj);

    /* Load the user defined I2C clock */
    pSalI2CHND->I2CMaster = I2C_MASTER_MODE;
    if (enable_slave) 
        pSalI2CHND->I2CMaster = I2C_SLAVE_MODE;

    /* Init I2C now */
    RtkI2CInitForPS(pSalI2CHND);
}

// See I2CSlave.h
#define NoData         0 // the slave has not been addressed
#define ReadAddressed  1 // the master has requested a read from this slave (slave = transmitter)
#define WriteGeneral   2 // the master is writing to all slave
#define WriteAddressed 3 // the master is writing to this slave (slave = receiver)

int i2c_slave_receive(i2c_t *obj) {

    int i2cslvrevsts = NoData;
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    i2cslvrevsts = RtkSalI2CSts(pSalI2CHND);
    return i2cslvrevsts;
}

int i2c_slave_read(i2c_t *obj, char *data, int length) {

    u32 I2CInTOTcnt     = 0;
    u32 InTimeoutCount  = 0;
    u32 InStartCount    = 0;
    
    //uint8_t i2cdatlocal;
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    pSalI2CHND->pRXBuf            = &i2crxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pRXBuf->DataLen   = length;
    pSalI2CHND->pRXBuf->pDataBuf  = (u8 *)data;

    if (RtkI2CReceive(pSalI2CHND) != HAL_OK) {
        return 0;   //error
    }
    else {
        /* Calculate user time out parameters */
        I2CInTOTcnt = 300;
        if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
            InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
            InStartCount = HalTimerOp.HalTimerReadCount(1);
        }
        while((pSalI2CHND->DevSts != I2C_STS_IDLE) && 
              (pSalI2CHND->DevSts != I2C_STS_ERROR) &&
              (pSalI2CHND->DevSts != I2C_STS_TIMEOUT)) {
            /* Time-Out check */
            if (InTimeoutCount > 0) {
                if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                    pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                    pSalI2CHND->ErrType = I2C_ERR_RX_ADD_TO;
                    return ((int)(length));
                }
            }
            else {
                if (I2CInTOTcnt == 0) {
                    pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                    pSalI2CHND->ErrType = I2C_ERR_RX_ADD_TO;
                    return ((int)(length));
                }
            }         
        }

        if (pSalI2CHND->DevSts != I2C_STS_TIMEOUT)
            return ((int)(length - pSalI2CHND->pTXBuf->DataLen));
        else
            return ((int)(length));
    }
}

int i2c_slave_write(i2c_t *obj, const char *data, int length) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    pSalI2CHND->pTXBuf            = &i2ctxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pTXBuf->DataLen   = length;
    //obj->i2c->pTXBuf->TargetAddr= obj->i2c->I2CAckAddr;
    //obj->i2c->pTXBuf->RegAddr   = 0;
    pSalI2CHND->pTXBuf->pDataBuf  = (u8 *)data;

    if (RtkI2CSend(pSalI2CHND) != HAL_OK) {
        return 0;   //error
    }

    return 1;
}

/** \brief Description of i2c_slave_set_for_rd_req
 *  
 *    i2c_slave_set_for_rd_req is used to set/clear i2c slave RD_REQ interrupt mask.
 *    If RD_REQ interrupt is set, slave could invoke read request callback when it gets 
 *    a read command from other i2c master.
 *    
 *   \param i2c_t *obj  : i2c object
 *   \param int set     : set or clear for read request. Once it's set, i2c would invoke read request callback when a
 *                        read command is sent to it.
 *   \return result
 */
int i2c_slave_set_for_rd_req(i2c_t *obj, int set) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    PHAL_I2C_INIT_DAT   pHalI2CInitDat      = NULL;
    PHAL_I2C_OP         pHalI2COP           = NULL; 
    u32 I2CLocalTemp;
    
    pSalI2CMngtAdpt = &(obj->SalI2CMngtAdpt);
    pSalI2CHND      = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    pHalI2CInitDat  = pSalI2CMngtAdpt->pHalInitDat;
    pHalI2COP       = pSalI2CMngtAdpt->pHalOp;
    
    I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);

    if (set) {
        I2CLocalTemp |= BIT_IC_INTR_MASK_M_RD_REQ;
    } else {
        I2CLocalTemp &= (~BIT_IC_INTR_MASK_M_RD_REQ);
    }
    
    pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
    pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);

    return 1;
}

/** \brief Description of i2c_slave_set_for_data_nak
 *  
 *    i2c_slave_set_for_data_nak is used to set/clear i2c slave NAK or ACK data part in transfer.
 *    
 *   \param i2c_t *obj  : i2c object
 *   \param int set     : set or clear for data NAK.
 *   \return result
 */
int i2c_slave_set_for_data_nak(i2c_t *obj, int set_nak) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    PHAL_I2C_INIT_DAT   pHalI2CInitDat      = NULL;
    PHAL_I2C_OP         pHalI2COP           = NULL; 
    u32 I2CLocalTemp;

    pSalI2CMngtAdpt = &(obj->SalI2CMngtAdpt);
    pSalI2CHND      = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    pHalI2CInitDat  = pSalI2CMngtAdpt->pHalInitDat;
    pHalI2COP       = pSalI2CMngtAdpt->pHalOp;
    I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_STATUS);
    
    //if (set_nak) {
        while (BIT_IC_STATUS_SLV_ACTIVITY & I2CLocalTemp) {
            I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_STATUS);
        }
    //}
    
    HAL_I2C_WRITE32(pSalI2CHND->DevNum, REG_DW_I2C_IC_SLV_DATA_NACK_ONLY, set_nak);
}

#endif // CONFIG_I2C_SLAVE_EN

#endif // CONFIG_I2C_EN


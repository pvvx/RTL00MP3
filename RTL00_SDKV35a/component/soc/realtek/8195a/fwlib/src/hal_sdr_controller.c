/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "rtl8195a.h"
#include "platform_opts.h"
#include "hal_sdr_controller.h"
#include "rtl8195a_sdr.h"
#include "flash_api.h"

#ifdef CONFIG_SDR_EN 

#ifndef USE_SRC_ONLY_BOOT
#define USE_SRC_ONLY_BOOT	0
#endif

#if !USE_SRC_ONLY_BOOT
//#define SDRAM_INIT_USE_TCM_HEAP
//#define SDRAM_INIT_USE_FLASH_API
#endif


#if 0
#define HAL_SDR_WRITE32(addr, value32)  HAL_WRITE32(SDR_CTRL_BASE, addr, value32)
#define HAL_SDR_WRITE16(addr, value16)  HAL_WRITE16(SDR_CTRL_BASE, addr, value16)
#define HAL_SDR_WRITE8(addr, value8)    HAL_WRITE8(SDR_CTRL_BASE, addr, value8)
#define HAL_SDR_READ32(addr)            HAL_READ32(SDR_CTRL_BASE, addr)
#define HAL_SDR_READ16(addr)            HAL_READ16(SDR_CTRL_BASE, addr)
#define HAL_SDR_READ8(addr)             HAL_READ8(SDR_CTRL_BASE, addr)

#define HAL_SDRAM_WRITE32(addr, value32)  HAL_WRITE32(SDR_SDRAM_BASE, addr, value32)
#define HAL_SDRAM_WRITE16(addr, value16)  HAL_WRITE16(SDR_SDRAM_BASE, addr, value16)
#define HAL_SDRAM_WRITE8(addr, value8)    HAL_WRITE8(SDR_SDRAM_BASE, addr, value8)
#define HAL_SDRAM_READ32(addr)            HAL_READ32(SDR_SDRAM_BASE, addr)
#define HAL_SDRAM_READ16(addr)            HAL_READ16(SDR_SDRAM_BASE, addr)
#define HAL_SDRAM_READ8(addr)             HAL_READ8(SDR_SDRAM_BASE, addr)
#endif

#define DEBUG_SDRAM 2
//#define CONFIG_SDR_VERIFY

extern SPIC_INIT_PARA SpicInitParaAllClk[3][CPU_CLK_TYPE_NO];
extern DRAM_DEVICE_INFO SdrDramInfo;

/*
HAL_CUT_B_RAM_DATA_SECTION
DRAM_INFO SdrDramDev = {
  DRAM_INFO_TYPE,
  DRAM_INFO_COL_ADDR_WTH, 
  DRAM_INFO_BANK_SZ, 
  DRAM_INFO_DQ_WTH
};


HAL_CUT_B_RAM_DATA_SECTION
DRAM_MODE_REG_INFO SdrDramModeReg = {
  BST_LEN_4, 
  SENQUENTIAL,
  0x3, // Mode0Cas: 3 
  0x0, // Mode0Wr 
  0,   // Mode1DllEnN  
  0,   // Mode1AllLat  
  0    // Mode2Cwl   
};

HAL_CUT_B_RAM_DATA_SECTION
DRAM_TIMING_INFO SdrDramTiming = {
  DRAM_TIMING_TRFC,         // TrfcPs;
  DRAM_TIMING_TREFI,        // TrefiPs;
  DRAM_TIMING_TWRMAXTCK,    // WrMaxTck;
  DRAM_TIMING_TRCD,         // TrcdPs; 
  DRAM_TIMING_TRP,          // TrpPs;
  DRAM_TIMING_TRAS,         // TrasPs;
  DRAM_TIMING_TRRD,         // TrrdTck;
  DRAM_TIMING_TWR,          // TwrPs;
  DRAM_TIMING_TWTR,         // TwtrTck; 
  //13090,      // TrtpPs;
  DRAM_TIMING_TMRD,         // TmrdTck;
  DRAM_TIMING_TRTP,         // TrtpTck;
  DRAM_TIMING_TCCD,         // TccdTck;
  DRAM_TIMING_TRC           // TrcPs;
};

HAL_CUT_B_RAM_DATA_SECTION
DRAM_DEVICE_INFO SdrDramInfo = {
  &SdrDramDev,
  &SdrDramModeReg,
  &SdrDramTiming,
  DRAM_TIMING_TCK,
  DFI_RATIO_1
};
*/

#define FPGA
#define FPGA_TEMP
#define SDR_CLK_DLY_CTRL 0x40000300
#define MIN_RD_PIPE 0x0
#define MAX_RD_PIPE 0x7

#define DRAM_CALIBRATION_IN_NVM 1
#ifndef CONFIG_IMAGE_SEPARATE       // Store SPIC Calibration only for seprated image
#undef DRAM_CALIBRATION_IN_NVM 
#define DRAM_CALIBRATION_IN_NVM 0
#endif

#ifdef FPGA
#ifdef FPGA_TEMP
#define MAX_TAP_DLY 0xC
#else
#define MAX_TAP_DLY 0x7F
#define SPEC_MAX_TAP 0xFF
#endif
#else
#define MAX_TAP_DLY 99		// 0~99
#define SPEC_MAX_TAP 99
#define WINDOW_COMBIN		// combine window [0~a] and [b~99] (for asic mode)
#endif
 
#define TAP_DLY 0x1
#define REC_NUM 512
 
 
u32 SdrControllerInit(VOID);
VOID DramInit(DRAM_DEVICE_INFO *);
s32 MemTest(u32 loop_cnt);
u32 SdrCalibration(VOID);
//u32 Sdr_Rand2(VOID);
//#define Sdr_Rand2 Rand

#ifndef SDRAM_INIT_USE_TCM_HEAP
#if !USE_SRC_ONLY_BOOT
//3 Note: stack overfloat if the arrary is declared in the task
//HAL_CUT_B_RAM_DATA_SECTION
extern u32  AvaWds[2][REC_NUM];
#endif
#else
typedef struct {
	u32 m[2][REC_NUM];
} sAvaWds, * pAvaWds;
#endif

#endif // CONFIG_SDR_EN
/*
HAL_CUT_B_RAM_DATA_SECTION
unsigned int rand_x = 123456789;
*/
#ifdef CONFIG_SDR_EN

#ifndef __GNUC__
//#pragma arm section code = ".hal.sdrc.text"
#pragma arm section rodata = ".rodata.hal.sdrc"
//, rwdata = ".hal.sdrc.data"
//, zidata = ".hal.sdrc.bss"
//#pragma arm section bss = ".hal.sdrc.bss"
#endif

#ifdef CONFIG_SDR_VERIFY
enum{
	LLT,
    TXRPT,
    RXBUFF,
	TXBUFF,	
};
#define REPORT_OFFSET   0x8000
#define RAMASK_OFFSET   0x8800
#define LLT_H_ADDR      0x650
#define TXREPORT_H_ADDR 0x660
#define RXBUFF_H_ADDR   0x670
#define TXBUFF_H_ADDR   0x680

#define REG_PKTBUF_DBG_CTRL_8723B		0x0140

int
rt_rpt_h_addr(u8 rpt)
{
    u32 r_val, offset;
    
    if (rpt == LLT){
        offset = LLT_H_ADDR;
    }
    else if (rpt == TXRPT){
        offset = TXREPORT_H_ADDR;
    }
    else if (rpt == RXBUFF){
        offset = RXBUFF_H_ADDR;
    }
    else if (rpt == TXBUFF){
        offset = TXBUFF_H_ADDR;
    }
    else {
    }

    r_val = ((HAL_READ32(WIFI_REG_BASE, REG_PKTBUF_DBG_CTRL_8723B)&0xFFFFF000)|offset);
    HAL_WRITE32(WIFI_REG_BASE, REG_PKTBUF_DBG_CTRL_8723B, r_val);
}



int
rt_txrpt_read32(u8 macid, u8 offset)
{
    u32 r_val;
    
    rt_rpt_h_addr(TXRPT);
    r_val = HAL_READ32(WIFI_REG_BASE, (REPORT_OFFSET + macid*4 + offset));
    
    return r_val;
}

int
rt_txrpt_read16(u8 macid, u8 offset)
{
    u16 r_val;
    
    rt_rpt_h_addr(TXRPT);
    r_val = HAL_READ16(WIFI_REG_BASE, (REPORT_OFFSET + macid*8 + offset));
    
    return r_val;
}

int
rt_txrpt_read8(u8 macid, u8 offset)
{
    u8 r_val;
    
    rt_rpt_h_addr(TXRPT);
    r_val = HAL_READ8(WIFI_REG_BASE, (REPORT_OFFSET + macid*16 + offset));
    
    return r_val;
}

int
rt_txrpt_read_1b(u8 macid, u8 offset, u8 bit_offset)
{
    u8 r_val = ((rt_txrpt_read8(macid, offset) & BIT(bit_offset))?1:0);

    return r_val;
}


int
rt_txrpt_write32(u8 macid, u8 offset, u32 val)
{
    rt_rpt_h_addr(TXRPT);
    HAL_WRITE32(WIFI_REG_BASE, (REPORT_OFFSET + macid*4 + offset), val);
}

int
rt_txrpt_write16(u8 macid, u8 offset, u16 val)
{
    rt_rpt_h_addr(TXRPT);
    HAL_WRITE16(WIFI_REG_BASE, (REPORT_OFFSET + macid*8 + offset), val);
}

int
rt_txrpt_write8(u8 macid, u8 offset, u8 val)
{
    rt_rpt_h_addr(TXRPT);
    DBG_8195A("Write addr %x %x\n", (REPORT_OFFSET + macid*16 + offset), val);
    HAL_WRITE8(WIFI_REG_BASE, (REPORT_OFFSET + macid*16 + offset), val);
}

int
rt_txrpt_write_1b(u8 macid, u8 offset, u8 bit_offset, u8 val)
{
    u8 r_val = rt_txrpt_read8(macid, offset);

    if (val){
        r_val |= BIT(bit_offset);
    }
    else {
        r_val &= (~BIT(bit_offset));
    }
    
    HAL_WRITE8(WIFI_REG_BASE, (REPORT_OFFSET + macid*16 + offset), r_val);
}


u8
ReadTxrptsdr8(
    IN u8 Macid,
    IN u8 Offset)
{
    u8 r_val;
    
    r_val = rt_txrpt_read8(Macid, Offset);
    return r_val;
}

VOID
WriteTxrptsdr8(
    IN u8 Macid,
    IN u8 Offset,
    IN u8 Val)
{
    rt_txrpt_write8(Macid, Offset, Val);
}

VOID 
SdrTestApp(
    IN  VOID *Data
)
{
    u32 *Cmd =(u32*)Data;
    u32 Loop, LoopIndex, Value32, Addr, Loop1, LoopIndex1;

    switch (Cmd[0]) {
        case 1:
                DBG_8195A("Initial SDR\n");

                //1 "SdrControllerInit" is located in Image1, so we shouldn't call it in Image2 
                if (!SdrControllerInit()) {
                    DBG_8195A("SDR Calibartion Fail!\n");
                }
            break;
        case 2:
                Loop = Cmd[1];
                Loop1 = Cmd[2];
                DBG_8195A("Verify SDR: Loop = 0x%08x Loop1 = 0x%08x\n",Loop, Loop1);
                
                for (LoopIndex1=0; LoopIndex1 < Loop1; LoopIndex1++) {
                
                for (LoopIndex=0; LoopIndex < Loop; LoopIndex++) {
                    Value32 = Rand2();
                    Addr = Rand2();
                    Addr &= 0x1FFFFF;
                    Addr &= (~0x3);

                        if (!(LoopIndex & 0xFFFFF)) {
                            DBG_8195A("Alive: LOOP = 0x%08x, LOOP = 0x%08x\n",LoopIndex1, LoopIndex);
                    }

                    //        DBG_8195A("Value: 0x%x; Addr: 0x%x\n", Value32, Addr+SDR_SDRAM_BASE);        
                    HAL_SDRAM_WRITE32(Addr, Value32);

                    if (Value32 != HAL_SDRAM_READ32(Addr)) {
                        DBG_8195A("Loop:%d; Addr: 0x%08x => CheckData error: W: 0x%08x /R:0x%x\n"
                            ,LoopIndex
                            ,Addr
                            ,Value32
                            ,HAL_SDRAM_READ32(Addr));

                            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSTBY_INFO2, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSTBY_INFO2)+1);
                        break;
                    }        
                }            
                }
                DBG_8195A("Verify SDR Success\n");
            break;

        case 3:
            DBG_8195A("WL read RPT MACID %x\n", Cmd[1]);
            {
                u8 i =0;
                for(i=0;i<16;i++) {
                    DBG_8195A("WL RPT offset %d = %x\n", i, ReadTxrptsdr8(Cmd[1],i));
                }
            }
            break;
        case 4:
            DBG_8195A("WL write RPT MACID %x\n", Cmd[1]);
            {
                u8 i =0;
                for(i=0;i<16;i++) {
                    WriteTxrptsdr8(Cmd[1],i,Cmd[2]);
                    //DBG_8195A("WL RPT offset %d = %x\n", i, ReadTxrptsdr8(Cmd[1],i));
                }
            }
            break;
        default:
            break;
    }
    
}
    
#endif

HAL_SDRC_TEXT_SECTION  
VOID
SdrCtrlInit(
VOID
){
//	ConfigDebugErr |=  _DBG_MISC_;
//    DBG_8195A("SDR Ctrl Init\n");
    HAL_SYS_CTRL_WRITE32(REG_SYS_REGU_CTRL0,
                        ((HAL_SYS_CTRL_READ32(REG_SYS_REGU_CTRL0) & 0xfffff) | BIT_SYS_REGU_LDO25M_ADJ(0x0e)));

    LDO25M_CTRL(ON);
}

HAL_SDRC_TEXT_SECTION  
u32 
SdrControllerInit(
VOID
)
{
//	ConfigDebugErr |=  _DBG_MISC_;
    DBG_8195A("SDR Controller Init\n");

    HAL_SYS_CTRL_WRITE32(REG_SYS_REGU_CTRL0,
                        ((HAL_SYS_CTRL_READ32(REG_SYS_REGU_CTRL0) & 0xfffff) | BIT_SYS_REGU_LDO25M_ADJ(0x03)));

    SRAM_MUX_CFG(0x2);

    SDR_CLK_SEL(SDR_CLOCK_SEL_VALUE);

    HAL_PERI_ON_WRITE32(REG_GPIO_PULL_CTRL4,0);

    ACTCK_SDR_CCTRL(ON);

    SLPCK_SDR_CCTRL(ON);

    PinCtrl(SDR, 0, ON);

    HAL_PERI_ON_WRITE32(REG_GPIO_PULL_CTRL4,0);

    MEM_CTRL_FCTRL(ON);
     
    HalDelayUs(3000);
     
    // sdr initialization
    DramInit(&SdrDramInfo);

    // sdr calibration
    if(!SdrCalibration()) {
        return 0;
    }
    else {
        return 1;
    }
}


HAL_SDRC_TEXT_SECTION 
VOID
DramInit (
    IN  DRAM_DEVICE_INFO *DramInfo
)
{
	DBG_8195A("%s(%p)\n", __func__, DramInfo);
    u32 CsBstLen = 0;            // 0:bst_4, 1:bst_8
    u32 CasWr = 0;//, CasWrT;       // cas write latency
    u32 CasRd = 0, CasRdT = 0, CrlSrt = 0;  // cas read latency
    u32 AddLat;
    u32 DramEmr2 = 0, DramMr0 = 0;
    u32 CrTwr, DramMaxWr, DramWr;
    u32 CrTrtw = 0, CrTrtwT = 0;
    u32 DrmaPeriod; 
    DRAM_TYPE         DdrType = DRAM_SDR;
    DRAM_DQ_WIDTH     DqWidth;
    DRAM_COLADDR_WTH    Page;
    u32 DfiRate;

    DfiRate = 1 << (u32) (DramInfo->DfiRate);
    DrmaPeriod = (DramInfo->DdrPeriodPs)*(DfiRate); // according DFI_RATE to setting

    // In PHY, write latency == 3
    DramMaxWr= (DramInfo->Timing->WrMaxTck)/(DfiRate) +1;
    DramWr = ((DramInfo->Timing->TwrPs) / DrmaPeriod) +1;
    CrTwr = ((DramInfo->Timing->TwrPs) / DrmaPeriod) + 3;

    if (CrTwr < DramMaxWr) {
//        CrTwr = CrTwr;
    }
    else {
        CrTwr = DramMaxWr;
    }

    if ((DramInfo->Dev->DeviceType) == DRAM_DDR_2) {
        DdrType = DRAM_DDR_2;
        if (DramInfo->ModeReg->BstLen == BST_LEN_4) {
            CsBstLen = 0; //bst_4
            CrTrtwT = 2+2;    //4/2+2
            DramMr0 = 0x2;
        }
        else { // BST_LEN_8
            CsBstLen = 1; // bst_8
            CrTrtwT = 4+2;    // 8/2+2
            DramMr0 = 0x3;
        }
        CasRd = DramInfo->ModeReg->Mode0Cas;
        AddLat = DramInfo->ModeReg ->Mode1AllLat;
        CasWr = CasRd + AddLat -1;
        DramEmr2 = 0;
     
        DramMr0  =(((DramWr%6)-1)                  << (PCTL_MR_OP_BFO+1)) | // write_recovery
                   (0                                << PCTL_MR_OP_BFO    ) | // dll
                   (DramInfo->ModeReg->Mode0Cas << PCTL_MR_CAS_BFO   ) |
                   (DramInfo->ModeReg->BstType  << PCTL_MR_BT_BFO    ) |
                   DramMr0;
    }
    else if ((DramInfo->Dev->DeviceType) == DRAM_DDR_3) {
        DdrType = DRAM_DDR_3;
        if (DramInfo->ModeReg->BstLen == BST_LEN_4) {
            CsBstLen = 0; //bst_4
            DramMr0 = 0x2;
        }
        else { // BST_LEN_8
            CsBstLen = 1; // bst_8
            DramMr0 = 0x0;
        }

        CrlSrt = (DramInfo->ModeReg->Mode0Cas >> 1);
        if (((DramInfo->ModeReg->Mode0Cas) & 0x1) ) {
            CasRdT = CrlSrt+ 12;
        }
        else {
            CasRdT = CrlSrt+ 4;
        }

        if (DramInfo->ModeReg->Mode1AllLat == 1) { // CL-1
            AddLat = CasRd -1;
        }
        else if (DramInfo->ModeReg->Mode1AllLat == 2){ // CL-2
            AddLat = CasRd -2;
        }
        else {
            AddLat = 0;
        }

        CasRd = CasRdT + AddLat;

        CasWr =  DramInfo->ModeReg->Mode2Cwl + 5 + AddLat;

        DramEmr2 = DramInfo->ModeReg->Mode2Cwl << 3;

        if (DramWr == 16) {
            DramWr = 0;
        }
        else if (DramWr <= 9) {  // 5< wr <= 9
            DramWr = DramWr - 4;
        }
        else {
            DramWr = (DramWr + 1) / 2;
        }

        DramMr0  =(DramWr                            << (PCTL_MR_OP_BFO+1) ) | // write_recovery
                (0                                   << PCTL_MR_OP_BFO     ) | // dll
                ((DramInfo->ModeReg->Mode0Cas >>1 )  << PCTL_MR_CAS_BFO    ) |
                (DramInfo->ModeReg->BstType          << PCTL_MR_BT_BFO     ) |
                ((DramInfo->ModeReg->Mode0Cas & 0x1) << 2                  ) |
                DramMr0;

        CrTrtwT = (CasRdT + 6) - CasWr;

    }  // ddr2/ddr3
    else if ((DramInfo->Dev->DeviceType) == DRAM_SDR) {
            DdrType = DRAM_SDR;
        if (DramInfo->ModeReg->BstLen == BST_LEN_4) {
            DramMr0 = 2; // bst_4
            CsBstLen = 0; //bst_4
            CasRd = 0x2;
        }
        else { // BST_LEN_8
            DramMr0 = 3; // bst_8
            CsBstLen = 1; // bst_8
            CasRd  = 0x3;
        }

        CasWr = 0;

        DramMr0  =(CasRd                    << PCTL_MR_CAS_BFO) |
                (DramInfo->ModeReg->BstType << PCTL_MR_BT_BFO ) |
                DramMr0;

        CrTrtwT = 0; // tic: CasRd + rd_rtw + rd_pipe
    } // SDR


     // countting tRTW
    if ((CrTrtwT & 0x1)) {
        CrTrtw = (CrTrtwT+1) /(DfiRate);
    }
    else {
        CrTrtw = CrTrtwT /(DfiRate);
    }

    DqWidth = (DramInfo->Dev->DqWidth); 
    Page = DramInfo->Dev->ColAddrWth +1; // DQ16 -> memory:byte_unit *2
    if (DqWidth == DRAM_DQ_32) { // paralle dq_16 => Page + 1
        Page = Page +1;
    }
#if 1
    
    // WRAP_MISC setting
    HAL_SDR_WRITE32(REG_SDR_MISC,(
                           (Page                 << WRAP_MISC_PAGE_SIZE_BFO) |  
                           (DramInfo->Dev->Bank  << WRAP_MISC_BANK_SIZE_BFO) |
                           (CsBstLen             << WRAP_MISC_BST_SIZE_BFO ) |
                           (DqWidth              << WRAP_MISC_DDR_PARAL_BFO)
                          ));
    // PCTL setting
    HAL_SDR_WRITE32(REG_SDR_DCR,(
                          (0x2      << PCTL_DCR_DFI_RATE_BFO) |
                          (DqWidth << PCTL_DCR_DQ32_BFO    ) |
                          (DdrType << PCTL_DCR_DDR3_BFO    )
                         ));

    HAL_SDR_WRITE32(REG_SDR_IOCR,(
                           ((CasRd -4)/(DfiRate)  << PCTL_IOCR_TPHY_RD_EN_BFO ) |
                           (0                        << PCTL_IOCR_TPHY_WL_BFO   ) |
                           (((CasWr -3)/(DfiRate)) << PCTL_IOCR_TPHY_WD_BFO   ) |
                           (0                        << PCTL_IOCR_RD_PIPE_BFO   )
                          ));

    if ((DramInfo->Dev->DeviceType) != SDR) { // DDR2/3 
        HAL_SDR_WRITE32(REG_SDR_EMR2,DramEmr2);
        HAL_SDR_WRITE32(REG_SDR_EMR1,(
                                (1               << 2 ) | //RTT
                                (1               << 1 ) | //D.I.C
                                (DramInfo->ModeReg->Mode1DllEnN )
                                ));
     } // DDR2/3

    HAL_SDR_WRITE32(REG_SDR_MR,DramMr0);

    HAL_SDR_WRITE32(REG_SDR_DRR, (
                      (0                                             << PCTL_DRR_REF_DIS_BFO) |
                      (9                                             << PCTL_DRR_REF_NUM_BFO) |
                      ((((DramInfo->Timing->TrefiPs)/DrmaPeriod)+1) << PCTL_DRR_TREF_BFO   ) |
                      ((((DramInfo->Timing->TrfcPs)/DrmaPeriod)+1)  << PCTL_DRR_TRFC_BFO   )
                     ));

    HAL_SDR_WRITE32(REG_SDR_TPR0,(
                      ((((DramInfo->Timing->TrtpTck)/DfiRate)+1)   << PCTL_TPR0_TRTP_BFO) |
                      (CrTwr                                        << PCTL_TPR0_TWR_BFO ) |
                      ((((DramInfo->Timing->TrasPs)/DrmaPeriod)+1) << PCTL_TPR0_TRAS_BFO) |
                      ((((DramInfo->Timing->TrpPs)/DrmaPeriod)+1)  << PCTL_TPR0_TRP_BFO )
                     ));

    HAL_SDR_WRITE32(REG_SDR_TPR1, (
                    (CrTrtw                                       << PCTL_TPR1_TRTW_BFO) |
                    ((((DramInfo->Timing->TwtrTck)/DfiRate)+3)   << PCTL_TPR1_TWTR_BFO) |
                    ((((DramInfo->Timing->TccdTck)/DfiRate)+1)   << PCTL_TPR1_TCCD_BFO) |
                    ((((DramInfo->Timing->TrcdPs)/DrmaPeriod)+1) << PCTL_TPR1_TRCD_BFO) |
                    ((((DramInfo->Timing->TrcPs)/DrmaPeriod)+1)  << PCTL_TPR1_TRC_BFO ) |
                    (((DramInfo->Timing->TrrdTck/DfiRate)+1)     << PCTL_TPR1_TRRD_BFO)
                    ));

    HAL_SDR_WRITE32(REG_SDR_TPR2, (
                        (DramInfo->Timing->TmrdTck << PCTL_TPR2_TMRD_BFO ) |
                        (0                         << PCTL_TPR2_INIT_NS_EN_BFO  ) |
                        (2                         << PCTL_TPR2_INIT_REF_NUM_BFO)
                        ));

    // set all_mode _idle
    HAL_SDR_WRITE32(REG_SDR_CSR,0x700);

    // start to init
    HAL_SDR_WRITE32(REG_SDR_CCR,0x01);
    while ((HAL_SDR_READ32(REG_SDR_CCR)& 0x1) == 0x0);

    // enter mem_mode
    HAL_SDR_WRITE32(REG_SDR_CSR,0x600);
#else
    volatile struct ms_rxi310_portmap *ms_ctrl_0_map;
    ms_ctrl_0_map = (struct ms_rxi310_portmap*) SDR_CTRL_BASE;
//    ms_ctrl_0_map = ms_ctrl_0_map;
    // WRAP_MISC setting
    ms_ctrl_0_map->misc = //0x12;
                          (
                           (Page                   << WRAP_MISC_PAGE_SIZE_BFO) |  
                           (DramInfo->Dev->Bank << WRAP_MISC_BANK_SIZE_BFO) |
                           (CsBstLen             << WRAP_MISC_BST_SIZE_BFO ) |
                           (DqWidth               << WRAP_MISC_DDR_PARAL_BFO)
                          );
    // PCTL setting
    ms_ctrl_0_map->dcr = //0x208;
                         (
                          (0x2      << PCTL_DCR_DFI_RATE_BFO) |
                          (DqWidth << PCTL_DCR_DQ32_BFO    ) |
                          (DdrType << PCTL_DCR_DDR3_BFO    )
                         );

    ms_ctrl_0_map->iocr = (
                           ((CasRd -4)/(DfiRate)  << PCTL_IOCR_TPHY_RD_EN_BFO ) |
                           (0                        << PCTL_IOCR_TPHY_WL_BFO   ) |
                           (((CasWr -3)/(DfiRate)) << PCTL_IOCR_TPHY_WD_BFO   ) |
                           (0                        << PCTL_IOCR_RD_PIPE_BFO   )
                          );

    if ((DramInfo->Dev->DeviceType) != SDR) { // DDR2/3 
        ms_ctrl_0_map->emr2 = DramEmr2;

        ms_ctrl_0_map->emr1 = (
                                (1               << 2 ) | //RTT
                                (1               << 1 ) | //D.I.C
                                (DramInfo->ModeReg->Mode1DllEnN )
                                );
    } // DDR2/3

    ms_ctrl_0_map->mr = DramMr0;

    ms_ctrl_0_map->drr = (
                      (0                                                 << PCTL_DRR_REF_DIS_BFO) |
                      (9                                                 << PCTL_DRR_REF_NUM_BFO) |
                      ((((DramInfo->Timing->TrefiPs)/DrmaPeriod)+1)<< PCTL_DRR_TREF_BFO   ) |
                      ((((DramInfo->Timing->TrfcPs)/DrmaPeriod)+1) << PCTL_DRR_TRFC_BFO   )
                     );

    ms_ctrl_0_map->tpr0= (
                      ((((DramInfo->Timing->TrtpTck)/DfiRate)+1)   << PCTL_TPR0_TRTP_BFO) |
                      (CrTwr                                            << PCTL_TPR0_TWR_BFO ) |
                      ((((DramInfo->Timing->TrasPs)/DrmaPeriod)+1) << PCTL_TPR0_TRAS_BFO) |
                      ((((DramInfo->Timing->TrpPs)/DrmaPeriod)+1)  << PCTL_TPR0_TRP_BFO )
                     );

    ms_ctrl_0_map->tpr1= (
                    (CrTrtw                                           << PCTL_TPR1_TRTW_BFO) |
                    ((((DramInfo->Timing->TwtrTck)/DfiRate)+3)   << PCTL_TPR1_TWTR_BFO) |
                    ((((DramInfo->Timing->TccdTck)/DfiRate)+1)   << PCTL_TPR1_TCCD_BFO) |
                    ((((DramInfo->Timing->TrcdPs)/DrmaPeriod)+1) << PCTL_TPR1_TRCD_BFO) |
                    ((((DramInfo->Timing->TrcPs)/DrmaPeriod)+1)  << PCTL_TPR1_TRC_BFO ) |
                    (((DramInfo->Timing->TrrdTck/DfiRate)+1)     << PCTL_TPR1_TRRD_BFO)
                    );

    ms_ctrl_0_map->tpr2= (
                        (DramInfo->Timing->TmrdTck << PCTL_TPR2_TMRD_BFO ) |
                        (0                         << PCTL_TPR2_INIT_NS_EN_BFO  ) |
                        (2                         << PCTL_TPR2_INIT_REF_NUM_BFO)
                        );
    // set all_mode _idle
    ms_ctrl_0_map->csr = 0x700;

    // start to init
    ms_ctrl_0_map->ccr = 0x1;
    while (((ms_ctrl_0_map->ccr)& 0x1) == 0x0);

    // enter mem_mode
    ms_ctrl_0_map->csr= 0x600;
#endif
} // DramInit

 //3 
extern void *
_memset( void *s, int c, SIZE_T n );

HAL_SDRC_TEXT_SECTION
u32 
SdrCalibration(
    VOID
)
{
#ifdef FPGA
#ifdef FPGA_TEMP
//    u32 Value32;
#endif
#else
//    u32 Value32;
#endif
	DBG_8195A("%s()\n", __func__);
    u32 RdPipe = 0, TapCnt = 0, Pass = 0, AvaWdsCnt = 0;
    u32 RdPipeCounter, RecNum[2], RecRdPipe[2];//, AvaWds[2][REC_NUM];
    BOOL RdPipeFlag, Result;
//    BOOL PassFlag = 0;
    u8 flashtype = 0;

    flashtype = SpicInitParaAllClk[0][0].flashtype;
    
    Result = _FALSE;

#if DRAM_CALIBRATION_IN_NVM
	// read calibration data from system data 0x5d~0x6c
	SPIC_INIT_PARA SpicInitPara;	
	u32 valid;
	union { u8 b[4]; u32 l;} value;
////
#ifdef	SDRAM_INIT_USE_FLASH_API
	flash_turnon();
	if(fspic_isinit == 0) flash_init(&flashobj);
#endif
////

	u32 CpuType = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (0x70)) >> 4);
	
	valid = RdPipe = TapCnt = 0xFFFFFFFF;
	value.l	= HAL_READ32(SPI_FLASH_BASE, FLASH_SDRC_PARA_BASE+8*CpuType);
	if((value.b[0]^value.b[1])==0xFF)
		valid = value.b[0];
#if DEBUG_SDRAM > 1
	DiagPrintf("dump1 %x, %x %x %x %x \n\r", value.l, value.b[0], value.b[1], value.b[2], value.b[3]);
#endif
	value.l	= HAL_READ32(SPI_FLASH_BASE, FLASH_SDRC_PARA_BASE+8*CpuType+4);
	if((value.b[0]^value.b[1])==0xFF)
		RdPipe = value.b[0];
	if((value.b[2]^value.b[3])==0xFF)
		TapCnt = value.b[2];	
#if DEBUG_SDRAM > 1
	DiagPrintf("dump2 %x, %x %x %x %x \n\r", value.l, value.b[0], value.b[1], value.b[2], value.b[3]);
#endif
	if((valid==1)&&(RdPipe!=0xFFFFFFFF)&&(TapCnt!=0xFFFFFFFF)){
		// wait DRAM settle down
		HalDelayUs(10);
		// load previous dram calibration data
		HAL_SDR_WRITE32(REG_SDR_IOCR, ((HAL_SDR_READ32(REG_SDR_IOCR) & 0xff) | (RdPipe << PCTL_IOCR_RD_PIPE_BFO)));
		SDR_DDL_FCTRL(TapCnt);
		if(MemTest(3))
			return _TRUE;
	}	
#endif	

#if !USE_SRC_ONLY_BOOT
#ifdef SDRAM_INIT_USE_TCM_HEAP
	pAvaWds AvaWds = (pAvaWds) tcm_heap_calloc(sizeof(u32)*REC_NUM*2);
#else
    _memset((u8*)AvaWds, 0, sizeof(AvaWds));
#endif
#else
    u32  AvaWds[2][REC_NUM];
    _memset((u8*)AvaWds, 0, sizeof(u32)*REC_NUM*2);
#endif

    volatile struct ms_rxi310_portmap *ms_ctrl_0_map;
    ms_ctrl_0_map = (struct ms_rxi310_portmap*) SDR_CTRL_BASE;
//    ms_ctrl_0_map = ms_ctrl_0_map;
//    PassFlag = PassFlag;
    RdPipeCounter =0;

//    DBG_8195A("%d\n",__LINE__);
 
    for(RdPipe=MIN_RD_PIPE; RdPipe<=MAX_RD_PIPE; RdPipe++) {
//        ms_ctrl_0_map->iocr = (ms_ctrl_0_map->iocr & 0xff) | (RdPipe << PCTL_IOCR_RD_PIPE_BFO);
        HAL_SDR_WRITE32(REG_SDR_IOCR, ((HAL_SDR_READ32(REG_SDR_IOCR) & 0xff) | (RdPipe << PCTL_IOCR_RD_PIPE_BFO)));

        DBG_SDR_INFO("IOCR: 0x%x; Write: 0x%x\n", HAL_SDR_READ32(REG_SDR_IOCR), (RdPipe << PCTL_IOCR_RD_PIPE_BFO));
#if DEBUG_SDRAM > 1
        DBG_8195A("IOCR: 0x%x; Write: 0x%x\n",ms_ctrl_0_map->iocr, (RdPipe << PCTL_IOCR_RD_PIPE_BFO));
#endif

        RdPipeFlag = _FALSE;
//        PassFlag = _FALSE;
        AvaWdsCnt = 0;

        for(TapCnt=0; TapCnt < (MAX_TAP_DLY+1); TapCnt++) {
            // Modify clk delay
#ifdef FPGA
#ifdef FPGA_TEMP
            SDR_DDL_FCTRL(TapCnt);
//            Value32 = (RD_DATA(SDR_CLK_DLY_CTRL) & 0xFF00FFFF);
//            Value32 = (Value32 | (TapCnt << 16));
//            WR_DATA(SDR_CLK_DLY_CTRL, Value32);
#else
            HAL_SDR_WRITE32(REG_SDR_DLY0, TapCnt);
//            ms_ctrl_0_map->phy_dly0 = TapCnt;
#endif
            DBG_SDR_INFO("DLY: 0x%x; Write: 0x%x\n", HAL_PERI_ON_READ32(REG_PESOC_MEM_CTRL), TapCnt);
#else
            SDR_DDL_FCTRL(TapCnt);
//            Value32 = (RD_DATA(SDR_CLK_DLY_CTRL) & 0xFF00FFFF);
//            Value32 = (Value32 | (TapCnt << 16));
//            WR_DATA(SDR_CLK_DLY_CTRL, Value32);
#endif

            Pass = MemTest(10000);
//            PassFlag = _FALSE;

            if(Pass==_TRUE) {   // PASS

                if (!RdPipeFlag) {
                    DBG_SDR_INFO("%d Time Pass\n", RdPipeCounter);
                    RdPipeCounter++;
                    RdPipeFlag = _TRUE;
                    RecRdPipe[RdPipeCounter - 1] = RdPipe;
                }
#ifdef SDRAM_INIT_USE_TCM_HEAP
                AvaWds->m[RdPipeCounter-1][AvaWdsCnt] = TapCnt;
#else
                AvaWds[RdPipeCounter-1][AvaWdsCnt] = TapCnt;
#endif
                AvaWdsCnt++;

                RecNum[RdPipeCounter-1] = AvaWdsCnt;

                if((TapCnt+TAP_DLY)>=MAX_TAP_DLY) {
                    break;
                }

//                PassFlag = _TRUE;
                
                DBG_SDR_INFO("Verify Pass => RdPipe:%d; TapCnt: %d\n", RdPipe, TapCnt);

            }
            else {      // FAIL
//                if(PassFlag==_TRUE) {   
//                    break;
//                }
//                else {
                    if (RdPipeCounter > 0) {
                        RdPipeCounter++;
                        if (RdPipeCounter < 3) {
                            RecNum[RdPipeCounter-1] = 0;
                            RecRdPipe[RdPipeCounter - 1] = RdPipe;
                        }
                        break;
                    }
//                }
            }
        } // for TapCnt


        if (RdPipeCounter > 2) {
            u8 BestRangeIndex, BestIndex;
            
#ifdef CONFIG_SDR_VERIFY //to reduce log
            u32 i;
            DBG_SDR_INFO("Avaliable RdPipe 0\n");                
            
            for (i=0;i<256;i++) {
                DBG_SDR_INFO("%d\n", AvaWds[0][i]);                
            }
            DBG_SDR_INFO("Avaliable RdPipe 1\n");                
            for (i=0;i<256;i++) {
                DBG_SDR_INFO("%d\n", AvaWds[1][i]);                
            }
#endif
            
            DBG_SDR_INFO("Rec 0 => total counter %d; RdPipe:%d;\n", RecNum[0], RecRdPipe[0]);
            DBG_SDR_INFO("Rec 1 => total counter %d; RdPipe:%d;\n", RecNum[1], RecRdPipe[1]);

            BestRangeIndex = (RecNum[0] > RecNum[1]) ? 0 : 1;

            BestIndex = RecNum[BestRangeIndex]>>1;
#ifdef SDRAM_INIT_USE_TCM_HEAP
            DBG_SDR_INFO("The Finial RdPipe: %d; TpCnt: 0x%x\n", RecRdPipe[BestRangeIndex], AvaWds->m[BestRangeIndex][BestIndex]);
#else
            DBG_SDR_INFO("The Finial RdPipe: %d; TpCnt: 0x%x\n", RecRdPipe[BestRangeIndex], AvaWds[BestRangeIndex][BestIndex]);
#endif
            // set RdPipe and tap_dly
//            ms_ctrl_0_map->iocr = (ms_ctrl_0_map->iocr & 0xff) | (RecRdPipe[BestRangeIndex] << PCTL_IOCR_RD_PIPE_BFO);
            HAL_SDR_WRITE32(REG_SDR_IOCR, ((HAL_SDR_READ32(REG_SDR_IOCR) & 0xff) | (RecRdPipe[BestRangeIndex] << PCTL_IOCR_RD_PIPE_BFO)));
            
#ifdef FPGA
#ifdef FPGA_TEMP
#ifdef SDRAM_INIT_USE_TCM_HEAP
            SDR_DDL_FCTRL(AvaWds->m[BestRangeIndex][BestIndex]);
#else
            SDR_DDL_FCTRL(AvaWds[BestRangeIndex][BestIndex]);
#endif
    
//            Value32 = (RD_DATA(SDR_CLK_DLY_CTRL) & 0xFF00FFFF);
//            Value32 = Value32 | (AvaWds[BestRangeIndex][BestIndex] << 16);
//            WR_DATA(SDR_CLK_DLY_CTRL, Value32);
#else
            HAL_SDR_WRITE32(REG_SDR_DLY0, AvaWds[BestRangeIndex][BestIndex]);
//            ms_ctrl_0_map->phy_dly0 = AvaWds[BestRangeIndex][BestIndex];
#endif
#else
            SDR_DDL_FCTRL(AvaWds[BestRangeIndex][BestIndex]);
//            Value32 = (RD_DATA(SDR_CLK_DLY_CTRL) & 0xFF00FFFF);
//            Value32 = Value32 | (AvaWds[BestRangeIndex][BestIndex] << 16);
//            WR_DATA(SDR_CLK_DLY_CTRL, Value32);
#endif            
		#if DRAM_CALIBRATION_IN_NVM
			RdPipe = RecRdPipe[BestRangeIndex];
#ifdef SDRAM_INIT_USE_TCM_HEAP
			TapCnt = AvaWds->m[BestRangeIndex][BestIndex];
#else
			TapCnt = AvaWds[BestRangeIndex][BestIndex];
#endif
			
			value.b[0] = (u8)RdPipe;
			value.b[1] = ~value.b[0];
			value.b[2] = (u8)TapCnt;
			value.b[3] = ~value.b[2];
#if DEBUG_SDRAM > 1
			DiagPrintf("dump1w %x, %x %x %x %x \n\r", value.l, value.b[0], value.b[1], value.b[2], value.b[3]);
#endif
			if( HAL_READ32(SPI_FLASH_BASE, FLASH_SDRC_PARA_BASE+8*CpuType+4) == 0xFFFFFFFF)
			{
				HAL_WRITE32(SPI_FLASH_BASE, FLASH_SDRC_PARA_BASE+8*CpuType+4, value.l);
				if(flashtype == FLASH_MICRON){
					SpicWaitOperationDoneRtl8195A(SpicInitPara);
				}
				else
					SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
			}
			
			valid = 1;
			value.b[0] = (u8)valid;
			value.b[1] = ~value.b[0];
			value.b[2] = 0xFF;
			value.b[3] = 0xFF;
#if DEBUG_SDRAM > 1
			DiagPrintf("dump1w %x, %x %x %x %x \n\r", value.l, value.b[0], value.b[1], value.b[2], value.b[3]);
#endif
			if( HAL_READ32(SPI_FLASH_BASE, FLASH_SDRC_PARA_BASE+8*CpuType) == 0xFFFFFFFF )
			{
				HAL_WRITE32(SPI_FLASH_BASE, FLASH_SDRC_PARA_BASE+8*CpuType, value.l);
				if(flashtype == FLASH_MICRON){
					SpicWaitOperationDoneRtl8195A(SpicInitPara);
				}
				else
					SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
			}
		#endif				
            Result = _TRUE;
            break;
        }

        if (RdPipeCounter == 0) {

            DBG_SDR_INFO("NOT Find RdPipe\n"); 
        }
    }

#ifdef SDRAM_INIT_USE_TCM_HEAP
    tcm_heap_free(AvaWds);
#endif
    return Result;
} // SdrCalibration
 
// HAL_RAM_DATA_SECTION
/*
 
HAL_SDRC_TEXT_SECTION 
VOID
ChangeRandSeed(
    IN  u32 Seed
)
{
    rand_x = Seed;
}
 
HAL_SDRC_TEXT_SECTION 
u32 
Sdr_Rand2(
    VOID
)
{
    HAL_RAM_DATA_SECTION static unsigned int y = 362436;

    HAL_RAM_DATA_SECTION static unsigned int z = 521288629;

    HAL_RAM_DATA_SECTION static unsigned int c = 7654321;

    unsigned long long t, a= 698769069;

    rand_x = 69069 * rand_x + 12345;
    y ^= (y << 13); y ^= (y >> 17); y ^= (y << 5);
    t = a * z + c; c = (t >> 32); z = t;

    return rand_x + y + z;
}

*/
extern __attribute__ ((long_call)) unsigned int Rand(void);
HAL_SDRC_TEXT_SECTION 
s32 
MemTest(
    u32 LoopCnt
)
{
    u32 LoopIndex = 0;
    u32 Value32, Addr;
    for (LoopIndex = 0; LoopIndex<LoopCnt; LoopIndex++) {
//        Value32 = Sdr_Rand2();
//        Addr = Sdr_Rand2();
        Value32 = Rand();
        Addr = Rand();
        Addr &= 0x1FFFFF;
        Addr &= (~0x3);
        
        HAL_SDRAM_WRITE32(Addr, Value32);

        if (HAL_SDRAM_READ32(Addr) != Value32) {
            DBG_8195A("Test %d: No match addr 0x%x => 0x%x != 0x%x\n",LoopIndex,
                Addr, Value32, HAL_SDRAM_READ32(Addr));
            return _FALSE;
        }
        else {
    //            HAL_SDRAM_WRITE32(Addr, 0);        
        }
    }
    return _TRUE;

} // MemTest

//#if defined ( __ICCARM__ )
u8 IsSdrPowerOn(
    VOID
)
{
    if ( HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN) & BIT(21) ) {
        return 0;
    } else {
        return 1;
    }
}
//#endif

#else // ifndef CONFIG_SDR_EN

HAL_SDRC_TEXT_SECTION VOID SdrPowerOff(
    VOID
)
{
    SDR_PIN_FCTRL(OFF);
    LDO25M_CTRL(OFF);
    HAL_WRITE32(PERI_ON_BASE, REG_SOC_FUNC_EN, HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN) | BIT(21));
}


HAL_SDRC_TEXT_SECTION VOID SdrCtrlInit(VOID)
{
	DBG_SDR_ERR("No SDRAM!\n");
	SdrPowerOff();
}

HAL_SDRC_TEXT_SECTION u32 SdrControllerInit( VOID)
{
	DBG_SDR_ERR("No SDRAM!\n");
	SdrPowerOff();
	return 1;
}
#endif  // end of "#ifdef CONFIG_SDR_EN"



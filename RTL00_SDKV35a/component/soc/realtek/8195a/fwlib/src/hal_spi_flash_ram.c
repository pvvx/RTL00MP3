/*
 * hal_spi_flash_ram.c
 * reversed hal_spi_flash_ram.o pvvx 17/10/16 ...
 * 'Revers' is not finished, not tested, not to include in the project!
 */

#include "rtl8195a.h"

//-------------------------------------------------------------------------
// Function declarations
//extern VOID SpicLoadInitParaFromClockRtl8195A (IN  u8 CpuClkMode, IN  u8 BaudRate, IN  PSPIC_INIT_PARA pSpicInitPara);

//-------------------------------------------------------------------------
// Data declarations
//char algn_1[]; // weak
//int unk_2; // weak

typedef struct _SPIC_INIT_PARA_ROM_ {
    u8  BaudRate;
    u8  RdDummyCyle;
    u8  DelayLine;
    u8  Rsvd;
}SPIC_INIT_PARA_ROM, *PSPIC_INIT_PARA_ROM;

SPIC_INIT_PARA HAL_FLASH_DATA_SECTION SpicInitParaAllClk[3][6]; // SpicInitParaAllClk[SPIC_BIT_MODE][CPU_CK_SEL]

#ifdef INCLUDE_ROM_FUNCTIONS

extern SPIC_INIT_PARA_ROM SpicInitCPUCLK[3]; // ROM: 0x30c98;

SPIC_INIT_PARA_ROM SpicInitCPUCLK[4] = {
		{ 0x01, 0x01, 0x5E, 0x00 },
		{ 0x01, 0x01, 0x00, 0x00 },
		{ 0x01, 0x02, 0x23, 0x00 },
		{ 0x01, 0x05, 0x05, 0x00 }
};
//----- ROM: SpicRxCmdRtl8195A
// recieve command
u8 SpicRxCmdRtl8195A(u8 cmd)
{
	HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
			(HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_ADDR_CH(3)|BIT_DATA_CH(3)|BIT_CTRL_TMOD(3))))
			| BIT_CTRL_TMOD(3)); // SpicOneBitMode +..., 40006000 = 40006000 & 0xFFF0FCFF | 0x300;
	HAL_SPI_WRITE32(REG_SPIC_DR0, cmd); // 40006060 = cmd;
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
	while (HAL_SPI_READ32(REG_SPIC_SR) & BIT_BUSY); // 40006028 & 1
	return cmd;
}

//----- ROM: SpicWaitBusyDoneRtl8195A
// wait sr[0] = 0, wait transmission done
VOID SpicWaitBusyDoneRtl8195A(VOID)
{
	while (HAL_SPI_READ32(REG_SPIC_SR) & BIT_BUSY);
}

//----- ROM: SpicGetFlashStatusRtl8195A
// RDSR, read spi-flash status register
u8 SpicGetFlashStatusRtl8195A(SPIC_INIT_PARA SpicInitPara)
{
  HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_CTRL_NDF(1)); //40006004 = 1;

  u16 save_reg = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH); // v2 = 4000611C;
  HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH,
  			(HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH)
  					& (~((BIT_MASK_RD_DUMMY_LENGTH) << BIT_SHIFT_RD_DUMMY_LENGTH)))
			| SpicInitPara.RdDummyCyle); // 4000611C = v1 | (4000611C >> 16 << 16);
  HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
			(HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_ADDR_CH(3)|BIT_DATA_CH(3)|BIT_CTRL_TMOD(3))))
			| BIT_CTRL_TMOD(3)); // SpicOneBitMode +..., 40006000 = 40006000 & 0xFFF0FCFF | 0x300;
  HAL_SPI_WRITE32(REG_SPIC_DR0, FLASH_CMD_RDSR); // 40006060 = 5; // RDSR (read status register)
  HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
  while (HAL_SPI_READ32(REG_SPIC_SR) & BIT_BUSY); // 40006028 & 1
  u8 result = HAL_SPI_READ32(REG_SPIC_DR0); // v40006060;
  HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
  HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH,
		  (HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH)
		    		& (~((BIT_MASK_RD_DUMMY_LENGTH) << BIT_SHIFT_RD_DUMMY_LENGTH)))
		  | save_reg);
  return result;
}

//----- ROM: SpicWaitWipDoneRtl8195A
// wait spi-flash status register[0] = 0
VOID SpicWaitWipDoneRtl8195A(SPIC_INIT_PARA SpicInitPara)
{
  while (SpicGetFlashStatusRtl8195A(SpicInitPara) & 1);
}

//----- ROM: SpicTxCmdRtl8195A
// transfer command
VOID SpicTxCmdRtl8195A(u8 cmd, SPIC_INIT_PARA SpicInitPara)
{
  HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
  			HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_ADDR_CH(3)|BIT_DATA_CH(3)|BIT_CTRL_TMOD(3)))); //  40006000 &= 0xFFF0FCFF;
  HAL_SPI_WRITE32(REG_SPIC_DR0, cmd); // 40006060 = cmd;
  HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
  while (HAL_SPI_READ32(REG_SPIC_SR) & BIT_BUSY);
  while (SpicGetFlashStatusRtl8195A(SpicInitPara) & 1);
}

//----- ROM: SpicSetFlashStatusRtl8195A
// WRSR, write spi-flash status register
VOID SpicSetFlashStatusRtl8195A(u32 data, SPIC_INIT_PARA SpicInitPara)
{
  u32 save_reg = HAL_SPI_READ32(REG_SPIC_ADDR_LENGTH); //v2 = 40006118;
  SpicTxCmdRtl8195A(FLASH_CMD_WREN, SpicInitPara); // WREN (write enable)
  HAL_SPI_WRITE32(REG_SPIC_CTRLR0, HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_CTRL_TMOD(3)))); //  40006000 &= 0xFFFFFCFF;
  HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, BIT_CTRL_ADDR_PHASE_LENGTH(1)); // 40006118 = 1;
  HAL_SPI_WRITE32(REG_SPIC_DR0, data); // 40006060 = v1;
  HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); //40006008 = 1;
  while (HAL_SPI_READ32(REG_SPIC_SR) & BIT_BUSY);
  HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); //40006008 = 0;
  HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, save_reg);
  while (SpicGetFlashStatusRtl8195A(SpicInitPara) & 1);
}

int SpicCalibrationPattern[] = { 0x96969999, 0xFC66CC3F, 0x3CC33C0, 0x6231DCE5 }; // weak

//----- ROM: SpicCmpDataForCalibrationRtl8195A
// compare read_data and golden_data
u32 SpicCmpDataForCalibrationRtl8195A(VOID)
{
	int i;
	for(i = 0; i < 4; i++) if (SpicCalibrationPattern[i] != HAL_READ32(SPI_FLASH_BASE, i<<2))  return 0;
	return 1;
}

//----- ROM: SpicLoadInitParaFromClockRtl8195A
VOID SpicLoadInitParaFromClockRtl8195A (IN  u8 CpuClkMode, IN  u8 BaudRate, IN  PSPIC_INIT_PARA pSpicInitPara)
//int __fastcall SpicLoadInitParaFromClockRtl8195A(int a1, int a2, int a3)
{
  int spic_baud_rate = (HAL_SYS_CTRL_READ32(REG_SYS_EFUSE_SYSCFG6) >> BIT_SHIFT_SYS_SPIC_INIT_BAUD_RATE_SEL) & BIT_MASK_SYS_SPIC_INIT_BAUD_RATE_SEL; // (40000038 >> 26) & 3;
  PSPIC_INIT_PARA_ROM spic_tab_rom = &SpicInitCPUCLK[spic_baud_rate];
  u8 DelayLine = spic_tab_rom->DelayLine;
  u32 reg_data = HAL_SYS_CTRL_READ32(REG_SYS_SYSTEM_CFG1) & BIT_SYSCFG_ALDN_STS; // 400001F4 & 1;
  if (reg_data)  {
	  reg_data = HAL_SYS_CTRL_READ32(REG_SYS_EEPROM_CTRL0) & 0x20; // 400000E0 & 0x20;
    if (reg_data) {
    	reg_data = HAL_SYS_CTRL_READ32(REG_SYS_EFUSE_SYSCFG6) & (1 << BIT_SHIFT_SYS_CPU_CLK_SEL); // 40000038 & 0x1000000;
      if (reg_data) {
    	  DelayLine = 3 * ((HAL_SYS_CTRL_READ32(REG_SYS_EFUSE_SYSCFG6) >> 16) & 0x1F) ; // 3 * ((v40000038 >> 16) & 0x1F);
    	  reg_data = ((HAL_SYS_CTRL_READ32(REG_SYS_EFUSE_SYSCFG6) >> 16) & 0xFF) >> 5;
      }
    }
  }
  pSpicInitPara->BaudRate = spic_tab_rom->BaudRate;
  pSpicInitPara->RdDummyCyle = reg_data + spic_tab_rom->RdDummyCyle;
  pSpicInitPara->DelayLine = DelayLine;
}

//----- ROM: SpicInitRtl8195A
// spi-flash controller initialization
VOID SpicInitRtl8195A(u8 InitBaudRate, u8 SpicBitMode)
{
  SPIC_INIT_PARA SpicInitPara; //char v5;
  u32 CpuClkMode = (HAL_PERI_ON_READ32(REG_SYS_CLK_CTRL1)
  			>> BIT_SHIFT_PESOC_OCP_CPU_CK_SEL) & BIT_MASK_PESOC_OCP_CPU_CK_SEL; //  v3 = (40000014 >> 4) & 7;
  SpicLoadInitParaFromClockRtl8195A(CpuClkMode, InitBaudRate, &SpicInitPara);
  HAL_SPI_READ32(REG_SPIC_CTRLR1); // 0x40006004;
  HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); //40006008 = 0;
  HAL_SPI_WRITE32(REG_SPIC_BAUDR, InitBaudRate); // 40006014 = v2;
  HAL_SPI_WRITE32(REG_SPIC_SER, BIT_CTRL_SER(1)); // 40006010 = 1;
  HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH,
    			(HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH)
    					& (~((BIT_MASK_RD_DUMMY_LENGTH) << BIT_SHIFT_RD_DUMMY_LENGTH)))
  			| SpicInitPara.RdDummyCyle); // 4000611C = v1 | (4000611C >> 16 << 16);
  HAL_PERI_ON_WRITE32(REG_PESOC_MEM_CTRL,
		  (HAL_PERI_ON_READ32(REG_PESOC_MEM_CTRL) & BIT_MASK_PESOC_FLASH_DDL_CTRL << BIT_SHIFT_PESOC_FLASH_DDL_CTRL)
		  | SpicInitPara.DelayLine); //  40000300 = 40000300 & 0xFFFFFF00 | v7;
  HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_CTRL_NDF(4)); //40006004 = 4;
  if (SpicBitMode == SpicDualBitMode)
	  HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
			  (HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_CTRL_CMD_CH(3)|BIT_ADDR_CH(3)|BIT_DATA_CH(3))))
			  | BIT_ADDR_CH(1) | BIT_DATA_CH(1)); // 40006000 = 40006000 & 0xFFC0FFFF | 0x50000;
  else if (SpicBitMode >= SpicDualBitMode) {
    if (SpicBitMode == SpicQuadBitMode)
  	  HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
  			  (HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_CTRL_CMD_CH(3)|BIT_ADDR_CH(3)|BIT_DATA_CH(3))))
  			  | BIT_ADDR_CH(2) | BIT_DATA_CH(2)); // 40006000 = 40006000 & 0xFFC0FFFF | 0xA0000;
  }
  else // (SpicBitMode == SpicOneBitMode)
	  HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
			  HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_CTRL_CMD_CH(3)|BIT_ADDR_CH(3)|BIT_DATA_CH(3)))); //40006000 &= 0xFFC0FFFF;
}

//----- ROM: SpicEraseFlashRtl8195A
// CE, flash chip erase
VOID SpicEraseFlashRtl8195A(VOID)
{
  SPIC_INIT_PARA SpicInitPara;
  u32 CpuClkMode = (HAL_PERI_ON_READ32(REG_SYS_CLK_CTRL1)
  			>> BIT_SHIFT_PESOC_OCP_CPU_CK_SEL) & BIT_MASK_PESOC_OCP_CPU_CK_SEL; //  v3 = (40000014 >> 4) & 7;
  SpicLoadInitParaFromClockRtl8195A(CpuClkMode, 1, &SpicInitPara);
  DBG_MISC_ERR("Init Baud Rate: 0x%x\n", SpicInitPara.BaudRate);
  DBG_MISC_ERR("Init RD Dummy: 0x%x\n", SpicInitPara.RdDummyCyle);
  DBG_MISC_ERR("Init Delay Line: 0x%x\n", SpicInitPara.DelayLine);
  while (SpicGetFlashStatusRtl8195A(SpicInitPara) & 1);
  while (!(SpicGetFlashStatusRtl8195A(SpicInitPara) & 2)) SpicTxCmdRtl8195A(FLASH_CMD_WREN, SpicInitPara);
  DBG_MISC_ERR("Erase Cmd Set\n");
  SpicTxCmdRtl8195A(FLASH_CMD_CE, SpicInitPara);
  while (SpicGetFlashStatusRtl8195A(SpicInitPara) & 2); // write enable latch
}

//----- SpiFlashApp
void SpiFlashApp(u32 * parm)
{
  SPIC_INIT_PARA SpicInitPara;

  u32 CpuClkMode = (HAL_PERI_ON_READ32(REG_SYS_CLK_CTRL1)
  			>> BIT_SHIFT_PESOC_OCP_CPU_CK_SEL) & BIT_MASK_PESOC_OCP_CPU_CK_SEL; //  v3 = (40000014 >> 4) & 7;
  SpicLoadInitParaFromClockRtl8195A(CpuClkMode, 1, &SpicInitPara);
  if (parm[0] == 1)
  {
    DBG_MISC_ERR("Initial Spi Flash Controller\n");
    HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT_SOC_FLASH_EN); // 40000210 |= 0x10u;
    HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL, HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_ACTCK_FLASH_EN); // 40000230 |= 0x100u;
    HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL, HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_SLPCK_FLASH_EN); // 40000230 |= 0x200u;
    HalPinCtrlRtl8195A(SPI_FLASH, 0, 1);
    if (parm[1] == SpicDualBitMode)
    {
    	DBG_MISC_ERR("Initial Spic Two bit mode\n");
    	SpicInitRtl8195A(SpicInitPara.BaudRate, SpicDualBitMode);
    }
    else if (parm[1] < SpicDualBitMode)
    {
    	DBG_MISC_ERR("Initial Spic One bit mode\n");
    	SpicInitRtl8195A(SpicInitPara.BaudRate, SpicOneBitMode);
    }
    else if (parm[1] == SpicQuadBitMode)
    {
    	DBG_MISC_ERR("Initial Spic Four bit mode\n");
    	SpicInitRtl8195A(SpicInitPara.BaudRate, SpicQuadBitMode);
    }
  }
  else if (parm[0] == 2)
  {
	 DBG_MISC_ERR("Erase Flash Start\n");
	 SpicEraseFlashRtl8195A();
	 DBG_MISC_ERR("Erase Flash End\n");
  }
}

#endif

//----- SpicRxCmdRefinedRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicRxCmdRefinedRtl8195A(IN u8 cmd, IN SPIC_INIT_PARA SpicInitPara) {
	SPIC_INIT_PARA SpicInitParaa;
	u32 CpuClkMode = (HAL_PERI_ON_READ32(REG_SYS_CLK_CTRL1)
			>> BIT_SHIFT_PESOC_OCP_CPU_CK_SEL) & BIT_MASK_PESOC_OCP_CPU_CK_SEL; //  v3 = (40000014 >> 4) & 7;
	PSPIC_INIT_PARA pspicp = &SpicInitParaAllClk[0][CpuClkMode];
	if (!pspicp->Rsvd) {
		SpicLoadInitParaFromClockRtl8195A(CpuClkMode, 1, &SpicInitParaa);
		pspicp = &SpicInitParaa;
	}
	DBG_SPIF_INFO("%s(0x%x, 0x%x)\n", "SpicRxCmdRefinedRtl8195A", cmd, *((u32 *)(*pspicp)));
	u32 AutoLengthSave = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH); //  v5 = 4000611C;
	HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH,
			(HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH) & (~((BIT_MASK_RD_DUMMY_LENGTH) << BIT_SHIFT_RD_DUMMY_LENGTH))) | pspicp->RdDummyCyle); // 4000611C = v6->RdDummyCyle | (4000611C >> 16 << 16);
	u32 BaudSave = HAL_SPI_READ32(REG_SPIC_BAUDR); // 40006014;
	HAL_SPI_WRITE32(REG_SPIC_BAUDR,
			((HAL_SPI_READ32(REG_SPIC_BAUDR) >> 16) << 16) | pspicp->BaudRate);
	u32 MemCtrlSave = HAL_PERI_ON_READ32(REG_PESOC_MEM_CTRL); // 40000300;
	HAL_PERI_ON_WRITE32(REG_PESOC_MEM_CTRL,
			(HAL_PERI_ON_READ32(REG_PESOC_MEM_CTRL) & 0xFFFFFF00) | pspicp->DelayLine);
	HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
			((HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_ADDR_CH(3)|BIT_DATA_CH(3)|BIT_CTRL_TMOD(3)))) | BIT_CTRL_TMOD(3))); // SpicOneBitMode +..., 40006000 = 40006000 & 0xFFF0FCFF | 0x300;
	HAL_PERI_ON_WRITE32(REG_SPIC_DR0, cmd); //  40006060 = v2;
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
	SpicWaitBusyDoneRtl8195A();
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH,
			(HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH) >> 16 << 16) | AutoLengthSave);
	HAL_SPI_WRITE32(REG_SPIC_BAUDR,
			(HAL_SPI_READ32(REG_SPIC_BAUDR) >> 16 << 16) | BaudSave);
	HAL_PERI_ON_WRITE32(REG_SPIC_CTRLR0,
			(HAL_SPI_READ32(REG_SPIC_CTRLR0) & 0xFFFFFF00) | MemCtrlSave);
}

//----- SpicInitRefinedRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicInitRefinedRtl8195A(IN u8 InitBaudRate, IN u8 SpicBitMode) {
	PSPIC_INIT_PARA pspicp;
	SPIC_INIT_PARA TmpSpicInitPara;

	TmpSpicInitPara.BaudRate = InitBaudRate;
	TmpSpicInitPara.id[0] = SpicBitMode;
	u32 CpuClkMode = (HAL_PERI_ON_READ32(REG_SYS_CLK_CTRL1)
			>> BIT_SHIFT_PESOC_OCP_CPU_CK_SEL) & BIT_MASK_PESOC_OCP_CPU_CK_SEL; // v4 = (40000014 >> 4) & 7;
	PSPIC_INIT_PARA pspicp = &SpicInitParaAllClk[CpuClkMode];
	if (!pspicp->Rsvd) {
		SpicLoadInitParaFromClockRtl8195A(CpuClkMode, 1, &TmpSpicInitPara);
		pspicp = &TmpSpicInitPara;
	}
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	HAL_SPI_WRITE32(REG_SPIC_BAUDR, InitBaudRate);
	HAL_SPI_WRITE32(REG_SPIC_SER, BIT_CTRL_SER(1)); //  40006010 = 1;
	HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH,
			(HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH) & (~((BIT_MASK_RD_DUMMY_LENGTH) << BIT_SHIFT_RD_DUMMY_LENGTH))) | pspicp->RdDummyCyle); // 4000611C = v6->RdDummyCyle | (4000611C >> 16 << 16);
	HAL_PERI_ON_WRITE32(REG_PESOC_MEM_CTRL,
			(HAL_PERI_ON_READ32(REG_PESOC_MEM_CTRL) & (~(BIT_MASK_PESOC_FLASH_DDL_CTRL << BIT_SHIFT_PESOC_FLASH_DDL_CTRL))) | pspicp->DelayLine); // 40000300 = v6->DelayLine | 40000300 & 0xFFFFFF00;
	HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_CTRL_NDF(4)); //40006004 = 4;
	u32 regdata = HAL_SPI_READ32(REG_SPIC_CTRLR0)
			& (~((BIT_MASK_CMD_CH << BIT_SHIFT_CMD_CH)
					| (BIT_MASK_DATA_CH << BIT_SHIFT_DATA_CH)
					| (BIT_MASK_ADDR_CH << BIT_SHIFT_ADDR_CH))); // 0xFFC0FFFF;
	if (SpicBitMode == SpicDualBitMode) {
		regdata |= BIT_CTRL_ADDR_CH(1) | BIT_CTRL_DATA_CH(1); // 0x50000; // v7 = 40006000 & 0xFFC0FFFF | 0x50000;
	} else if (SpicBitMode >= SpicDualBitMode) {
		if (SpicBitMode != SpicQuadBitMode)	return;
		regdata |= BIT_CTRL_ADDR_CH(2) | BIT_CTRL_DATA_CH(2); // v7 = 40006000 & 0xFFC0FFFF | 0xA0000;
	}
	HAL_SPI_WRITE32(REG_SPIC_CTRLR0, regdata); // 40006000 = v7;
}

//----- SpicReadIDRtl8195A
//VOID HAL_FLASH_TEXT_SECTION SpicReadIDRtl8195A(VOID)
void  SpicReadIDRtl8195A(SPIC_INIT_PARA SpicInitPara)
{
	u16 flash_type;
	u8 flashtype;
	u32 flash_id;
	u32 flash_density;
	SPIC_INIT_PARA spic_para = SpicInitPara;
//	memset(&spic_para, 0, sizeof(not_used));
//	spic_para = SpicInitPara;
	DBG_SPIF_INFO("%s(0x%x)\n", "SpicReadIDRtl8195A", *((u32 *)spic_para));
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_CTRL_NDF(3)); //40006004 = 3;
	SpicRxCmdRefinedRtl8195A(FLASH_CMD_RDID, (SPIC_INIT_PARA) spic_para);
	flash_id = HAL_SPI_READ32(REG_SPIC_DR0); // 40006060;
	flash_type = HAL_SPI_READ32(REG_SPIC_DR0) >> 8; // 40006060 >> 8;
	flash_density = (HAL_SPI_READ32(REG_SPIC_DR0) >> 16) & 0xFF; // (40006060 >> 16) & 0xFF;
	switch (flash_id) {
	case 0x20:
		flashtype = FLASH_MICRON;
		break;
	case 0xC2:
		if (flash_density == 0x18) flashtype = FLASH_MXIC_4IO;
		else flashtype = FLASH_MXIC;
		break;
	case 0xEF:
		flashtype = FLASH_WINBOND;
		break;
	case 0x1C:
		flashtype = FLASH_EON;
		break;
	default:
		DBG_MISC_ERR("Invalid ID\n");
		flashtype = FLASH_OTHERS;
		break;
	}
	int i, j;
LABEL_23:
	for(i = 0; i < 3; i++){
		for(j = 0; j < 6; j++) {
			SpicInitParaAllClk[i][j].id[0] = flash_id;
			SpicInitParaAllClk[i][j].id[1] = flash_type;
			SpicInitParaAllClk[i][j].id[2] = flash_density;
			SpicInitParaAllClk[i][j].flashtype = flashtype;
		};
	};
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
}

//----- SpicConfigAutoModeRtl8195A
void HAL_FLASH_TEXT_SECTION SpicConfigAutoModeRtl8195A(enum _SPIC_BIT_MODE_ SpicBitMode) {
	u32 regd;
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;

	HAL_SPI_WRITE32(REG_SPIC_VALID_CMD, HAL_SPI_READ32(REG_SPIC_VALID_CMD) & 0xFFFFFE01); // 40006120 &= 0xFFFFFE01;
	if (SpicBitMode == SpicDualBitMode) {
		HAL_SPI_WRITE32(REG_SPIC_WRITE_DUAL_ADDR_DATA, 0x0A2); // 400060FC = 162;
		HAL_SPI_WRITE32(REG_SPIC_READ_DUAL_ADDR_DATA, 0x3B); // 400060E8 = 59;
		regd = HAL_SPI_READ32(REG_SPIC_VALID_CMD) | BIT_RD_DUAL_I | BIT_WR_BLOCKING; // v1 = 40006120 | 0x202;
	} else {
		if (SpicBitMode != SpicQuadBitMode) return;
		HAL_SPI_WRITE32(REG_SPIC_WRITE_QUAD_ADDR_DATA, 0x38); // 40006104 = 56;
		HAL_SPI_WRITE32(REG_SPIC_READ_QUAD_ADDR_DATA, 0xEB); // 400060F0 = 235;
		HAL_SPI_WRITE32(REG_SPIC_WRITE_QUAD_DATA, 0x32); // 40006100 = 50;
		HAL_SPI_WRITE32(REG_SPIC_READ_QUAD_DATA, 0x6B); // 400060EC = 107;
		if (SpicInitParaAllClk[0][0].flashtype == FLASH_MXIC_4IO)
			regd = HAL_SPI_READ32(REG_SPIC_VALID_CMD) | BIT_RD_QUAD_O | BIT_WR_BLOCKING; // v1 = 40006120 | 0x208;
		else
			regd = HAL_SPI_READ32(REG_SPIC_VALID_CMD) | BIT_RD_QUAD_IO | BIT_WR_BLOCKING; // v1 = 40006120 | 0x210;
	}
	HAL_SPI_WRITE32(REG_SPIC_VALID_CMD, regd); // 40006120 = v1;
}

#define SPI_FBLK 128
//----- SpicUserReadRtl8195A
void HAL_FLASH_TEXT_SECTION SpicUserReadRtl8195A(uint32_t Length, uint32_t addr, uint8_t *data, int BitMode) {

	if(Length == 0 || data == NULL) return;
	u32 cmd;
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
			(HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_ADDR_CH(3)|BIT_DATA_CH(3)|BIT_CTRL_TMOD(3)))) | BIT_CTRL_TMOD(3)); // SpicOneBitMode +..., 40006000 = 40006000 & 0xFFF0FCFF | 0x300;
	if (BitMode == SpicDualBitMode) {
		HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
				HAL_SPI_READ32(REG_SPIC_CTRLR0) | BIT_CTRL_FAST_RD(1)); // 40006000 |= 0x40000u;
		cmd = FLASH_CMD_DREAD;
	}
	else cmd = FLASH_CMD_READ;
	u32 faddr = addr;
	u32 size = Length;
	u32 blksize;
	u8 * des = data;
	HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, BIT_CTRL_ADDR_PHASE_LENGTH(3)); // 40006118 = 3;
	HAL_SPI_WRITE32(REG_SPIC_SER, BIT_CTRL_SER(1)); //  40006010 = 1;
	HAL_SPI_WRITE32(REG_SPIC_DR0,
			((u16)faddr >> 8 << 16) | (((faddr >> 16) & 0xFF) << 8) | (faddr << 24) | cmd); // 40006060  = ((u16)v7 >> 8 << 16) | (((v7 >> 16) & 0xFF) << 8) | (v7 << 24) | v8;
	blksize = (u32)data & 3;
	if(blksize) {
		blksize = 4 - blksize;
#if DEBUGSOO > 4
		DiagPrintf("fr1:%p<-%p[%u]\n", des, faddr, blksize);
#endif
		if(size < blksize) blksize = size;
		HAL_SPI_WRITE32(REG_SPIC_CTRLR1, blksize); //40006004 = v9;
		HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
		size -= blksize;
		faddr += blksize;
		SpicWaitBusyDoneRtl8195A();
		uint32 data_buf = HAL_SPI_READ32(REG_SPIC_DR0); // SPI_FLASH_CTRL_BASE + REG_SPIC_DR0;
		do {
			*(uint8 *)des++ = data_buf;
			data_buf >>= 8;
		} while(--blksize);
		HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	}
	while(size) {
		if(size < SPI_FBLK) blksize = size;
		else blksize = SPI_FBLK;
#if DEBUGSOO > 5
		DiagPrintf("fr2:%p<-%p[%u]\n", des, faddr, blksize);
#endif
		HAL_SPI_WRITE32(REG_SPIC_CTRLR1, blksize); //40006004 = v4;
		HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
		size -= blksize;
		faddr += blksize;
		SpicWaitBusyDoneRtl8195A();
		uint32 *srcdw = (uint32 *)(SPI_FLASH_CTRL_BASE + REG_SPIC_DR0);
		while(blksize >> 2) {
			*((uint32 *)des) = *srcdw++;
			des++;
			blksize -= 4;
		};
		if(blksize) {
#if DEBUGSOO > 4
			DiagPrintf("fr3:%p<-%p[%u]\n", des, faddr, blksize);
#endif
			uint32 data_buf = *srcdw;
			do {
				*(uint8 *)des++ = data_buf;
				data_buf >>= 8;
			} while(--blksize);
			break;
		};
	};
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0); // 40006008 = v4;
	HAL_SPI_WRITE32(REG_SPIC_FLUSH_FIFO, BIT_FLUSH_FIFO); // 40006128 = 1;
}


#define SPIC_RDBLK_SIZE 32
//----- SpicUserReadFourByteRtl8195A
void HAL_FLASH_TEXT_SECTION SpicUserReadFourByteRtl8195A(uint32_t Length, uint32_t addr, uint32_t *data, int BitMode) {
	u32 blklen = Length;
	u32 faddr = addr;
	u32 pdes = data;
	u32 cmd;
	u32 cnt;

	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); //40006008 = 0;
	HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
			((HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_ADDR_CH(3)|BIT_DATA_CH(3)|BIT_CTRL_TMOD(3)))) | BIT_CTRL_TMOD(3))); // SpicOneBitMode +..., 40006000 = 40006000 & 0xFFF0FCFF | 0x300;
	if (BitMode == SpicDualBitMode) {
		HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
				HAL_SPI_READ32(REG_SPIC_CTRLR0) | BIT_CTRL_FAST_RD(1)); // 40006000 |= 0x40000u;
		cmd = FLASH_CMD_DREAD; // v8 = 59;
	} else if (BitMode) {
		if (SpicInitParaAllClk[0][0].flashtype == FLASH_MXIC_4IO) {
			HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
					HAL_SPI_READ32(REG_SPIC_CTRLR0) | BIT_CTRL_CK_MTIMES(1)); // 40006000 |= 0x80000u;
			cmd = FLASH_CMD_QREAD; // cmd = 0x6B;
		} else {
			HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
					HAL_SPI_READ32(REG_SPIC_CTRLR0) | BIT_CTRL_CK_MTIMES(1) | BIT_CTRL_CMD_CH(2)); // 40006000 |= 0xA0000u;
			cmd = FLASH_CMD_4READ; // cmd = 0xEB;
		}
	}
	else cmd = FLASH_CMD_READ; // cmd = 0x03;
	if (blklen >= SPIC_RDBLK_SIZE)		HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_CTRL_NDF(SPIC_RDBLK_SIZE)); //40006004 = 32;
	if (blklen < SPIC_RDBLK_SIZE)		HAL_SPI_WRITE32(REG_SPIC_CTRLR1, blklen); //40006004 = v4;
	HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, BIT_CTRL_ADDR_PHASE_LENGTH(3)); // 40006118 = 3;
	HAL_SPI_WRITE32(REG_SPIC_SER, BIT_CTRL_SER(1)); //  40006010 = 1;
	HAL_SPI_WRITE32(REG_SPIC_DR0,
			((u16)addr >> 8 << 16) | (((addr >> 16) & 0xFF) << 8) | (addr << 24) | cmd); // 40006060 = ((u16)addr >> 8 << 16) | (v6 << 8) | (addr << 24) | v8;
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); //40006008 = 1;
	SpicWaitBusyDoneRtl8195A();
	cnt = 0;
	while (blklen) {
		if (cnt == SPIC_RDBLK_SIZE) {
			faddr += SPIC_RDBLK_SIZE;
			HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); //40006008 = 0;
			if (blklen >= SPIC_RDBLK_SIZE) HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_CTRL_NDF(SPIC_RDBLK_SIZE)); //40006004 = 32;
			if (blklen < SPIC_RDBLK_SIZE) HAL_SPI_WRITE32(REG_SPIC_CTRLR1, blklen); //40006004 = v4;
			HAL_SPI_WRITE32(REG_SPIC_DR0,
					cmd | (faddr << 24) | (((faddr >> 16) & 0xFF) << 8) | ((u16)faddr >> 8 << 16)); // 40006060 = v8 | (v5 << 24) | (((v5 >> 16) & 0xFF) << 8) | ((u16)v5 >> 8 << 16);
			HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
			SpicWaitBusyDoneRtl8195A();
			cnt = 0;
		}
		cnt += 4;
		*pdes++ = HAL_SPI_READ32(REG_SPIC_DR0); // 40006060;
		blklen -= 4;
	}
	HAL_SPI_WRITE32(REG_SPIC_FLUSH_FIFO, BIT_FLUSH_FIFO); // 40006128 = 1;
}

//----- SpicGetExtendAddrRtl8195A
u8 HAL_FLASH_TEXT_SECTION SpicGetExtendAddrRtl8195A(SPIC_INIT_PARA SpicInitPara) {
	SPIC_INIT_PARA SpicInitParaa = SpicInitPara;
	DBG_SPIF_INFO("%s(0x%x)\n", "SpicGetExtendAddrRtl8195A",
			SpicInitPara, SpicInitPara);
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_CTRL_NDF(1)); //40006004 = 1;
	SpicRxCmdRefinedRtl8195A(FLASH_CMD_REAR, SpicInitParaa);
	u8 result = HAL_SPI_READ32(REG_SPIC_DR0); // 40006060;
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	return result;
}


//----- SpicGetConfigRegRtl8195A
u8 HAL_FLASH_TEXT_SECTION SpicGetConfigRegRtl8195A(SPIC_INIT_PARA SpicInitPara) {
	SPIC_INIT_PARA SpicInitParaa = SpicInitPara;
	DBG_SPIF_INFO("%s(0x%x)\n", "SpicGetConfigRegRtl8195A",	((u32 *)SpicInitPara)[0]);
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_CTRL_NDF(1)); //40006004 = 1;
	SpicRxCmdRefinedRtl8195A(FLASH_CMD_RDCR, SpicInitParaa);
	u8 result = HAL_SPI_READ32(REG_SPIC_DR0); // 40006060;
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	return result;
}

//----- SpicGetFlashStatusRefinedRtl8195A
u8 HAL_FLASH_TEXT_SECTION SpicGetFlashStatusRefinedRtl8195A(IN SPIC_INIT_PARA SpicInitPara)
{
	SPIC_INIT_PARA SpicInitParaa = SpicInitPara;

	DBG_SPIF_INFO("%s(0x%x)\n", "SpicGetFlashStatusRefinedRtl8195A", ((u32 *)SpicInitPara)[0]);
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_CTRL_NDF(1)); //40006004 = 1;
	SpicRxCmdRefinedRtl8195A(FLASH_CMD_RDSR, SpicInitParaa);
	u8 result = HAL_SPI_READ32(REG_SPIC_DR0); // 40006060;
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	return result;
}

//----- SpicWaitWipDoneRefinedRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicWaitWipDoneRefinedRtl8195A(IN SPIC_INIT_PARA SpicInitPara)
{
	SPIC_INIT_PARA SpicInitParaa = SpicInitPara;

	DBG_SPIF_INFO("%s(0x%x)\n", "SpicWaitWipDoneRefinedRtl8195A", ((u32 *)SpicInitParaa)[0]);
	while (SpicGetFlashStatusRefinedRtl8195A(SpicInitParaa) & 1);
}

//----- SpicTxCmdWithDataRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicTxCmdWithDataRtl8195A(IN u8 cmd,
		IN u8 DataPhaseLen, IN u8* pData, IN SPIC_INIT_PARA SpicInitPara)
{
	SPIC_INIT_PARA SpicInitParaa = SpicInitPara;
	u8 * pdes = pData;
	int i;

	pdes = pData;
	DBG_SPIF_INFO("%s(0x%x, 0x%x, 0x%x, 0x%x)\n",
			"SpicTxCmdWithDataRtl8195A", cmd, DataPhaseLen, pData, ((u32 *)SpicInitParaa)[0]);
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	if (DataPhaseLen > 15) {
		DBG_SPIF_WARN("SpicTxInstRtl8195A: Data Phase Leng too Big(%d)\n", DataPhaseLen);
		DataPhaseLen = 15;
	}
	HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, DataPhaseLen); // 40006118 = v7;
	HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
			(HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_ADDR_CH(3)|BIT_DATA_CH(3)|BIT_CTRL_TMOD(3))))); // SpicOneBitMode: 40006000 &= 0xFFF0FCFF;
	HAL_SPI_WRITE32(REG_SPIC_DR0, cmd); // 40006060 = v6;
	for(i = 0; i < DataPhaseLen; i++) HAL_SPI_WRITE32(REG_SPIC_DR0, pdes[i]); // 40006060 = v9;
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
	SpicWaitBusyDoneRtl8195A();
	if (SpicInitParaAllClk[0][0].flashtype == 4)
		SpicWaitOperationDoneRtl8195A(SpicInitParaa);
	else
		SpicWaitWipDoneRefinedRtl8195A(SpicInitParaa);
}

//----- SpicGetFlashFlagRtl8195A
u8 HAL_FLASH_TEXT_SECTION SpicGetFlashFlagRtl8195A(SPIC_INIT_PARA SpicInitPara) {
	u8 result;
	SPIC_INIT_PARA SpicInitParaa = SpicInitPara;
	DBG_SPIF_INFO("%s(0x%x)\n", "SpicGetFlashFlagRtl8195A", ((u32 *)SpicInitPara)[0]);
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_CTRL_NDF(1)); //40006004 = 1;
	SpicRxCmdRefinedRtl8195A(FLASH_CMD_RFSR, SpicInitParaa);
	result = HAL_SPI_READ32(REG_SPIC_DR0); // 40006060; // HAL_SPI_WRITE32(REG_SPIC_DR0, v6); // 40006060 =
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	if (HAL_SPI_READ32(REG_SPIC_DR0) & 2) { // 40006060 & 2 )
		if (HAL_SPI_READ32(REG_SPIC_DR0) & 0x80) // SRWD (status register write protect)
			DBG_SPIF_WARN("Attempts to Program / Erase Protected Area.\n");
		*(u32 *) &SpicInitParaa = *(u32 *) &SpicInitParaa.id[0]; // ???
		SpicTxCmdWithDataRtl8195A(FLASH_CMD_CFSR,  0, 0, SpicInitParaa);
	}
	return result;
}

//----- SpicWaitOperationDoneRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicWaitOperationDoneRtl8195A(IN SPIC_INIT_PARA SpicInitPara)
//void  SpicWaitOperationDoneRtl8195A(SPIC_INIT_PARA SpicInitPara, int a2)
{
	SPIC_INIT_PARA SpicInitParaa = SpicInitPara;
	DBG_SPIF_INFO("%s(0x%x)\n", "SpicWaitOperationDoneRtl8195A",((u32 *)SpicInitPara)[0]);
	while (!(SpicGetFlashFlagRtl8195A(SpicInitParaa) & 0x80));
}

//----- SpicDeepPowerDownFlashRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicDeepPowerDownFlashRtl8195A(SPIC_INIT_PARA SpicInitPara)
{
	if ((HAL_SYS_CTRL_READ32(REG_SYS_EFUSE_SYSCFG6) & BIT_SYS_CPU_CLK_SEL(2)) // v40000038 & 0x2000000
	&& (((HAL_SYS_CTRL_READ32(REG_SYS_SYSTEM_CFG0) & 0xff) >> 4) > 1)) { // (u8) (400001F0 >> 4) > 1)
		HAL_SYS_CTRL_WRITE32(REG_CPU_PERIPHERAL_CTRL, HAL_SYS_CTRL_READ32(REG_CPU_PERIPHERAL_CTRL) | BIT_SPI_FLSH_PIN_EN); // 400002C0 |= 1u;
		SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
		DBG_MISC_ERR("Deep power down\n");
		HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
				HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_ADDR_CH(3)|BIT_DATA_CH(3)|BIT_CTRL_TMOD(3)))); // SpicOneBitMode: 40006000 &= 0xFFF0FCFF;
		HAL_SPI_WRITE32(REG_SPIC_DR0, 0xFFFFFFB9); // 40006060 = -71;
		HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
		HalDelayUs(20);
	}
}

//----- SpicUserProgramRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicUserProgramRtl8195A(IN u8 * data, IN SPIC_INIT_PARA SpicInitPara,
		IN u32 addr, IN u32 * LengthInfo)
// void  SpicUserProgramRtl8195A(uint8_t *data@<R0>, uint32_t addr@<R3>, u32 a3@<R1>, u32 a4@<R2>, SPIC_INIT_PARA SpicInitPara, uint32_t *LengthInfo)
{
	uint8_t *v6; // r5@1
	int v7; // r2@1
	u32 v8; // r1@1
	int v9; // r2@4
	uint32_t v10; // r10@10
	uint32_t v11; // r0@11
	int v12; // r1@11
	char *v13; // r0@11
	u32 v14; // r4@11
	char v15; // t1@12
	int v16; // r11@12
	u8 v17; // r1@18
	uint32_t v18; // r2@19
	int v19; // r0@21
	int v20; // r11@21
	int v21; // r9@27
	int v22; // r2@31
	int v23; // r1@31
	int v24; // r3@32
	int v25; // r3@34
	u32 v26; // r0@34
	int v27; // r2@39
	int v28; // r1@41
	int v29; // r4@41
	int v30; // t1@43
	int v31; // r0@44
//	int v32; // r2@46
	SPIC_INIT_PARA SpicInitParab; // [sp+0h] [bp-40h]@0
	SPIC_INIT_PARA SpicInitParaa; // [sp+8h] [bp-38h]@1
	uint32_t writeword; // [sp+10h] [bp-30h]@1
	uint32_t lastwriteword; // [sp+14h] [bp-2Ch]@1

	SpicInitParaa = SpicInitPara;
	v6 = data;
	writeword = 0;
	lastwriteword = 0;
	v7 = (u8) addr;
	v8 = **(u32 **) &SpicInitPara;
	if ((u8) addr) {
		if (v8 <= 0xFF && v8 + v7 <= 0xFF)
			v9 = **(u32 **) &SpicInitPara;
		else
			v9 = 256 - v7;
	} else if (v8 >= 0x100) {
		v9 = 256;
	} else {
		v9 = **(u32 **) &SpicInitPara;
	}
	v10 = addr & 3;
	**(u32 **) &SpicInitPara = v8 - v9;
	if (addr & 3) {
		v11 = addr & 3;
		addr -= v10;
		writeword = *(u32 *) (addr - 0x68000000);
		v10 = 4 - v10;
		v12 = (int) v6;
		v13 = (char *) &writeword + v11 - 1;
		v14 = v9;
		do {
			v15 = *(u8 *) v12++;
			--v14;
			(v13++)[1] = v15;
			v16 = v12;
		} while (v14 && (u8) (v9 - v14) < v10);
	} else {
		if ((u32) v9 > 3)
			writeword = (data[2] << 16) | (data[1] << 8) | *data
					| (data[3] << 24);
		v16 = (int) data;
		v14 = v9;
	}
	v17 = v14 & 3;
	if (v14 & 3) {
		v17 = (u8) v17;
		v18 = addr + v14 - 0x68000000;
		if (v10)
			v18 = addr + v14 - 0x67FFFFFC;
		v19 = 0;
		lastwriteword = *(u32 *) (v18 - v17);
		v20 = v16 + v14 - v17;
		while ((u8) v19 < v17) {
			*((u8 *) &lastwriteword + v19) = *(u8 *) (v20 + v19);
			++v19;
		}
		v16 = v20 + v17;
		if (!v10 && v14 <= 3) {
			v14 = 0;
			writeword = lastwriteword;
		}
	}
	v21 = HAL_SPI_READ32(REG_SPIC_ADDR_LENGTH); // 40006118
	*(u32 *) &SpicInitParab.BaudRate = *(u32 *) &SpicInitParaa.id[0];
	SpicTxCmdWithDataRtl8195A(FLASH_CMD_WREN, 0, 0, *(u32 *) &SpicInitParaa, SpicInitParab);
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
			HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_CTRL_TMOD(3)))); // 40006000 &= 0xFFFFFCFF;
	HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, BIT_CTRL_ADDR_PHASE_LENGTH(3)); // 40006118 = 3;
	HAL_SPI_WRITE32(REG_SPIC_DR0, writeword); // 40006060 = writeword;
	if (v10) v16 = (int) &v6[v10];
	else if (v14 > 3) {
		v16 = (int) (v6 + 4);
		v14 -= 4;
	}
	v22 = v16;
	v23 = v16 + 4;
	while (1) {
		v24 = v22;
		if (v14 <= 4)
			break;
		v14 -= 4;
		if (v22 & 3) {
			v25 = (*(u8 *) (v22 + 2) << 16) | (*(u8 *) (v22 + 1) << 8)
					| *(u8 *) (v23 - 4) | (*(u8 *) (v22 + 3) << 24);
			writeword = v25;
			v26 = v14;
			HAL_SPI_WRITE32(REG_SPIC_DR0, v25); // 40006060 = v25;
			v24 = v23;
		} else {
			v24 = v23;
			HAL_SPI_WRITE32(REG_SPIC_DR0, *(u32 *)(v23 - 4)); //40006060 = *(u32 *)(v23 - 4);
			v26 = v14;
		}
		v22 += 4;
		v23 += 4;
		if (v22 == v16 + 24)
			goto LABEL_39;
	}
	v26 = v14;
LABEL_39:
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
	v27 = v24 + v26;
	if (v24 & 3) {
		while ((u32) (v27 - v24) > 3) {
			v28 = (*(u8 *) (v24 + 2) << 16) | (*(u8 *) (v24 + 1) << 8);
			v29 = *(u8 *) v24;
			v24 += 4;
			writeword = v28 | v29 | (*(u8 *) (v24 - 1) << 24);
			HAL_SPI_WRITE32(REG_SPIC_DR0, writeword); // 40006060 = writeword;
		}
	} else {
		while ((u32) (v27 - v24) > 3) {
			v30 = *(u32 *) v24;
			v24 += 4;
			HAL_SPI_WRITE32(REG_SPIC_DR0, v30); // 40006060 = v30;
		}
	}
	v31 = v26 & 3;
	if (v31)
		HAL_SPI_WRITE32(REG_SPIC_DR0, lastwriteword); // 40006060 = lastwriteword;
	SpicWaitBusyDoneRtl8195A(v31);
	if (SpicInitParaa.flashtype == 4)
		SpicWaitOperationDoneRtl8195A(SpicInitParaa);
	else
		SpicWaitWipDoneRefinedRtl8195A(SpicInitParaa);
	HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0;
	HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, v21); // 40006118 = v21;
}

//----- SpicWaitWipRtl8195A
int HAL_FLASH_TEXT_SECTION SpicWaitWipRtl8195A(VOID)
//int  SpicWaitWipRtl8195A(SPIC_INIT_PARA SpicInitPara)
{
	SPIC_INIT_PARA SpicInitPara;
	SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
	return 1;
}

//----- SpicTxFlashInstRtl8195A
void HAL_FLASH_TEXT_SECTION SpicTxFlashInstRtl8195A(uint8_t cmd, int DataPhaseLen, uint8_t *pData) {
	uint8_t v3; // r8@1
	int v4; // r7@1
	uint8_t *v5; // r6@1
	int v6; // r2@5
	SPIC_INIT_PARA v7; // [sp+0h] [bp-28h]@0
	SPIC_INIT_PARA SpicInitPara; // [sp+8h] [bp-20h]@0

	v3 = cmd;
	v4 = DataPhaseLen;
	v5 = pData;
	SpicWaitWipDoneRefinedRtl8195A(SpicInitPara, (int) pData);
	DBG_MISC_ERR();
	while (1) {
		*(u32 *) &v7.BaudRate = *(u32 *) &SpicInitPara.id[0];
		if (SpicGetFlashStatusRefinedRtl8195A(SpicInitPara) & 2)
			break;
		SpicTxCmdWithDataRtl8195A(FLASH_CMD_WREN, 0, 0, *(u32 *) &SpicInitPara, v7);
	}
	SpicTxCmdWithDataRtl8195A(cmd, v4, v5, *(u32 *) &SpicInitPara, v7);
	if (SpicInitParaAllClk[0][0].flashtype == 4) {
		SpicWaitOperationDoneRtl8195A(SpicInitPara, v6);
	} else if (SpicInitParaAllClk[0][0].flashtype == 5) {
		SpicWaitWipDoneRefinedRtl8195A(SpicInitPara, v6);
	} else {
		while (SpicGetFlashStatusRefinedRtl8195A(SpicInitPara) & 2)	;
	}
}

//----- SpicEraseFlashRefinedRtl8195A
void HAL_FLASH_TEXT_SECTION SpicEraseFlashRefinedRtl8195A(void) {
	SpicTxFlashInstRtl8195A(FLASH_CMD_CE, 0, 0);
}

//----- SpicDieEraseFlashRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicDieEraseFlashRtl8195A(IN u32 Address)
{
	u8 Addr[4];
	Addr[0] = Address >> 16;
	Addr[1] = Address >> 8;
	Addr[2] = Address;
	SpicTxFlashInstRtl8195A(FLASH_CMD_DE, 3, (uint8_t *) &Addr);
}

//----- SpicBlockEraseFlashRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicBlockEraseFlashRtl8195A(IN u32 Address)
{
	u8 Addr[4];
	Addr[0] = Address >> 16;
	Addr[1] = Address >> 8;
	Addr[2] = Address;
	SpicTxFlashInstRtl8195A(FLASH_CMD_BE, 3, (uint8_t *) &Addr);
}

//----- SpicSectorEraseFlashRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicSectorEraseFlashRtl8195A(IN u32 Address)
//void  SpicSectorEraseFlashRtl8195A(uint32_t Address, int a2, int a3)
{
	u8 Addr[4];
	Addr[0] = Address >> 16;
	Addr[1] = Address >> 8;
	Addr[2] = Address;
	SpicTxFlashInstRtl8195A(FLASH_CMD_SE, 3, (uint8_t *) &Addr);
}

//----- SpicSetExtendAddrRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicSetExtendAddrRtl8195A(IN u32 data, IN SPIC_INIT_PARA SpicInitPara)
{
	uint32_t dataa = data;
	SpicTxFlashInstRtl8195A(0xC5u, 1, (uint8_t *) &dataa);
}

//----- SpicSetFlashStatusRefinedRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicSetFlashStatusRefinedRtl8195A(IN u32 data,
		IN SPIC_INIT_PARA SpicInitPara)
{
	int DataPhaseLen;
	u32 Buf = data;
	if (SpicInitParaAllClk[0][0].flashtype - 2 > 1)
		DataPhaseLen = 1;
	else
		DataPhaseLen = 2;
	SpicTxFlashInstRtl8195A(FLASH_CMD_WRSR, DataPhaseLen, (uint8_t *) &Buf);
}

//----- SpicWriteProtectFlashRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicWriteProtectFlashRtl8195A(IN u32 Protect)
//void  SpicWriteProtectFlashRtl8195A(__int64 Protect)
{
	SPIC_INIT_PARA SpicInitPara;
	u32 x;
	u8 v2 = SpicGetFlashStatusRefinedRtl8195A(Protect, SpicInitPara);
	if (Protect)
		x = v2 | 0x1C;
	else
		x = v2 & 0xE3;
	SpicSetFlashStatusRefinedRtl8195A(x, SpicInitPara);
}

//----- SpicDisableRtl8195A
VOID HAL_FLASH_TEXT_SECTION SpicDisableRtl8195A(VOID) {
	HAL_SYS_CTRL_WRITE32(REG_CPU_PERIPHERAL_CTRL, HAL_SYS_CTRL_READ32(REG_CPU_PERIPHERAL_CTRL) & ~BIT_SPI_FLSH_PIN_EN); // 400002C0 &= 0xFFFFFFFE;
}

//----- SpicNVMCalLoad
VOID SpicNVMCalLoad(u8 BitMode, u8 CpuClk)
//void  SpicNVMCalLoad(int BitMode, int CpuClk)
{
	int v2; // r4@1
	int v3; // r8@1
	int v4; // r11@1
	int v5; // r7@1
	int v6; // r6@1
	int v7; // r2@4
	int v8; // r1@4
	int v9; // r2@6
	char *v10; // r4@6
	uint32_t spci_para; // [sp+Ch] [bp-2Ch]@4

	v2 = CpuClk + 6 * BitMode;
	v3 = 0x40006120;
	v4 = 0x40006014;
	v5 = BitMode;
	v6 = CpuClk;
	if (0x40006014 == 1) {
		BitMode = HalGetCpuClk(BitMode, CpuClk);
		if (BitMode == 166666666) {
			40006120 |= 0x202u;
			HAL_SPI_WRITE32(REG_SPIC_SSIENR, v4); // 40006008 = v4;
			BitMode = SpicWaitBusyDoneRtl8195A(166666666);
		}
	}
	v7 = *(u32 *) (8 * v2 - 0x67FF6F80);
	v8 = v7 + 1;
	spci_para = *(u32 *) (8 * v2 - 0x67FF6F80);
	if (v7 != -1) {
		if ((*(u32 *) (8 * v2 - 0x67FF6F7C) ^ v7) == -1) {
			v9 = v6 + 6 * v5;
			v10 = (char *) SpicInitParaAllClk + 8 * v9;
			SpicInitParaAllClk[0][v9].BaudRate = spci_para;
			v10[1] = BYTE1(spci_para);
			v10[2] = BYTE2(spci_para);
			v10[3] = BYTE3(spci_para);
			v8 = ConfigDebugInfo;
			BitMode = ConfigDebugInfo << 19;
			DBG_SPIF_INFO("SpicNVMCalLoad: Calibration Loaded(BitMode %d, CPUClk %d): BaudRate=0x%x RdDummyCyle=0x%x DelayLine=0x%x\r\n",
					v5, v6, SpicInitParaAllClk[0][v9].BaudRate, (u8)v10[1],
					(u8)v10[2]);
		} else {
			v8 = ConfigDebugWarn << 19;
			DBG_SPIF_WARN("SpicNVMCalLoad: Data in Flash(@ 0x%x = 0x%x 0x%x) is Invalid\r\n",
					8 * v2 + 36992);
		}
	}
	if (40006014 == 1 && HalGetCpuClk(BitMode, v8) == 166666666) {
		40006120 = v3;
		HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
	}
}

//----- SpicNVMCalLoadAll
VOID HAL_FLASH_TEXT_SECTION SpicNVMCalLoadAll(void) {
	int v0; // r5@1
	int v1; // r4@2
	int v2; // r1@3

	v0 = 0;
	do {
		v1 = 0;
		do {
			v2 = (u8) v1++;
			SpicNVMCalLoad(v0, v2);
		} while (v1 != 6);
		v0 = (u8) (v0 + 1);
	} while (v0 != 3);
}

//----- SpicNVMCalStore
VOID HAL_FLASH_TEXT_SECTION SpicNVMCalStore(u8 BitMode, u8 CpuClk)
//void  SpicNVMCalStore(int BitMode, int CpuClk, int a3)
{
	int v3; // r6@1
	int v4; // r5@1
	int v5; // r4@5
	int v6; // r8@5
	int v7; // r2@9
	int v8; // r3@16
	int v9; // r3@19
	int spci_para; // [sp+Ch] [bp-34h]@6
	SPIC_INIT_PARA SpicInitPara; // [sp+10h] [bp-30h]@0

	v3 = BitMode;
	v4 = CpuClk;
	if (!SpicInitParaAllClk[0][CpuClk + 6 * BitMode].id[0])
		SpicReadIDRtl8195A(BitMode, CpuClk, a3);
	DBG_SPIF_INFO("SpicNVMCalStore==> BitMode=%d CpuClk=%d\r\n", v3,
			v4);
	v5 = v4 + 6 * v3;
	v6 = v5;
	if (*(u32 *) (8 * v5 + 0x98009080) == -1) {
		LOBYTE (spci_para) = SpicInitParaAllClk[0][v5].BaudRate;
		BYTE1 (spci_para) = SpicInitParaAllClk[0][v6].RdDummyCyle;
		BYTE2 (spci_para) = SpicInitParaAllClk[0][v6].DelayLine;
		BYTE3 (spci_para) = SpicInitParaAllClk[0][v6]._anon_0.Rsvd;
		*(u32 *) (8 * v5 + 0x98009080) = spci_para;
		if (SpicInitParaAllClk[0][v6].flashtype == 4)
			SpicWaitOperationDoneRtl8195A(SpicInitPara, spci_para);
		else
			SpicWaitWipDoneRefinedRtl8195A(SpicInitPara, spci_para);
		*(u32 *) (8 * v5 + 0x98009084) = ~spci_para;
		v7 = SpicInitParaAllClk[0][v4 + 6 * v3].flashtype;
		if (v7 == 4)
			SpicWaitOperationDoneRtl8195A(SpicInitPara, 4);
		else
			SpicWaitWipDoneRefinedRtl8195A(SpicInitPara, v7);
			DBG_SPIF_INFO("SpicNVMCalStore(BitMode %d, CPUClk %d): Calibration Stored: BaudRate=0x%x RdDummyCyle=0x%x DelayLine=0x%x\r\n",
				v3, v4, SpicInitParaAllClk[0][v4 + 6 * v3].BaudRate,
				SpicInitParaAllClk[0][v4 + 6 * v3].RdDummyCyle,
				SpicInitParaAllClk[0][v4 + 6 * v3].DelayLine);
		if (*(u32 *) (8 * v5 + 0x98009080) != spci_para) {
			v8 = *(u32 *) (8 * v5 + 0x98009080);
			DBG_SPIF_ERR("SpicNVMCalStore Err(Offset=0x%x), Wr=0x%x Rd=0x%x \r\n",
					8 * v5);
		}
		if (*(u32 *) (8 * v5 + 0x98009084) != ~spci_para) {
				v9 = *(u32 *) (8 * v5 + 0x98009084);
				DBG_SPIF_ERR("SpicNVMCalStore Err(Offset=0x%x), Wr=0x%x Rd=0x%x \r\n",
						v6 * 8 + 4);
		}
	} else DBG_SPIF_ERR("SpicNVMCalStore: The flash memory(@0x%x = 0x%x) is not able to be write, Erase it first!!\r\n",
				v6 * 8 + 0x9080);
}

//----- SpicCalibrationRtl8195A
int HAL_FLASH_TEXT_SECTION SpicCalibrationRtl8195A(int SpicBitMode, uint32_t DefRdDummyCycle) {
	u32 Delay_start; // v6; // r4@8
	int v7; // r12@8
	int RdDummyCyle; // int v8; // r10@8
	u32 Delay_end; // signed int v9; // r8@8
	int xbaud; // signed int v10; // r7@8
	int v11; // r2@12
	int v12; // r9@12
	int v13; // r5@14
	int v14; // r6@14
	int v15; // r3@14
	u8 DelayLine; // v21; // r4@40
	u16 Baud; // int v25; // [sp+18h] [bp-40h]@8
	int BitMode; // [sp+1Ch] [bp-3Ch]@1
	int v28; // [sp+24h] [bp-34h]@12
	SPIC_INIT_PARA SpicInitPara; // [sp+28h] [bp-30h]@0
	char v30; // [sp+30h] [bp-28h]@6
	u8 CpuClkMode = (HAL_PERI_ON_READ32(REG_SYS_CLK_CTRL1)
	  			>> BIT_SHIFT_PESOC_OCP_CPU_CK_SEL) & BIT_MASK_PESOC_OCP_CPU_CK_SEL; //  v3 = (40000014 >> 4) & 7;
	BitMode = SpicBitMode;

	// v3 = DefRdDummyCycle;

	PSPIC_INIT_PARA pspic_para = &SpicInitParaAllClk[SpicBitMode][CpuClkMode];

	if (!pspic_para->Rsvd)	SpicNVMCalLoad(SpicBitMode, CpuClkMode);
//	v4 = CpuClkMode + 6 * BitMode;
//	v5 = (char *) SpicInitParaAllClk + 8 * v4;

	if (!pspic_para->DelayLine) {
		HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(0)); // 40006008 = 0
		HAL_SPI_WRITE32(REG_SPIC_BAUDR, pspic_para->BaudRate); // 40006014 = (u8)v5[0]
		HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH,
				(HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH)
		  		& (~((BIT_MASK_RD_DUMMY_LENGTH) << BIT_SHIFT_RD_DUMMY_LENGTH)))
				| pspic_para->RdDummyCyle); // 4000611C = (4000611C >> 16 << 16) | (u8)v5[1],
		HAL_PERI_ON_WRITE32(REG_PESOC_MEM_CTRL,
				(HAL_PERI_ON_READ32(REG_PESOC_MEM_CTRL) & BIT_MASK_PESOC_FLASH_DDL_CTRL << BIT_SHIFT_PESOC_FLASH_DDL_CTRL)
				| pspic_para->DelayLine); // 40000300 = 40000300 & 0xFFFFFF00 | (u8)v5[2]
		HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1));// 40006008 = 1,
		SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
	}
	if(!SpicCmpDataForCalibrationRtl8195A()) {
		DBG_MISC_ERR("SPI calibration\n");
		Delay_start = 0;
		v7 = 2 * DefRdDummyCycle;
		Baud = 0;
		RdDummyCyle = 0;
		Delay_end = 0;
		xbaud = 1;
		while ( 1 )
		{
			if ((xbaud == 1) && (!BitMode) && (!CpuClkMode))
			goto LABEL_35;
			v11 = 0;

			HAL_SPI_WRITE32(REG_SPIC_BAUDR, xbaud); // 40006014 = v10;
			HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
			v28 = v7 + 20;
			v12 = v7;
LABEL_13:
			if ( v12 < (u32)v28 ) break;
			if ( v11 ) {
				DBG_MISC_ERR("Find the avaiable window\n");
				DBG_MISC_ERR("Baud:%x; auto_length:%x; Delay start:%x; Delay end:%x\n", Baud, RdDummyCyle, Delay_start, Delay_end);
				HAL_SPI_WRITE32(REG_SPIC_BAUDR, Baud & 0xFFF); //40006014 = v25 & 0xFFF;
				pspic_para->BaudRate = Baud;
//				v20 = (char *)SpicInitParaAllClk + 8 * (CpuClkMode + 6 * BitMode);
				HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH,
						(HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH)
				  		& (~((BIT_MASK_RD_DUMMY_LENGTH) << BIT_SHIFT_RD_DUMMY_LENGTH)))
						| RdDummyCyle); // 4000611C = (4000611C >> 16 << 16) | v8
				pspic_para->RdDummyCyle = RdDummyCyle;
				pspic_para->DelayLine = (u32)(Delay_start + Delay_end) >> 1;
				HAL_PERI_ON_WRITE32(REG_PESOC_MEM_CTRL,
						(HAL_PERI_ON_READ32(REG_PESOC_MEM_CTRL) & BIT_MASK_PESOC_FLASH_DDL_CTRL << BIT_SHIFT_PESOC_FLASH_DDL_CTRL)
						| pspic_para->DelayLine); // 40000300 = 40000300 & 0xFFFFFF00 | DelayLine
				pspic_para->Rsvd = 1;
				HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
				SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
				SpicNVMCalStore(BitMode, CpuClkMode);
				return 1;
			}
LABEL_35:
			if (++xbaud == 5)	return 0;
			v7 += 2 * DefRdDummyCycle;
		}
		v13 = 0;
		HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH,
				(HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH)
		  		& (~((BIT_MASK_RD_DUMMY_LENGTH) << BIT_SHIFT_RD_DUMMY_LENGTH)))
				| v12); // 4000611C = (4000611C >> 16 << 16) | v12,
		HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_CTRL_SPIC_EN(1)); // 40006008 = 1;
		v14 = 0;
		v15 = 99;
		while ( 1 )
		{
			HAL_PERI_ON_WRITE32(REG_PESOC_MEM_CTRL,
					(HAL_PERI_ON_READ32(REG_PESOC_MEM_CTRL) & BIT_MASK_PESOC_FLASH_DDL_CTRL << BIT_SHIFT_PESOC_FLASH_DDL_CTRL)
					| v13); // 40000300 = 40000300 & 0xFFFFFF00 | v13
			if ( !SpicCmpDataForCalibrationRtl8195A() )
			{
				if ((v14 == 1) && ((v11 == 1) || ((v13 - v15) > (u32)(Delay_end - Delay_start))))
				{
					Baud = xbaud;
					RdDummyCyle = v12;
					Delay_end = v13;
					Delay_start = v15;
				}
				v14 = 0;
				goto LABEL_30;
			}
			if ( !v14 )
			{
				v11++;
				v15 = v13;
			}
			if ( v13 == 99 )
			{
				if ( v11 == 1 )
				{
					Baud = xbaud;
LABEL_23:
					RdDummyCyle = v12;
					Delay_end = 99;
					Delay_start = v15;
					goto LABEL_28;
				}
				if ( 99 - v15 > (u32)(Delay_end - Delay_start) )
				{
					Baud = xbaud;
					goto LABEL_23;
				}
			}
LABEL_28:
			v14 = 1;
LABEL_30:
			++v13;
			if ( v13 == 100 )
			{
				++v12;
				goto LABEL_13;
			}
		}
	}
	return 1;
}

//----- SpicFlashInitRtl8195A
extern BOOLEAN HAL_FLASH_TEXT_SECTION SpicFlashInitRtl8195A(u8 SpicBitMode)
{
	SPIC_INIT_PARA SpicInitPara;
	u8 DefRdDummyCycle;
	bool result;
	*(u32 *) &SpicInitPara.BaudRate = 0; // = SpicBitMode ?
#if defined(E_CUT_ROM_DOMAIN) || (!defined(CONFIG_RELEASE_BUILD_LIBRARIES))
	*(u32 *) &SpicInitPara.id[0] = 0;
#endif
	// v2 = SpicBitMode;
	if (!SpicInitParaAllClk[0][0].id[0]) SpicReadIDRtl8195A(SpicInitPara);
	SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
	switch(SpicBitMode) {
	case SpicOneBitMode:
		SpicConfigAutoModeRtl8195A(SpicOneBitMode);
		DefRdDummyCycle = 0;
		break;
	case SpicDualBitMode:
		SpicConfigAutoModeRtl8195A(SpicDualBitMode);
		DefRdDummyCycle = 8;
		break;
	case SpicQuadBitMode:
		SpicConfigAutoModeRtl8195A(SpicQuadBitMode);
		if (SpicInitParaAllClk[0][0].flashtype == FLASH_EON)
			DefRdDummyCycle = 6;
		else
			DefRdDummyCycle = 8;
		break;
	default:
		DBG_MISC_ERR("No Support SPI Mode!\n");
		SpicConfigAutoModeRtl8195A(SpicOneBitMode);
		DefRdDummyCycle = 0;
	}
	if (SpicCalibrationRtl8195A(SpicBitMode, DefRdDummyCycle)) {
		result = 1;
	} else {
		DBG_MISC_ERR("SPI calibration fail and recover one bit mode\n");
		SpicLoadInitParaFromClockRtl8195A(0, 0, &SpicInitPara);
		SpicInitRefinedRtl8195A(SpicInitPara.BaudRate, SpicOneBitMode);
		SpicConfigAutoModeRtl8195A(SpicOneBitMode);
		result = 0;
	}
	return result;
}

//----- SpicOneBitCalibrationRtl8195A
u32 HAL_FLASH_TEXT_SECTION SpicOneBitCalibrationRtl8195A(IN u8 SysCpuClk)
{
	SpicConfigAutoModeRtl8195A(SpicOneBitMode);
	u32 result = SpicCalibrationRtl8195A(SpicOneBitMode, 0);
	if (result)	result = 1;
	return result;
}


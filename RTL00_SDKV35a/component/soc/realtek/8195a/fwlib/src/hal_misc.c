/*
 * hal_misc.c
 *
 *  Created on: 08/10/2016
 *      Author: pvvx
*/
#include "rtl8195a.h"

//-------------------------------------------------------------------------
// Function declarations
/*
void HalReInitPlatformTimer(void);
void HalSetResetCause(IN HAL_RESET_REASON reason);
HAL_RESET_REASON HalGetResetCause(void);
*/
// void HalTimerOpInit_Patch(IN void *Data); // in hal_timer.h 
//-------------------------------------------------------------------------
// Data declarations
// extern HAL_TIMER_OP HalTimerOp; // This variable declared in ROM code (in hal_timer.h )


//----- HalReInitPlatformTimer
void HalReInitPlatformTimer(void)
{
  TIMER_ADAPTER TimerAdapter;
  HAL_PERI_ON_WRITE32(REG_OSC32K_CTRL, HAL_PERI_ON_READ32(REG_OSC32K_CTRL) | BIT_32K_POW_CKGEN_EN); // 40000270 |= 1
  HAL_PERI_ON_WRITE32(REG_SOC_FUNC_EN, HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) | BIT_SOC_GTIMER_EN); // 40000210 |= 0x10000
  HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL, HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_ACTCK_TIMER_EN); // 40000230 |= 0x4000
  HAL_PERI_ON_WRITE32(REG_PESOC_CLK_CTRL, HAL_PERI_ON_READ32(REG_PESOC_CLK_CTRL) | BIT_SOC_SLPCK_TIMER_EN); // 40000230 |= 0x8000
  HAL_PERI_ON_WRITE32(REG_PON_ISO_CTRL, HAL_PERI_ON_READ32(REG_PON_ISO_CTRL) & (~BIT_ISO_OSC32K_EN)); // 40000204 &= 0xFFFFFFEF
  TimerAdapter.TimerIrqPriority = 0;
  TimerAdapter.TimerLoadValueUs = 0;
  TimerAdapter.TimerMode = FREE_RUN_MODE;
  TimerAdapter.IrqDis = 1;
  TimerAdapter.TimerId = 1;
  HalTimerOpInit_Patch(&HalTimerOp);
  HAL_TIMER_OP x;
  HalTimerOp.HalTimerInit(&TimerAdapter);
  HalTimerOp.HalTimerEn(1);
}

//----- HalSetResetCause
void HalSetResetCause(HAL_RESET_REASON reason)
{
	HAL_PERI_ON_WRITE32(REG_SYS_DSLP_TIM_CTRL, HAL_PERI_ON_READ32(REG_SYS_DSLP_TIM_CTRL) | BIT31); // 40000094 |= 0x80000000
	HAL_PERI_ON_WRITE32(REG_SYS_DSLP_TIM_CTRL, (HAL_PERI_ON_READ32(REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x9700 | (reason & 0xff)); // 40000094 = (40000094 >> 16 << 16) | 0x9700 | (u8)reason;
    while(HAL_PERI_ON_READ32(REG_SYS_DSLP_TIM_CTRL) &  0x8000);
}

//----- HalGetResetCause
HAL_RESET_REASON HalGetResetCause(void)
{
	HAL_PERI_ON_WRITE32(REG_SYS_DSLP_TIM_CTRL, HAL_PERI_ON_READ32(REG_SYS_DSLP_TIM_CTRL) | BIT31); // 40000094 |= 0x80000000
	HAL_PERI_ON_WRITE32(REG_SYS_DSLP_TIM_CTRL, (HAL_PERI_ON_READ32(REG_SYS_DSLP_TIM_CTRL) & 0xFFFF00FF) | 0x8700); // 40000094 = 40000094 & 0xFFFF00FF | 0x8700
	while(HAL_PERI_ON_READ32(REG_SYS_DSLP_TIM_CTRL) &  0x8000);
	return HAL_PERI_ON_READ32(REG_SYS_DSLP_TIM_CTRL);
}



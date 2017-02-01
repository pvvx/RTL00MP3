/*
 * hal_sdio_host.c (disasm hal_sdio_host.o)
 *
 * RTL8710/11 pvvx 12/2016
 */
#include "rtl8195a.h"
#ifdef CONFIG_SDIO_HOST_EN
#include "sd.h"
#include "sdio_host.h"
#include "hal_sdio_host.h"
#include "rtl8195a_sdio_host.h"
#include "hal_pinmux.h"
//#ifdef RTL8710AF
	#include "hal_gpio.h"
	#include "PinNames.h"
	#include "hal_gpio.h"
//#endif
//-------------------------------------------------------------------------
// Function declarations

//-------------------------------------------------------------------------
// Data declarations

//----- HalSdioHostInit(PHAL_SDIO_HOST_ADAPTER)
HAL_Status HalSdioHostInit(IN VOID *Data) {
	PHAL_SDIO_HOST_ADAPTER pSdioHostAdapter = (PHAL_SDIO_HOST_ADAPTER) Data;
	HAL_Status result;
	if (FunctionChk(SDIOH, 0) != 0) {
		if (pSdioHostAdapter != NULL) {
			if (HalSdioHostInitHostRtl8195a(pSdioHostAdapter) != HAL_OK) {
				result = HAL_ERR_HW;
			} else {
				result = HalSdioHostInitCardRtl8195a(pSdioHostAdapter);
				if (result == HAL_OK) {
					REG_POWER_STATE RegPwrState;
					RegPwrState.FuncIdx = SDIOH;
					RegPwrState.PwrState = ACT;
					RegPowerState(RegPwrState);
				}
			}
		} else
			result = HAL_ERR_PARA;
	} else
		result = HAL_ERR_UNKNOWN;
	return result;
}

//----- HalSdioHostEnable(PHAL_SDIO_HOST_ADAPTER)
HAL_Status HalSdioHostEnable(void *Data) {
//	PHAL_SDIO_HOST_ADAPTER pSdioHostAdapter = (PHAL_SDIO_HOST_ADAPTER) Data;
	HAL_Status result = HalSdioHostEnableRtl8195a(Data);
	REG_POWER_STATE RegPwrState;
	RegPwrState.FuncIdx = SDIOH;
	RegPwrState.PwrState = ACT;
	if (result == HAL_OK)
		RegPowerState(RegPwrState);
	return result;
}

//----- HalSdioHostDeInit(PHAL_SDIO_HOST_ADAPTER)
HAL_Status HalSdioHostDeInit(IN VOID *Data) {
	PHAL_SDIO_HOST_ADAPTER pSdioHostAdapter = (PHAL_SDIO_HOST_ADAPTER) Data;
	REG_POWER_STATE SdioHostPwrState;
	u8 HwState;
	HAL_Status result;
	SdioHostPwrState.FuncIdx = SDIOH;
	SdioHostPwrState.PwrState = ACT;

	QueryRegPwrState(SdioHostPwrState.FuncIdx, &SdioHostPwrState.PwrState,
			&HwState);
	if (SdioHostPwrState.PwrState & 0xF7) {
		HalSdioHostEnable(pSdioHostAdapter);
		QueryRegPwrState(SdioHostPwrState.FuncIdx, &SdioHostPwrState.PwrState,
				&HwState);
	}
	if (SdioHostPwrState.PwrState == ACT) {
		SdioHostPwrState.PwrState = INACT;
		RegPowerState(SdioHostPwrState);
	}
	if (pSdioHostAdapter != NULL)
		result = HalSdioHostDeInitRtl8195a(pSdioHostAdapter);
	else
		result = HAL_ERR_PARA;
	return result;
}

//----- HalSdioHostDisable(PHAL_SDIO_HOST_ADAPTER)
HAL_Status HalSdioHostDisable(IN VOID *Data) {
	HAL_Status result = HalSdioHostDisableRtl8195a(Data);
	if (result == HAL_OK) {
		REG_POWER_STATE RegPwrState;
		RegPwrState.FuncIdx = SDIOH;
		RegPwrState.PwrState = SLPCG;
		RegPowerState(RegPwrState); // 0x0441
	}
	return result;
}

//----- HalSdioHostOpInit(PHAL_SDIO_HOST_ADAPTER)
void HalSdioHostOpInit(void *Data) {
	PHAL_SDIO_HOST_OP phsha = (PHAL_SDIO_HOST_ADAPTER)Data;
	phsha->HalSdioHostInitHost = &HalSdioHostInitHostRtl8195a;
	phsha->HalSdioHostInitCard = &HalSdioHostInitCardRtl8195a;
	phsha->HalSdioHostDeInit = &HalSdioHostDeInitRtl8195a;
	phsha->HalSdioHostRegIrq = &HalSdioHostIrqInitRtl8195a;
	phsha->HalSdioHostReadBlocksDma = &HalSdioHostReadBlocksDmaRtl8195a;
	phsha->HalSdioHostWriteBlocksDma = &HalSdioHostWriteBlocksDmaRtl8195a;
	phsha->HalSdioHostStopTransfer = &HalSdioHostStopTransferRtl8195a;
	phsha->HalSdioHostGetCardStatus = &HalSdioHostGetCardStatusRtl8195a;
	phsha->HalSdioHostGetSdStatus = &HalSdioHostGetSdStatusRtl8195a;
	phsha->HalSdioHostChangeSdClock = &HalSdioHostChangeSdClockRtl8195a;
	phsha->HalSdioHostErase = &HalSdioHostEraseRtl8195a;
	phsha->HalSdioHostGetWriteProtect = &HalSdioHostGetWriteProtectRtl8195a;
	phsha->HalSdioHostSetWriteProtect = &HalSdioHostSetWriteProtectRtl8195a;
//#ifdef RTL8710AF
	if(HalGetChipId() != CHIP_ID_8195AM) {
		GPIOState[0] &= ~((1 << 8) - 1);
		{
			for (int i = 0; i <= 6; i++)
				HAL_GPIO_PullCtrl(i, PullNone);
			HAL_GPIO_PullCtrl(PA_6, PullDown);
			HAL_GPIO_PullCtrl(PA_7, PullDown);
		}
	}
//#endif
}

#endif // CONFIG_SDIO_HOST_EN

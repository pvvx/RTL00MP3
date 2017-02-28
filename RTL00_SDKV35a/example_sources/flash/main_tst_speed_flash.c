/*
 *  Test: Speed rd Flash
 */

#include "rtl8195a.h"
//#include "cortex.h"
//#include "rtl8710.h"
//#include "rom_lib.h"
//#include "mask.h"
#include "core_cm3.h"
#include "flash_api.h"

//extern uint8_t __StackTop;

int main(void)
{
	u32 t[10];
    int i = 333333, x = SpicDualBitMode + 1;
    HalPinCtrlRtl8195A(JTAG, 0, 1);
	HalCpuClkConfig(1); // 0 - 166666666 Hz, 1 - 83333333 Hz, 2 - 41666666 Hz, 3 - 20833333 Hz, 4 - 10416666 Hz, 5 - 4000000 Hz
	ConfigDebugErr = -1;
	ConfigDebugInfo = -1;
	ConfigDebugWarn = -1;
	VectorTableInitRtl8195A(0x1FFFFFFC);
	HalInitPlatformLogUartV02();
	HalInitPlatformTimerV02();
	HalShowBuildInfoV02();
	flash_turnon();
	flash_init(&flashobj);
//  HalPinCtrlRtl8195A(SPI_FLASH, 0, 1); //    SPI_FLASH_PIN_FCTRL(ON); // enable spi flash pins
//	SpicLoadInitParaFromClockRtl8195AV02();
//	SpicInitRtl8195AV02(2, SpicDualBitMode); //
//    SpicWaitBusyDoneRtl8195A();
	DiagPrintf("Flash[0]: 0x%08X\r\n", *(volatile u32 *)SPI_FLASH_BASE );
//    HalDelayUs(1000000);
	DiagPrintf("CPU CLK : %d Hz\r\n", HalGetCpuClk());
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	if(!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
		DWT->CYCCNT = 0;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}
	while(x) {
		x--;
		DiagPrintf("<---- Init %d ---->\n", x);
		if (!SpicFlashInitRtl8195A(x)) {// SpicOneBitMode)){

			DiagPrintf("SPI Init Fail!\n"); // DBG_SPIF_ERR?
			HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSTBY_INFO3, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSTBY_INFO3)|0xf);
			while(1);
		}
		t[0] = DWT->CYCCNT;
		DiagPrintf("Test: t0 = %d\r\n", DWT->CYCCNT - t[0]);
		t[0] = DWT->CYCCNT;
		volatile u32 * ptr = (volatile u32 *)SPI_FLASH_BASE+0x4000;
		for(i=0; i < 16384; i++) *ptr++;
		t[1] = DWT->CYCCNT - t[0];
		DiagPrintf("read(): tFlash = %d, clk/byte = %d\r\n", t[1], t[1] >> 16);
		ptr = (volatile u32 *)SPI_FLASH_BASE+0x10000;
		t[0] = DWT->CYCCNT;
		memcpy((u8 *)0x10060000,(u8 *) SPI_FLASH_BASE+0x10000, 65536);
		for(i=0; i < 16384; i++) *ptr++;
		t[2] = DWT->CYCCNT - t[0];
		DiagPrintf("memcpy(): tFlash = %d, clk/byte = %d\r\n", t[2], t[2] >> 16);
		ptr = (volatile u32 *)0x10060000;
		t[0] = DWT->CYCCNT;
		for(i=0; i < 16384; i++) *ptr++;
		t[3] = DWT->CYCCNT - t[0];
		DiagPrintf("Speed rd RAM = %d, clk/byte = %d\r\n", t[3], t[3]>>16);
		ptr = (volatile u32 *)0x1FFF0000;
		t[0] = DWT->CYCCNT;
		for(i=0; i < 16384; i++) *ptr++;
		t[4] = DWT->CYCCNT - t[0];
		DiagPrintf("Speed rd TCM = %d, clk/byte = %d\r\n", t[4], t[4]>>16);
		DiagPrintf("read(): tFlash/tTCM = %d, tFlash/tRAM = %d\r\n", t[1]/t[4], t[1]/t[3]);
		DiagPrintf("memcpy(): tFlash/tTCM = %d, tFlash/tRAM = %d\r\n", t[2]/t[4], t[2]/t[3]);

		t[0] = DWT->CYCCNT;
		SpicUserReadRtl8195A(16384*2, 0, (u8 *)0x10060000, x);
		t[1] = DWT->CYCCNT - t[0];
		DiagPrintf("Spic 1Read to RAM = %d, clk/byte = %d\r\n", t[1], t[1]>>16);
		t[0] = DWT->CYCCNT;
		SpicUserReadFourByteRtl8195A(16384*2, 0, (u32 *)0x10060000, x);
		t[1] = DWT->CYCCNT - t[0];
		DiagPrintf("Spic 4Read to RAM = %d, clk/byte = %d\r\n", t[1], t[1]>>16);

		t[0] = DWT->CYCCNT;
		SpicUserReadRtl8195A(16384*2, 0, (u8 *)0x1FFF0000, x);
		t[1] = DWT->CYCCNT - t[0];
		DiagPrintf("Spic 1Read to TCM = %d, clk/byte = %d\r\n", t[1], t[1]>>16);
		t[0] = DWT->CYCCNT;
		SpicUserReadFourByteRtl8195A(16384*2, 0, (u32 *)0x1FFF0000, x);
		t[1] = DWT->CYCCNT - t[0];
		DiagPrintf("Spic 4Read to TCM = %d, clk/byte = %d\r\n", t[1], t[1]>>16);

	}
	DiagPrintf("Flash[0]: 0x%08X\r\n", *(volatile u32 *)SPI_FLASH_BASE );
	DiagPrintf("End");
	while(1);
}




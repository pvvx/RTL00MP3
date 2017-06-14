
#include "PinNames.h"
#include "bitband_io.h"

volatile u8 * BitBandAddr(void *addr, u8 bit) {
	return (volatile u8 *)(BITBAND_ADDR((u32)addr, bit));
}

volatile u8 * BitBandPeriAddr(void *addr, u8 bit) {
	return (volatile u8 *)(BITBAND_PERI((u32)addr, bit));
}

volatile u8 * GetOutPinBitBandAddr(PinName pin) {
	u32 paddr = NULL;
	u32 ippin = HAL_GPIO_GetIPPinName_8195a(pin);
	if(ippin != 0xff) {
		// paddr = 0x42000000 + (0x40001000 + 0x0c * (ippin >> 5) - 0x40000000) * 32 + ((ippin & 0x1f) * 4);
		paddr =  BitBandPeriAddr((void *)(GPIO_REG_BASE + GPIO_PORTB_DR * (ippin >> 5)),  ippin & 0x1f);
	}
	return paddr;
}

volatile u8 * GetInPinBitBandAddr(PinName pin) {
	volatile u8 * paddr = NULL;
	u32 ippin = HAL_GPIO_GetIPPinName_8195a(pin);
	if(ippin != 0xff) {
		// paddr = 0x42000000 + (0x40001000 + 0x0c * (ippin >> 5) - 0x40000000) * 32 + ((ippin & 0x1f) * 4);
		paddr = BitBandPeriAddr((void *)(GPIO_REG_BASE + GPIO_EXT_PORTA + (ippin >> 5) * 4),  ippin & 0x1f);
	}
	return paddr;
}

volatile u8 * HardSetPin(PinName pin, PinDirection pdir, PinMode pmode, u8 val)
{
	volatile u8 *paddr = NULL;
	u32 ippin = HAL_GPIO_GetIPPinName_8195a(pin);
	if(ippin != 0xff) {
		// paddr = 0x42000000 + (0x40001000 + 0x0c * (ippin >> 5) - 0x40000000) * 32 + ((ippin & 0x1f) * 4);
		paddr = BitBandPeriAddr((void *)(GPIO_REG_BASE + GPIO_PORTB_DR * (ippin >> 5)),  ippin & 0x1f);
	}
	if(paddr && _pHAL_Gpio_Adapter) {
		if (_pHAL_Gpio_Adapter->Gpio_Func_En == 0)  GPIO_FuncOn_8195a();
		paddr[0] = val;						// data register
		paddr[(GPIO_PORTB_DDR - GPIO_PORTB_DR) * 32] = pdir;	// data direction
#if 1 // if use HAL_Gpio_Adapter
		uint32 * p = &_pHAL_Gpio_Adapter->Local_Gpio_Dir[ippin >> 5];
		if(pdir) *p |= 1 << (ippin & 0x1f);
		else *p &= ~(1 << (ippin & 0x1f));
#endif
		paddr[(GPIO_PORTB_CTRL - GPIO_PORTB_DR) * 32] = 0;		// data source control, we should keep it as default: data source from software
		HAL_GPIO_PullCtrl_8195a(pin, pmode);				// set GPIO_PULL_CTRLx
	}
	return paddr;
}


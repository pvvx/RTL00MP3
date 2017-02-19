
/* ENTRY(Reset_Handler) */


MEMORY
{
  ROM  (rx) 		: ORIGIN = 0x00000000, LENGTH = 1M
  SRAM (rwx) 		: ORIGIN = 0x10000000, LENGTH = 448K
  TCM  (rwx)		: ORIGIN = 0x1FFF0000, LENGTH = 65K
  SDRAM_RAM (rwx)	: ORIGIN = 0x30000000, LENGTH = 2M
}

SECTIONS
{
  .rom :
  {
    __rom_image_start__ = .;
    KEEP(*(.rom));
    __rom_image_end__ = .;
    
  } > ROM

  .sram : /* use in rom */
  {
	__ram_image_start__ = .;
	KEEP(*(.ram_dedecated_vector_table)) 	/* 0x10000000: NewVectorTable */
	KEEP(*(.ram_user_define_irq_table)) 	/* 0x10000100: UserIrqFunTable */
	KEEP(*(.ram_user_define_data_table)) 	/* 0x10000200: UserIrqDataTable */
/*	__rom_bss_start__ = .; */
	KEEP(*(.hal.ram.bss)) 					/* 0x10000300: CfgSysDebugWarn .. _pHAL_Gpio_Adapter */
	KEEP(*(.timer2_7_vector_table.data))	/* 0x10000358: Timer2To7VectorTable */
	KEEP(*(.infra.ram.bss)) 				/* 0x10000370: first .. z4 */
	KEEP(*(.mon.ram.bss))					/* 0x10000384: pUartLogCtl .. ArgvArray */
	KEEP(*(.wlan_ram_map)) 					/* 0x100006d4: rom_wlan_ram_map, FalseAlmCnt, ROMInfo, DM_CfoTrack */
	KEEP(*(.ram.rom.wlanmap))				/* align(8) */
	KEEP(*(.libc.ram.bss)) 					/* 0x10000760: rom_libgloss_ram_map __rtl_malloc_av_ __rtl_errno */
/*	__rom_bss_end__ = .; */
/*    __ram_start_table_start__ = .; */
/* 0x10000bc8: bootloader */
	KEEP(*(SORT(.start.ram.data*))) 
/* __image1_validate_code__	= .; */
	KEEP(*(.image1.validate.rodata))
	
    KEEP(*(.infra.ram.data*))
    KEEP(*(.timer.ram.data*))
    KEEP(*(.cutb.ram.data*)) 
    KEEP(*(.cutc.ram.data*)) 
	KEEP(*(.data));
    KEEP(*(.hal.ram.data)) /* 0x10001c60: _reent *rtl_impure_ptr */	/* 0x10001c68:  _reent impure_data */
	KEEP(*(.libc.reent)) 
	KEEP(*(.rom.unc.data))
	KEEP(*(.sdr.rand2.data)) /* Sdr_Rand2() y,z,c */
/*	KEEP(*(.hal.flash.data)) */
/* 0x100020c0: end ram-rom */
    __ram_image_end__ = .;
  } > SRAM
}

SECTIONS
{
	__rom_bss_start__ = 0x10000300; 		/* use in rom */
	__rom_bss_end__ = 0x10000bc8;			/* use in rom */
	__ram_start_table_start__ = 0x10000bc8;	/* use in rom */ 
	__image1_validate_code__ = 0x10000bdc;	/* needed by ram code */ 
	_rtl_impure_ptr = 0x10001c60;			/* for standard library */
}


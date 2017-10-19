
ENTRY(Reset_Handler)
ENTRY(main)

INCLUDE "export-rom_v04.txt"

MEMORY
{
  ROM (rx)  		: ORIGIN = 0x000000,   LENGTH = 1M	    /* end 0x00100000 */
  ROM_USED_RAM (rwx): ORIGIN = 0x10000000, LENGTH = 0x2400	/* end 0x10002400 */
  BOOT_RAM (rwx)	: ORIGIN = 0x10000bc8, LENGTH = 21560	/* end 0x10006000 */
  ROM_HEAP (rwx)	: ORIGIN = 0x10002400, LENGTH = 3K		/* end 0x10003000 */
  RAM_HEAP1 (rwx)	: ORIGIN = 0x10003000, LENGTH = 12K		/* end 0x10006000 */
  BD_RAM (rwx)		: ORIGIN = 0x10006000, LENGTH = 424K	/* end 0x10070000 */
  TCM (rwx)			: ORIGIN = 0x1FFF0000, LENGTH = 64K		/* end 0x20000000 */
  TCM_TAB (rwx)		: ORIGIN = 0x1FFFFD00, LENGTH = 768		/* end 0x20000000 */
  SDRAM_RAM (rwx)	: ORIGIN = 0x30000000, LENGTH = 2M	 	/* end 0x30200000 */
}

EXTERN(RAM_IMG2_VALID_PATTEN)
EXTERN(InfraStart)

SECTIONS
{
	/* 0x00000000: ROM */

	.rom :
	{
		__rom_image_start__ = .;
		KEEP(*(.rom));
		__rom_image_end__ = .;
	} > ROM

	/* 0x10000000: SRAM */
	
	.rom_ram : /* use in rom */
	{
		__ram_image_start__ = .;
		KEEP(*(.ram_dedecated_vector_table)) 	/* 0x10000000: NewVectorTable */
		KEEP(*(.ram_user_define_irq_table)) 	/* 0x10000100: UserIrqFunTable */
		KEEP(*(.ram_user_define_data_table)) 	/* 0x10000200: UserIrqDataTable */
		__rom_bss_start__ = .;
		KEEP(*(.hal.ram.bss)) 					/* 0x10000300: CfgSysDebugWarn .. _pHAL_Gpio_Adapter */
		KEEP(*(.timer2_7_vector_table.data))	/* 0x10000358: Timer2To7VectorTable */
		KEEP(*(.infra.ram.bss)) 				/* 0x10000370: first .. z4 */
		KEEP(*(.mon.ram.bss))					/* 0x10000384: pUartLogCtl .. ArgvArray */
		KEEP(*(.wlan_ram_map)) 					/* 0x100006d4: rom_wlan_ram_map, FalseAlmCnt, ROMInfo, DM_CfoTrack */
		KEEP(*(.ram.rom.wlanmap))				/* align(8) */
		KEEP(*(.libc.ram.bss)) 					/* 0x10000760: rom_libgloss_ram_map __rtl_malloc_av_ __rtl_errno */
		__rom_bss_end__ = .;
	} > ROM_USED_RAM

	/* 0x10000bc8: bootloader */

	.ram_image1.text . : /* use in rom & boot */
	{
		/* __ram_start_table_start__ = .; */
		__ram_image1_text_start__ = .;
		KEEP(*(.boot.start.ram.data*)) 
		/* __image1_validate_code__	= .; */
		KEEP(*(.image1.validate.rodata))
	
	    KEEP(*(.infra.ram.data*))
	    KEEP(*(.timer.ram.data*))
	    KEEP(*(.cutb.ram.data*)) 
	    KEEP(*(.cutc.ram.data*)) 
		KEEP(*(.libc.reent)) 
		KEEP(*(.rom.unc.data)) 
		KEEP(*(.sdr.rand2.data))

	    PROVIDE (__ram_image_end__ = .);		/* 0x100020c0: end */

		/* boot & images data */

	    KEEP(*(.hal.ram.data))
		KEEP(*(.hal.flash.data))
		KEEP(*(.boot.rodata*))
		KEEP(*(.boot.text*))
		KEEP(*(.boot.data*))
	    __image1_bss_start__ = .;
		KEEP(*(.boot.bss*))
	    __image1_bss_end__ = .;
		__ram_image1_text_end__ = .;

	} > BOOT_RAM

	.romheap :
	{
		__rom_heap_start__ = .; 
		end = __rom_heap_start__;
	  	. = ALIGN(0x1000);
		__rom_heap_end__ = .; 
	} > ROM_HEAP

	.ram_heap1 : 
	{
  		__ram_heap1_start__ = .;
		/*   *(.heap1*) */
	} > RAM_HEAP1

	.tcm :
	{
	  	__ram_tcm_start__ = .;
  		__tcm_heap_start__ = .;
	    *(.tcm.heap)
	} > TCM

	.soc_ps_monitor :
	{ 
  		__tcm_heap_end__ = .;
	} > TCM_TAB

	.image2.start.table :
	{
		__ram_heap1_end__ = .;
		__ram_image2_text_start__ = .;
		__image2_entry_func__ = .;
		.image2.start.table1$$Base = .;
		KEEP(*(SORT(.image2.ram.data*)))
		__image2_validate_code__ = .;
		KEEP(*(.image2.validate.rodata*))
		KEEP(*(.custom.validate.rodata*))
	} > BD_RAM
	
	.ram_image2.text :
	{
		*(.infra.ram.start*)
		. = ALIGN(4);
		KEEP(*(.init))

		/* init data */
		. = ALIGN(4);
		PROVIDE (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE (__init_array_end = .);

		. = ALIGN(4);
		KEEP(*(.fini))

		. = ALIGN(4);
		PROVIDE (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE (__fini_array_end = .);

		*(.mon.ram.text*)
		*(.hal.flash.text*)	 
		*(.hal.sdrc.text*)
		*(.hal.gpio.text*)
		*(.fwu.text*)
		*(.otg.rom.text*)
		*(.text*)
		*(.sdram.text*)
		*(.p2p.text*)
		*(.wps.text*)
		*(.websocket.text*)
	} > BD_RAM

	.ram_image2.rodata :
	{
	*(.rodata*)
		*(.fwu.rodata*)
		*(.sdram.rodata*)
		*(.p2p.rodata*)
		*(.wps.rodata*)
		*(.websocket.rodata*)
		. = ALIGN(4);
		xHeapRegions = .;
		LONG(__ram_heap1_start__)
		LONG(__ram_heap1_end__ - __ram_heap1_start__)
		LONG(__ram_heap2_start__)
		LONG(__ram_heap2_end__ - __ram_heap2_start__)
		LONG(__sdram_heap_start__)
		LONG(__sdram_heap_end__ - __sdram_heap_start__)
		LONG(0)
		LONG(0)
		UartLogRamCmdTable = .;
		KEEP(*(SORT(.mon.tab*)))
		KEEP(*(SORT(.sdram.mon.tab*)))
		UartLogRamCmdTable_end = .;
		LONG(0)
	} > BD_RAM
	
	PROVIDE(UartLogRamCmdTableSize = UartLogRamCmdTable_end - UartLogRamCmdTable);

	.ram.data : 
	{
		__data_start__ = .;
		*(.data*)
		*(.p2p.data*)
		*(.wps.data*)
		*(.websocket.data*)
		*(.sdram.data*)
		__data_end__ = .;
		__ram_image2_text_end__ = .;
	} > BD_RAM
	
	.ram.bss :
	{
		__bss_start__ = .;
		.ram.bss$$Base = .;
		*(.hal.flash.data*)
		*(.hal.sdrc.data*)
		*(.hal.gpio.data*)
		*(.fwu.data*)
		*(.bdsram.data*)
		*(.bfsram.data*)
		*(COMMON)
		*(.bss*)
		*(.sdram.bss*)
		*(.p2p.bss*)
		*(.wps.bss*)
		*(.websocket.bss*)
		*(.ssl_ram_map*)
		__bss_end__ = .;
		.ram.bss$$Limit = .;
		
	} > BD_RAM

	.ram_heap2 : 
	{
		. = ALIGN(8);
		__ram_heap2_start__ = .;
		*(.heap*) /* ucHeap */
	} > BD_RAM
	__ram_heap2_end__ = 0x10070000;
	
	.sdr_text :
	{
		__sdram_data_start__ = .;
	} > SDRAM_RAM

	.sdr_rodata :
	{
	} > SDRAM_RAM

	.sdr_data : 
	{
		__sdram_data_end__ = .;
	} > SDRAM_RAM

	.sdr_bss :
	{
		__sdram_bss_start__ = .;
		__sdram_bss_end__ = .;
		. = ALIGN(8);
		__sdram_heap_start__ = .;
	} > SDRAM_RAM
	__sdram_heap_end__ = 0x30200000;

	.boot.head :
	{
		KEEP(*(.loader.head*))
	}
	ASSERT(__ram_image_end__ == 0x100020c0, "Error rom-bios-boot code & data!")
}

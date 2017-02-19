

ENTRY(Reset_Handler)

INCLUDE "export-rom_v04.txt"


MEMORY
{
  ROM (rx)  	: ORIGIN = 0x000000, LENGTH = 1M
  ROM_RAM1 (rwx): ORIGIN = 0x10000000, LENGTH = 0x2100	/* end 0x10002100 */
  BOOT_RAM (rwx): ORIGIN = 0x10000bc8, LENGTH = 13368	/* end 0x10006000 */
  RECY_RAM (rwx): ORIGIN = 0x10002100, LENGTH = 0x3F00	/* end 0x10006000 */
  BD_RAM (rwx)  : ORIGIN = 0x10006000, LENGTH = 424K	/* end 0x10070000 */
  ROM_RAM3 (rwx): ORIGIN = 0x1006D000, LENGTH = 12K		/* end 0x10070000 */
  TCM (rwx) 	: ORIGIN = 0x1FFF0000, LENGTH = 64K		/* end 0x20000000 */
  SDRAM (rwx)  	: ORIGIN = 0x30000000, LENGTH = 2M		/* end 0x30200000 */
}

EXTERN(PreProcessForVendor)
EXTERN(RtlBootToSram)
EXTERN(_rtl_impure_ptr)
EXTERN(impure_data)

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
		/*	__rom_bss_start__ = .; */
		KEEP(*(.hal.ram.bss)) 					/* 0x10000300: CfgSysDebugWarn .. _pHAL_Gpio_Adapter */
		KEEP(*(.timer2_7_vector_table.data))	/* 0x10000358: Timer2To7VectorTable */
		KEEP(*(.infra.ram.bss)) 				/* 0x10000370: first .. z4 */
		KEEP(*(.mon.ram.bss))					/* 0x10000384: pUartLogCtl .. ArgvArray */
		KEEP(*(.wlan_ram_map)) 					/* 0x100006d4: rom_wlan_ram_map, FalseAlmCnt, ROMInfo, DM_CfoTrack */
		KEEP(*(.ram.rom.wlanmap))				/* align(8) */
		KEEP(*(.libc.ram.bss)) 					/* 0x10000760: rom_libgloss_ram_map __rtl_malloc_av_ __rtl_errno */
		/* __rom_bss_end__ = .; */
	} > ROM_RAM1

	/* 0x10000bc8: bootloader */
	.ram_image1.text . : /* use in rom & boot */
	{
		/* __ram_start_table_start__ = .; */
		__ram_image1_text_start__ = .;
		KEEP(*(SORT(.start.ram.data*))) 
		/* __image1_validate_code__	= .; */
		KEEP(*(.image1.validate.rodata))
	
	    KEEP(*(.infra.ram.data*))
	    KEEP(*(.timer.ram.data*))
	    KEEP(*(.cutb.ram.data*)) 
	    KEEP(*(.cutc.ram.data*)) 
		KEEP(*(.data));
	    KEEP(*(.hal.ram.data))
		KEEP(*(.libc.reent)) 
		KEEP(*(.rom.unc.data))
		KEEP(*(.sdr.rand2.data))
		build/obj/project/src/user/rtl_bios_data.o (.rodata*)
	    __ram_image_end__ = .;
		/* 0x100020c0: end */
		build/obj/project/src/user/rtl_boot.o (.text* .rodata*)
	    __image1_bss_start__ = .;
	    __image1_bss_end__ = .;
		__ram_image1_text_end__ = .;
	} > BOOT_RAM

	.tcm :
	{
	    __tcm_start__ = .;
	    *(.tcm.heap)
	    __tcm_end__ = .;
	} > TCM

	.image2.start.table :
	{
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
	    *(.mon.ram.text*)
	    *(.hal.flash.text*)   
	    *(.hal.sdrc.text*)
	    *(.hal.gpio.text*)
	    *(.fwu.text*)
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
	} > BD_RAM

	.ram.data : 
	{
	    __data_start__ = .;
	    *(.data*)
	    *(.sdram.data*)
	    *(.p2p.data*)
	    *(.wps.data*)
	    *(.websocket.data*)
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
	    *(.bss*)
	    *(COMMON)
	    *(.bdsram.data*)
	    *(.sdram.bss*)
	    *(.p2p.bss*)
	    *(.wps.bss*)
	    *(.websocket.bss*)
	    __bss_end__ = .;
	    .ram.bss$$Limit = .;
	} > BD_RAM
	
	.bf_data : 
	{
	    __buffer_data_start__ = .;
	    *(.bfsram.data*)
	    __buffer_data_end__ = .;
	} > BD_RAM

	.bf_data2 : 
	{
	    __buffer_data_start2__ = .;
	    __buffer_data_end2__ = .;
    
	} > RECY_RAM

	.sdr_text :
	{
    	__sdram_data_start__ = .;
	} > SDRAM

	.sdr_rodata :
	{
	} > SDRAM

	.sdr_data : 
	{
		__sdram_data_end__ = .;
	} > SDRAM

	.sdr_bss :
	{
	    __sdram_bss_start__ = .;
	    __sdram_bss_end__ = .;
	} > SDRAM

	.heap :
	{
		__end__ = .;
		end = __end__;
		*(.heap*)
		__HeapLimit = .;
	} > BD_RAM

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy :
	{
		*(.stack)
	} > BD_RAM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(BD_RAM) + LENGTH(BD_RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")

	.boot.head :
	{
		KEEP(*(.loader.head*))
	}

	.image1.head :
	{
		KEEP(*(SORT(.image1.head*)))
	}

	.image2.head :
	{
		KEEP(*(SORT(.image2.head*)))
	}

	.image3.head :
	{
		KEEP(*(SORT(.image3.head*)))
	}

	.image4.head :
	{
		KEEP(*(SORT(.image4.head*)))
	}

}

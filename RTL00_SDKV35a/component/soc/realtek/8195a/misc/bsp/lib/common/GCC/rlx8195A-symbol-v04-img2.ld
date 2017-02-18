

ENTRY(Reset_Handler)

INCLUDE "export-rom_v04.txt"


MEMORY
{
  ROM_USED_RAM (rwx): ORIGIN = 0x10000bc8, LENGTH = 21560	/* end 0x10006000 */
  ROM_HEAP (rwx)	: ORIGIN = 0x10002400, LENGTH = 3K		/* end 0x10003000 */
  RAM_HEAP1 (rwx)	: ORIGIN = 0x10003000, LENGTH = 12K		/* end 0x10006000 */
  BD_RAM (rwx)		: ORIGIN = 0x10006000, LENGTH = 424K	/* end 0x10070000 */
  TCM (rwx)			: ORIGIN = 0x1FFF0000, LENGTH = 64K		/* end 0x20000000 */
  TCM_TAB (rwx)		: ORIGIN = 0x1FFFFD00, LENGTH = 768		/* end 0x20000000 */
  SDRAM_RAM (rwx)	: ORIGIN = 0x30000000, LENGTH = 2M	 	/* end 0x30200000 */
}

EXTERN(RAM_IMG2_VALID_PATTEN)
EXTERN(main)
EXTERN(InfraStart)
EXTERN(gImage2EntryFun0)


SECTIONS
{
  __rom_bss_start__ = 0x10000300;
  __rom_bss_end__ = 0x10000bc8;
  
  .bootloader :
  {
    KEEP(*(.loader.data*))
  } > ROM_USED_RAM

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
  
  OVERLAY 0x1FFF0000:
  {
  	.valid
  	{ 
	  	__ram_tcm_start__ = .;
	    *mem.o (.bss*)
	    *memp.o (.bss*) 
  		__tcm_heap_start__ = .;
	    *(.tcm.heap)
  	}
  	.dummy 
  	{ 
	  	__ram_image1_text_start__ = .;
	    __ram_start_table_start__ = .;
	    KEEP(*(SORT(.start.ram.data*)))
	    __ram_start_table_end__ = .;
		__image1_validate_code__ = .;
	    KEEP(*(.image1.validate.rodata*))
	    KEEP(*(.infra.ram.data*))
	    KEEP(*(.timer.ram.data*))
	    KEEP(*(.cutb.ram.data*))
	    KEEP(*(.cutc.ram.data*))
	    KEEP(*(.hal.ram.data*))
	    __image1_bss_start__ = .;
	    .ram_image1.bss$$Base = .;
    	__image1_bss_end__ = .;
    	.ram_image1.bss$$Limit = .;
    	__ram_image1_data_end__ = .;
	    
	    *(.hal.ram.text*)    
	    *(.infra.ram.text*)
  	}
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
	*(.bfsram.data*)
    *(.sdram.bss*)
    *(.p2p.bss*)
    *(.wps.bss*)
    *(.websocket.bss*)
    __bss_end__ = .;
    .ram.bss$$Limit = .;
    
  } > BD_RAM

  .ram_heap2 : 
  {
    . = ALIGN(8);
  	__ram_heap2_start__ = .;
    KEEP(*(.heap*)) /* ucHeap */
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
}



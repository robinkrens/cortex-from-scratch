/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Linker file for Cortex-M3 STM32 based boards
 * Boards have similar FLASH and SRAM ORIGINs
 * LENGTHs differs of course.
 * 
 * _start flag is the first procedure to be 
 * executed (linked to beginning of FLASH at 
 * 0x08000000). The procedure should do some
 * basic things, such as set up the stack and
 * reset and hard fault handler (see start.asm)
 * *
 * _endofbss flag is used to calculate the end 
 * of .bss and the start of (a possible) kernel
 * heap
 *
 * */

KHEAP_SIZE = 0x100; 

MEMORY
{
	FLASH (xr) : ORIGIN = 0x08000000, LENGTH = 512K
	SRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 64K
} 

ENTRY(_start) 

SECTIONS
{
	. = 0x0;
	.text : ALIGN(4)
	{
		/* (.vector_table */
		*(.text)
		*(.rodata) 
		data_lma = .;
	}
        . = 0x20000000;	
	data_vma = .;
	.data : AT (data_lma)
	{
		*(.data)
	} 
	data_end = .;
	.bss : ALIGN(4) 
	{
		*(.bss)
	}
       	_endofbss = .;	
	_beginofheap = .;
	. = . + KHEAP_SIZE;
	. = ALIGN(8);
	_endofheap = .;
}

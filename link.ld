/* */
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
	}
        . = 0x20000000;	
	.data :  
	{
		*(.data)
	} 
	.bss : ALIGN(4) 
	{
		*(.bss)
	}
       	_endofbss = .;	
}

MEMORY
{
	FLASH (xr) : ORIGIN = 0x00000000, LENGTH = 512K
	SRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 64K
}

ENTRY(_start) 

SECTIONS
{
	.text : ALIGN(4)
	{
		/* (.vector_table */ 
		*(.text)

	} > FLASH
	.data : 
	{
		*(.data)
	} > SRAM
}

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>

/* 
 * Vector table, each entry contains an interrupt
 * service routine:  * 
 *
 * interrupt vector 1-15: processor exceptions
 * interrupt vector 16-32: irq0 - irq ..
 * */

uint32_t ivt[32];


void ivt_set_gate(unsigned char num, void * isr(), short pri) {

	ivt[num] = (uint32_t) isr;
	*NVIC_ISER0 = (1 << ((uint32_t)(num) & 0x1F));
	/* Priorities */
}


/* Dummy interrupt */
void * dummy_isr() {

	uart_puts("EXCEPTION X: SYSTEM HALTED\n");
	for(;;);
}

/* Initialize interrupt vector  */
void ivt_init() {

	/* clear entiry IVT, in SRAM location for SRAM + .data (in .bss section) */
	memset(&ivt, 0, (sizeof(uint32_t) * 87));

	// stack top is loaded from the first entry table on boot/reset
	// don't need to relocate or init this here
	extern void * reset,  * nmi, * hardfault;

	ivt_set_gate(1, dummy_isr, 0);
	ivt_set_gate(2, dummy_isr, 0);
	ivt_set_gate(3, dummy_isr, 0);
	ivt_set_gate(4, dummy_isr, 0);
	ivt_set_gate(5, dummy_isr, 0);
	ivt_set_gate(6, dummy_isr, 0);

	/* the vector table starts at 0x0. Since the address 0x0 point to 
	 * bootcode, it is on ROM or FLASH. The vector table can be
	 * relocated to other memory locations. We can do this by setting 
	 * a register in the NVIC called the vector table offset register */

	*SCB_VTOR = (volatile uint32_t) &ivt; 

}

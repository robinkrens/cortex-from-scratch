#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>

/* TIMER, TODO: move to timer.c */



/* Interrupt vector can contain 16 exceptions and up to 256 interrupts
 * In this code we use 16 exceptions and 36 interrupts  
 * (note: this should be aligned to the vector table size) 
 * Offsets of each entry is:  0x0, 0x80, 0x100, 0x180 .. */

/* interrupt vector 1-15: processor exceptions
 * interrupt vector 16-43: irq0 - irq .. */
uint32_t ivt[43];

/* * base is location of interrupt service request
 * */
void ivt_set_gate(unsigned char num, void * isr(), short pri) {

	ivt[num] = (uint32_t) isr;
	// TODO priority
}

// test Interrupt service routine
void * test_ISR() {
	uart_puts("PING!");
}

/* Initialize interrupt vector  */

void ivt_init() {

	/* clear entiry IVT, in SRAM location for SRAM + .data (in .bss section) */
	memset(&ivt, 0, (sizeof(uint32_t) * 44));

	// stack top is loaded from the first entry table on boot/reset
	// don't need to relocate or init this here

	// copy old vectors 
	extern void * reset,  * nmi, * hardfault;
	//extern uint32_t reset, nmi, hardfault;
	//ivt[1] = &reset;
	//ivt[2] = &nmi;
	//ivt[3] = &hardfault;

	ivt_set_gate(1, test_ISR, 0);
	ivt_set_gate(2, test_ISR, 0);
	ivt_set_gate(3, test_ISR, 0);
	ivt_set_gate(15, test_ISR ,0);

	// enable all interrupts
	
	*SYSCTRL_RCGC1 = *SYSCTRL_RCGC1 | 0x00010000;

	// TODO systimer 
	// ivt[15]
	*NVIC_EN0 = *NVIC_EN0 | 0x00008003;
//	*NVIC_EN1 = (volatile uint32_t) 0xFFFFFFFF; // TODO not all registers

	// priority levels are 0 by default (only executable by kernel)
	
	/* disable all interrupts
	 * MOV R0, #1 ; disable all
	 * MSR PRIMASK, R0 
	 * MOV R0, #0 ; allow all
	 * MSR PRIMASK, R0 */

	
	
	/* relocate the vector table to (S)RAM 
	 * vector table starts at 0x0. since the address 0x0 point to bootcode, it is on ROM or FLASH. 
	 * the value cannot be changed during runtime. however, the vector table can be
	 * relocated to other memory locations in the code or RAM later on
	 *
	 * we can do this by setting a register in the NVIC called
	 * the vector table offset register (address 0xE000ED08).  */


	*NVIC_VECTTBL = (volatile unsigned long) &ivt; 

}

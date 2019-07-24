/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Set up of basic exceptions and interrupts. These interrupts
 * don't do much, except for halting the system. 
 * ivt_set_gate(interrupt nr, function, priority) can be used
 * later to define more appropriate handling. See timer (timer.c) 
 * or serial or (uart.c) handling for non-trivial examples.
 *
 * The actual code is not much, but there are a lot of details
 * to consider. Besides that, in case more control is desired over
 * entering and exiting interrupts (what is pushed on the stack) 
 * A so-called naked function can be used. See below for more
 * details.
 * 
 *
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h>
#include <sys/mmap.h>

#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/regfunc.h>

/* 
 * These values are pushed on the stack just before
 * entering the ISR. Normally, this would require
 * assembly or inline assembly code. I use a so-called
 * 'naked function' __attribute__ ((interrupt)) which
 * gives me a little bit more control over the caller
 *
 * The following register are pushed to the stack
 * in reverse order:
 *
 * */
struct interrupt_frame {

	uint32_t r0; // N-32
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr; // N-4
};

/* 
 * Vector table, each entry contains an interrupt
 * service routine:  
 *
 * interrupt vector 1-15: processor exceptions
 * interrupt vector 16-92: irq0 - irq ..
 *
 * Vector table needs to be aligned in memory.
 * */

uint32_t __attribute__((aligned(0x100))) ivt[92];

/* each message corresponds to each and every exception. 
 * We get the correct message by accessing
 *  exception_message[interrupt_number] 
 *  exception_message[0] is not used (=MSP)*/

char * exception_message(uint8_t intnr) {

	char * messages[] = {
	    "--",
	    "RESET",
	    "NMI",
	    "HARD FAULT",
	    "MEMMANAGE FAULT",
	    "BUS FAULT",
	    "USAGE FAULT",
	    "RESERVED",
	    "SVC",
	    "DEBUG MONITOR",
	    "RESERVED",
	    "RESERVED",
	    "RESERVED",
	    "RESERVED",
	    "PENDSV",
	    "SYSTICK",
	    "IRQ1",
	    "IRQ2",
	    "IRQ3",
	    "IRQ4",
	    // add more if needed
	};

	if (intnr < 20) // TODO: strlen
		return messages[intnr];

	return NULL;
}

void ivt_set_gate(unsigned char num, void * isr(), short pri) {

	ivt[num] = (uint32_t) isr;
	*NVIC_ISER0 = (1 << ((uint32_t)(num) & 0x1F));
	/* Priorities */
}


/* Dummy interrupt: comment out the comment to use a naked f
 * function */

// __attribute__ ((interrupt)) 
void * dummy_isr(/* struct interrupt_frame * frame */) {

	uint8_t nr = *SCB_VTOR_ST & 0xFF;
	
	cputs("EXCEPTION: ");
	cputs(exception_message(nr));
	cputs("\nSYSTEM HALTED\n");
	
	for(;;);
}

/* Initialize interrupt vector  */
void ivt_init() {

	/* clear entire IVT, in SRAM location for SRAM + .data (in .bss section) */
	memset(&ivt, 0, (sizeof(uint32_t) * 92));

	// stack top is loaded from the first entry table on boot/reset
	// don't need to relocate or init this here
	extern void * reset,  * nmi, * hardfault;

	for (int i = 1; i <= 64 ; i++) {
		ivt_set_gate(i, dummy_isr, 0);
	}

	/* The vector table is intially at 0x0. The vector table can be
	 * relocated to other memory locations. We can do this by setting 
	 * a register in the NVIC called the vector table offset register */

	regw_u32(SCB_VTOR, (uint32_t) &ivt, 0, OWRITE);

}

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>

/* 
 * These values are pushed on the stack just before
 * entering the ISR. Normally, this would require
 * assembly or inline assembly code. I use a so-called
 * 'naked function' __attribute__ ((interrupt)) which
 * gives me a little bit more control over the caller
 *
 * The following register are pushed to the stack
 * in reverse order
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
 * interrupt vector 16-64: irq0 - irq ..
 * */

uint32_t ivt[64];

/* each message corresponds to each and every exception. 
 * We get the correct message by accessing
 *  exception_message[interrupt_number] 
 *  exception_message[0] is not used (=MSP)*/
unsigned char *exception_messages[] =
{
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
    "PENDSV",
    "SYSTICK",
    "IRQ1",
    "IRQ2",
    "IRQ3",
    "IRQ4",
    // add more if needed
};

void ivt_set_gate(unsigned char num, void * isr(), short pri) {

	ivt[num] = (uint32_t) isr;
	*NVIC_ISER0 = (1 << ((uint32_t)(num) & 0x1F));
	/* Priorities */
}


/* Dummy interrupt */
__attribute__ ((interrupt)) 
void * dummy_isr(struct interrupt_frame * frame) {


	uint32_t * p = (volatile uint32_t *) 0x21000000;

	addrtohex(frame->r0);
	addrtohex(frame->r1);
	addrtohex(frame->r2);
	addrtohex(frame->r3);
	addrtohex(frame->r12);
	addrtohex(frame->lr);
	addrtohex(frame->pc);
	addrtohex(frame->psr);
	
	//__asm__ __volatile__ ("MRS r0, IPSR");
	//addrtohex(frame->r0);
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

	for (int i = 1; i < 7; i++) {
		ivt_set_gate(i, dummy_isr, 0);
	}

	/* the vector table starts at 0x0. Since the address 0x0 point to 
	 * bootcode, it is on ROM or FLASH. The vector table can be
	 * relocated to other memory locations. We can do this by setting 
	 * a register in the NVIC called the vector table offset register */

	//*SCB_VTOR = (volatile uint32_t) &ivt; 
	regw_u32(SCB_VTOR, (uint32_t) &ivt, 0, 0x01);

}

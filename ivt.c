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
#include <lib/tinyprintf.h>

/* 
 * These values are pushed on the stack just before
 * entering the ISR. I use a so-called
 * 'naked function' __attribute__ ((naked)) which
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
} frame;

/* 
 * Vector table, each entry contains an interrupt
 * service routine: 
 * interrupt vector 1-15: processor exceptions
 * interrupt vector 16-92: irq0 - irq ..
 * Vector table needs to be aligned in memory.
 * */
uint32_t __attribute__((aligned(0x100))) ivt[92];

/* Each message corresponds to an exception in the vector table. */
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
    "RESERVED",
    "RESERVED",
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

return "UNKNOWN";

}

/* Function to set entries of the interrupt vector table 
 * */
void ivt_set_gate(unsigned char num, void * isr(), short pri) {

	ivt[num] = (uint32_t) isr;
	//if (num <= 32)
	//*NVIC_ISER0 = (1 << ((uint32_t)(num) & 0x1F));
	/* TODO: Priorities */
}


/* Dummy interrupt: shows the saved stack registers and then
 * halts */
__attribute__ ((naked)) void * dummy_isr(void) {

	uint32_t * current_sp;

	/* Test whether system call was invoked from supervisor (use MSP) or
	 * user (use PSP) mode */
	asm volatile (
	"tst lr, #4" "\n\t"
	"ite eq" "\n\t"
	"mrseq %0, msp" "\n\t"
	"mrsne %0, psp" : "=r" (current_sp));

	memcpy(&frame, current_sp, sizeof(struct interrupt_frame));

	uint8_t nr = *SCB_ICSR & 0xFF;
	printf("EXCEPTION: %s\n", exception_message(nr));
	printf("STACKFRAME:\n");
	printf("R0:%p\n",frame.r0);
	printf("R1:%p\n",frame.r1);
	printf("R2:%p\n",frame.r2);
	printf("R3:%p\n",frame.r3);
	printf("R12:%p\n",frame.r12);
	printf("LR:%p\n",frame.lr);
	printf("PC:%p (<-- last function call)\n",frame.pc);
	printf("PSR:%p\n",frame.psr);
	
	printf("CPU STATUS:");
	printf("%x\n", *SCB_CFSR);
	for(;;); 
}

/* Initialize interrupt vector  */
void ivt_init() {

	/* clear entire IVT location in memory  */
	memset(&ivt, 0, (sizeof(uint32_t) * 92));

	/* The reset, NMI and hardfault handlers are originally
	 * defined in the assembly start up and can be 
	 * reused or overwritten. 
	*  */
	extern void * reset,  * nmi, * hardfault;

	/* set dummy handlers */
	for (int i = 1; i <= 64 ; i++) {
		ivt_set_gate(i, dummy_isr, 0);
	}


	/* Enable memory management, bus and usage fault exceptions handlers
	 * If these are not enabled, the processor treats them as a hard
	 * faults. Unpriviliged access will cause a busfault in case no MPU */
	rsetbit(SCB_SHCSR, 16); // MPU violation
	rsetbit(SCB_SHCSR, 17); // Bus faults
	rsetbit(SCB_SHCSR, 18); // Usage faults
	
	/* Enable various other faults */
	// rsetbit(SCB_CCR, 4); // division by zero, (needed if you write a lot
	// of assembly, otherwise the compiler probably leave these divisions out


	/* The vector table is intially at 0x0. The vector table can be
	 * relocated to other memory locations. We can do this by setting 
	 * a register in the NVIC called the vector table offset register */

	rwrite(SCB_VTOR, (uint32_t) &ivt);

}

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>

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

//__attribute__ ((interrupt))
void * systick_handler(/* struct interrupt_frame * frame */) {

//	uint32_t volatile status;
	//uart_puts("TICKING\n");
//	for(;;);
}


void systick_init() {

	/* Enable the counter and enable the interrupt
	 * associated with it */
	*STK_CTRL = (volatile uint32_t) 0x00000003;

	/* The counter reload register here holds 
	 * 0x1000 -- that's 4096 clock cycles -- if 
	 * it is down to zero it is restores the value */
	*STK_RELOAD = (volatile uint32_t) 0x00400000; 

	/* Every time the counter counts down to zero
	 * a systick exception is asserted. Systick has
	 * exception number 15. in the vector table  */
	ivt_set_gate(15, systick_handler, 0); 
}

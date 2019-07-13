#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>


void * systick_handler() {

//	*RANDOM_ADDR = (volatile uint32_t) 0x10101010 ;
//	uart_puts("TEST");
}

void systick_init() {

	/* Enable the counter and enable the interrupt
	 * associated with it */
	*STK_CTRL = (volatile uint32_t) 0x00000003;

	/* The counter reload register here holds 
	 * 0x1000 -- that's 4096 clock cycles -- if 
	 * it is down to zero it is restores the value */
	*STK_RELOAD = (volatile uint32_t) 0x00000400; 

	/* Every time the counter counts down to zero
	 * a systick exception is asserted. Systick has
	 * exception number 15. in the vector table  */
	ivt_set_gate(15, systick_handler, 0); 
}

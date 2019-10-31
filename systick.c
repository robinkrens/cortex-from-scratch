/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/8/14 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * SysTick of Cortex M* MCUs. Have a look at the more complex RTC 
 * in case more accurate timing is needed.
 *
 * 
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h>
#include <sys/mmap.h>

#include <lib/regfunc.h>
#include <lib/tinyprintf.h>

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

	printf("Ticking...\n");

}


__attribute__ ((naked))
       	void * pendsv_handler_c(void) {
//
//	asm volatile ("push {r0-r11, lr}");
//	asm volatile ("mrs %0, psp" : "=r" (oldpsp));
//	asm volatile ("push {lr}");
//
//	//asm volatile ("push {lr}");
//	
//	//printf("FROM MSP %x", oldpsp);
//	//PSP_array[curr_task] = oldpsp;
//	//curr_task = next_task;
//	//newpsp = PSP_array[next_task];	
//
//	asm volatile ("msr psp, %0" : : "r" (newpsp)); 
//
//	asm volatile("pop {lr}");
//	//asm volatile ("pop {r0-r12}");
//	asm volatile("bx lr"); // return
}

uint32_t set_psp(uint32_t) __attribute__( ( naked ) );
uint32_t set_psp(uint32_t stackie) {
  
	asm volatile ("msr psp, r0" "\n\t"
		  "bx lr");
}

void systick_init() {

	/* Every time the counter counts down to zero
	 * a systick exception is invoked. Systick has
	 * exception number 15. in the vector table  */
	ivt_set_gate(15, systick_handler, 0); 

	/* Get calibration and set this to 1 sec
	 * !Most boards have a 1 ms or 10 ms 
	 * calibration value */
	int calib = (*STK_CALIB << 0) * 200;

	/* The counter reload registers counts down to zero
	 * and then it is restores the value */
	rwrite(STK_RELOAD, calib);
	
	/* Enable the counter and enable the interrupt
	 * associated with it */
	rsetbit(STK_CTRL, 0);
	rsetbit(STK_CTRL, 1);

}

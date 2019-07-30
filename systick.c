#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h>
#include <sys/mmap.h>

#include <lib/regfunc.h>
#include <lib/stdio.h>

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

//	cputs("TICKING\n");
//	for(;;);
}


void systick_init() {

	/* Every time the counter counts down to zero
	 * a systick exception is asserted. Systick has
	 * exception number 15. in the vector table  */
	ivt_set_gate(15, systick_handler, 0); 

	/* Get calibration and set this to 1 sec
	 * !Most boards have a 1 ms or 10 ms 
	 * calibration value */
	int calib = (*STK_CALIB << 0) * 500;

	/* The counter reload registers counts down to zero
	 * and then it is restores the value */
	rwrite(STK_RELOAD, calib);
	
	/* Enable the counter and enable the interrupt
	 * associated with it */
	rsetbit(STK_CTRL, 0);
	rsetbit(STK_CTRL, 1);


}

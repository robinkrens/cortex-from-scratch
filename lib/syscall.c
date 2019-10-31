/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/9/21 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * System calls for various user functions
 *
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <lib/syscall.h>

/* Arguments are placed in r0, r1, r2 by convention
 * And "parsed" to the the kernel
 * Right after the svc call, r0 contains status
 * of call (ie. OK, SYSERR) */

int theos_uptime() {

	asm volatile("svc 7");
	int ret;
	asm volatile("mov %0, r0" : "=r" (ret));	
	return ret;
}

	
int theos_init(uint32_t * p) {
	asm volatile("svc 1");

	return -1;
}

__attribute__ ((naked))
int theos_switch(uint32_t * p1, uint32_t * p2) {

	asm volatile("push {lr}");
	asm volatile("svc 4");
	asm volatile("pop {lr}");
	asm volatile("bx lr");
	// should not be here
	for(;;);
	
}

int theos_test(int arg1, int arg2, int arg3) {
	
	asm volatile("svc 11");
	int ret;
	asm volatile("mov %0, r0" : "=r" (ret));	
	return ret;
}

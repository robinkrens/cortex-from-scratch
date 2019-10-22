
/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/9/21 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * System calls for user functions
 *
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <lib/syscall.h>


//__attribute__ ((naked))
static int theos_syscall(int SYSCALL_N, int SYSCALL_N2) {

	asm volatile ("svc 11");
	
	return 0;
}

//__attribute__ ((naked))
void theos_test(int dummy, int dummy2) {

	theos_syscall(0xB1, 0xB2);
}

/* void theos_cputs(const char * str, size_t len) {

	//syscall(#, 0, 0, 0 ..);
	theos_syscall(22, 44);
} */

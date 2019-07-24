/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 *
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h> 
#include <sys/mmap.h>

#include <drivers/uart.h>

void main()
{
	ivt_init();
	uart_init();

	systick_init();
	
	sysinfo();

	extern void stub();
	//stub();
	//__asm__ __volatile__ ("ldc p1, cr1, r0");

	terminal();

	for(;;) {

 	}
}

/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Main intialize basic components of the boards
 * and jumps to a terminal
 *
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h> 
#include <sys/mmap.h>

#include <drivers/uart.h>
#include <drivers/led.h>
#include <drivers/tm1637.h>

void main()
{
	ivt_init();
	
	uart_init();
	systick_init();

	led_init();
	
	sysinfo();

	//extern void stub();
	//stub();
	//__asm__ __volatile__ ("ldc p1, cr1, r0");

	while(1) {
		int r;
		for (int i = 0; i < 50000; i++) {
			r = 0;	
		}
		led_on();
		for (int i = 0; i < 50000; i++) {
			r = 0;	
		}
		led_off();
	}
	// terminal();

	for(;;) {

 	}
}

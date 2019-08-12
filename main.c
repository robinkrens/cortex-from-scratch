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

#include <lib/regfunc.h>
#include <lib/stdio.h>
#include <lib/tinyprintf.h>

#include <drivers/uart.h>
#include <drivers/led.h>
#include <drivers/tm1637.h>
#include <drivers/at24c.h>
#include <drivers/tsensor.h>

//void sleep() {
//
//	__asm__ __volatile__("wfe");
//
//}

void main()
{
	clock_init();
	ivt_init();
	uart_init();
//	cputs("ROBSYS LOADING...\n");
	//systick_init();
//	tsensor_output(0xFFFF, 0x7FFF);

	init_printf(NULL, putc);
	// SPEED_TEST
/*	cputs("START TEST (8MHz) \n");
	int a;
	for (int i = 0; i < 20000000; i++) {
		a + 2;	
	}
	a = 0;
	cputs("END TEST\n");
	
	//! 
	clock_init();

	cputs("START TEST (??MHz) \n");
	for (int i = 0; i < 20000000; i++) {
		a + 2;	
	}
	cputs("END TEST\n"); */
	sysinfo();


//	tsensor_input(5000);
//	run();

	led_init();
	eeprom_at24c_init();
	eeprom_test();
//	rtc_init();

//	tm1637_init();
//	tm1637_start();

	//uint32_t test = hextoreg("12345678");
		
//	cputs(regtohex(test));

	//extern void stub();
	//stub();
	//__asm__ __volatile__ ("ldc p1, cr1, r0");

/* 	while(1) {
		int r;
		for (int i = 0; i < 50000; i++) {
			r = 0;	
		}
		led_on();
		for (int i = 0; i < 50000; i++) {
			r = 0;	
		}
		led_off();
	} */
	terminal();

	for(;;) {

 	}
}

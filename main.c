/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Main initialize basic components of the board
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
//#include <drivers/tm1637.h>
//#include <drivers/at24c.h>
//#include <drivers/tsensor.h>
#include <drivers/mk450_joystick.h>

void main()
{

	/* Initialize the clock system, */
	clock_init();

	/* Setup the interrupt vector table */
	ivt_init();

	/* Initialze basic input and output over serial */
	uart_init();

	/* Cortex M* integrated systick, can be replaced
	 * by the more accurate RTC.
	systick_init();
	*/
	

	/* Set up a very small libc library */
	init_printf(NULL, putc);

	/* Display some basic info at startup */
	sysinfo();

	/* On board LEDs*/
	led_init();

	/* Real time clock */
	rtc_init();

	/* Eeprom Driver
	eeprom_at24c_init();
	eeprom_test();
	*/
	
	/* LED Segment Driver */
	//tm1637_init();

	/* ASM Blocking routine */
	//for (int i = 0; i < 1000; i++)
	//	_block(10000);

	/* TEMP SENSOR 
	tsensor_printid();
	uint16_t temp = tsensor_get_temp();
	printf("Current temperature: %d °C\n", temp); */

	/* ADC Joystick module */
	mk450_init();	

	/* Start up terminal */
	terminal();
	
	/* Should not be here, endless loop */
	for(;;) {

 	}
}

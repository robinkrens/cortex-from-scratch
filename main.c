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
#include <lib/pool.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/tinyprintf.h>

#include <drivers/uart.h>
#include <drivers/led.h>
//#include <drivers/tm1637.h>
//#include <drivers/at24c.h>
//#include <drivers/tsensor.h>
//#include <drivers/mk450_joystick.h>
#include <drivers/st7735s.h>

mem_pool_t kheap_pool;

void main()
{

	/* Initialize the clock system, */
	clock_init();

	/* Setup the interrupt vector table */
	ivt_init();

	/* Initialze basic input and output over serial */
	uart_init();

	/* TFT screen */
	// tft_init();
	
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

	extern uint32_t * _beginofheap;
	//printf("%p", &_beginofheap);
	kpool_init(&kheap_pool, 0x10, 10, (uint32_t *) &_beginofheap);

//	printf("%p\n", &kheap_pool);

	char * string = (char *) kalloc(&kheap_pool);
	char * string2 = (char *) kalloc(&kheap_pool);
	char * string3 = (char *) kalloc(&kheap_pool);


	memset(string, 0xFF, 0x10);
	memset(string2, 0xEE, 0x10);
	memset(string3, 0xDD, 0x10);

	printf("%p\n", string);
	printf("%p\n", string2);
	printf("%p\n", string3);

	kfree(&kheap_pool, string);

	char * string6 = (char *) kalloc(&kheap_pool);


	memset(string6, 0xCC, 0x10);
//	char * string7 = (char *) kalloc(&kheap_pool);
	printf("%p\n", string6);
//	printf("%p\n", string7);


	kfree(&kheap_pool, string2);

	char * string7 = (char *) kalloc(&kheap_pool);
	memset(string7, 0xBB, 0x10);


	char * string8 = (char *) kalloc(&kheap_pool);
	memset(string8, 0xAA, 0x10);

	char * string9 = (char *) kalloc(&kheap_pool);
	memset(string9, 0x99, 0x10);
	//free(string);

	kfree(&kheap_pool, string3);

	//char * string2 = (char *) alloc();
	
	//string2 = "taalb";

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
	// mk450_init();	


	/* Start up terminal */
	terminal();
	
	/* Should not be here, endless loop */
	for(;;) {

 	}
}

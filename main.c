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
#include <sys/process.h>

#include <lib/regfunc.h>
#include <lib/pool.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/tinyprintf.h>
#include <lib/syscall.h>

#include <drivers/uart.h>
#include <drivers/led.h>
//#include <drivers/tm1637.h>
//#include <drivers/at24c.h>
//#include <drivers/tsensor.h>
//#include <drivers/mk450_joystick.h>
#include <drivers/st7735s.h>


/* Example of multitasking */
process_t p1;
process_t p2;

uint32_t stackp1[500];
uint32_t stackp2[500];

void process1(void);
void process2(void);



void main()
{

	/* Load .data segment into SRAM */
	extern uint32_t * data_lma, data_vma, data_end;
	int size = (&data_end - &data_vma) * 4;
	memcpy(&data_vma, &data_lma, size);

	/* Initialize the clock system, */
	clock_init();

	/* Setup the interrupt vector table */
	ivt_init();

	/* Initialze basic input and output over serial */
	uart_init();

	/* TFT screen */
	// tft_init();
	
	/* Set up a very small libc library */
	init_printf(NULL, putc);

	/* Heap init */
	//kheap_init();
	//printf("%p\n", get_kheap());
	
	/* Display some basic info at startup */
	sysinfo();

	/* On board LEDs*/
	led_init();


	/* Real time clock */
	rtc_init();

	/* Initialize SVC handler for system calls*/
	syscall_init();

	/* System call test */
	int uptime = theos_uptime();
	printf("UPTIME: %d\n", uptime);

	/* Multi processes test */
//	int size_stack = sizeof(stackp1);
//	p1.stackptr = ((unsigned int) stackp1) + size_stack - 0x1C;
//	p1.stackptr[6] = (uint32_t) process1;
//	p1.stackptr[7] = 0x01000000;
//	p2.stackptr = ((unsigned int) stackp2) + size_stack - 0x1C;
//	p2.stackptr[6] = (uint32_t) process2;
//	p2.stackptr[7] = 0x01000000;
//	theos_init(&p1);

	/* Cortex M* integrated systick, can be replaced
	 * by the more accurate RTC. */
	//systick_init();

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

void process1(void) {
	
	while(1) {
		printf("process 1\n");
		_block(0xFFFFF);
		theos_switch(&p1, &p2);
	}

}
void process2(void) {
	while(1) {
		printf("process 2\n");
		_block(0xFFFFF);
		theos_switch(&p2, &p1);
	}
	
}

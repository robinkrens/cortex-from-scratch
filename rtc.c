/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/8/14 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Real Time Clock configuration. A Real Time Clock is configured to use
 * the more accurate external chrystal (LSE) and ticks at exactly
 * one second. (Can be further calibrated using a tamper pin)
 * Each clock tick triggers an interrupt and invokes the real time clock
 * service routine.
 *
 * Note: this is not the Cortex M3/M4 SysTick!
 * 
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h>
#include <sys/mmap.h>

#include <lib/regfunc.h>
#include <lib/tinyprintf.h>

#include <drivers/led.h>
#include <drivers/tm1637.h>
#include <drivers/mk450_joystick.h>


/* This handler is invoked each clock tick. I've included two examples 
 * one is writing the clock count to a 4 grid led display. Another is a simple LED blink */
void * rtc_handler() {

	// Real time clock output on a LED display 
	/* int cntvalue = *RTC_CNTL;
	if (cntvalue < 9999) {

	unsigned char dn[10] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6};
		
	int grid0, grid1, grid2, grid3;

 	grid0 = cntvalue % 10;
    	grid1 = ((cntvalue % 100) - grid0) / 10;
    	grid2 = ((cntvalue % 1000) - grid0 - grid1) / 100;
        grid3 = ((cntvalue % 10000) - grid0 - grid1 - grid2) / 1000;

	//printf("%d, %d, %d, %d\n", grid0, grid1, grid2, grid3);
	
	char current[4] = { dn[grid3], dn[grid2], dn[grid1], dn[grid0] }; 

	for (int i = 0; i < 4; i++) {
		set_grid(i, current[i], false);
	}

	set_display(true, 0); 
	} */

	// Simple LED blink 
	uint32_t curr = *RTC_CNTL;
	int even = *RTC_CNTL % 2;
	(!even) ? led_off() : led_on();
	
	// Double Axis Joystick 
	//printf("X: %x\n", mk450_getx());
	//printf("Y: %x\n", mk450_gety());

	rclrbit(RTC_CRL, 0); /* clear interrupt flag */
}

/* Enable a periodic interrupt. At each clock count a interrupt is triggered
 * and consequently the handler is invoked. */
static void periodic_intr() {

	while(!rchkbit(RTC_CRL, 5)); // Check last write is terminated
	rsetbit(RTC_CRL, 4); // start configure
	rsetbit(RTC_CRH, 0); // enable periodic (second) interrupt
	
	while(!rchkbit(RTC_CRL, 5)); 
	rwrite(RTC_PRLL, 0x7FFF); // 0x7FFF equals 1 second clock configuration 
	rclrbit(RTC_CRL, 4); // stop configure
	while(!rchkbit(RTC_CRL, 5)); // Check last write is terminated
	rsetbit(NVIC_ISER0, 3); // enable in register vector
}

/* Setup the Real time clock to work with the Low speed external 
 * chrystal (32.768 kHz) */
static void setup_rtc() {

	/* Enable PWREN and BKPEN */
	rsetbit(RCC_APB1ENR, 28);
	rsetbit(RCC_APB1ENR, 27);

	/* Enable access to backup registers and RTC */
	rsetbit(PWR_CR, 8);

	rsetbit(RCC_BDCR, 0); /* LSE enable */
	while(!rchkbit(RCC_BDCR, 1)); /* wait for LSE to come up */
	
	rsetbitsfrom(RCC_BDCR, 8, 0x1); /* use LSE as RTC source */
	rsetbit(RCC_BDCR, 15); /* enable RTC */
	
	ivt_set_gate(19, rtc_handler, 0); /* setup interrupt handler */

	//calibrate_rtc(); // uncomment if need, needs TAMPER PIN
	
	periodic_intr();// setup periodic interrupt

}

/* Function to calibrate the RTC even more accurate. 
 * Output to a so-called tamper pin. Uncomment if needed */
static void calibrate_rtc() {
		/* rsetbit(BKP_RTCCR, 7); // enable CC0, 
	while(!rchkbit(RTC_CRL, 5)); // Check last write is terminated
	rsetbit(RTC_CRL, 4);
	// Set up and check tamper pin
	rclrbit(RTC_CRL, 4);
	while(!rchkbit(RTC_CRL, 5)); // Check last write is terminated */
}

void rtc_init() {

#ifdef ENABLE_RTC
	setup_rtc();
#endif

}

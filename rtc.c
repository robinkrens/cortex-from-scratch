/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/30 - ROBIN KRENS	
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

#include <lib/regfunc.h>
#include <lib/stdio.h>

#include <drivers/led.h>


static void periodic_intr() {

	while(!rchkbit(RTC_CRL, 5)); // Check last write is terminated
	rsetbit(RTC_CRL, 4); // start configure
      	
	rsetbit(RTC_CRH, 0); // enable periodic (second) interrupt
	
	while(!rchkbit(RTC_CRL, 5)); 

	rwrite(RTC_PRLL, 0x7FFF); // 1 second
	rclrbit(RTC_CRL, 4); // stop configure
	while(!rchkbit(RTC_CRL, 5)); // Check last write is terminated
	rsetbit(NVIC_ISER0, 3); // enable in register vector
}

static void calibrate_rtc() {


//	rsetbit(BKP_RTCCR, 7); // enable CC0, 
//	while(!rchkbit(RTC_CRL, 5)); // Check last write is terminated
//	rsetbit(RTC_CRL, 4);

	// Set up and check tamper pin
	
//	rclrbit(RTC_CRL, 4);
//	while(!rchkbit(RTC_CRL, 5)); // Check last write is terminated
}

void * rtc_handler() {

	//cputs("TICKING IN REAL TIME\n");
	//uint32_t curr = *RTC_CNTL;
	int even = *RTC_CNTL % 2;
	(!even) ? led_off() : led_on();

	rclrbit(RTC_CRL, 0);
}

static void setup_rtc() {

// TODO: long time to get stable?
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

//	calibrate_rtc(); TODO: TAMPER PIN?
	
	periodic_intr();// setup periodic interrupt

}

void rtc_init() {

#ifdef ENABLE_RTC
	setup_rtc();
#endif

}

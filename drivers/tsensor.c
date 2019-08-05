/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/8/4 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Temperature sensor
 *
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/mmap.h>
#include <sys/robsys.h>

#include <lib/regfunc.h>
#include <lib/string.h>
#include <lib/tinyprintf.h>

#include <drivers/tsensor.h>

/* void * update_handler() {

	printf("HERE!");
	rclrbit(TIM4_SR1, 0);
} */

void tsensor_init( ) {

	int prescaler = 31; 

	rsetbit(RCC_APB2ENR, 3); // GPIOB enable
	rsetbit(RCC_APB1ENR, 2); // TIM4 enable
	rsetbitsfrom(TIM4_CR1, 5, 0x00); // edge-aligned mode
	rclrbit(TIM4_CR1, 4); // upcounter (clrbit!)

	rwrite(TIM4_PSC, 0xFFFF); // 1 MHz: 23
	rwrite(TIM4_ARR, 0xAB9); // preload register

	rwrite(GPIOB_CRL, 0x4A444444); // PB6 for Channel 1 TIM4 alternate 

	rwrite(TIM4_CCR1, 0x55C); // half of ARR
	rwrite(TIM4_RCR, 0x0F); // repeat
	rsetbit(TIM4_EGR, 0); // update generation
	
	rsetbitsfrom(TIM4_CCMR1, 4, 0x7); // PWM mode 1
	
	//rsetbit(TIM4_CCMR1, 3); // preload enable
	//rsetbit(TIM4_CR1, 7); // buffered

	//rsetbit(TIM4_CR1, 3); // one pulse mode

	rsetbit(TIM4_CCER, 0); // enable output channel 1
//	rsetbit(TIM4_BDTR, 15); // main output 
	rsetbit(TIM4_CR1, 0);

	/* INTERRUPTS */	
	//ivt_set_gate(41, update_handler, 0);

	//rsetbit(TIM4_DIER, 0);
	//rsetbit(NVIC_ISER0, 25); // interupt 41 - 32
}



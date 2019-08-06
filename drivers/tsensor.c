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

#define PRESCALER 36000 // 1 kHz

 void * update_handler() {

	if(rchkbit(TIM4_SR1, 1)) {

			printf("RISING EDGE CAUGHT\n");
			printf("CCR1: %p\n", *TIM4_CCR1);
	}
	
	if(rchkbit(TIM4_SR1, 2)) {
			printf("FALLING EDGE CAUGHT\n");
			printf("CCR2: %p\n", *TIM4_CCR2);
	}
	

	rclrbit(TIM4_SR1, 1);
	rclrbit(TIM4_SR1, 0);
	rclrbit(TIM4_SR1, 2); // 
	rclrbit(TIM4_SR1, 9); // OF
	rclrbit(TIM4_SR1, 10); // OF
	rclrbit(TIM4_SR1, 6);
	// TODO clear overflow tag
} 

void tsensor_output(uint16_t preload, uint16_t compare/*, uint16_t pulses */) {

	/* GPIO AND CLOCK */
	rsetbit(RCC_APB2ENR, 3); // GPIOB enable
	rwrite(GPIOB_CRL, 0x4A444444); // PB6 for Channel 1 TIM4 alternate 
	rsetbit(RCC_APB1ENR, 2); // TIM4 enable
	
	rsetbitsfrom(TIM4_CR1, 5, 0x00); // edge-aligned mode
	rclrbit(TIM4_CR1, 4); // upcounter (clrbit! not needed to set)

	rwrite(TIM4_PSC, PRESCALER - 1); // 1 MHz
	rwrite(TIM4_ARR, preload); // preload 
	rwrite(TIM4_CCR1, compare); // compare
	//rwrite(TIM4_RCR, pulses - 1); /* repeat ONLY IN ADVANCED TIMER */
	
	rsetbit(TIM4_EGR, 0); // update generation  
	
	rsetbit(TIM4_CR1, 3); // one pulse mode
	rsetbitsfrom(TIM4_CCMR1, 4, 0x7); // PWM mode 1
	
	//rsetbit(TIM4_CCMR1, 3); // preload enable
	//rsetbit(TIM4_CR1, 7); // buffered


	rsetbit(TIM4_CCER, 0); // enable output channel 1
	rsetbit(TIM4_CR1, 0); // start counter

	/* INTERRUPTS */	
	//ivt_set_gate(41, update_handler, 0);

	//rsetbit(TIM4_DIER, 0);
	//rsetbit(NVIC_ISER0, 25); // interupt 41 - 32
}

void tsensor_input(uint16_t preload) {

	uint16_t timestamp;	
	/* GPIO AND CLOCK */
	rsetbit(RCC_APB2ENR, 3); // GPIOB enable
	rwrite(GPIOB_CRL, 0x44444444); // Input floating (default state)
	rsetbit(RCC_APB1ENR, 2); // TIM4 enable
	
	//rsetbitsfrom(TIM4_CR1, 5, 0x00); // edge-aligned mode
	//rclrbit(TIM4_CR1, 4); // upcounter (clrbit! not needed to set)

	rwrite(TIM4_PSC, PRESCALER - 1); // 1 MHz
	rwrite(TIM4_ARR, preload); // preload 
	
	rsetbit(TIM4_CCMR1, 0); // input on TI1
	rsetbit(TIM4_CCMR1, 9);	// another input TI2
	rsetbit(TIM4_CCER, 5); // other polarity, inverted

	/* TODO: reg funct */
	rsetbit(TIM4_SMCR, 4); // 101
	rsetbit(TIM4_SMCR, 6); // 101
	


	// rsetbit(TIM4_SMCR, 2); // RESET rising edge triggers counter and generates update
	rsetbit(TIM4_SMCR, 2); // 110
	rsetbit(TIM4_SMCR, 1); // 110

	rsetbit(TIM4_CR1, 3); // one pulse mode // NOTE: RESET after finised preload
	// will catch multiple signal... can set fram
	
	rsetbit(TIM4_CCER, 0); // enable capture channel 1 (changed pos)
	rsetbit(TIM4_CCER, 4); // enable capture channel 2 
	/* Caught on rising edge, no need to change*/
	/* Clear capture event flag */
//	rsetbit(TIM4_CR1, 0); // RESET with no trigger mode start

	// enable capture channel 1 interrupt
	rsetbit(TIM4_DIER, 1);
	rsetbit(TIM4_DIER, 2);
        ivt_set_gate(46, update_handler, 0);
	rsetbit(NVIC_ISER0, 30);

}

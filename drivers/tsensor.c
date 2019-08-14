/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/8/4 - ROBIN KRENS	
 * PreInitial version 
 * 
 * $DESCRIPTION$
 * Temperature sensor 
 * [in dev]
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

#define PRESCALER 36000 // 1 Mhz

int *ccr1, *ccr2, *ccr1b, *ccr2b;
bool s1, s2;

 void * update_handler() {

/*	s1 = false;
	s2 = false;
	ccr1 = 0xFFFFFFFF;
       	ccr2 = 0xFFFFFFFF;
       	ccr1b = 0xFFFFFFFF;
       	ccr2b = 0xFFFFFFFF;
	
	if(rchkbit(TIM4_SR1, 1)) {
			s1 = true;
		//	printf("CCR1: %p\n", *TIM4_CCR1);
		//	printf("CCR2: %p\n", *TIM4_CCR2);
			ccr1 = *TIM4_CCR1;
			ccr2 = *TIM4_CCR2;
			rclrbit(TIM4_SR1, 1); 
	}
	
	if(rchkbit(TIM4_SR1, 2)) {
			s2 = true;
			ccr1b = *TIM4_CCR1;
			ccr2b = *TIM4_CCR2;
			rclrbit(TIM4_SR1, 2);
	}
	
	if(rchkbit(TIM4_SR1, 6)) {
	//		printf("TRIGGER\n");
			rclrbit(TIM4_SR1, 6);
	}

	rclrbit(TIM4_SR1, 0);
//	rclrbit(TIM4_SR1, 9); // OF
//	rclrbit(TIM4_SR1, 10); // OF

	// TODO clear overflow tag
	
	printf("SR1/CCR1: %p\n", ccr1);
	printf("SR1/CCR2: %p\n", ccr2);
	printf("SR2/CCR1: %p\n", ccr1b);
	printf("SR2/CCR2: %p\n", ccr2b);

	if (s1)
		printf("EDGE DOWN\n");
	if (s2)
		printf("EDGE UP\n");

	s1 = false;
	s2 = false; */
}

static void reset() {
	rwrite(GPIOB_CRL, 0x44444444);
}

void * tmp_update_handler() {

	printf("SR: %p\n", *TIM4_SR1);
	
	rclrbit(TIM4_CR1, 0);	/* EMULATOR STOP */
	rclrbit(TIM4_SR1, 0);
	rclrbit(TIM4_SR1, 1);
		reset();
		tsensor_input(0xFFFF);

//	if(rchkbit(TIM4_SR1, 1)) {
//		printf("TEST\n");
//	}

}

void * cnt_complete_handler() {
	rclrbit(TIM4_CR1, 0);
	rclrbit(TIM4_SR1, 0);
	rclrbit(TIM4_DIER, 0);
	rwrite(GPIOB_CRL, 0x44444444);
	printf("CNT COMPLETE\n");
	tsensor_input(0xFFFF);
}


void tsensor_simple(uint16_t preload) {
	
	rsetbit(RCC_APB1ENR, 2); // TIM4 enable

	rsetbitsfrom(TIM4_CR1, 5, 0x00); // edge-aligned mode
	rclrbit(TIM4_CR1, 4); // upcounter (clrbit! not needed to set)
	rsetbit(TIM4_CR1, 2); // only overflow generates update

	rwrite(TIM4_PSC, PRESCALER - 1);
	rwrite(TIM4_ARR, preload);
	rsetbit(TIM4_EGR, 0);
	
	ivt_set_gate(46, cnt_complete_handler, 0);
	rsetbit(NVIC_ISER0, 30); // interupt 41 - 32

	rsetbit(GPIOB_BSRR, 22); // 
	rsetbit(TIM4_DIER, 0);
	rsetbit(TIM4_CR1, 0);

}

void run() {

	rsetbit(RCC_APB2ENR, 3); // GPIOB enable
	rwrite(GPIOB_CRL, 0x47444444); // open drain general

	rsetbit(GPIOB_BSRR, 22); // high
	tsensor_simple(2000);
//	tsensor_output(580, 520);
//	reset();
//	tsensor_simple(580);
}

void tsensor_output(uint16_t preload, uint16_t compare/*, uint16_t pulses */) {

	/* GPIO AND CLOCK */
	rsetbit(RCC_APB2ENR, 3); // GPIOB enable
	rwrite(GPIOB_CRL, 0x4A444444); // PB6 for Channel 1 TIM4 alternate 
	rsetbit(RCC_APB1ENR, 2); // TIM4 enable
	
	rsetbitsfrom(TIM4_CR1, 5, 0x00); // edge-aligned mode
	rclrbit(TIM4_CR1, 4); // upcounter (clrbit! not needed to set)
	rsetbit(TIM4_CR1, 2); // only overflow generates update

	rwrite(TIM4_PSC, PRESCALER - 1); // 1 MHz
	rwrite(TIM4_ARR, preload); // preload 
	rwrite(TIM4_CCR1, compare); // compare
	//rwrite(TIM4_RCR, pulses - 1); /* repeat ONLY IN ADVANCED TIMER */
	
	rsetbit(TIM4_EGR, 0); // update generation  
	
	rsetbit(TIM4_CR1, 3); // one pulse mode
	rsetbitsfrom(TIM4_CCMR1, 4, 0x6); // mode
	
	//rsetbit(TIM4_CCMR1, 3); // preload enable
	//rsetbit(TIM4_CR1, 7); // buffered

	rsetbit(TIM4_CCER, 0); // enable output channeli 1
	rsetbit(TIM4_CCER, 1); // active low
	rsetbit(TIM4_CR1, 0); // start counter

	/* INTERRUPTS */	
	ivt_set_gate(46, tmp_update_handler, 0);

	rsetbit(TIM4_DIER, 1);
	rsetbit(NVIC_ISER0, 30); // interupt 41 - 32
	
}

void tsensor_input(uint16_t preload) {

	//uint16_t timestamp;	
	/* GPIO AND CLOCK */
	//rsetbit(RCC_APB2ENR, 3); // GPIOB enable
	//rwrite(GPIOB_CRL, 0x44444444); // Input floating (default state)
	//rsetbit(RCC_APB1ENR, 2); // TIM4 enable
	
	//rsetbitsfrom(TIM4_CR1, 5, 0x00); // edge-aligned mode
	//rclrbit(TIM4_CR1, 4); // upcounter (clrbit! not needed to set)

	rwrite(TIM4_PSC, PRESCALER - 1); // 1 MHz
	rwrite(TIM4_ARR, preload); // preload 

	
	rsetbit(TIM4_EGR, 0); // update generation  

	rsetbit(TIM4_CCMR1, 0); // input on TI1
	rsetbit(TIM4_CCMR1, 9);	// another input TI2
	rsetbit(TIM4_CCER, 1); // other polarity for T1, inverted

	/* TODO: reg funct */
	rsetbit(TIM4_SMCR, 4); // OLD: 101, new Edge detector
	rsetbit(TIM4_SMCR, 6); // 
	

	// rsetbit(TIM4_SMCR, 2); // RESET rising edge triggers counter and generates update
	rsetbit(TIM4_SMCR, 2); // OLD: 110
	rsetbit(TIM4_SMCR, 1);
	rsetbit(TIM4_SMCR, 0);
	//rsetbit(TIM4_SMCR, 1); // 110

	//rsetbit(TIM4_CR1, 3); // one pulse mode // NOTE: RESET after finised preload
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

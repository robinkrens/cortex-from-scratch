/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/9/11 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Dual Axis XY Joystick controller
 * Uses DMA to store converted input channels. The Analog to Digital
 * converter (ADC1) is in scan mode and continuously monitors input
 * channel 0 and 1. There is only one busy-wait for initial calibration
 * of the ADC. The reference voltage here is 3300 mV. For 2500 mV (idle
 * state of the joystick) this rougly translate to a value of 3100 
 * (or 0xC1C) in the 12-bit data register (ADC1_DR). 
 * Active and non-active (or left and right, up or down for the joystick)
 * are 0x0 and 0xFFF respectively. 
 * 
 * $USAGE$
 * Use getx and gety to get the current X and Y values of the joystick.
 * How to use for games: TODO (description)
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

#include <drivers/mk450_joystick.h>

/* X and Y values of the joystick 
 * Updated continiously */
uint16_t xyvalues[2] = {0,0};


/* Y return values is 
 * ~0xC1C mv means y is idle state 
 * ~0x0 far left
 * ~0xFFF far right */
uint16_t mk450_gety() {

	int y = xyvalues[1]; 
	return y;
}

/* X return values (see y) */
uint16_t mk450_getx() {

	int x = xyvalues[0];
	return x;
}

/* Initiate joystock module and DMA */
void mk450_init() {

	/* clock prescaler */
	rsetbitsfrom(RCC_CFGR, 14, 0x3);

	/* Peripherial init */
	rsetbit(RCC_APB2ENR, 2); // enable GPIOA
	rwrite(GPIOA_CRL, 0x44444400); // analog input on GPIOA0
	rsetbit(RCC_APB2ENR, 9); // enable ADC1

	/* DMA init */
	rsetbit(RCC_AHBENR, 0); // enable clock on DMA1
	rwrite(DMA_CPAR1, (uint32_t) ADC1_DR);
	rwrite(DMA_CMAR1, (uint32_t) xyvalues);
	rwrite(DMA_CNDTR1, 2); // two values X and Y values
	rsetbitsfrom(DMA_CCR1, 8, 0x1); // 16-bit
	rsetbitsfrom(DMA_CCR1, 10, 0x1); // 16-bit
	rsetbit(DMA_CCR1, 7); // memory increment mode
	rsetbit(DMA_CCR1, 5); // circular mode
	rsetbit(DMA_CCR1, 0); // channel enable
	
	/* Scan mode for two input channels */
	rsetbitsfrom(ADC1_SQR1, 20, 0x1); // 2 channels
	rsetbitsfrom(ADC1_SQR3, 0, 0x0); // ADC1_IN0
	rsetbitsfrom(ADC1_SQR3, 5, 0x1); // ADC1_IN1
	rsetbit(ADC1_CR1, 8); // scan mode
	
	/* Software start and channel config */
	rsetbitsfrom(ADC1_CR2, 17, 0x7); // swstart config
	rsetbit(ADC1_CR2, 20); // trigger enable
	rsetbitsfrom(ADC1_SMPR2, 0, 0x7); // 237 cycles
	rsetbitsfrom(ADC1_SMPR2, 3, 0x7); // 237 cycles
	rsetbit(ADC1_CR2, 1); // continious mode

	/* Calibrate */
	rsetbit(ADC1_CR2, 2); // calibrate
	_block(50);
	
	/* Wakeup TODO: check datasheet for double init */
	rsetbit(ADC1_CR2, 8); // enable DMA	
	rsetbit(ADC1_CR2, 0); // enable ADC
	rsetbit(ADC1_CR2, 22); // swstart go!

}

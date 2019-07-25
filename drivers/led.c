/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/25 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Hardly a driver, but a driver nonetheless. Enables clock on 
 * a GPIOx range and sets the GPIOx pin in push-pull output mode. 
 * Basic on - off function (writing a 1 or zero 0) 
 * 
 * $USAGE$
 * In the code below we enable GPIOC and use PC0 as output. But 
 * basically any GPIO range or pin could be used. Note that the 
 * output register is only word accessible.
 *
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/mmap.h>
#include <sys/robsys.h>

#include <lib/regfunc.h>
#include <lib/string.h>

#include <drivers/led.h>


void led_init() {

	regw_u8(RCC_APB2ENR, 0x1, 4, SETBIT); // enable GPIOC
	regw_u32(GPIOC_CRL, 0x44444442, 0, OWRITE); // set PC0 pin to output mode
	*GPIOC_ODR = 0xFFFF; // only writable in word mode

}

void led_on() {
	*GPIOC_ODR = 0x0001;
}

void led_off() {
	*GPIOC_ODR = 0x0000;
}


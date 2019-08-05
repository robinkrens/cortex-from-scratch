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

	rsetbit(RCC_APB2ENR, 5); // enable GPIOD
	rsetbit(RCC_APB2ENR, 2); // enable GPIOA

	//rwrite(GPIOD_CRL, 0x44444644); 
	rsetbitsfrom(GPIOD_CRL, 8, 0x6);
	rsetbitsfrom(GPIOA_CRH, 0, 0x6);
	rsetbit(GPIOD_ODR, 2);
	rclrbit(GPIOA_ODR, 8);

}

void led_on() {
	rsetbit(GPIOD_ODR, 2);
	rclrbit(GPIOA_ODR, 8);

}

void led_off() {
	rclrbit(GPIOD_ODR, 2);
	rsetbit(GPIOA_ODR, 8);
}


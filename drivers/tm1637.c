/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/25 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Basic driver for the TM1637. The TM1637 is 7 segment
 * ledclock peripheral. Communication is over I2C.
 *
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/mmap.h>
#include <sys/robsys.h>

#include <lib/regfunc.h>
#include <lib/string.h>

#include <drivers/tm1637.h>


void tm1637_init() {

 /* Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
 Configure the clock control registers
 Configure the rise time register
 Program the I2C_CR1 register to enable the peripheral
 Set the START bit in the I2C_CR1 register to generate a Start condition 

 ENABLE GPIOB6 and B7*/

 //regw_u8(RCC_APB1ENR, 0x1, 21, SETBIT);
 //regw_u32(RCC_APB2ENR, 0x1, 3, SETBIT);
 // //regw_u8(AFIO_EVCR, 0x89, 0, SETBIT);// set event control register, output on ?
 
 // regw_u32(GPIOB_CRL, 0xEE444444, 0, OWRITE);
 

}


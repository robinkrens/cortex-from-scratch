/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/25 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Basic driver for the TM1637. The TM1637 is 8 segment
 * ledclock peripheral. Communication is similar to I2C,
 * but not completely. There is no address selecting.
 *
 * Alternative I2C protocol : (W)rite or (R)ead
 * | Start | ADDRESS | W | ACK/NACK | DATA | ACK/NACK | (d+a*n) | St
 * | Start | ADDRESS | R | ACK/NACK | DATA | ACK/NACK | (d+a*n) | St
 *
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/mmap.h>
#include <sys/robsys.h>

#include <lib/regfunc.h>
#include <lib/string.h>
#include <lib/stdio.h>

#include <drivers/tm1637.h>

#define TIMEOUT 1000

#define DATASET	0x40
#define CONTROL	0x80
#define SETADDR	0xC0

#define DISPLAY_ON 	0x8F
#define DISPLAY_OFF 	0x11

/* STM32F1 microcontrollers do not provide the ability to pull-up SDA and SCL lines. Their
GPIOs must be configured as open-drain. So, you have to add two additional resistors to
pull-up I2C lines. Something between 4K and 10K is a proven value.
*/

void tm1637_init() {

 /* Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
 Configure the clock control registers CCR
 Configure the rise time register TRIS
 Program the I2C_CR1 register to enable the peripheral

 ENABLE GPIOB6 and B7*/

 regw_u32(RCC_APB1ENR, 0x1, 21, SETBIT);
 regw_u32(RCC_APB2ENR, 0x1, 3, SETBIT);
 // //regw_u8(AFIO_EVCR, 0x89, 0, SETBIT);// set event control register, output on ?
 
 regw_u32(GPIOB_CRL, 0xEE444444, 0, OWRITE);

 regw_u32(I2C_CR2, 0x2, 0, OWRITE); //2 MHz 
 regw_u8(I2C_TRISE, 0x3, 0, OWRITE); // MAX = 1000ns, TPCLK1 = 500ns (+1)
 regw_u32(I2C_CCR, 0x000A, 0, OWRITE); // standard mode， output 100 kHz (100hz* / perip)
 
 regw_u32(I2C_CR1, 0x1, 0, OWRITE); // enable

}

static void start_condition() {

	regw_u32(I2C_CR1, 0x1, 8, SETBIT); //start

}

static void stop_condition() {

	regw_u32(I2C_CR1, 0x1, 9, SETBIT); //stop
}

int ack_recv() {

	int cnt = 0;
	while(!(*I2C_SR1 & 0x1)) {
		cnt++;
		if (cnt > TIMEOUT)
			return 0;
	}

	return 1;

}

int delay() {

	int a = 0;
	for (int i = 0; i < TIMEOUT; i++)
		a++;
}

void tm1637_start() {

//	regw_u32(I2C_CR1, 0x1, 8, SETBIT);
//	uint32_t read_status = *I2C_SR1;

//	regw_u32(I2C_DR, DATASET, 0, OWRITE); 
	// conform DATA
//	read_status = *I2C_SR1;
//	read_status = *I2C_SR2;

	uint32_t statusr;

	start_condition();
	//uint32_t statusr = *I2C_SR1; // clear start_signal
	regw_u32(I2C_DR, 0x40, 0, OWRITE); // write to address CMD
	if(!ack_recv())
		cputs("TIMEOUT!");
	//statusr = *I2C_SR1;
	//statusr = *I2C_SR2;
	stop_condition();

	//delay();

	start_condition();
	//statusr = *I2C_SR1; // clear start_signal
	regw_u32(I2C_DR, 0xC1, 0, OWRITE);
	if(!ack_recv())
		cputs("TIMEOUT2!");
	//statusr = *I2C_SR1;
	//statusr = *I2C_SR2;
	regw_u32(I2C_DR, 0x7D, 0, OWRITE);
	if(!ack_recv())
		cputs("TIMEOUT3!");
	stop_condition();

	delay();

	start_condition();
	statusr = *I2C_SR1;
	regw_u32(I2C_DR, DISPLAY_ON, 0, OWRITE);
	if(!ack_recv())
		cputs("TIMEOUT4!");
	stop_condition();


	/* regw_u32(I2C_CR1, 0x1, 8, SETBIT); //start
	uint32_t read_status = *I2C_SR1; 
	regw_u32(I2C_DR, 0x40, 0, OWRITE); // write to address CMD
	read_status = *I2C_SR1;
	read_status = *I2C_SR2;
	regw_u32(I2C_CR1, 0x1, 9, SETBIT); //stop
	read_status = *I2C_SR1;

	regw_u32(I2C_CR1, 0x1, 8, SETBIT); //start
	read_status = *I2C_SR1;
	regw_u32(I2C_DR, 0xC1, 0, OWRITE); // segment address
	read_status = *I2C_SR1;
	read_status = *I2C_SR2;
	regw_u32(I2C_DR, 0x7D, 0, OWRITE); // write a six

	regw_u32(I2C_CR1, 0x1, 9, SETBIT); //stop
	read_status = *I2C_SR1;

	regw_u32(I2C_CR1, 0x1, 8, SETBIT); //start
	read_status = *I2C_SR1;

	regw_u32(I2C_DR, DISPLAY_ON, 0, OWRITE);
	read_status = *I2C_SR1;
	regw_u32(I2C_CR1, 0x1, 9, SETBIT); //stop */

}

void tm1637_stop() {

	//regw_u32(I2C_CR1, 0x0, 9, SETBIT);
}




/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/25 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
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
#include <lib/tinyprintf.h>

#include <drivers/at24c.h>

#define TIMEOUT 5000

#define READ_CMD	0xA1
#define WRITE_CMD	0xA0


/* STM32F1 microcontrollers do not provide the ability to pull-up SDA and SCL lines. Their
GPIOs must be configured as open-drain. So, you have to add two additional resistors to
pull-up I2C lines. Something between 4K and 10K is a proven value.
*/

void at24c_init() {

 /* Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
 Configure the clock control registers CCR
 Configure the rise time register TRIS
 Program the I2C_CR1 register to enable the peripheral

 ENABLE GPIOB6 and B7*/

 rsetbit(RCC_APB1ENR, 21);
 rsetbit(RCC_APB2ENR, 3);
 // //regw_u8(AFIO_EVCR, 0x89, 0, SETBIT);// set event control register, output on ?
 
 rwrite(GPIOB_CRL, 0xEE444444);

 rsetbitsfrom(I2C_CR2, 0, 0x2); // 36 MHz 
 rwrite(I2C_TRISE, 0x3); // MAX = 1000ns, TPCLK1 = 500ns (+1)
 rwrite(I2C_CCR, 0x000A); // standard mode， output 100 kHz (100hz* / perip)

 rsetbit(I2C_CR1, 10); // send ack if receive

 rsetbit(I2C_CR1, 0); // enable

}

static void start_condition() {

	rsetbit(I2C_CR1, 8); //start

}

static void stop_condition() {

	rsetbit(I2C_CR1, 9); //stop
}

static int ack_recv() {

	int cnt = 0;
	while(!(*I2C_SR1 & 0x2)) {
		cnt++;
		if (cnt > TIMEOUT)
			return 0;
	}

	int a = *I2C_SR2;
	return 1;

}

static int buf_empty() {
	int cnt = 0;
	while(!(*I2C_SR1 & 0x80)) {
		cnt++;
		if (cnt > TIMEOUT)
			return 0;
	}

	return 1;
}

static void data_recv() {
	//int cnt = 0;
	while(!(*I2C_SR1 & 0x4)) {
	///	cnt++;
	//	if (cnt > TIMEOUT)
	//		return 0;
	}
	//return 1;
}

static int delay() {

	int a = 0;
	for (int i = 0; i < 0xFFFF; i++)
		a++;
}

void at24c_run() {

//	regw_u32(I2C_CR1, 0x1, 8, SETBIT);
//	uint32_t read_status = *I2C_SR1;

//	regw_u32(I2C_DR, DATASET, 0, OWRITE); 
	// conform DATA
//	read_status = *I2C_SR1;
//	read_status = *I2C_SR2;

	uint32_t statusr;

	start_condition();
	//uint32_t statusr = *I2C_SR1; // clear start_signal
	rwrite(I2C_DR, 0xA0); // write to address CMD
	if(!ack_recv())
		cputs("CAN'T REACH DEVICE");

	rwrite(I2C_DR, 0x00);
	if(!buf_empty())
		cputs("FAIL");
	rwrite(I2C_DR, 0x00);
	if(!buf_empty()) 
		cputs("FAIL");
	//rwrite(I2C_DR, 0x61);
	//if(!buf_empty())
	//	cputs("FAIL");

	//statusr = *I2C_SR1;
	//statusr = *I2C_SR2;
	stop_condition();

//	start_condition();
//	rwrite(I2C_DR, 0xA0); // dummy write
//	if(!ack_recv())
//		cputs("CAN'T REACH DEVICE");
//
//	rwrite(I2C_DR, 0x00);
//	if(!buf_empty())
//		cputs("FAIL");
//	rwrite(I2C_DR, 0x00);
//	if(!buf_empty()) 
//		cputs("FAIL");
//
	delay();

	start_condition(); // restart condition
	rwrite(I2C_DR, 0xA1); // read? to address CMD
	if(!ack_recv())
		cputs("COULDN'T START READ CMD");


	data_recv();
		//cputs("NO RESPONSE");

	char a = (char) *I2C_DR;
	printf("DATA %c\n", a); 

	stop_condition();
	//delay();

	//start_condition();
	//statusr = *I2C_SR1; // clear start_signal
	//regw_u32(I2C_DR, 0xC1, 0, OWRITE);
	//if(!ack_recv())
	//	cputs("TIMEOUT2!");
	//statusr = *I2C_SR1;
	//statusr = *I2C_SR2;
	//regw_u32(I2C_DR, 0x7D, 0, OWRITE);
	//if(!buf_empty())
	//	cputs("TIMEOUT3!");
	//stop_condition();

/*	delay();

	start_condition();
	statusr = *I2C_SR1;
	regw_u32(I2C_DR, DISPLAY_ON, 0, OWRITE);
	if(!ack_recv())
		cputs("TIMEOUT4!");
	stop_condition(); */


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



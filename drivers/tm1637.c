/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/8/14 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Driver for the TM1637 for STM32 boards. 
 * The TM1637 is (4 digit) 8 segment ledclock peripheral. Communication 
 * is similar to I2C, but not completely. There is no device address
 * selecting. Instead, you can directly send a command / write data. 
 *
 * Most libraries for this chip that I have seen use 'Bit banging'.
 * Manually adjusting the clock and data pin (not using the I2C)
 *
 * This driver is different and uses STM32 I2C hardware. A problem of the
 * TM1637 chip, however, is that commands sometimes set bit 1 (LSB). Setting
 * the LSB will make the STM32 MCU switch to receiver mode. (A better design 
 * choice for commands would have to keep the LSB reserved). 
 *
 * Another thing to note is that this chip uses BIG-ENDIANESS
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

#include <drivers/tm1637.h>

#define TIMEOUT 5000

#define DOT true
#define NODOT false

#define WRITE_CMD 0x20

/* STM32F1 microcontrollers do not provide the ability to pull-up SDA and SCL lines. Their
GPIOs must be configured as open-drain. So, you have to add two additional resistors to
pull-up I2C lines. Something between 4K and 10K is a proven value.
*/

void tm1637_init() {

 /* Program the peripheral input clock generate correct timings.
  * Configure the clock control registers CCR
  * Configure the rise time register TRIS
  * Program the I2C_CR1 register to enable the peripheral
  * Enable GPIOB6 and B7*/
 
 rsetbit(RCC_APB1ENR, 21); // enable GPIOB
 rsetbit(RCC_APB2ENR, 3); // enable I2C
 rwrite(GPIOB_CRL, 0xEE444444); // open-drain
 rsetbitsfrom(I2C_CR2, 0, 0x2); // 2 MHz 
 rwrite(I2C_TRISE, 0x3); // MAX = 1000ns, TPCLK1 = 500ns (+1)
 rwrite(I2C_CCR, 0x000A); // standard mode
 rsetbit(I2C_CR1, 0); // enable

}

/* Reset the I2C channels and reinitialize */
void tm1637_reset() {

 rsetbit(I2C_CR1, 15); // master is busy, reset
 rsetbit(RCC_APB1RSTR, 21); // reset I2C
 rwrite(RCC_APB1RSTR, 0x00000000); // clear reset
 rsetbit(RCC_APB1ENR, 21);
 rsetbitsfrom(I2C_CR2, 0, 0x2); // 2 MHz 
 rwrite(I2C_TRISE, 0x3); // MAX = 1000ns, TPCLK1 = 500ns (+1)
 rwrite(I2C_CCR, 0x000A); // standard mode
 rsetbit(I2C_CR1, 0); // enable


}

/* Write value to grid, if no offset is given it writes to the first
 * grid. Grid 1 an 2 have an additional dot that can be set */
int set_grid(uint8_t offset, char value, bool dot) {

	//int (*ack)() = ack_recv; /* Scary function pointer :D */

	if (offset > 3) {
		printf("Offset incorrect");
	}

	// enable dot on display if desired
	if (dot && (offset == 1 || offset == 2)) {
		value = value | 0x1;
	}

	int start_pos_cmd  = 0x03  | (offset & 0x01) << 7 | (offset & 0x2) << 5 ;

	/* Initiate writing routine */
	start_condition();
	rwrite(I2C_DR, WRITE_CMD); 
	if(!ack_recv())
		return -1;

	stop_condition();
	
       	if(!idle())
		return -1;

	/* Set GRID offset */
	start_condition();
	rwrite(I2C_DR, start_pos_cmd); 
	if(!ack_recv())
		return -1;

	stop_condition();

	tm1637_reset();

	/* Write value to segments */
	start_condition();
	rwrite(I2C_DR, value);
	if(!ack_recv())
		return -1;
	stop_condition(); 

	tm1637_reset();

	return 0;
}

/* Turns on/off the led display. The degree of brightness can set [0..7]*/
int set_display(bool on, uint8_t degree) {

	int disp_cmd = 0x1; // off
        if (on) {
		disp_cmd = 0xF1; // TODO 
	}	

	start_condition();
	rwrite(I2C_DR, disp_cmd);
	if(!ack_recv()) {
		printf("Can't switch on display!");
		return -1;
	}
	stop_condition();

	tm1637_reset();

	return 0;

}


void tm1637_example() {

unsigned char display_number[10] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6};


	char love[4]  = { 0x1C, 0xFC, 0x7C, 0x9E };
	
	for (int i = 0; i < 4; i++) {
		set_grid(i, love[i], NODOT);
	}

	set_display(true, 0);

}

/* HELPER ROUTINES */

static void start_condition() {
	rsetbit(I2C_CR1, 8); //start bit
}

static void stop_condition() {
	rsetbit(I2C_CR1, 9); //stop bit
}

/* Wait for an acknowledge from the peripheral */
int ack_recv() {
	int cnt = 0;
	while(!(*I2C_SR1 & 0x2)) {
		cnt++;
		if (cnt > TIMEOUT) {
			printf("Error: no ack (addr bit) received \n");
			return 0;
		}
	}
	uint32_t a = *I2C_SR2; // need to read SR2 register!
	return 1;

}

/* Similar, but SR2 register is not read */
int ack10_recv() {
	int cnt = 0;
	while(!(*I2C_SR1 & 0x8)) {
		cnt++;
		if (cnt > TIMEOUT) {
			printf("Error: no ack (addr10 bit) received \n");
			return 0;
		}
	}
	return 1;

}

/* Check if lines are idle (i.e. stop condition finished)*/
int idle() {
	int cnt = 0;
	while(*I2C_SR2 & 0x2) {
		cnt++;
		if (cnt > TIMEOUT) {
			printf("Error: busy state\n");
			return 0;
		}
	}

	return 1;
}



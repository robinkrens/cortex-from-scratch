/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/8/11 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Driver for AT24C256 EEPROM for STM32 based boards. Communication protocol is I2C
 * Maximum write in limited to 64 bytes. Reading bytes is unlimited, but memory should
 * be allocated accordingly. 
 *
 * I2C protocol : (W)rite or (R)ead 
 * MCU is in master mode: either as transmitter or receiver
 * | P | DEVICEADDR + R/W bit | ACK | ADDR_PART1 | ACK | ADDR_PART2 | (d+a*n) | (N)ACK | S
 * Sending data: wait for ACK from EEPROM
 * Receiving data: send (n)ACK to EEPROM after receiving data
 *
 * STM32F1 microcontrollers do not provide the ability to pull-up SDA and SCL lines. Their
 * GPIOs must be configured as open-drain. So, you have to add two additional resistors to
 * pull-up I2C lines. Something between 4K and 10K is a proven value. 
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

#include <drivers/at24c.h>

#define TIMEOUT 5000
#define READ_CMD  	0xA1
#define WRITE_CMD 	0xA0
#define PAGE 64 	/* Bytes that can be written continiously */
#define BUFFER 64 	/* Reading buffer */


static char eeprombuf[BUFFER];

void eeprom_at24c_init() {

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
 rsetbit(I2C_CR2, 10); // buffer interrupt
 rsetbit(I2C_CR1, 0); // enable


}

/* Writes data to the EEPROM starting at address addr. Size can not
 * be more than one PAGE (64 bytes)  */
int eeprom_write(uint16_t addr, char * data, size_t size) {
	
	if(size > PAGE) {
		printf("Error: Maximum writable page size: %d\n", PAGE);
		return -1;
	}

	uint8_t hi_lo[] = { (uint8_t)(addr >> 8), (uint8_t)addr }; 
	
	start_condition();
	rwrite(I2C_DR, WRITE_CMD);
	if(!ack_recv()) 
		return -1;
	

	rwrite(I2C_DR, hi_lo[0]); // higher part of address
	if(!buf_empty()) 
		return -1;
	
	rwrite(I2C_DR,hi_lo[1]); // lower part of address
	if(!buf_empty()) 
		return -1;
	

	for (int i = 0; i < size; i++) {
		rwrite(I2C_DR, *data++);
		if(!buf_empty()) 
			return -1;
			
	}

	stop_condition();
	delay(); // wait for write action to finish	
	return 0;
}

/* Erase (write all 0xFF) data on EEPROM
 * Maximum hardware allowed sequential writes of 64 bytes */
int eeprom_erase() {
	
	uint16_t cur_addr = 0x0000;

	for (int i = 0; i < 512; i++) {
		
		printf("Writing at address: %#x (64 bytes) ", cur_addr);
		uint8_t hi_lo[] = { (uint8_t)(cur_addr >> 8), (uint8_t)cur_addr }; 
	
		start_condition();
		rwrite(I2C_DR, WRITE_CMD);
		if(!ack_recv()) 
			return -1;
	

		rwrite(I2C_DR, hi_lo[0]); // higher part of address
		if(!buf_empty()) 
			return -1;
	
		rwrite(I2C_DR,hi_lo[1]); // lower part of address
		if(!buf_empty()) 
			return -1;
	

		for (int i = 0; i < PAGE; i++) {
		rwrite(I2C_DR, 0xFF); // write all ones
		if(!buf_empty()) 
			return -1;
			
		}
		stop_condition();
		printf("[COMPLETE]\n");
		cur_addr += 0x40; // 64 bytes; next PAGE
		delay(); // wait for write to finish
	}
	
	return 0;
}

/* Random access read of num bytes
 * Initialize dummy write first to set correct address location
 * The read function differentiate between num = 1 num = 2 and num > 3
 * Data is saved to rvalues */
int eeprom_read(uint16_t addr, int num, char * rvalues) {

	uint8_t hi_lo[] = { (uint8_t)(addr >> 8), (uint8_t)addr }; 

	/* Dummy write to set address */	
	start_condition();
	rwrite(I2C_DR, WRITE_CMD);
	if(!ack_recv())
		return -1;

	rwrite(I2C_DR, hi_lo[0]); // higher part of address
	if(!buf_empty())
		return -1;

	rwrite(I2C_DR,hi_lo[1]); // lower part of address
	if(!buf_empty()) 
		return -1;
	
	stop_condition();
	
	delay(); // wait form EEPROM

	switch(num) {
		case 1:
			start_condition(); // restart condition
			rwrite(I2C_DR, READ_CMD); // read? to address CMD
			if(!ack_recv())
				return -1;
			stop_condition();
	
			if(!data_recv())
				return -1;

			rvalues[0] = (char) *I2C_DR;
			rvalues[1] = '\0';
			break;
		case 2:

			rsetbit(I2C_CR1, 10); // set ACK
			rsetbit(I2C_CR1, 11); // set POS
			start_condition(); // restart condition
			rwrite(I2C_DR, READ_CMD); // read to address CMD
			if(!ack_recv()) 
				return -1;	
			rclrbit(I2C_CR1, 10); // clear ACK
			if(!late_recv()) 
				return -1;
		
			stop_condition();
			
			rvalues[0] = (char) *I2C_DR;
			rvalues[1] = (char) *I2C_DR;
			rvalues[2] = '\0';
			break;

		default:
			rsetbit(I2C_CR1, 10); // set ACK
			start_condition(); // restart condition
			rwrite(I2C_DR, READ_CMD); // read to address CMD
			if(!ack_recv()) 
				return -1;
	
			for(int i = 0; i < num-3; i++) {
				if(!data_recv()) 
					return -1;
				rvalues[i] = (char) *I2C_DR;
			}

			if(!late_recv()) 
				return -1;
		
			rclrbit(I2C_CR1, 10);
			rvalues[num-3] = *I2C_DR;
			stop_condition();
			rvalues[num-2] = *I2C_DR;
			if(!data_recv()) 
				return -1;
		
			rvalues[num-1] = *I2C_DR;
			rvalues[num] = '\0';
	}

	return 0;

}

/* Dump all data on EEPROM to std out */
int eeprom_dump() {

	uint16_t curr_addr = 0x0000;

	for (int i = 0; i < 512; i++) {

		if(eeprom_read(curr_addr, 64, eeprombuf) == -1) {
			printf("Error: Can't (continue) dump");
			return -1;
		}
		printf("%#x:\n", curr_addr);
		for (int i = 0; i < strlen(eeprombuf); i++) {
			printf("%x ", eeprombuf[i]);
			if (((i % 16) == 0) && (i != 0))
				printf("\n");
		}
		printf("\n");
		curr_addr += 0x40; // 64 bytes
	}
	return 0;

}

/* HELPER SUBROUTINES */

static void start_condition() {
	rsetbit(I2C_CR1, 8); //start bit
}

static void stop_condition() {
	rsetbit(I2C_CR1, 9); //stop bit
}

/* Initial ACK received after address lookup 
 * read registers clear ADDR bit */
static int ack_recv() {
	int cnt = 0;
	while(!(*I2C_SR1 & 0x2)) {
		cnt++;
		if (cnt > TIMEOUT) {
			printf("Error: Can't reach device\n");
			return 0;
		}
	}
	int a = *I2C_SR2;
	return 1;
}

/* Check send buffer
 * Note: BLOCKING function */
static int buf_empty() {
	int cnt = 0;
	while(!(*I2C_SR1 & 0x80)) {
		cnt++;
		if (cnt > TIMEOUT) {
			printf("Error: Can't send data\n");
			return 0;
		}
	}
	return 1;
}

/* Check data receive buffer
 * Note: BLOCKING function */
static int data_recv() {
	int cnt = 0;
	while(!(*I2C_SR1 & 0x40)) {
		cnt++;
		if (cnt > TIMEOUT) {
			printf("Error: Timeout receiving data\n");
			return 0;
		}
	}
	return 1;
}

/* Similar as above, waits for two packages to be received
 * one in DR and one in the shadow register */
static int late_recv() {
	int cnt = 0;
	while(!(*I2C_SR1 & 0x4)) {
		cnt++;
		if (cnt > TIMEOUT) {
			printf("Error: Timeout receiving data\n");
			return 0;
		}
	}
	return 1;

}

/* Write delay form EEPROM chip */
static int delay() {
	int a = 0;
	for (int i = 0; i < 0xFFFF; i++)
		a++;
}


int eeprom_test() {
	char * gd = "Testing the EEPROM chip AT24C256 write and read function";
	eeprom_write(0x1000, gd, strlen(gd));
	
	uint16_t curr_addr = 0x1000;

	for (int i = 0; i < 4; i++) {

		if(eeprom_read(curr_addr, 16, eeprombuf) == -1) {
			printf("Can't (continue) dump");
			return -1;
		}
		printf("%#x: ", curr_addr);
		for (int i = 0; i < strlen(eeprombuf); i++) {
			printf("%x ", eeprombuf[i]);
		}
		printf("\n");
		curr_addr += 0x10; // 16 bytes
	}
}


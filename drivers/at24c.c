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
#define PAGE 64 /* Bytes that can be written continiously */
#define BUFFER 4 /* */

/* STM32F1 microcontrollers do not provide the ability to pull-up SDA and SCL lines. Their
GPIOs must be configured as open-drain. So, you have to add two additional resistors to
pull-up I2C lines. Something between 4K and 10K is a proven value. 
*/

static char eeprombuf[BUFFER];

void * cap_handler() {

	printf("a!");
}

void at24c_init() {

 /* Program the peripheral input clock in I2C_CR2 Register
  *  in order to generate correct timings. Configure the clock control registers CCR
 Configure the rise time register TRIS  Program the I2C_CR1 register to enable the peripheral Enable GPIOB6 and B7*/

 rsetbit(RCC_APB1ENR, 21);
 rsetbit(RCC_APB2ENR, 3);
 rwrite(GPIOB_CRL, 0xEE444444);
 rsetbitsfrom(I2C_CR2, 0, 0x2); // 2 MHz 
 rwrite(I2C_TRISE, 0x3); // MAX = 1000ns, TPCLK1 = 500ns (+1)
 rwrite(I2C_CCR, 0x000A); // standard mode， output 100 kHz (100hz* / perip)
 rsetbit(I2C_CR1, 10); // send ack if Master receives data
 rsetbit(I2C_CR2, 10); // buffer interrupt
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

// TODO: interrupt base, so it doesn't block
static void data_recv() {
	while(!(*I2C_SR1 & 0x40)) {
	}
}

static void late_recv() {
	while(!(*I2C_SR1 & 0x4)) {
	}
}

// TODO: polling
static int delay() {

	int a = 0;
	for (int i = 0; i < 0xFFFF; i++)
		a++;
}

int eeprom_write(uint16_t addr, char * data, size_t size) {
	
	if(size > PAGE) {
		printf("Maximum writable page size: %d\n", PAGE);
		return -1;
	}

	uint8_t hi_lo[] = { (uint8_t)(addr >> 8), (uint8_t)addr }; 
	
	start_condition();
	rwrite(I2C_DR, WRITE_CMD);
	if(!ack_recv()) {
		printf("Can't reach device");
		return -1;
	}

	rwrite(I2C_DR, hi_lo[0]); // higher part of address
	if(!buf_empty()) {
		printf("Can't address location");
		return -1;
	}
	rwrite(I2C_DR,hi_lo[1]); // lower part of address
	if(!buf_empty()) {
		printf("Can't address location");
		return -1;
	}

	for (int i = 0; i < size; i++) {
		rwrite(I2C_DR, *data++);
		if(!buf_empty()) {
			printf("Write error");
			return -1;
		}	
	}

	stop_condition();
	
	return 0;
}
/* Random access read based on dummy write  */
int eeprom_read(uint16_t addr, int num) {

	printf("ENTERING");	
	uint8_t hi_lo[] = { (uint8_t)(addr >> 8), (uint8_t)addr }; 

	/* Dummy write to set address */	
	start_condition();
	rwrite(I2C_DR, WRITE_CMD);
	if(!ack_recv()) {
		printf("Can't reach device");
		return -1;
	}

	rwrite(I2C_DR, hi_lo[0]); // higher part of address
	if(!buf_empty()) {
		printf("Can't address location");
		return -1;
	}
	rwrite(I2C_DR,hi_lo[1]); // lower part of address
	if(!buf_empty()) {
		printf("Can't address location");
		return -1;
	}
	stop_condition();
	
	delay(); // NEEDED?


	if (num == 1) {
		start_condition(); // restart condition
		rwrite(I2C_DR, READ_CMD); // read? to address CMD
		if(!ack_recv()) {
			printf("Can't initiate read");
			return -1;
		}
		stop_condition();
		data_recv();
		char c = (char) *I2C_DR;
		printf("DATA: %c\n", c);
	}

	else if (num == 2) {
		rsetbit(I2C_CR1, 10); // set ACK
		rsetbit(I2C_CR1, 11); // set POS
		start_condition(); // restart condition
		rwrite(I2C_DR, READ_CMD); // read to address CMD
		if(!ack_recv()) {
			printf("Can't initiate read");
			return -1;
		}	
		rclrbit(I2C_CR1, 10); // clear ACK
		late_recv();
		stop_condition();
		char c = (char) *I2C_DR;
		char c2 = (char) *I2C_DR;
		printf("DATA: %c,%c\n", c, c2);
	}

	else if (num > 2) {
		rsetbit(I2C_CR1, 10); // set ACK
		start_condition(); // restart condition
		rwrite(I2C_DR, READ_CMD); // read to address CMD
		if(!ack_recv()) {
			printf("Can't initiate read");
			return -1;
		}	
		for(int i = 0; i < num-3; i++) {
			data_recv();
			eeprombuf[i] = (char) *I2C_DR;
		}
		late_recv();
		rclrbit(I2C_CR1, 10);
		eeprombuf[num-3] = *I2C_DR;
		stop_condition();
		eeprombuf[num-2] = *I2C_DR;
		data_recv();
		eeprombuf[num-1] = *I2C_DR;
		eeprombuf[num] = '\0';
		printf("DATA: %s\n", eeprombuf); 
	}



//W 	rsetbit(I2C_CR1, 10); // send ack if Master receives data
//W//	data_recv();
//W//	char c = *I2C_DR;
//W//	printf("%d:%p\n", addr, c);
//W	for (int i = 0; i < BUFFER ; i++ ) {
//W		data_recv();
//W		buf[i] = (char) *I2C_DR;
//W	}
//W
//W	printf("%p, %p, %p, %p\n", *I2C_SR1, *I2C_SR2, *I2C_DR, *I2C_CR1);
//	printf("DATA: %s\n", buf);

}

void at24c_run() {

//	char * gd = "abcd";
//	eeprom_write(0x0000, gd, strlen(gd));

//	delay();
//	char * global_data = "abcdefghijklmnop";

//	eeprom_write(0x0080, global_data, strlen(global_data));
	
//	delay();

//	for (int i = 0; i < 0xFFFF; i++) { 
//		eeprom_read(i);
//		delay();
//	}

	//delay();

//	delay();
//	eeprom_read(0x0000, 1);
//	delay();
//	eeprom_read(0x0000, 2);
	delay();
	eeprom_read(0x0000, 4);
	delay();

//W	uint32_t statusr;
//W
//W	start_condition();
//W	rwrite(I2C_DR, WRITE_CMD); // write to address CMD
//W	if(!ack_recv())
//W		cputs("CAN'T REACH DEVICE");
//W
//W	rwrite(I2C_DR, 0x00);
//W	if(!buf_empty())
//W		cputs("FAIL");
//W	rwrite(I2C_DR, 0x03);
//W	if(!buf_empty()) 
//W		cputs("FAIL");
//W	//rwrite(I2C_DR, 0x61);
//W	//if(!buf_empty())
//W	//	cputs("FAIL");
//W
//W	//statusr = *I2C_SR1;
//W	//statusr = *I2C_SR2;
//W	stop_condition();

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
//W	delay();
//W
//W	start_condition(); // restart condition
//W	rwrite(I2C_DR, 0xA1); // read? to address CMD
//W	if(!ack_recv())
//W		cputs("COULDN'T START READ CMD");
//W
//W
//W	data_recv();
//W		//cputs("NO RESPONSE");
//W
//W	char a = (char) *I2C_DR;
//W	printf("DATA %c\n", a); 
//W
//W	stop_condition();
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



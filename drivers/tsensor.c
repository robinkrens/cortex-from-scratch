/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/8/28 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * DS18B20 temperature sensor implementation
 * Uses the 1-wire protocol. You will need to setup the temperature sensor
 * with an external pull up resistor. High is the idle state of the chip.  
 * This implementation does not use parasite power. Uses busy waits. 
 * The chip doesn't require too accurate timings. Busy waits are between 15
 * and 240 microseconds.
 *
 * The temperature conversion on the chip it takes a lot longer (up to 700ms).
 * In case temperature is often read, implementing this with interrupts might be
 * worthwile.
 *
 * Each series of commands is initiated with a long reset and prescence pulse.
 * Implemented:
 * 	- Read ID of Chip
 * 	- Convert and read temperature
 *
 * TODO:
 * 	- Scratchpad copy functionality (+EEPROM)
 * 	- Alarm functionality
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

/* Commands */
#define READ_ROM	0x33
#define SKIP_ROM	0xCC
#define READ_SCRATCH	0xBE
#define CONVERT_T 	0x44

/* Basic GPIO settings for output pulses and input */
static void in_conf() {
	rwrite(GPIOB_CRL, 0x44444444);
}

static void out_conf() {
	rwrite(GPIOB_CRL, 0x46444444); // open drain (with external pullup resistor)
}

/* Send command cmd over data wire. Each write slot should be
 * at least 60ms.   */
static void send_cmd(unsigned char cmd) {

	int pos = 0;

	for (int i = 0; i < 8; i++) {
		// initiate write slot
		out_conf();
		rclrbit(GPIOB_ODR, 6); // pull low

		// writing a logical 1 or 0 
		if ((cmd >> pos) & 0x01) {
			_block(5);
			in_conf();
			_block(60);
		}
		else {
			_block(60);
			in_conf();
		}
		pos++;
	}
}

/* Read reply of sensor. Depending on the command, the sensor
 * can send back up to 9 bytes */
static char get_byte() {

	char c = 0x00;

	for (int i = 0; i < 8; i++) {
		/* Initate write slot*/
		out_conf();
		rclrbit(GPIOB_ODR, 6);
		_block(3);
		/* Listen for reply */
		in_conf();
		if (rchkbit(GPIOB_IDR,6)) {
			c = c | (0x1 << i);
		}
		else {
			c = c | (0x0 << i);
		}
		/* Each read slot should be at least 60 microseconds
		 * before the next one is initiated */
		_block(60);
	}
	return c;
}


/* Initiate the sensor, send a reset pulse and wait consequently for 
 * presence pulse. Slots should be at least 450 and 240 microseconds.  
 * */
static int tsensor_init() {

	rsetbit(RCC_APB2ENR, 3); // GPIOB enable
	
	/* send presence pulse */
	out_conf();	
	rclrbit(GPIOB_ODR, 6); // pull low
	_block(450);	
	/* wait for the chips reply */
	in_conf();
	_block(60);
	if (!rchkbit(GPIOB_IDR, 6)) {
		//printf("Info: sensor detected\n");
		//get_id();
	}
	else {
		printf("Error: no temperature sensor found!");
		return -1;
	}
	_block(240); // finish intialization slot
	
	return 0;

} 




/* Wait for the chip to convert the temperature */
static void wait_tconvert() {

	printf("Info: Converting temp\n");
	/* initiate write slot */
	out_conf();
	rclrbit(GPIOB_ODR, 6);
	_block(3);
	in_conf();
	while (!rchkbit(GPIOB_IDR, 6)) {
		//printf(".");
		_block(60);
		out_conf();
		rclrbit(GPIOB_ODR, 6);
		_block(3);
		in_conf();
	}
}

/* Print some serial and CRC information about the chip */
void tsensor_printid() {

	tsensor_init();
	send_cmd(READ_ROM);

	// replies with 8 bytes
	int nbytes = 8;

	char scratchbuf[nbytes];
	memset(&scratchbuf, 0, sizeof(char) * nbytes);
	scratchbuf[nbytes+1] = '\n';
	
	for (int i = 0; i < nbytes; i++) {
		scratchbuf[i] = get_byte();
	}

	
	printf("Family Code: %#x\n", scratchbuf[0]);
	printf("Serial Number: 0x");
	for (int i = 1; i < 7; i++) {
		printf("%x", scratchbuf[i]);
	}
	printf("\n");
	printf("CRC Code: %#x\n", scratchbuf[7]);

}

/* Convert and read temperature of chip. Sensor has to be initialized twice
 * since there are two series of commands (see datasheet flowchart) */
uint16_t tsensor_get_temp() {

	/* initialize sensor and send commands */
	tsensor_init();
	send_cmd(SKIP_ROM);
	send_cmd(CONVERT_T);
	wait_tconvert();

	tsensor_init();
	send_cmd(SKIP_ROM);
	send_cmd(READ_SCRATCH);

	/* Scratchpad is 9 bytes, but we are only interested in 
	 * the first two bytes, since they contain the LSB and 
	 * MSB of temperature  */
	int nbytes = 2;

	char scratchbuf[nbytes];
	memset(&scratchbuf, 0, sizeof(char) * nbytes);
	scratchbuf[nbytes+1] = '\n';
	
	for (int i = 0; i < nbytes; i++) {
		scratchbuf[i] = get_byte();
	}

	// LSB first four bits are after floating point
	// uint8_t lsb_afltp = scratchbuf[0] & 0xF;
        
	int8_t lsb_bfltp = (scratchbuf[0] >> 4) & 0xF;
	// MSB only the first three bits are used
	uint8_t msb = scratchbuf[1] & 0x7;
	
	return  (msb << 4) + lsb_bfltp;
	
}

/* 
void test() {

	//get_id();
	uint16_t temp =	get_temp();
	printf("Current temperature: %d\n", temp);

} */

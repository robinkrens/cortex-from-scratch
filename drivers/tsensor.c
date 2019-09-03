/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/8/28 - ROBIN KRENS	
 * PreInitial version 
 * 
 * $DESCRIPTION$
 * TIMERS, non-blocking...
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

#define PRESCALER 8 // 1 MHz (1 microsecond)
#define MAXBUF 10

int cnt;
enum status { INIT, WAIT_INIT, INIT_DONE } current_status;
enum rstatus { READ, READ_INIT, READ_DONE } read_status;

static struct {
	uint8_t buf[MAXBUF];
	uint8_t pos;
} readbuf;

static struct {
	char cmd;
	uint8_t pos;
} sensor_cmd;

//
void read_init();

static void in_conf() {
	rwrite(GPIOB_CRL, 0x44444444);
}

static void out_conf() {
	rwrite(GPIOB_CRL, 0x46444444); // open drain (with pullup resistor)
}

/* set preload and generate update event */
static void timer_config(uint16_t preload) {
	rwrite(TIM4_ARR, preload);
	rsetbit(TIM4_EGR, 0);
}

static void presence_pulse_conf() {

	current_status = INIT;
	out_conf();
	rclrbit(GPIOB_ODR, 6); // low
	timer_config(480); // > 480 us
}

static void presence_reply_conf() {

	current_status = WAIT_INIT;
	in_conf();
	timer_config(60); // > 60 us
}

static void finish_init() {
	current_status = INIT_DONE;
	timer_config(480);
} 

/* Handlers for read, write and init pulses */

void * write_handler() {



	// i
	// in_conf();



	rclrbit(TIM4_SR1, 0);
	rclrbit(TIM4_SR1, 1);
	if (sensor_cmd.pos < 7) {
	
		if ((sensor_cmd.cmd >> sensor_cmd.pos+1) & 0x01) {
			printf("1\n");
			rwrite(TIM4_CCR1, 5);
			rsetbit(TIM4_EGR, 0);
		}
			
		else {
			printf("0");
			rwrite(TIM4_CCR1, 60);
			rsetbit(TIM4_EGR, 0);
	
		}
		sensor_cmd.pos++;
	}
	else {

		read_init();
	}
	
}

void * reply_handler() {


	cnt++;
	if (cnt > 16) {
		for(;;);
	}

	rclrbit(TIM4_SR1, 0);
	rclrbit(TIM4_SR1, 1);
	switch(read_status) {
		case(READ_INIT):
			in_conf();
			read_status = READ;
			rsetbit(GPIOB_BSRR, 22); // low (<- reset) 
			if (rchkbit(GPIOB_IDR, 6)) {
					printf("h\n");
			}
			else {
				printf("l\n");
			}
			timer_config(60);
			break;
		case(READ):
			out_conf();
			rclrbit(GPIOB_ODR, 6); // low
			read_status = READ_INIT;
			timer_config(1);
			break;
		case(READ_DONE):
			// terminate
			break;
	}


} 


void read_init() {


	rclrbit(TIM4_CR1, 0); // stop
	rclrbit(TIM4_CCER, 0);
	rclrbit(TIM4_CCER, 1);

	//rwrite(GPIOB_CRL, 0x46444444); // floating
	out_conf();
	rsetbit(TIM4_CR1, 2); // only overflow generates update
	read_status = READ_INIT;
	timer_config(1); // init 1us

	ivt_set_gate(46, reply_handler, 0);
	rsetbit(NVIC_ISER0, 30); // interupt 41 - 32
	
	//rclrbit(TIM4_DIER, 0);

	rclrbit(GPIOB_ODR, 6); // low
	rsetbit(TIM4_DIER, 0);

	rsetbit(TIM4_CR1, 0);

}


void write_init() {

	sensor_cmd.cmd = 0x33;
	sensor_cmd.pos = 0;

	rsetbit(RCC_APB2ENR, 3); // GPIOB enable
	rsetbit(RCC_APB1ENR, 2); // TIM4 enable
	rsetbitsfrom(TIM4_CR1, 5, 0x00); // edge-aligned mode
	rclrbit(TIM4_CR1, 4); // upcounter (clrbit! not needed to set)
	rsetbit(TIM4_CR1, 2); // only overflow generates update
	rwrite(TIM4_PSC, PRESCALER - 1);
	rwrite(GPIOB_CRL, 0x4A444444);
	

	timer_config(61);	

	if ((sensor_cmd.cmd >> sensor_cmd.pos) & 0x01) {
		printf("1\n");
		rwrite(TIM4_CCR1, 5); // < 15ms
	}
	else {
		printf("0\n");
		rwrite(TIM4_CCR1, 60);
	}

	rsetbitsfrom(TIM4_CCMR1, 4, 0x6); // forced high on match
	
	rsetbit(TIM4_CCER, 0);
	rsetbit(TIM4_CCER, 1);
	
	// set write handler
	ivt_set_gate(46, write_handler, 0);
	rsetbit(NVIC_ISER0, 30); // interupt 41 - 32
	
	//rsetbit(TIM4_DIER, 1);
	rsetbit(TIM4_DIER, 0);
	rsetbit(TIM4_CR1, 0);

}


void * init_handler() {

	switch(current_status) {
			case (INIT):
				printf("M: reset\n");
				presence_reply_conf();
				break;
			case (WAIT_INIT):
				if (!rchkbit(GPIOB_IDR, 6)) {
					printf("S: yes\n");
				}
				else {
					printf("S: no\n");
				}
				finish_init();
				break;

			case (INIT_DONE): 
				printf("M: fin\n");
				write_init();
				break;

			default:
				printf("no status\n");
			}

	rclrbit(TIM4_SR1, 0);
} 

/* TODO: write
 * uint8_t cmd = 0x33
 * if (cmd & 0x01)
 * 	1 slot
 * else 0 slot
 * cmd = cmd >> 1
 *
 * read, similar as pulse response */


void * bare_handler() {

//w2	cnt += 1;
//w2	printf("CHECKING STATUS\n");
//w2
//w2	if(rchkbit(GPIOB_IDR, 6)) {
//w2		printf("port high\n");
//w2	}
//w2	else {
//w2		printf("port low\n");
//w2	}
	

	cnt += 1;
	printf("Count event %d\n", cnt);
	int switchled = cnt % 2;
	if (switchled) {
		rwrite(GPIOB_CRL, 0x46444444); //  open drain general for sensor?
		printf("setting low\n");
		rclrbit(GPIOB_ODR, 6); // low
	}
	else {
		printf("pulling high \n");
		rwrite(GPIOB_CRL, 0x44444444); //  open drain general for sensor?
		//rsetbit(GPIOB_ODR, 6); // high
		
	}
	rclrbit(TIM4_SR1, 0);
}

void send_cmd(unsigned char cmd) {


	int pos = 0;

	for (int i = 0; i < 8; i++) {
		out_conf();
		rclrbit(GPIOB_ODR, 6);
		if ((cmd >> pos) & 0x01) {
			_block(5);
			in_conf();
			_block(60);
		//	printf("1");
		}
		else {
			_block(60);
			in_conf();
		//	printf("0");
		}
		pos++;
	}
}

void read_reply() {

	for (int i = 0; i < 64; i++) {
		out_conf();
		rclrbit(GPIOB_ODR, 6);
		_block(3);
		in_conf();
		if (rchkbit(GPIOB_IDR,6))
		{
			printf("1");
		}	
		else {
			printf("0");
		}
		_block(60);
	}

}

void tsensor_init() {

	rsetbit(RCC_APB2ENR, 3); // GPIOB enable
	out_conf();	
	rclrbit(GPIOB_ODR, 6); // loq 
	_block(450);	
	in_conf();
	_block(60);
	if (!rchkbit(GPIOB_IDR, 6)) {
		printf("reply");
	}
	else {
		printf("nothing");
	}
	_block(240);


//	rsetbit(RCC_APB2ENR, 3); // GPIOB enable
//	rsetbit(RCC_APB1ENR, 2); // TIM4 enable
//
//	rsetbitsfrom(TIM4_CR1, 5, 0x00); // edge-aligned mode
//	rclrbit(TIM4_CR1, 4); // upcounter (clrbit! not needed to set)
//	rsetbit(TIM4_CR1, 2); // only overflow generates update
//
//	rwrite(TIM4_PSC, PRESCALER - 1);
//	presence_pulse_conf();
////	rwrite(TIM4_ARR, preload);
////	rsetbit(TIM4_EGR, 0);
//	
//	ivt_set_gate(46, init_handler, 0);
//	rsetbit(NVIC_ISER0, 30); // interupt 41 - 32
//
////w	rsetbit(GPIOB_ODR, 6); // 
//	rsetbit(TIM4_DIER, 0);
//	rsetbit(TIM4_CR1, 0); // start

} 

void wait_for_sensor() {

	out_conf();
	rclrbit(GPIOB_ODR, 6);
	_block(3);
	in_conf();
	while (!rchkbit(GPIOB_IDR, 6)) {
		printf("c");
		_block(60);
		out_conf();
		rclrbit(GPIOB_ODR, 6);
		_block(3);
		in_conf();
	}
}

void run() {

	cnt = 0;
//w2	rsetbit(RCC_APB2ENR, 3);
//w2	rwrite(GPIOB_CRL, 0x48444444); // input with pull up down
//w2	tsensor_simple(5000); 

	//cnt = 0;
	//rsetbit(RCC_APB2ENR, 3); // GPIOB enable
	//rwrite(GPIOB_CRL, 0x46444444); //  open drain general for sensor?

	//rsetbit(GPIOB_BSRR, 22); // low (<- reset) 

	tsensor_init();
       	send_cmd(0xCC);
	send_cmd(0x44);
	wait_for_sensor();

	tsensor_init();	
	send_cmd(0xCC);
	send_cmd(0xBE);
	
	//send_cmd(0xCC);
	//send_cmd(0x44);
	//wait_for_sensor();	
	//send_cmd(0xBE);
	//wait_for_sensor();
	read_reply();
	//send_cmd(0x33);
	//read_reply();
//	write_init();

//	tsensor_output(580, 520);
//	reset();
//	tsensor_simple(580);
}

//void tsensor_output(uint16_t preload, uint16_t compare/*, uint16_t pulses */) {
//	/* GPIO AND CLOCK */
//	rsetbit(RCC_APB2ENR, 3); // GPIOB enable
//	rwrite(GPIOB_CRL, 0x4A444444); // PB6 for Channel 1 TIM4 alternate 
//	rsetbit(RCC_APB1ENR, 2); // TIM4 enable
//	
//	rsetbitsfrom(TIM4_CR1, 5, 0x00); // edge-aligned mode
//	rclrbit(TIM4_CR1, 4); // upcounter (clrbit! not needed to set)
//	rsetbit(TIM4_CR1, 2); // only overflow generates update
//
//	rwrite(TIM4_PSC, PRESCALER - 1); // 1 MHz
//	rwrite(TIM4_ARR, preload); // preload 
//	rwrite(TIM4_CCR1, compare); // compare
//	//rwrite(TIM4_RCR, pulses - 1); /* repeat ONLY IN ADVANCED TIMER */
//	
//	rsetbit(TIM4_EGR, 0); // update generation  
//	
//	rsetbit(TIM4_CR1, 3); // one pulse mode
//	rsetbitsfrom(TIM4_CCMR1, 4, 0x6); // mode
//	
//	//rsetbit(TIM4_CCMR1, 3); // preload enable
//	//rsetbit(TIM4_CR1, 7); // buffered
//
//	rsetbit(TIM4_CCER, 0); // enable output channeli 1
//	rsetbit(TIM4_CCER, 1); // active low
//	rsetbit(TIM4_CR1, 0); // start counter
//
//	/* INTERRUPTS */	
//	ivt_set_gate(46, tmp_update_handler, 0);
//
//	rsetbit(TIM4_DIER, 1);
//	rsetbit(NVIC_ISER0, 30); // interupt 41 - 32
//	
//} 

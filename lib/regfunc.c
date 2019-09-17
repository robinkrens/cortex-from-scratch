/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Helper functions to set registers 
 *
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <lib/regfunc.h>
#include <lib/string.h>
#include <lib/stdio.h>

#include <lib/tinyprintf.h>

#include <sys/mmap.h>
#include <sys/robsys.h>

// register set bit at position
void rsetbit(volatile uint32_t * reg, short pos) {
	*reg = *reg | (0x1 << pos);
}

// register set bits from certain pos
void rsetbitsfrom(volatile uint32_t * reg, short pos, int val) {
	*reg = *reg | (val << pos);
}

// register clear bit at position
void rclrbit(volatile uint32_t * reg, short pos) {
	*reg = *reg & ~(0x1 << pos);
}

// check if a bit is set 
int rchkbit(volatile uint32_t * reg, short pos) {
	if ((*reg >> pos) & 0x1)
		return 1;
	return 0;
}

// register (over)write
void rwrite(volatile uint32_t * reg, uint32_t val) {
	*reg = val;
}


/* Deprecated use printf instead
char hexbuf[8];
char * regtohex(uint32_t addr) {
	char tmpbuf[6] = {'A', 'B', 'C', 'D', 'E', 'F'};
	memset(&hexbuf, 0, sizeof(uint32_t) * 8);


	for (int i = 0; i < 8 ; i++) {
		uint32_t tmp = addr;
	 	tmp = tmp >> (28 - (i * 4));
		tmp = tmp & 0xF;
		if ((tmp >= 0) && tmp < 10) {
			hexbuf[i] = (char) tmp + 48;
		}
		else {
			hexbuf[i] = tmpbuf[tmp - 10];
		}
	}
	return &hexbuf[0];	
} */

// TODO: implement simple scanf functions
int singlehextoreg(char  hex) {

	int conv = 0;
	if (hex >= 'A' && hex <= 'F') 
		conv = hex - '7';

	else {
		conv = hex - '0';
	}
	return conv;

}

uint32_t hextoreg(char * a) {
	
	uint32_t x = 0;
	int tmp;
	for(int i = 0; i < 8; i++) {
		tmp = singlehextoreg(*a++);
	 	x += tmp << (28 - (i * 4));
	}
	return x;

}

/* Busy-loop block implementation. Each iteration will take 3 CPU cycles.
 * Of course, when interrupts are enabled, the exact delay time will be 
 * uncertain. 
 *
 * Example: for a standard STM32x config board (8MHz) the maximum delay is
 * 0xFFFF * (1/8,000,000) * 3 = 24.58ms 
 * 0xFFFFFFFF * (1/8MHz) * 3 = 1610ms
 * */
static void __block(uint32_t count) {

	asm volatile("b1: subs %0, %1, #1" "\n\t"
		"bne b1" : "=r" (count) : "r" (count));
}

/* Delay us microsecond
 * Note: delay includes setup time (about 4 clockcycles), so is quite
 * inaccurate  */
void _block(uint32_t us) {
	
	uint32_t count = (us/3) * CLKSPEED_MHZ; // x cycles 
	__block(count);

}


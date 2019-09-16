/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/9/14 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * 
 * $USAGE$
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

#include <drivers/st7735s.h>

void tft_command();
void tft_init() {

	/* Peripherial init */
	rsetbit(RCC_APB1ENR, 14); // enable SPI2
	rsetbit(RCC_APB2ENR, 3); // enable GPIOB

	/* The PINS used are respective PB12, PB13 and PB15 
	 * NSS (or CS): alternative function pusp-pull
	 * NSS Output is (always high) enabled with this setting
	 * SCK Master: alternate function push-pull
	 * MOSI (or DI): alternate function push-pull */ 
	rwrite(GPIOB_CRH, 0xA4AA4444);

	/* Chip select: software enabled */
	rsetbit(SPI2_CR1, 9);
	rsetbit(SPI2_CR1, 8);

	// LOW and first edge: standard settings	

	rsetbit(SPI2_CR1, 15); // one-wire mode
	rsetbit(SPI2_CR1, 14); // start with transfer
	rsetbit(SPI2_CR1, 4); // FPLCK div 8
	rsetbit(SPI2_CR1, 2); // master selection
	rsetbit(SPI2_CR1, 6); // enable SPI

	tft_command();	

}


void tft_command() {

	/* 9-bit hack */
	uint8_t cmd = 0x0A;

	rwrite(SPI2_DR, cmd);
	while (!rchkbit(SPI2_SR, 1));
	rclrbit(SPI2_CR1, 14); // receive
}

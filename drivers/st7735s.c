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
#include <stdarg.h>

#include <sys/mmap.h>
#include <sys/robsys.h>

#include <lib/regfunc.h>
#include <lib/string.h>
#include <lib/tinyprintf.h>

#include <drivers/st7735s.h>

#define TIMEOUT 500

void tft_test();
int tft_command(uint8_t cmd, int argsc, ...);
void tft_init() {

	/* Peripherial init */
	rsetbit(RCC_APB1ENR, 14); // enable SPI2
	rsetbit(RCC_APB2ENR, 3); // enable GPIOB
	rsetbit(RCC_APB2ENR, 4); // enable GPIOC

	/* The PINS used are PB12, PB13, PB15 and PC6 respectively
	 * NSS (or CS): alternative function pusp-pull
	 * NSS Output is (always high) enabled with this setting
	 * SCK Master: alternate function push-pull
	 * MOSI (or DI): alternate function push-pull 
	 * D/CX (or A0): Command or data write line PC6 */
	rwrite(GPIOB_CRH, 0xA4AA4444);
	rwrite(GPIOC_CRL, 0x42444444);

	/* Chip select: software enabled
	 * In case for a hardware setup, connect NSS (CS) to 
	 * ground  */
	rsetbit(SPI2_CR1, 9);
	rsetbit(SPI2_CR1, 8);

	rsetbit(SPI2_CR1, 15); // one-wire mode
	rsetbit(SPI2_CR1, 14); // start with transfer
	rsetbit(SPI2_CR1, 4); // FPLCK div 8
	rsetbit(SPI2_CR1, 2); // master selection
	rsetbit(SPI2_CR1, 6); // enable SPI

	/* Init sequence */
	tft_command(TFT_SWRESET, 0);
	_block(120000); 
	tft_command(0x11, 0);
	_block(120000); 

	/* Frame rate control */
	tft_command(TFT_FRMCTR1, 3, 0x01, 0x2C, 0x2D);
	tft_command(TFT_FRMCTR2, 3, 0x01, 0x2C, 0x2D);
	tft_command(TFT_FRMCTR3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D);	

	/* Power control */
	tft_command(TFT_PWCTR1, 3, 0xA2, 0x02, 0x84);
	tft_command(TFT_PWCTR2, 1, 0xC5);
	tft_command(TFT_PWCTR3, 2, 0x0A, 0x00);
	tft_command(TFT_PWCTR4, 2, 0x8A, 0x2A);
	tft_command(TFT_PWCTR5, 2, 0x8A, 0xEE);
	tft_command(TFT_VMCTR1, 1, 0x0E);


	tft_command(TFT_INVOFF, 0);
	tft_command(TFT_COLMOD, 1, 0x05); // 0x05
	tft_command(TFT_MADCTL, 1, 0xC0); // TODO: check

	tft_command(TFT_CASET, 4, 0x00, 0x00, 0x00, 0x7F);
	tft_command(TFT_RASET, 4, 0x00, 0x00, 0x00, 0x9F);

	/* Gamma Settings */
	tft_command(TFT_GMCTRP1, 16, 0x02, 0x1C, 0x07, 0x12,
			0x37, 0x32, 0x29, 0x2D,
			0x29, 0x25, 0x2B, 0x39,
			0x00, 0x01, 0x03, 0x10);
	tft_command(TFT_GMCTRN1, 16, 0x03, 0x1D, 0x07, 0x06,
			0x2E, 0x2C, 0x29, 0x2D,
			0x2E, 0x2E, 0x37, 0x3F,
			0x00, 0x00, 0x02, 0x10);

	/* Turn on */
	tft_command(TFT_NORON, 0);
	_block(10000);
	tft_command(TFT_DISPON, 0);
	_block(100000);

	/* Test pixel */
	//tft_command(.., 3, 0xFC, 0xFC, 0xFC);

	for (int i = 0; i < 100; i ++) {
		tft_command(TFT_CASET, 4, 0x00, i, 0x00, i+1);
		tft_command(TFT_RASET, 4, 0x00, i, 0x00, i+1);
		tft_command(TFT_RAMWR, 2, 0xFF, 0xFF);
	}
	
	/* tft_command(TFT_RAMWR, 100,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0
			);

	_block(10000); */

	tft_command(TFT_CASET, 4, 0x00, 0x08, 0x00, 0x09);
	tft_command(TFT_RASET, 4, 0x00, 0x08, 0x00, 0x09);
	tft_command(TFT_RAMRD, 0);

	//tft_command(0x0C, 0);
	//tft_command(0x0A, 0);
	
	rclrbit(SPI2_CR1, 14); // receive

        while(!rchkbit(SPI2_SR, 0));
        uint8_t chip_id = *SPI2_DR;
        printf("COLMOD: %#x\n", chip_id);

	rclrbit(SPI2_CR1, 8); // deselect
}

/* Helper function */
static int txbuf_empty () {
	int cnt = 0;
        while(!rchkbit(SPI2_SR, 1)) {
                cnt++;
                if (cnt > TIMEOUT) {
                        printf("Error: transmit timeout!\n");
                        return 0;
                }
        }
        return 1;

}



int tft_command(uint8_t cmd, int argsc, ...) {

	va_list ap;

	// command
	rclrbit(GPIOC_ODR, 6);
	rwrite(SPI2_DR, cmd);
	if (!txbuf_empty()) 
		return -1;

	// parameter or data
	if (argsc > 0) {
		
		va_start(ap, argsc);

		rsetbit(GPIOC_ODR, 6);	
		for (int i = 0; i < argsc; i++) {
			uint8_t p = (uint8_t) va_arg(ap, unsigned int);
			rwrite(SPI2_DR, p);
			if (!txbuf_empty())
				return -1;
			//printf("%c", (uint8_t) va_arg(ap, unsigned int));
		}

		va_end(ap);
	}

	return 0;
}	


/* write command: CS stay low
 * read command: */

void tft_test() {

	
	// DC is data/command pin set and reset
	
	/* uint8_t cmd = 0x11;
	rwrite(SPI2_DR, cmd);
	while (!rchkbit(SPI2_SR, 1));

	for (int i = 0; i < 10; i++)
		_block(0xFFFF); */	

	uint8_t cmd = 0x3A;
	uint8_t p1 = 0x5;

	// command
	rwrite(SPI2_DR, cmd);
	while (!rchkbit(SPI2_SR, 1)); // check line busy?


	// paramater 
	rsetbit(GPIOC_ODR, 6);
	rwrite(SPI2_DR, p1);
	while (!rchkbit(SPI2_SR, 1)); // 

	cmd = 0x0C;
	rclrbit(GPIOC_ODR, 6);
	rwrite(SPI2_DR, cmd);
	while (!rchkbit(SPI2_SR, 1)); // 	

	rclrbit(SPI2_CR1, 14); // receive

	while(!rchkbit(SPI2_SR, 0));
	uint8_t chip_id = *SPI2_DR;
	printf("Chip id: %#x\n", chip_id);


}

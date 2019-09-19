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
#include <lib/fonts/wogfont.h>

#include <drivers/st7735s.h>

#define TIMEOUT 500
#define SCRWIDTH 132
#define SCRHEIGHT 132
#define STARTX 5
#define STARTY 5
#define XPOS(x) (x * 6)
#define YPOS(y) (y * 8)
#define BUFFER 352

static struct {
	 uint16_t cpos;
	 uint8_t * textmemptr;
       	 uint8_t buf[BUFFER];
         uint8_t x;
       	 uint8_t y;
} tftscreen;

void tft_init() {

	tftscreen.x = 0;
	tftscreen.y = 0;
	tftscreen.cpos = 0;
	tftscreen.textmemptr = tftscreen.buf;

	memset(tftscreen.buf, 0x00, 352); 

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

	/* Before turning on the display, fill the display
	 * so no random display data is shown */
	tft_fill(0,0,SCRWIDTH-1,SCRHEIGHT-1,0x0000);

	/* Turn on */
	tft_command(TFT_NORON, 0);
	_block(10000);
	tft_command(TFT_DISPON, 0);
	_block(100000);
	
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


/* Function to fill an area starting at (beginx, beginy)
 * and end ending at (endx, endy */
int tft_fill(uint8_t beginx, uint8_t beginy, uint8_t endx,
	       	uint8_t endy, uint16_t color) {

	/* The CASET and RASET commands set the begin X/Y
	 * and end X/Y of the fill area. Both are split over 
	 * two paramters/bytes. Most screen are small, so you will 
	 * only need to consider the first 8 MSBs. Parameter 1 and 
	 * 3 are always 0. 
	 *
	 * In theory, TFT screens of 65,535 by 65,535 could be
	 * supported */
	tft_command(TFT_CASET, 4, 0x00, beginx, 0x00, endx);
	tft_command(TFT_RASET, 4, 0x00, beginy, 0x00, endy);

	/* After setting the fill area, we can send the color
	 * data. The chip autoincrements the address so we can
	 * keep writing continiously for the fill area. Note
	 * that each pixel requires two bytes to be send (16 
	 * bit mode: ...)   */
	uint32_t totalwrites = (endx - beginx) * (endy - beginy) * 2;
	//printf("tw: %d, x: %d, y: %d\n", totalwrites, beginx, beginy);
	tft_command(TFT_RAMWR, 0);
	rsetbit(GPIOC_ODR, 6); // data = 1	
	for (int i = 0; i < totalwrites; i++) {
			rwrite(SPI2_DR, (uint8_t) (color >> 8));
			if (!txbuf_empty())
				return -1;
			rwrite(SPI2_DR, (uint8_t) (color & 0xFF));
			if (!txbuf_empty())
				return -1;
	}
	return 0;
}

/* Function to individually set a pixel 
 * Refer to tft_fill, similar calls  */
int tft_setpixel(uint8_t x, uint8_t y, uint16_t color) {

	tft_command(TFT_CASET, 4, 0x00, x, 0x00, x+1);
	tft_command(TFT_RASET, 4, 0x00, y, 0x00, y+1);
tft_command(TFT_RAMWR, 2, (uint8_t) (color >> 8), (uint8_t) (color & 0xFF));
	return 0;
}


int tft_puts(char * str) {

     	for (int i = 0; i < strlen(str); i++)  {
		tft_putc(0xFFFF, 0x0000, str[i]);

	}
	
}

void tft_clrln() {

	tft_puts("                     ");
	tftscreen.buf[BUFFER - 21] = '\0';
	tftscreen.cpos -= 21;
	tftscreen.y = 14;
}

/* Text scroll function 
 * Refeed the buffer */
int tft_scroll() {

	/* Scroll the buffer  */
	memcpy(tftscreen.textmemptr, tftscreen.textmemptr + 21, BUFFER - 21);
	for (int i = 21; i >= 0; i--)
		tftscreen.buf[BUFFER - 21] = '\0';

	tftscreen.x = 0;
	tftscreen.y = 0;
	tftscreen.cpos = 0;
	
	tft_puts(tftscreen.buf); // CHECK: ending
	tft_clrln();
}

void tft_linefill() {

	// TODO
	
}

/* Low-level function to print a character to the display
 * Should not be used directly */
int tft_putc(uint16_t fg, uint16_t bg, int c) {

	
	int totalpixels = 35;
	int column = 0;
	int row = 0;
	uint8_t current;

	if (tftscreen.y >= 15) {
		tft_scroll();
	}

	tft_command(TFT_CASET, 4, 0x00, STARTX + XPOS(tftscreen.x), 0x00, (STARTX + 4) + XPOS(tftscreen.x));
	tft_command(TFT_RASET, 4, 0x00, STARTY + YPOS(tftscreen.y), 0x00, (STARTY + 6) + YPOS(tftscreen.y));
	
	tft_command(TFT_RAMWR, 0);
	rsetbit(GPIOC_ODR, 6); // data = 1	
	for (int i = 0; i < totalpixels; i++) {
		
		current = ASCII5x7[(c * 5) + column]; 

		if ((current >> (7 - row)) & 0x1) {
			rwrite(SPI2_DR, (uint8_t) (fg >> 8));
			if (!txbuf_empty())
				return -1;
			rwrite(SPI2_DR, (uint8_t) (fg & 0xFF));
			if (!txbuf_empty())
				return -1;
		}
		else {
			rwrite(SPI2_DR, (uint8_t) (bg >> 8));
			if (!txbuf_empty())
				return -1;
			rwrite(SPI2_DR, (uint8_t) (bg & 0xFF));
			if (!txbuf_empty())
				return -1;
		}

		/* Algoritm dependent on draw mode: top down, left right */
		column++;
		if (column > 4) {
			column = 0;
			row++;	
		}
	}
	tftscreen.buf[tftscreen.cpos] = c;
	tftscreen.cpos++;

	tftscreen.x++;
	if (tftscreen.x > 20) {
		tftscreen.x = 0;
		tftscreen.y++;
	}

	return 0;
}



/* Invokes commands with a variable list of paramaters. Sending parameters
 * requires the D/CX line to be high  */
int tft_command(uint8_t cmd, int argsc, ...) {

	va_list ap;
	// command
	rclrbit(GPIOC_ODR, 6); // D/CX line low
	rwrite(SPI2_DR, cmd);
	if (!txbuf_empty()) 
		return -1;

	// parameter or data
	if (argsc > 0) {
		va_start(ap, argsc);
		rsetbit(GPIOC_ODR, 6); // D/CX line high	
		for (int i = 0; i < argsc; i++) {
			uint8_t p = (uint8_t) va_arg(ap, unsigned int);
			rwrite(SPI2_DR, p);
			if (!txbuf_empty())
				return -1;
		}
		va_end(ap);
	}
	return 0;
}	


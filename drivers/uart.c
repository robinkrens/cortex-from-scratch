#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/mmap.h>
#include <sys/robsys.h>

#include <lib/regfunc.h>
#include <lib/string.h>

#include <drivers/uart.h>

#define UARTBUF 256
#define ECHO 1

static struct {
       	 uint8_t buf[UARTBUF];
         uint32_t rpos;
       	 uint32_t wpos;
} linefeed;

void set_baudrate();

void * uart_handler() {

 	while (rchkbit(USART1_SR, 5)) {
		char echochar = *USART1_DR;
                linefeed.buf[linefeed.wpos++] = echochar;
                 if (linefeed.wpos == UARTBUF)
                         linefeed.wpos = 0;
	}
      		
}

void uart_init() {

	linefeed.rpos = 0; 
	linefeed.wpos = 0;

	//memset(&linefeed, 0, (sizeof(struct linefeed) ));
	//rsetbitsfrom(RCC_APB2ENR, 0, 0x4005);
	rsetbit(RCC_APB2ENR, 0);
	rsetbit(RCC_APB2ENR, 2);
	rsetbit(RCC_APB2ENR, 14);
	
	/* (after enable GPIOA), on PA9&PA10 and set mode
	 *  to alternative output */
	rwrite(GPIOA_CRH, 0x444444D4);
	// set event control register, output on PA, Pin 9 TODO: check
	rsetbitsfrom(AFIO_EVCR, 0, 0x89);

	//disable temporarily to set values
	rclrbit(USART1_CR1, 13);

	set_baudrate();

	//set stop bit, default is 1 stop bit 0x00
	rwrite(USART1_CR2, 0x0000);
	
	/* parity = 8 bit, UART1 enabled,
	 * TX and RX enabled, interrupts enabled */
	rwrite(USART1_CR1, 0x0000302C);

	ivt_set_gate(53, uart_handler, 0);
	
	rsetbit(NVIC_ISER1, 5);
}

void uart_putc(unsigned char ch) {
	
  	if (ch == '\n') {
		while(!rchkbit(USART1_SR, 6));
		regw_u8(USART1_DR, 0x0D, 0, OWRITE); // return line
	}
		while(!rchkbit(USART1_SR, 6));
		regw_u8(USART1_DR, ch, 0, OWRITE);
}

char uart_getc(void) {
         char c;

         if (linefeed.rpos != linefeed.wpos) {
                 c = linefeed.buf[linefeed.rpos++];
                 if (linefeed.rpos == UARTBUF)
                         linefeed.rpos = 0;
                 return c;
         }
         return 0;
 }

/* Calculate baud rate. Example how this is done
 * to set this register on a stm32
 * Desired baudrate: 115200,  CLK: 8 MHz
 * Desired Baudrate = CLK / (16 * USARTDIV)
 * USARTDIV = 4.34
 * FRACTION: 16 x 0.34 = 0d5.44 0d5 -> 0x5
 * MANTISSA: 0d4.34 0d4 -> 0x4 
 * USART_BRR = 0x45*/
	
void set_baudrate() {

//	rwrite(USART1_BRR, 0x000001A1); 48 MHZ
//	rwrite(USART1_BRR, 0x0000022B); 64 MHz
//	rwrite(USART1_BRR, 0x00000138); 36 MHz
//	rwrite(USART1_BRR, 0x00000271); 72 MHz
#ifdef ENABLE_HSE
	rwrite(USART1_BRR, 0x00000138);
#else
	rwrite(USART1_BRR, 0x00000045);
#endif
}

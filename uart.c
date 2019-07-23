#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>
#include <drivers.h>

#define RXNE ((*USART1_SR >> 5) & 0x1)
#define UARTBUF 256
#define ECHO 1

static struct {
       	 uint8_t buf[UARTBUF] ;
         uint32_t rpos;
       	 uint32_t wpos;
} linefeed;


void * uart_handler() {

	//uart_puts("echo: ");
 	while (RXNE) {
		char echochar = *USART1_DR;
		//uart_putc(echochar);
                linefeed.buf[linefeed.wpos++] = echochar;
                 if (linefeed.wpos == UARTBUF)
                         linefeed.wpos = 0;
		//regw_u32(USART1_DR, echochar, 0, O_WRITE);
	}
	//uart_putc('\n');
      		
}

void uart_init() {

	regw_u32(RCC_APB2ENR, 0x4005, 0, SETBIT);// enable clock to UART1, AFIO and GPIOA
	
	/* (after enable GPIOA), on PA9&PA10 and set mode
	 *  to alternative output */
	regw_u32(GPIOA_CRH, 0x444444D4, 0, OWRITE);
	regw_u8(AFIO_EVCR, 0x89, 0, OWRITE);// set event control register, output on PA, Pin 9

	//disable temporarily to set values
	regw_u8(USART1_CR1, 0x0, 13, SETBIT);

	/* baud rate 115200,  8MHz / (16 * USARTDIV)
	 * USARTDIV = 4.34
	 * FRACTION: 16 x 0.34 = 0d5.44 0d5 -> 0x5
	 * MANTISSA: 0d4.34 0d4 -> 0x4 
	 * USART_BRR = 0x45*/

	regw_u32(USART1_BRR, 0x00000045, 0, OWRITE);
	regw_u32(USART1_CR2, 0x0000, 0, OWRITE); //set stop bit, default is 1 stop bit 0x00
	
	/* parity = 8 bit, UART1 enabled,
	 * TX and RX enabled, interrupts enabled */
	//regw_u32(USART1_CR1, 0x000030AC, 0, O_WRITE);
	regw_u32(USART1_CR1, 0x0000302C, 0, OWRITE);

	ivt_set_gate(53, uart_handler, 0);
	
	*NVIC_ISER1 = (1 << 5); // Enable UART interrupt at NVIC
}

static void wait() {
	for (int i = 0; i < 100; i++);
}

extern void uart_putc(unsigned char ch) {
	
  	if (ch == '\n') {
		while (*USART1_SR & 0x0C) { } // transmit data register empty and complete
		regw_u8(USART1_DR, 0x0D, 0, OWRITE); // return line
	}

	while (*USART1_SR & 0x0C) {} 
		regw_u8(USART1_DR, ch, 0, OWRITE);

	wait();
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


// move to library 
extern void uart_puts(unsigned char *str) {
    int i;
    for (i = 0; i < strlen(str); i++)     {
        uart_putc(str[i]);
    }
}


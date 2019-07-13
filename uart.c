#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>

void uart_init() {

	// enable clock to UART1
	*RCC_APB2ENR = 0x4005;
	*GPIOA_CRH = 0x444444D4;
	*AFIO_EVCR = 0x89;
	
	*USART1_CR1 = (0 << 13); // disable control register
	*USART1_CR3 = 0xC0;

	/* baud rate 9600 
	 * 115200 = 8MHz / (16 * USARTDIV)
	 * USARTDIV = 4.34
	 * FRACTION: 16 x 0.34 = 0d5.44 0d5 -> 0x5
	 * MANTISSA: 0d4.34 0d4 -> 0x4 
	 * USART_BRR = 0x45*/

	*USART1_BRR = 0x00000045;

	/* parity = 8 bit, UART1 enabled, TX and RX enabled, interrupts enabled */
	*USART1_CR1 = (volatile uint32_t) 0x000030AC; 

/* 
 * Configure UART0:
	1. Disable the UART by clearing the UARTEN bit in the UARTCTL register.
	2. Write the integer portion of the BRD to the UARTIBRD register.
	3. Write the fractional portion of the BRD to the UARTFBRD register.
	4. Write the desired serial parameters to the UARTLCRH register
	5. Enable the UART by setting the UARTEN bit in the UARTCTL register.
*/

/* TODO: bitrate: How fast is CPU running?*/

/* *UART0_CTRL = 0;
*UART0_IBRD = 27; 
*UART0_FBRD = 9;
*UART0_LCRH = 0x60; 
*UART0_CTRL = 0x301;

*UART1_CTRL = 0;
*UART1_IBRD = 27; 
*UART1_FBRD = 9;
*UART1_LCRH = 0x60; 
*UART1_CTRL = 0x301;
*/

}

extern void uart_putc(unsigned char ch) {
	
//	if (ch == '\n') {
//		while ((*UART1_DR & 0x8)); // busy bit
//		*USART1_DR = 0x0D; // return line
//	}
//	
//	while ((*UART0_FLAG & 0x8)); // busy bit
	*USART1_DR = ch;
}

extern void uart_puts(unsigned char *str)
{

    int i;

    for (i = 0; i < strlen(str); i++)
    {
        uart_putc(str[i]);
    }
}


char * uart_read() {

	return NULL;
}



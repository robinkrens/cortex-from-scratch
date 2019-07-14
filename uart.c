#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>

void uart_init() {

	
	*RCC_APB2ENR = 0x4005; // enable clock to UART1, AFIO and GPIOA
	*GPIOA_CRH = 0x444444D4; // (after enable GPIOA), on PA9&PA10 and set mode to alternative output
	*AFIO_EVCR = 0x89; // (after enable) set event control register, output on PA, Pin 9

	*USART1_CR1 = (0 << 13); // disable temporarily to set values

	/* baud rate 115200,  8MHz / (16 * USARTDIV)
	 * USARTDIV = 4.34
	 * FRACTION: 16 x 0.34 = 0d5.44 0d5 -> 0x5
	 * MANTISSA: 0d4.34 0d4 -> 0x4 
	 * USART_BRR = 0x45*/

	/* baud rate 2400 
	 * 
	 * 16 * 0.33 -> 0x5
	 * 208 -> 0x34 */

	*USART1_BRR = (volatile uint32_t) 0x00000045;
	
	*USART1_CR2 = (volatile uint32_t) 0x0000; // set stop bit, default is 1 stop bit 0x00
	// enable DMA 
	// *USART1_CR3 = 0x0C0; 
	/* parity = 8 bit, UART1 enabled,
	 * TX and RX enabled, interrupts enabled */
	*USART1_CR1 = (volatile uint32_t) 0x000030AC; 

/* 
 * Configure UART0:
	1. Disable the UART by clearing the UARTEN bit in the UARTCTL register.
	2. Write the integer portion of the BRD to the UARTIBRD register.
	3. Write the fractional portion of the BRD to the UARTFBRD register.
	4. Write the desired serial parameters to the UARTLCRH register
	5. Enable the UART by setting the UARTEN bit in the UARTCTL register.
*/
}


extern void uart_putc(unsigned char ch) {
	
  	if (ch == '\n') {
		while (*USART1_SR & 0x0C) { } // transmit data register empty and complete
		*USART1_DR = 0x0D; // return line
	}
	
	while ((*USART1_SR & 0xFF) == 0x0C) {} // busy bit  
		*USART1_DR = ch;

}

void wait() {
	for (int i = 0; i < 100; i++);
}
extern void uart_puts(unsigned char *str)
{

    int i;

    for (i = 0; i < strlen(str); i++)
    {
	wait();
        uart_putc(str[i]);
    }
}



char * uart_read() {

	return NULL;
}



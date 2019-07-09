#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>

#define SYSCTRL_RCC ((volatile unsigned long *)(0x400FE060))
#define SYSCTRL_RIS ((volatile unsigned long *)(0x400FE050))
#define SYSCTRL_RCGC1 ((volatile unsigned long *)(0x400FE104))
#define SYSCTRL_RCGC2 ((volatile unsigned long *)(0x400FE108))
#define GPIOPA_AFSEL ((volatile unsigned long *)(0x40004420))

#define UART0_DATA ((volatile unsigned long *)(0x4000C000))
#define UART0_FLAG ((volatile unsigned long *)(0x4000C018))
#define UART0_IBRD ((volatile unsigned long *)(0x4000C024))
#define UART0_FBRD ((volatile unsigned long *)(0x4000C028))
#define UART0_LCRH ((volatile unsigned long *)(0x4000C02C))
#define UART0_CTRL ((volatile unsigned long *)(0x4000C030))
#define UART0_RIS ((volatile unsigned long *)(0x4000C03C))

#define UART1_DATA ((volatile unsigned long *)(0x4000D000))
#define UART1_FLAG ((volatile unsigned long *)(0x4000D018))
#define UART1_IBRD ((volatile unsigned long *)(0x4000D024))
#define UART1_FBRD ((volatile unsigned long *)(0x4000D028))
#define UART1_LCRH ((volatile unsigned long *)(0x4000D02C))
#define UART1_CTRL ((volatile unsigned long *)(0x4000D030))
#define UART1_RIS ((volatile unsigned long *)(0x4000D03C))


void uart_init() {

/* To use the UARTs, the peripheral clock must be enabled by setting the UART0, UART1, or UART2
bits in the RCGC1 register. (section 12.4: Initialization and Configuration */

*SYSCTRL_RCGC1 = *SYSCTRL_RCGC1 | 0x0003;

/* 
 * Configure UART0:
	1. Disable the UART by clearing the UARTEN bit in the UARTCTL register.
	2. Write the integer portion of the BRD to the UARTIBRD register.
	3. Write the fractional portion of the BRD to the UARTFBRD register.
	4. Write the desired serial parameters to the UARTLCRH register
	5. Enable the UART by setting the UARTEN bit in the UARTCTL register.
*/

/* TODO: bitrate: How fast is CPU running?*/

*UART0_CTRL = 0;
*UART0_IBRD = 27; 
*UART0_FBRD = 9;
*UART0_LCRH = 0x60; 
*UART0_CTRL = 0x301;

*UART1_CTRL = 0;
*UART1_IBRD = 27; 
*UART1_FBRD = 9;
*UART1_LCRH = 0x60; 
*UART1_CTRL = 0x301;


}

extern void uart_putc(unsigned char ch) {
	
	if (ch == '\n') {
		while ((*UART0_FLAG & 0x8)); // busy bit
		*UART0_DATA = 0x0D; // return line
	}
	
	while ((*UART0_FLAG & 0x8)); // busy bit
	*UART0_DATA = ch;
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



#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>

#define RXNE ((*USART1_SR >> 5) & 0x1)

#define O_WRITE 0x01
#define SET 0x02
#define CLEAR 0x03

void * uart_handler() {

	uart_puts("\n echo: ");
 	while (RXNE) {
		char echochar = *USART1_DR;
	//	regw_u8(USART1_DR, echochar, 0, O_WRITE);
	uart_putc(echochar);
	}  
//for(;;);

}
void uart_init() {

	// global interrupt setup
//	regw_u32(EXTI_IMR, 0x000FFFFF, 0, O_WRITE);
//	regw_u32(EXTI_RTSR, 0x000FFFFF, 0, O_WRITE);

		
	regw_u32(RCC_APB2ENR, 0x4005, 0, SET);// enable clock to UART1, AFIO and GPIOA
	
	/* (after enable GPIOA), on PA9&PA10 and set mode
	 *  to alternative output */
	regw_u32(GPIOA_CRH, 0x444444D4, 0, O_WRITE);
	regw_u8(AFIO_EVCR, 0x89, 0, O_WRITE);// set event control register, output on PA, Pin 9

	//disable temporarily to set values
	regw_u8(USART1_CR1, 0x0, 13, SET);

	/* baud rate 115200,  8MHz / (16 * USARTDIV)
	 * USARTDIV = 4.34
	 * FRACTION: 16 x 0.34 = 0d5.44 0d5 -> 0x5
	 * MANTISSA: 0d4.34 0d4 -> 0x4 
	 * USART_BRR = 0x45*/

	regw_u32(USART1_BRR, 0x00000045, 0, O_WRITE);
	regw_u32(USART1_CR2, 0x0000, 0, O_WRITE); //set stop bit, default is 1 stop bit 0x00
	
	/* parity = 8 bit, UART1 enabled,
	 * TX and RX enabled, interrupts enabled */
	//regw_u32(USART1_CR1, 0x000030AC, 0, O_WRITE);
	regw_u32(USART1_CR1, 0x0000302C, 0, O_WRITE);

	ivt_set_gate(53, uart_handler, 0);
	
	*NVIC_ISER1 = (1 << 5); // Enable UART interrupt at NVIC
}

void wait() {
	for (int i = 0; i < 100; i++);
}

extern void uart_putc(unsigned char ch) {
	
  	if (ch == '\n') {
		while (*USART1_SR & 0x0C) { } // transmit data register empty and complete
		regw_u8(USART1_DR, 0x0D, 0, O_WRITE); // return line
	}

	while (*USART1_SR & 0x0C) {} 
		regw_u8(USART1_DR, ch, 0, O_WRITE);

	wait();
}

extern void uart_puts(unsigned char *str) {
    int i;
    for (i = 0; i < strlen(str); i++)     {
        uart_putc(str[i]);
    }
}



char uart_read() { 

	/* while (buffer not empty)
	 * 	read()
	 * 	uart_putc(ch) // echo
	 * 		if ch = enter
	 * 		process inquiry.
	 */
	
}



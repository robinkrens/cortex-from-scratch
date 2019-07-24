#ifndef __UART_H
#define __UART_H

/* SERIAL INIT AND BASIC I/O 
 * DON'T USE DIRECTLY USE, RATHER
 * USE HIGHER LEVEL I/O 
 * */
extern void uart_init();
extern void uart_putc(unsigned char ch); 
char uart_getc(void); 

#endif

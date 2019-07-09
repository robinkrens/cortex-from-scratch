#ifndef __SYSTEM_H
#define __SYSTEM_H

/* MAIN.C */
extern void *memcpy(void*, void*, size_t);
extern void *memset(void*, unsigned char, size_t);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);
extern int strlen(const char *);
extern unsigned char inportb (uint32_t _addr);
extern void outportb (uint32_t _addr, unsigned char _data);

/* UART.C */
extern void uart_init();
extern void uart_putc(unsigned char);
extern void uart_puts(unsigned char *);

#endif

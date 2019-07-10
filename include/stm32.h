#ifndef __SYSTEM_H
#define __SYSTEM_H

/* MAIN.C */
extern void *memcpy(void*, void*, size_t);
extern void *memset(void*, unsigned char, size_t);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);
extern int strlen(const char *);

/* UART.C */
extern void uart_init();
extern void uart_putc(unsigned char);
extern void uart_puts(unsigned char *);

/* IVT.C */
extern void ivt_init();
extern void ivt_set_gate(unsigned char, void *(), short);

/* SYSTICK.C */
extern void systick_init();

#endif

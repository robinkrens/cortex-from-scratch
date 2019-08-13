#ifndef __SYSTEM_H
#define __SYSTEM_H


/* CLOCK.C 
 * Board specific clock settings. These boards often come with two 
 * external oscillators: one high speed (8MHz) and one low speed (~30kHz).
 * These values are used throughout the code to calculator desired baud
 * rates etc.
 */
//#define ENABLE_HSE	
//efine CRYSTAL_MHZ 	8
//efine CLKSPEED_MHZ 	72
extern void clock_init();
// extern int clock_test();
// extern void clock_reset();

/* RTC.C */
#define ENABLE_RTC	
extern void rtc_init();

/* IVT.C */
extern void ivt_init();
extern void ivt_set_gate(unsigned char, void *(), short);

/* SYSTICK.C */
extern void systick_init();

/* SYSINFO.C */
extern void sysinfo();

/* POOL.c */
extern void pool_init(size_t, unsigned int, uint32_t *);
extern void * alloc();
extern void free();

/* TERM.C */
extern void terminal();

#endif

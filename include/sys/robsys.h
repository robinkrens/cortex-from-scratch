#ifndef __SYSTEM_H
#define __SYSTEM_H


/* CLOCK.C 
 * Board specific clock settings. These boards often come with two 
 * external oscillators: one high speed (8MHz) and one low speed (~30kHz).
 * These values are used throughout the code to calculator desired baud
 * rates etc.
 */
//#define ENABLE_HSE 		
//#define CRYSTAL_MHZ 	8
#define CLKSPEED_MHZ 8
extern void clock_init();
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

/* HEAP.c */
/* typedef void * (kalloc)(void * s);
 * typedef void (kfree)(void * s, void * p); */
extern void kheap_init();
extern void * get_kheap();
extern void * kalloc(void * s);
extern void kfree(void * s, void * p);
extern void kheap_info(void * s);

/* TERM.C */
extern void terminal();

#endif

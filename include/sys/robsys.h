#ifndef __SYSTEM_H
#define __SYSTEM_H

/* CLOCK.C */
extern void clock_init();

/* IVT.C */
extern void ivt_init();
extern void ivt_set_gate(unsigned char, void *(), short);

/* SYSTICK.C */
extern void systick_init();

/* SYSINFO.C */
extern void sysinfo();

/* MM.C DELETE TODO */
//extern void mm_init();
//extern void * malloc(size_t);
//extern void free(void *);
//extern void test_memory(uint32_t *);

/* POOL.c */
extern void pool_init(size_t, unsigned int, uint32_t *);
extern void * alloc();
extern void free();

/* TERM.C */
extern void terminal();

#endif

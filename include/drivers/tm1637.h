#ifndef __TM1637_H
#define __TM1637_H

/* HELPER SUBROUTINES DECLARATIONS */
static void start_condition();
static void stop_condition();
static int ack_recv();
static int ack10_recv();
static int idle();
static void delay();

extern void tm1637_init();
extern int set_grid(uint8_t offset, char value, bool dot);
extern int set_display(bool on, uint8_t degree);

extern void tm1637_example();

#endif

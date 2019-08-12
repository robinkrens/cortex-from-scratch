#ifndef __AT24C_H
#define __AT24C_H

/* HELPER SUBROUTINES DECLARATIONS */
static void start_condition();
static void stop_condition();
static int ack_recv();
static int buf_empty();
static int data_recv();
static int late_recv();
static int delay();

extern void eeprom_at24c_init();
extern int eeprom_write(uint16_t addr, char * data, size_t size);
extern int eeprom_erase();
extern int eeprom_read(uint16_t addr, int num, char * rvalues);
extern int eeprom_dump();

extern int eeprom_test();

#endif

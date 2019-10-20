/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version
 * Display some system information, calculate
 * the amount of SRAM available 
 * 
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h>
#include <sys/mmap.h>

#include <lib/tinyprintf.h>
#include <lib/regfunc.h>


uint32_t get_msp(void);

void sysinfo() {

	uint32_t tmp = *MCU_ID;
	printf("SYSTEM LOADING...\n\n");
	printf("DEV ID: ");

	if (tmp & 0x414) 
		printf("H. DENSITY\n");
	else {
		printf("UNKNOWN\n");
	}

	/* tmp = (tmp >> 16);
	printf("REV: ");
	switch	(tmp) {
		case 0x1000:
		      printf("REVISION A\n");
		      break;
		case 0x1001:
		      printf("REVISION Z\n");
		      break;
		case 0x1003:
		      printf("REVISION 1/2/3/X/Y\n");
		      break;
		default:
		      printf("UNKNOWN\n");
	} */

	extern char _endofbss;

	uint32_t current_stack = get_msp();
	uint32_t stack_usage = (SRAM_OFFSET + SRAM_SIZE) - current_stack;
	uint32_t data_bss = (uint32_t) &_endofbss - SRAM_OFFSET;
	uint32_t mem_free = SRAM_SIZE - stack_usage - data_bss;

	extern uint32_t KHEAP_SIZE;
	
	printf("TOTAL MEM: %#x\n", SRAM_SIZE);
	printf("FREE MEM: %#x\n", mem_free);
	printf("STACK USE: %#x\n", stack_usage);
	printf("HEAP_SIZE: %#x\n\n", &KHEAP_SIZE);

}

/* Get the master stack pointer's position
 * Here we use a so-called naked function, 
 * that doesn't automatically push and pop  */

uint32_t get_msp(void) __attribute__( ( naked ) );
uint32_t get_msp(void)
{
  uint32_t msp = 0;
  __asm__ volatile ("mrs %0, msp\n\t" 
                  "mov r0, %0 \n\t"
                  "bx lr     \n\t"  : "=r" (msp) );
  return(msp);
}

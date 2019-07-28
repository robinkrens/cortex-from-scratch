/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version 
 * 
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h>
#include <sys/mmap.h>

#include <lib/stdio.h>
#include <lib/regfunc.h>


uint32_t get_msp(void);

void sysinfo() {

	uint32_t tmp = *MCU_ID;
	cputs("# DEVICE ID: ");

	if (tmp & 0x414) 
		cputs("HIGH DENSITY\n");
	else {
		cputs("UNKNOWN\n");
	}

	tmp = (tmp >> 16);
	cputs("# REVISION: ");
	switch	(tmp) {
		case 0x1000:
		      cputs("REVISION A\n");
		      break;
		case 0x1001:
		      cputs("REVISION Z\n");
		      break;
		case 0x1003:
		      cputs("REVISION 1/2/3/X/Y\n");
		      break;
		default:
		      cputs("UNKNOWN\n");
	}

	extern char _endofbss;
	
	uint32_t current_stack = get_msp();
	uint32_t stack_usage = (SRAM_OFFSET + SRAM_SIZE) - current_stack;
	uint32_t data_bss = &_endofbss - SRAM_OFFSET;
	uint32_t mem_free = SRAM_SIZE - stack_usage - data_bss;

	cputs("# TOTAL MEMORY: ");
	cputs(regtohex(SRAM_SIZE));
	cputchar('\n');
	cputs("# FREE MEMORY: ");
	cputs(regtohex(mem_free));
	cputchar('\n');
	cputs("# STACK USAGE: ");
	cputs(regtohex(stack_usage));
	cputchar('\n');

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

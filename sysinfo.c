#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>

#define MEM_SIZE	0x00010000 
#define MEM_OFFSET	0x20000000

uint32_t get_msp(void);

void sysinfo() {

	uint32_t tmp = *MCU_ID;
	cputs("# ROBSYS 0.1 LOADING...\n");
	cputs("# DEVICE ID: ");

	if (tmp & 0x414) 
		cputs("HIGH DENSITY\n");
	else {
		cputs("UNKNOWN\n");
	}

	tmp = (tmp >> 16);
	uart_puts("# REVISION: ");
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
	uint32_t stack_usage = (MEM_OFFSET + MEM_SIZE) - current_stack;
	uint32_t data_bss = &_endofbss - MEM_OFFSET;
	uint32_t mem_free = MEM_SIZE - stack_usage - data_bss;

	cputs("# TOTAL MEMORY: ");
	addrtohex(MEM_SIZE);
	cputchar('\n');
	cputs("# FREE MEMORY: ");
	addrtohex(mem_free);
	cputchar('\n');
	cputs("# STACK USAGE: ");
	addrtohex(stack_usage);
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

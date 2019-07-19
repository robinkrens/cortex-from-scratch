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
	uart_puts("# ROBSYS 0.1 LOADING...\n");
	uart_puts("# DEVICE ID: ");

	if (tmp & 0x414) 
		uart_puts("HIGH DENSITY\n");
	else {
		uart_puts("UNKNOWN\n");
	}

	tmp = (tmp >> 16);
	uart_puts("# REVISION: ");
	switch	(tmp) {
		case 0x1000:
		      uart_puts("REVISION A\n");
		      break;
		case 0x1001:
		      uart_puts("REVISION Z\n");
		      break;
		case 0x1003:
		      uart_puts("REVISION 1/2/3/X/Y\n");
		      break;
		default:
		      uart_puts("UNKNOWN\n");
	}

	extern char _endofbss;
	
	uint32_t current_stack = get_msp();
	uint32_t stack_usage = (MEM_OFFSET + MEM_SIZE) - current_stack;
	uint32_t data_bss = &_endofbss - MEM_OFFSET;
	uint32_t mem_free = MEM_SIZE - stack_usage - data_bss;

	uart_puts("# TOTAL MEMORY: ");
	addrtohex(MEM_SIZE);
	uart_putc('\n');
	uart_puts("# FREE MEMORY: ");
	addrtohex(mem_free);
	uart_putc('\n');
	uart_puts("# STACK USAGE: ");
	addrtohex(stack_usage);
	uart_putc('\n');

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

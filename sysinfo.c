#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>


void sysinfo() {

	uint32_t tmp = *MCU_ID;

	uart_puts("CHECKING SYS INFO\n");
	uart_puts("# DEVICE ID: ");

	if (tmp & 0x414) {
		uart_puts("HIGH DENSITY\n");
	}
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

}


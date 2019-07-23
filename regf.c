#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>

/* write value (uint8_t) to register */
void regw_u8(volatile uint32_t * reg, uint8_t val, short shift, short flag) {

	switch(flag) {
		case OWRITE:
			*reg = (val << shift);
			break;
		case SETBIT:
			*reg = *reg | (val << shift);
			break;
		case CLRBIT:
			*reg = (val << shift);
			break;
	}
}

/* write value (uint32_t) to register */
void regw_u32(volatile uint32_t * reg, uint32_t val, short shift, short flag) {

	switch(flag) {
		case OWRITE:
			//*reg = (val << shift);
			*reg = (val << shift);
			break;
		case SETBIT:
			*reg = *reg | (val << shift);
			break;
		case CLRBIT:
			//
			break;
	}
}



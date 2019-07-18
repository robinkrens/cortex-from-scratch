#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>

#define O_WRITE 0x01
#define SET 0x02
#define CLEAR 0x03

/* write value (uint8_t) to register */
void regw_u8(volatile uint32_t * reg, uint8_t val, short shift, short flag) {

	switch(flag) {
		case O_WRITE:
			(*(volatile uint32_t *) (reg)) = (val << shift);
			break;
		case SET:
			(*(volatile uint32_t *) (reg)) = 
			((*(volatile uint32_t *) (reg)) | (val << shift));
			break;
		case CLEAR:
			(*(volatile uint32_t *) (reg)) = (val << shift);
			break;
	}
}

/* write value (uint32_t) to register */
void regw_u32(volatile uint32_t * reg, uint32_t val, short shift, short flag) {

	switch(flag) {
		case O_WRITE:
			(*(volatile uint32_t *) (reg)) = (val << shift);
			break;
		case SET:
			(*(volatile uint32_t *) (reg)) = 
			((*(volatile uint32_t *) (reg)) | (val << shift));
		case CLEAR:
			//
			break;
	}
}



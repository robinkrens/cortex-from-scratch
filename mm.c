#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>

/* TOTAL SRAM MEMORY: 64kB 
 * 64 chunks of 1kB 
 * 128 chunks of 512 bytes
 * SIMPLE BITMAP IMPLEMENTATION
 * */

#define CHUNKS  128
#define FREE 	0x00
#define ALLOC	0x01

#define BASE 0x20000400

#define SET_SIZE(s) 	(s << 8)
#define SET_FLAGS(f) 	(f << 24)
#define IS_ALLOC(c)   	((c >> 24) & 0x0F )
#define PADDR(i)	(0x20000400 + (i * 0x200))
#define I_OFFSET(p)     ((p  - 0x20000400))

uint32_t chunk[CHUNKS];

/* 
 * | FLAGS | SIZE   | RESERVED	|
 * | 0x00  | 0x0000 | 0x00     	|
 *
 * */


void mm_init() {

	// interrupt vector
	chunk[0] = SET_SIZE(96) | SET_FLAGS(ALLOC); 

	// test
	/* uart_puts("ALLOC:\n");
	int * p = mm_alloc(100);
	*p = 0x12345678;

	addrtohex(p);
	addrtohex(*p);

	uart_puts("FREE:\n");

	int * p2 = mm_alloc(100);
	*p2 = 0xFFFFAAAA;
	addrtohex(p2);
	addrtohex(*p2);

	free(p);
	free(p2); */
}

 void test_memory(uint32_t * ptr) {

	*ptr = 0xEEEEEEEE;
	for (int i = 0; i < 100; i++) {
		ptr++;
		*ptr = 0xEEEEEEEE;
	}

} 


void * mm_alloc(size_t size) { // in bytes

	if (size > 512) {
		uart_puts("SYSERROR: WE CAN'T ALLOCATE THAT MUCH!\n");
		return NULL; 
	}

	/* check which chunk is free */
	for (int i = 1; i < CHUNKS; i++) {
		if (!IS_ALLOC(chunk[i])) {
			chunk[i] = SET_SIZE(size) | SET_FLAGS(ALLOC);
		       	return (void *) PADDR(i);
		}
	}
	
	uart_puts("SYSERROR: OUT OF MEMORY\n");
	return NULL;
}

void free(void *ptr) {

	uint32_t index = (uint32_t) I_OFFSET(ptr) / 0x200;

	uint32_t tmp = chunk[index];
	if (!IS_ALLOC(tmp))
		uart_puts("SYSERROR: ALREADY FREED!\n");

	else if(index < CHUNKS) {
		chunk[index] = SET_FLAGS(FREE) | SET_SIZE(0);
	}
		
}


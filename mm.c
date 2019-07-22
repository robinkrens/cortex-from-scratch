/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Simple bitmap on bitband memory implementation for kernel
 * heap. Sensitive to fragmentation over time. Bitband 
 * memory makes it possible to access each bit of memory
 * atomically. 
 * 
 * DEPRECATED
 *
 * */


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>


#define CHUNKS 256

#define MEM_VALUE(addr) *((volatile uint32_t *) (addr))
#define MEM_ADDR(addr) ((uint32_t *) (addr))
#define BITBAND(a, b) ((a & 0xF0000000) + 0x02000000 + ((a &0xFFFFF)<<5) + (b<<2))
#define INDEXTOADDR(a) (((a & 0xFFFFF) >> 5) + ((a & 0xFF000000) - 0x02000000))


/* Total SRAM: ~ 64kB 
 * Divided into chunks (256 bytes)
 * Each bit will index a chunk
 * Bits needed: 0x100 (= 4 uint32_t)
 * */

//uint32_t chunk_index[4];

void mm_init() {


//	memset(&chunk_index, 0, sizeof(uint32_t) * 4);


//	uint32_t *p = MEM_ADDR(0x20000278);
	
//	extern stub();
//	stub();
	// __asm__ __volatile__ ("udiv r1, r3 ,%0" :: "r"(0)); 
//
//	for(;;);
//	*p = (uint32_t volatile) 0x12345678;
//	*x = (uint32_t volatile) 0x12345679;
//	addrtohex(p);
//	addrtohex(*p);
//	MEM_ADDR(x) = 0x1;
//
	

//	char * new = malloc(10);
//	addrtohex(new);
//	char * new2 = malloc(10);
//	addrtohex(new2);


	//uint32_t * test = MEM_ADDR(0x20000000);
	//uint32_t random_location = MEM_VALUE(0x20000000);

	//uint32_t random_location = 0x20000900;

	//MEM_VALUE(random_location);
	//MEM_VALUE(BITBAND(random_location, 0)) = 0x1;

	//addrtohex(MEM_VALUE(random_location));


	
}

 void test_memory(uint32_t * ptr) {

	*ptr = 0xEEEEEEEE;
	for (int i = 0; i < 100; i++) {
		ptr++;
		*ptr = 0xEEEEEEEE;
	}

} 

/* BIT BAND SCAN */

 /* uint32_t fits(uint32_t * current, size_t size) {
 
 	uint32_t addr_start = current;
 
 	for (int i = 1; i < size; i++) {
 		current + 4; // next bit offset is 0x4
 		if ((MEM_VALUE(current)) == 0x1) 
 			return 0x0;
 	}
 	return addr_start;
 } */


/* void * malloc(size_t size) {

	if (size < 256) {

		extern char * _endofbss;

		uint32_t start = (uint32_t) &chunk_index[0];	
		uint32_t current;
		int offset = 0x100;

		uint32_t * index = MEM_ADDR(BITBAND(start, 0));
		for(int i = 0; i < CHUNKS; i++) {
			if (*index == 0x0) {
				addrtohex(*index);
				*index = 0x1;
				return INDEXTOADDR(((uint32_t)index)) + (i * offset);
			}
			index += 0x04;
		}
		return NULL;
	}
} */
 



/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Fixed-sized memory pool allocation. A so called * memory-pool
 * (i.e. 4kB * 12 blocks) can be initialized. Note: this allocator
 * is for use on processors without an MMU (memory management 
 * unit). A MPU (memory protection unit), if available can be used
 * to protect certain zones.
 *
 * This work is based on an article of Ben Kenwright.
 * 
 * Preconditions: programmer should make sure the SRAM entry point
 * + (blocks * blocksize) is free. 
 *
 * $SAMPLE USAGE$
 * KERNEL: can initialize a big pool for all user tasks
 *
 * USER TASKS/PROCESS: can use this to dynamically allocate their
 * own memory (i.e. heap)
 * 
 * 
 * * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>


struct MEMPOOL {

	unsigned short blocks; 
	unsigned short block_size; 
	unsigned short free_blocks;
	unsigned short blocks_alloc; 
	uint32_t * SRAM_entry;
	uint32_t * m_next; 

};

struct MEMPOOL mem;

 
void pool_init(size_t size_arg, unsigned int blocks_arg, uint32_t * entry_SRAM) {

	 mem.blocks = blocks_arg;
	 mem.block_size = size_arg;
	 mem.SRAM_entry = entry_SRAM;
	 memset(entry_SRAM, 0x00, (sizeof(char) * (size_arg * blocks_arg)));
	 mem.free_blocks = blocks_arg;
	 mem.m_next = mem.SRAM_entry;


/*	 uint32_t * a =  alloc();
	 uint32_t * b = alloc();
	 uint32_t * c = alloc();
	 free(c);
	 uint32_t * d = alloc();

	 addrtohex(a);
	 addrtohex(b);
	 addrtohex(c);
	 addrtohex(d); */

 }


 /* void deletepool()  {
	 mem.SRAM_entry = NULL;
 } */

/* Helper functions */
uint32_t * AddrFromIndex(unsigned int i)  {
	return mem.SRAM_entry + ( i * mem.block_size );

 }
 
unsigned int IndexFromAddr(const uint32_t * p) {
	return (((unsigned int)(p - mem.SRAM_entry)) / mem.block_size);

}

/* alloc and free */ 
void * alloc() {
 	if (mem.blocks_alloc < mem.blocks ) {
		 unsigned int * p = (unsigned int *)AddrFromIndex( mem.blocks_alloc );
	 	*p = mem.blocks_alloc + 1;
		 mem.blocks_alloc++;
	 }

	 void* ret = NULL;
	 if ( mem.free_blocks > 0 ) {
		 ret = (void*)mem.m_next;
		 --mem.free_blocks;
	 if (mem.free_blocks!=0) {
		mem.m_next = AddrFromIndex( *((unsigned int*)mem.m_next) );
	 }
	 else {
		 mem.m_next = NULL;
	 }
 	}
	 
	 return ret;
 }

void free(void* p)  {
	 if (mem.m_next != NULL) {
		 (*(unsigned int *)p) = IndexFromAddr( mem.m_next );
		 mem.m_next = (uint32_t *)p;
 	}
	 else {
		 *((unsigned int*)p) = mem.blocks;
		 mem.m_next = (uint32_t *) p;
	 }
		
	 ++mem.free_blocks;
 }

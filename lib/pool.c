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
 * Blocks are fixed size. In some situations ideal, in some situation
 * may be not. It's fast.
 * 
 * Preconditions: programmer should make sure the SRAM entry point
 * + (blocks * blocksize) is free. 
 *
 * $SAMPLE USAGE$
 * KERNEL: initialize one (or multiple!) fixed memory-sized 
 * kernal heaps
 *
 * USER TASKS/PROCESS: can use this to dynamically allocate their
 * own memory (i.e. heap). You might argue that a fixed size memory
 * pool is not ideal, but you would be amazed that many programs
 * use structs of more or less similar size.
 * 
 * 
 * * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <lib/pool.h>
#include <lib/tinyprintf.h>
#include <lib/string.h>

 
void pool_init(mem_pool_t * pool, size_t size_arg, unsigned int blocks_arg, unsigned char* entry_SRAM) {

	 pool->blocks = blocks_arg;
	 pool->block_size = size_arg;
	 pool->blocks_init = 0;
	 pool->SRAM_entry = entry_SRAM;
	 memset(entry_SRAM, 0x00, (sizeof(char) * (size_arg * blocks_arg)));
	 pool->free_blocks = blocks_arg;
	 pool->m_next = pool->SRAM_entry;
 }

 /* void deletepool()  {
	 pool->SRAM_entry = NULL;
 } */

/* Helper functions */
unsigned char* AddrFromIndex(mem_pool_t * pool, unsigned int i)  {
	return pool->SRAM_entry + ( i * pool->block_size );

 }
 
unsigned int IndexFromAddr(mem_pool_t * pool, const unsigned char* p) {
	return (((unsigned int)(p - pool->SRAM_entry)) / pool->block_size);

}

/* alloc and free */ 
void * alloc(void * s) {
	mem_pool_t * pool = (mem_pool_t *) s;
 	if (pool->blocks_init < pool->blocks ) {
		 unsigned int * p = (unsigned int *)AddrFromIndex(pool, pool->blocks_init );
	 	*p = pool->blocks_init + 1;
		 pool->blocks_init++;
	 }

	 void* ret = NULL;
	 if ( pool->free_blocks > 0 ) {
		 ret = (void*)pool->m_next;
		 --pool->free_blocks;
	 if (pool->free_blocks!=0) {
		pool->m_next = AddrFromIndex(pool, *((unsigned int*)pool->m_next) );
	 }
	 else {
		 pool->m_next = NULL;
	 }
 	}
	 
	 return ret;
 }

void free(void * s, void* p)  {

	mem_pool_t * pool = (mem_pool_t *) s;
	if (pool->m_next != NULL) {
		(*(unsigned int *)p) = IndexFromAddr(pool, pool->m_next );
		pool->m_next = (unsigned char*)p;
	}
	else {
		 *((unsigned int*)p) = pool->blocks;
		 pool->m_next = (unsigned char*) p;
	}
		
	++pool->free_blocks;
}

/* Heap info helper functions */

void heap_info(void * s) {

	mem_pool_t * pool = (mem_pool_t *) s;

	printf("HEAP INFO:\n");
	printf("BLOCKS FREE: %d\n", pool->free_blocks);

	unsigned char* curr_addr = pool->SRAM_entry;

	// Loop over heap blocks
	for (int i = 0; i < pool->blocks; i++) {
		printf("\nBLOCK %d", i+1);
		for (int j = 0; j < (pool->block_size-1); j++) {

			if (j % 10 == 0)
				printf("\n%x ", curr_addr);
			printf("%#x ", *curr_addr);
			curr_addr++;
		}
		printf("\n");
		curr_addr++;
		
	}
	printf("\n");

}




/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/10/20 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * 	
 * kalloc();
 * kfree(); 
 *
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <lib/pool.h>

#define BLOCKSIZE 	0x10
#define BLOCKS		10
/* #define KHEAP_SIZE	0x100 SEE LINK.LD */

static mem_pool_t kheap_pool;

void kheap_init() {
	
	extern unsigned char * _beginofheap;
	pool_init(&kheap_pool, BLOCKSIZE, BLOCKS, (unsigned char *) &_beginofheap);
	//kalloc = &alloc;
}

void * kalloc(void * s) {

	return alloc(s);
}

void kfree(void * s, void * p) {

	free(s, p);
}

void kheap_info(void *s) {

	heap_info(s);
}

void * get_kheap() {

	return (void *) &kheap_pool;
}




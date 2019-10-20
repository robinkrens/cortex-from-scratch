/* Fixed size memory pool allocation  */
typedef struct mem_pool mem_pool_t;

struct mem_pool {

	unsigned short blocks; 
	unsigned short block_size; 
	unsigned short free_blocks;
	unsigned short blocks_init; 
	unsigned char * SRAM_entry;
	unsigned char * m_next; 

};

extern void kpool_init(mem_pool_t *, size_t size_arg, unsigned int blocks_arg, unsigned char * entry_SRAM); 
extern void * kalloc(mem_pool_t * );
extern void kfree(mem_pool_t *, void* p); 
extern void kheap_info(mem_pool_t *); 

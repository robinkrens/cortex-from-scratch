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

void pool_init(mem_pool_t *, size_t size_arg, unsigned int blocks_arg, unsigned char * entry_SRAM); 
void * alloc(void * s);
void free(void * s, void* p); 
void heap_info(void * s); 

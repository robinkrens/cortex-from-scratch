/* Fixed size memory pool allocation  */
extern void pool_init(size_t size_arg, unsigned int blocks_arg, uint32_t * entry_SRAM); 
extern void * alloc();
extern void free(void* p); 

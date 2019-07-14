#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>

uint32_t free_base;


void mm_init() {

	free_base = MEM_SIZE;

}

size_t mm_free() {

	return MEM_SIZE; 

}


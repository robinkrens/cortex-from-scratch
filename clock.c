#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>


void clock_init() {


      *RCC_CR = *RCC_CR | 0x00010000;
      for (int i = 0; i < 200; i++) {
      }
      *RCC_CFGR = *RCC_CFGR | 0x05000001;
}

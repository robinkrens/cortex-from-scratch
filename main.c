#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h> // <-- your own header file located located in ./include


void *memcpy(void *dest, void *src, size_t count)
{   
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

/* fillout memory with 'val' (i.e. all zeroes)
 */
void *memset(void *dest, unsigned char val, size_t count)
{
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

/* same as above but shorter */
unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

int strlen(const char *str)
{
    int retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}

unsigned char inportb (uint32_t _addr) 
{
    unsigned char rv;
    //__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;

}

void outportb (uint32_t _addr, unsigned char _data)
{

  // __asm__ __volatile__ ("str %1, %0"  : "=r" (_data)  : "m" (_addr) );

   // __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void main()
{
	uart_init();
	uart_puts("LOADING SYSTEM...\n");

	ivt_init();

    	//extern void stub();
	//stub();

	// loop
	for(;;) {

 	}
}

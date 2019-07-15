#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h> // <-- your own header file located located in ./include
#include <mmap.h>


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




void main()
{

	ivt_init();
//	clock_init();
	uart_init();
	systick_init();
	uart_puts("LOADING SYSTEM 0.1 ...\n");
	sysinfo();
	mm_init();

	int * p2 = mm_alloc(200);
	*p2 = 0x12345678;

	test_memory(p2);

	addrtohex(p2);
	addrtohex(*p2);


	//addrtohex((volatile uint32_t) 0x12345678 );
	//addrtohex((volatile uint32_t) SCB_VTOR );

//	asm("cpsie i"); // enable irq , cpsied f (disable faukts(

	// loop
	for(;;) {

 	}
}

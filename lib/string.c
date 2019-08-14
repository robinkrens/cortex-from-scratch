/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Re-implementation of POSIX String functions
 *
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <lib/string.h> 

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

int strcmp(char * a, char * b) {

	int alen = strlen(a);
	int blen = strlen(b);

	if (alen != blen)
		return 0;

	for (; alen != 0; alen--) {
			if (*a++ != *b++)
				return 0;
	}
	return 1;
}

char * strchr(const char *s, char c) {
         for (; *s; s++)
                 if (*s == c)
                         return (char *) s;
         return 0;
}


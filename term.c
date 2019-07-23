#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stm32.h>
#include <mmap.h>

#define SERIAL 1
#define BUFSIZE 256

static char buf[BUFSIZE];

/* Abstraction layer for I/O communication */

char getchar(void) {
        char c;
	if (SERIAL) {
	         while ((c = uart_getc()) == 0);
                /* do nothing */
	}
        return c;
}

void cputchar(char c) {

	if (SERIAL) {
		uart_putc(c);
	}

}

void cputs(unsigned char *str) {
     
     int i;
     for (i = 0; i < strlen(str); i++)     {
         cputchar(str[i]);
    }

}

void terminal() {
 
	char *buf;
        cputs("Terminal running!\n");
 
         while (1) {
                 buf = readline("> ");
                 /* if (buf != NULL)
                         if (runcmd(buf, tf) < 0)
                                 break; */
         }
}

char * readline(char *prompt)
{
	int i, c, echoing;

	if (prompt != NULL)
		cputs(prompt); 

	i = 0;
	echoing = 1;
	while (1) {
		c = getchar();
		if (c < 0) {
			cputs("read error");
			return NULL;
		} else if ((c == '\b' || c == '\x7f') && i > 0) {
			if (echoing)
				cputchar('\b');
			i--;
		} else if (c >= ' ' && i < BUFSIZE-1) {
			if (echoing)
				cputchar(c);
			buf[i++] = c;
		} else if (c == '\n' || c == '\r') {
			if (echoing)
				cputchar('\n');
			buf[i] = 0;
			return buf;
		}
	}
}

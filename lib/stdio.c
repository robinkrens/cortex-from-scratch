#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <lib/stdio.h>
#include <lib/string.h>
#include <drivers/uart.h>

#define SERIAL 1
#define TFT 0
#define BUFSIZE 256

static char stdbuf[BUFSIZE];

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
			stdbuf[i++] = c;
		} else if (c == '\n' || c == '\r') {
			if (echoing)
				cputchar('\n');
			stdbuf[i] = 0;
			return stdbuf;
		}
	}
}

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h>
#include <sys/mmap.h>

#include <lib/stdio.h>

#define SERIAL 1
#define BUFSIZE 200


int help(int, char**);

/* 
 * Built in commands
 * 	info -- shows basic info of system
 * 	reset -- software reset
 * 	show [ADDRESS-ADDRESS] -- shows SRAM range
 * 	switchmode -- switch to unprivileged mode
 * */

static char buf[BUFSIZE];


struct cmd {
	char * name;
	char * description;
	int (*function)(int argc, char ** argsv);
};

static struct cmd builtin[] = 
	{ "info", "show info", help};

int help(int argc, char ** argsv) {
	sysinfo();
	return 0;
}

void terminal() {
 
	char *buf;
        cputs("Terminal running!\n");
 
         while (1) {
                 buf = readline("root# ");
                 /* if (buf != NULL)
                         if (runcmd(buf, tf) < 0)
                                 break; */
         }
}

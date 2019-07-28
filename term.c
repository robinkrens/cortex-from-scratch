#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h>
#include <sys/mmap.h>

#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/regfunc.h>

#include <drivers/led.h>

#define SERIAL 1
#define BUFSIZE 200
#define MAXARGS 5
#define WHITESPACE "\t\r\n "
#define BUILTINCMDS 3

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
	int (*function)(int argc, char ** argsv);
};

struct cmd builtincmds[4];

int help(int argc, char ** argsv) {
	sysinfo();
	return 0;
}

int led(int argc, char ** argsv) {


	if (argsv[1] != NULL) {
		if (strcmp(argsv[1], "on")) {
			cputs("LED ON\n");
			led_on();
			}
		else if (strcmp(argsv[1], "off")) {
			cputs("LED OFF\n");
			led_off();
			}
	}
	return 0;
}

int show(int argc, char ** argsv) {

	if ((argsv[1] != NULL) && (strlen(argsv[1]) == 8)) {
	
		uint32_t * check = (uint32_t *) hextoreg(argsv[1]);
		cputs("REGISTER 0x");
		cputs(argsv[1]);
		cputs(" VALUE: ");
		cputs(regtohex(*check));
		cputchar('\n');
		return 1;
	
	}

	return 0;
}

int exec_cmd(char * buf) {

	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cputs("Too many arguments");
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < BUILTINCMDS; i++) {
		if (strcmp(argv[0], builtincmds[i].name))
			return builtincmds[i].function(argc, argv);
	}
	cputs("Unknown command");
	return 0;

} 

void terminal() {

       	builtincmds[0].name = "help";
	builtincmds[0].function = help;

	builtincmds[1].name = "led";
	builtincmds[1].function = led;

	builtincmds[2].name = "show";
	builtincmds[2].function = show;

	char *buf;
        cputs("WELCOME TO ROBSYS!\n");
 
         while (1) {
                 buf = readline("root# ");
                 	if (buf != NULL)
                         if (exec_cmd(buf) < 0)
                                 break;
         }
}


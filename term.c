/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/8/14 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Small terminal with some built-in debug commands
 * 
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h>
#include <sys/mmap.h>

#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/regfunc.h>
#include <lib/tinyprintf.h>

#include <drivers/led.h>

#define SERIAL 1
#define BUFSIZE 200
#define MAXARGS 5
#define WHITESPACE "\t\r\n "
#define BUILTINCMDS 4

/* 
 * Built in commands
 * 	info -- shows basic info of system
 * 	uptime -- uptime; read from the RTC register
 * 	reset -- software reset TODO
 * 	showmem xxxxxxxx -- shows address value
 * 	led -- led on/off
 * 	switchmode -- switch to unprivileged mode TODO
 * */

static char buf[BUFSIZE];


struct cmd {
	char * name;
	int (*function)(int argc, char ** argsv);
};

struct cmd builtincmds[BUILTINCMDS];

int info(int argc, char ** argsv) {
	sysinfo();
	return 0;
}

int uptime(int arg, char ** argsv) {
	printf("CURRENT UPTIME: %d seconds \n", *RTC_CNTL);
}

int led(int argc, char ** argsv) {

	if (argsv[1] != NULL) {
		if (strcmp(argsv[1], "on")) {
			printf("LED ON\n");
			led_on();
			}
		else if (strcmp(argsv[1], "off")) {
			printf("LED OFF\n");
			led_off();
			}
	}
	return 0;
}

int showmem(int argc, char ** argsv) {

	if ((argsv[1] != NULL) && (strlen(argsv[1]) == 8)) {
	
		uint32_t * check = (uint32_t *) hextoreg(argsv[1]);
		printf("LOCATION 0x%s, VALUE: %#x\n", argsv[1], *check);
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
			printf("Too many arguments\n");
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
	printf("Unknown command\n");
	return 0;

} 

void terminal() {

       	builtincmds[0].name = "info";
	builtincmds[0].function = info;

	builtincmds[1].name = "led";
	builtincmds[1].function = led;

	builtincmds[2].name = "showmem";
	builtincmds[2].function = showmem;

	builtincmds[3].name = "uptime";
	builtincmds[3].function = uptime;


	char *buf;
 
         while (1) {
                 buf = readline("root# ");
                 	if (buf != NULL)
                         if (exec_cmd(buf) < 0)
                                 break;
         }
}


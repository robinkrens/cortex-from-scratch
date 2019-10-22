/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/9/20 - ROBIN KRENS      
 * Initial version 
 * 
 * $DESCRIPTION$
 * 
 * |----------------------------|
 * | SYSTEM CALL 	| #	|
 * |--------------------|-------|
 * | THEOS_getenv	|	|
 * | THEOS_killenv	|	|
 * | THEOS_setenv	|	|
 * | THEOS_newenv	|	|
 * | THEOS_cputs	|	|
 * | THEOS_omnigod	|	|
 * | THEOS_brk		|	|
 * | THEOS_time		|	|
 * | THEOS_magic	|	|
 * |----------------------------|
 *
 * TODO: include in header enum
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


/*
 * This is a so-called first interrupt handler
 * The naked attribute makes sure the compiler doesn't
 * places registers on the stack.  */

__attribute__ ((naked))
void * _svc_handler(void) {

	uint32_t * current_sp;

	/* Test whether system call was invoked from supervisor (use MSP) or
	 * user (use PSP) mode */
	asm volatile (
	"tst lr, #4" "\n\t"
	"ite eq" "\n\t"
	"mrseq %0, msp" "\n\t"
	"mrsne %0, psp" : "=r" (current_sp));

	/* An exception (or interrupt) before entering this handler
	 * places the following on the stack 
	 * 
	 * R0
	 * R1
	 * R2
	 * R3
	 * R12
	 * LR
	 * PC <- placed at current_sp[6]
	 * PSR 
	 *
	 * PC contains the return address that will continue after this SVC handler
	 * is finised. The previous address (the svc # call) is at PC - 2, the
	 * first byte contains the svc number.
	 * */

	uint8_t svc_nr = ((char *) current_sp[6])[-2];

	printf("SYSTEM CALL NR: %d", svc_nr);

	for (;;);

}

void syscall(unsigned int * args) {

	uint32_t svc_number = 99;
	printf("SYSCALL NR: %x", svc_number);

	for(;;);
	/* switch(SYSCALL_NO) {
		case THEOS_cputs:
			kernel_cputs(a1, a2);
			break;
		default:
			for (;;);
	} */

}

void syscall_init() {

	/* SVC is located at position 11 in the interrupt vector table  */
	ivt_set_gate(11, _svc_handler, 0);

}

static void kernel_cputs(char * s, size_t l) {

	// TODO
}


void kernel_omnigod() {

	/* */

}




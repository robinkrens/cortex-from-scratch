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


/* the function gets called for 
 * */
__attribute__ ((naked))
void * __svc_handler__(int x) {

	uint8_t svc_nr;

	asm volatile (
	"tst lr, #4" "\n\t"
	"ite eq" "\n\t"
	"mrseq r0, msp" "\n\t"
	"mrsne r0, psp" "\n\t"
	"ldr r0, [r0, #24]" "\n\t"
	"ldrb %0, [r0, #-2]" : "=r" (svc_nr) );

	printf("SYSTEM CALL NR: %d", svc_nr);

	for(;;);

 	volatile uint32_t * sp;
	
	asm volatile (
	"tst lr, #4" "\n\t" 
	"ite eq" "\n\t"
	"mrseq %0, msp" "\n\t" 
	"mrsne r0, psp" : "=r" (sp) );


	for (int i = 0; (sp + i) < 0x20010000; i++) {
		printf("ADDRESS: %p, VALUE: %x\n", (sp + i), *(sp + i));
	}

	for (;;);	
	//asm ("mov %0, pc" : "=r" (link_register));
	//printf("%x\n", link_register);
	
	volatile uint32_t * svc_number = (uint32_t *) 0x20022222;
	*svc_number = sp[-6];
	//       	= sp[6];
	//uint8_t tmp  = link_register[-2];
	printf("SVC nr: %x", *svc_number);
	//printf("%d, %d, %d, %d, %d, %d\n", msp[6], msp[0], msp[1], msp[2], msp[3], msp[4]);
	
	for (;;);
	//return NULL;	
	return 0;

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
//	extern void * _syscall;
	extern void * hardfault;

	ivt_set_gate(11, __svc_handler__, 0);


}

static void kernel_cputs(char * s, size_t l) {

	// TODO
}


void kernel_omnigod() {

	/* */

}




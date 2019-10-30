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
 * | THEOS_INIT		| 1	|
 * | THEOS_DESTROY	| 2	|
 * | THEOS_RESCHED	| 3	|
 * | THEOS_GETC		| 4	|
 * | THEOS_CPUTS	| 5	|
 * | THEOS_UPTIME	| 6	|
 * | THEOS_OMNIGOD	| 7	|
 * | THEOS_TIME		| 8	|
 * | THEOS_MAGIC	| 99	|
 * |----------------------------|
 *
 * TODO: include in header enum
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h>
#include <sys/mmap.h>
#include <sys/process.h>

#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/regfunc.h>
#include <lib/tinyprintf.h>

enum {
	THEOS_INIT = 1,
	THEOS_CREATE,
	THEOS_DESTROY,
	THEOS_SWITCH,
	THEOS_GETC,
	THEOS_CPUTS,
	THEOS_UPTIME
};


static int sys_uptime() {

	int uptime = *RTC_CNTL;
	return uptime;
}

/* Context switch routine, saves stack pointer of 
 * the current process, loads the stack pointer
 * of the next.
 *
 * Other registers and flags (i.e. PSR, PC are popped
 * by exit of this system call.
 * Precondition: Usage of PSP   */
int sys_switch(process_t * currp, process_t * newp) {

	uint32_t tmp_stackptr = 0xFFFFFFFF;
	asm volatile("mrs %0, psp" : "=r" (tmp_stackptr));
	currp->stackptr = tmp_stackptr;	

	asm volatile("msr psp, %0" : : "r" (newp->stackptr) ); // load new pointer 
	uint32_t chk_stackptr;
	asm volatile("mrs %0, psp" : "=r" (chk_stackptr));
	//asm volatile("isb");
	printf("OLD: %x\n", currp->stackptr);
	printf("NEW: %x\n", newp->stackptr);
	printf("CHK: %x\n", chk_stackptr);
	//for(;;);

	return 0;

}

int sys_init(process_t * p) {

	asm volatile("msr psp, %0" : : "r" (p->stackptr));	
	return 0;
}

static int sys_stub() {

	return 0;

}

/*
 * This is a so-called first interrupt handler
 * The naked attribute makes sure the compiler doesn't
 * places registers on the stack */

/* 
 * SVC is similar to an exception or interrupt
 * A stack frame is pushed on entry and popped
 * on exit BY PROCESSOR. Please see in-function command */

__attribute__ ((naked))
void * _svc_handler(void) {

	uint32_t * callee_sp;

	/* Test whether system call was invoked from supervisor (use MSP) or
	 * user (use PSP) mode */
	asm volatile (
	"tst lr, #4" "\n\t"
	"ite eq" "\n\t"
	"mrseq %0, msp" "\n\t"
	"mrsne %0, psp" "\n\t"
	"push {lr}" "\n\t"
	"push {r4-r11}" : "=r" (callee_sp));

	/* An exception (or interrupt) before entering this handler
	 * places the following on the stack 
	 * 
	 * R0 <- args[0]
	 * R1 <- args[1]
	 * R2 
	 * R3
	 * R12
	 * LR <- Not a real LR, A certain 'mask'
	 * PC <- placed at callee_sp[6], see description below
	 * PSR <- Status of processor before call
	 *
	 * PC contains the return address that will continue after this SVC handler
	 * is finised. The previous address (the svc # call) is at PC - 2, the
	 * first byte contains the svc number.
	 * */

	uint8_t svc_nr = ((char *) callee_sp[6])[-2];
	int ret = -1;

	switch(svc_nr) {
		case THEOS_INIT:
			sys_init((process_t *) callee_sp[0]);
			asm volatile (
				"ldr r0, =0xFFFFFFFD" "\n\t"
				"mov lr, r0" "\n\t"
				"bx lr");
			break;
		case THEOS_SWITCH:
			sys_switch((process_t *) callee_sp[0], (process_t *) callee_sp[1]);
			//asm volatile (
			//	"ldr r0, =0xFFFFFFFD" "\n\t"
			//	"mov lr, r0" "\n\t"
			//	"bx lr");
			
			break;
		case THEOS_CPUTS:  
			break;
		case THEOS_GETC:
			break;
		case THEOS_UPTIME:
			ret = sys_uptime();
			break;
		default:
			break;
	}

	//printf("SYSTEM CALL NR: %d\n", svc_nr);
	/* Check arguments, placed in r0, r1, r2 
	 * (can get more if you save the stack) */
	//uint32_t arg1 = callee_sp[0];
	//uint32_t arg2 = callee_sp[1];
	//uint32_t arg3 = callee_sp[2];
	//printf("ARGUMENTS: %x, %x, %x\n", arg1, arg2, arg3);

	/* Return value in r0 for callee */
	callee_sp[0] = ret;

	asm volatile ("pop {r4-r11}");
	/* exception return, 0xFFFFFFFX (<-- last value is flag
	 * and gives the processor information to return to what */
	asm volatile ("pop {lr}"); 
	asm volatile ("bx lr");


}

void syscall_init() {
	/* SVC is located at position 11 in the interrupt vector table  */
	ivt_set_gate(11, _svc_handler, 0);
}



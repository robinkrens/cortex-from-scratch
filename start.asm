/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 */

/* _start sets up the stack and jumps to the reset vector */

	.equ STACK_TOP, 0x20010000 /* placed at 64kB, top of SRAM */
	.text
	.global _start
	.global reset, nmi, hardfault
	.global _svc_handler
	.code 16
	.syntax unified
_start:
	.word STACK_TOP, reset, nmi, hardfault
	.type reset, function

/* A reset vector (or bootcode) will call main in main.c
   this is the so called 'entry to C' */
reset:
	b main
	b reset 

/* These are consequently the nmi and hardfault vector handlers
   before booting and entering main, these can actually be called
   (machine somehow has a failure). That's why they are included here.
   Later the interrupt vector will be relocated to SRAM and modified.  */

nmi:
	b nmi

hardfault:
	mov r5, 9
	b hardfault2

hardfault2:
	b hardfault2

	.end


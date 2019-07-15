	.equ STACK_TOP, 0x20008000 /* placed at 32kB, TODO: could place at top of SRAM? */
	.text
	.global _start
	.global reset, nmi, hardfault
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
   Later the interrupt vector
   will be relocated to SRAM and the will be copied / modified.  */

nmi:
	b nmi

hardfault: 
	b hardfault
.global stub
stub:
	mov r1, #'z'
	ldr r0, [r1]
	bx lr
	/* ldr R0,=10
	mov R1,#0
	udiv.w R2, R0, R1 */

/*	.data
	.word 'x' */
	.end


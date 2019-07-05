	.equ STACK_TOP, 0x20000800
	.text
	.global _start
	.code 16
	.syntax unified
_start:
	.word STACK_TOP, start
	.type start, function

/* Start of main program */
start:
	movs r0, #5
	movs r1, #1
loop:
	adds r1, r0
	subs r0, #1
	bne loop
	/* Result is now in R1 */
	ldr r0, =Result
	str r1, [r0] 

deadloop:
	b deadloop

	.data
	.word 0 /* add two words*/
	.word 0 
Result:
	.word 0 /* results now is in 0x20000008 (0x20000000 + 2 w) */
	.end


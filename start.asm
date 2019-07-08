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
	b main
	b start

	.data
Result:
	.word 'X' /* results now is in 0x20000008 (0x20000000 + 2 w) */
	.end


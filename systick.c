/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/8/14 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * SysTick of Cortex M* MCUs. Have a look at the more complex RTC 
 * in case more accurate timing is needed.
 *
 * 
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h>
#include <sys/mmap.h>

#include <lib/regfunc.h>
#include <lib/tinyprintf.h>

#define NUMB_TASKS 3

int count;
int next_task;
int curr_task;
uint32_t currentp;
uint32_t nextp;

//struct proces {
//	int32_t nr;
//	uint32_t *stack_ptr;
//	uint32_t mask;
//};
//
//struct proces p1, p2, p3;
//
//
//	uint32_t oldpsp;
//	uint32_t newpsp;
//
//uint32_t task0_stack[50];
//uint32_t task1_stack[50];
//uint32_t task2_stack[50];
//uint32_t PSP_array[3];

struct interrupt_frame {

        uint32_t r0; // N-32
        uint32_t r1;
        uint32_t r2;
        uint32_t r3;
        uint32_t r12;
        uint32_t lr;
        uint32_t pc;
        uint32_t psr; // N-4
};

//__attribute__ ((interrupt))
void * systick_handler(/* struct interrupt_frame * frame */) {

	//printf("Ticking...\n");
	//printf("Current task: %d", curr_task);
	//count++;




	//switch(curr_task) {
	//	case(0): next_task = 1; break;
	//	case(1): next_task = 2; break;
	//	case(2): next_task = 0; break;
	//	default: next_task = 0; break;
	//} 

	//if (curr_task != next_task) {
	
		
	
	//} 

}


__attribute__ ((naked))
       	void * pendsv_handler_c(void) {
//
//	asm volatile ("push {r0-r11, lr}");
//	asm volatile ("mrs %0, psp" : "=r" (oldpsp));
//	asm volatile ("push {lr}");
//
//	//asm volatile ("push {lr}");
//	
//	//printf("FROM MSP %x", oldpsp);
//	//PSP_array[curr_task] = oldpsp;
//	//curr_task = next_task;
//	//newpsp = PSP_array[next_task];	
//
//	asm volatile ("msr psp, %0" : : "r" (newpsp)); 
//
//	asm volatile("pop {lr}");
//	//asm volatile ("pop {r0-r12}");
//	asm volatile("bx lr"); // return
}

uint32_t set_psp(uint32_t) __attribute__( ( naked ) );
uint32_t set_psp(uint32_t stackie) {
  
	asm volatile ("msr psp, r0" "\n\t"
		  "bx lr");
}

void systick_init() {

//	count = 0;
//	curr_task = 0;
//	next_task = 1;

	/* Every time the counter counts down to zero
	 * a systick exception is invoked. Systick has
	 * exception number 15. in the vector table  */
	ivt_set_gate(15, systick_handler, 0); 

	/* Get calibration and set this to 1 sec
	 * !Most boards have a 1 ms or 10 ms 
	 * calibration value */
	int calib = (*STK_CALIB << 0) * 200;

	/* The counter reload registers counts down to zero
	 * and then it is restores the value */
	rwrite(STK_RELOAD, calib);
	
	/* Enable the counter and enable the interrupt
	 * associated with it */
	rsetbit(STK_CTRL, 0);
	rsetbit(STK_CTRL, 1);

//	extern void task0(void), task1(void), task2(void);
//
//	int size_stack = sizeof(task0_stack);
//	
//	p1.stack_ptr  = ((unsigned int) task0_stack) + size_stack - 0x1C;
//	p1.stack_ptr[6] = (uint32_t) task0;
//	p1.stack_ptr[7] = 0x01000000;
//	p2.stack_ptr  = ((unsigned int) task0_stack) + size_stack - 0x1C;
//	p2.stack_ptr[6] = (uint32_t) task1;
//	p2.stack_ptr[7] = 0x01000000;
//	p3.stack_ptr  = ((unsigned int) task0_stack) + size_stack - 0x1C;
//	p3.stack_ptr[6] = (uint32_t) task2;
//	p3.stack_ptr[7] = 0x01000000;
//
//	set_psp(p1.stack_ptr[-7]);
//
//	for(;;);	

	/* Initialize processes */
	//PSP_array[0] = ((unsigned int) task0_stack) + sizeof(task0_stack) - 32*4;
	//HW_ADDR(PSP_array[0] + 0x18) = (unsigned long) task0;
        //HW_ADDR(PSP_array[0] + 0x1C) = 0x01000000;	
	//PSP_array[1] = ((unsigned int) task1_stack) + sizeof(task1_stack) - 32*4;
	//HW_ADDR(PSP_array[1] + 0x18) = (unsigned long) task1;
        //HW_ADDR(PSP_array[1] + 0x1C) = 0x01000000;	
	//PSP_array[2] = ((unsigned int) task2_stack) + sizeof(task2_stack) - 32*4;
	//HW_ADDR(PSP_array[2] + 0x18) = (unsigned long) task2;
        //HW_ADDR(PSP_array[2] + 0x1C) = 0x01000000;	

	//extern void pendsv_handler;

	ivt_set_gate(14, pendsv_handler_c, 0);

	//set_psp((PSP_array[curr_task] + 32*4 ));
	
	//int startpsp = PSP_array[curr_task] + 16*4;

	//asm volatile ("msr psp, %0" : : "r" (startpsp));
	
	
	//asm volatile ("mov r0, 0x3" "\n\t" 
	//"msr control, r0" "\n\t"
	//"isb" "\n\t"); 

	//for(;;);

	//set current PSP
	//printf("0: %x\n", PSP_array[0]);
	//printf("1: %x", PSP_array[0]);

	//task0();

}

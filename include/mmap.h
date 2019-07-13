/* 
 * Example memory map for the Cortex-A3
 * Implementations vary among manufacturers. This one is
 * a STM32F013RC6. Addresses of peripherals vary amongst 
 * manufacturers of boards with similar chips
 * 
 * */

/* RANDOM DEBUG LOCATION */
#define RANDOM_ADDR ((volatile uint32_t *)(0x21000000))

/* SYSTEM CONTROL BLOCK REGISTER */
#define SCB_VTOR ((volatile uint32_t *)( 0xE000ED08)) // VECTOR TABLE

/* NESTED VECTOR INTERRUPT CONTROL REGISTER */
#define NVIC_ISER0 ((volatile uint32_t*)( 0xE000E100)) // interrupt set enable register

/* SYSTICK REGISTER */
#define STK_CTRL ((volatile uint32_t *)(0xE000E010))
#define STK_RELOAD ((volatile uint32_t *)(0xE000E014))


/* SYSTEM CONTROL REGISTER */
#define SYSCTRL_RCC ((volatile unsigned long *)(0x40021000))
#define RCC_APB2ENR ((volatile unsigned long *)(0x40021018)) // register to enable USART1

#define SYSCTRL_RIS ((volatile unsigned long *)(0x400FE050))
#define SYSCTRL_RCGC1 ((volatile unsigned long *)(0x400FE104))
#define SYSCTRL_RCGC2 ((volatile unsigned long *)(0x400FE108))
#define GPIOPA_AFSEL ((volatile unsigned long *)(0x40004420))

#define GPIOA_CRH ((volatile unsigned long *)(0x40010804))
#define AFIO_EVCR ((volatile unsigned long *)(0x40010000))

#define USART1_SR ((volatile unsigned long *)(0x40013800))
#define USART1_DR ((volatile unsigned long *)(0x40013804))
#define USART1_BRR ((volatile unsigned long *)(0x40013808))
#define USART1_CR1 ((volatile unsigned long *)(0x4001380C))
#define USART1_CR3 ((volatile unsigned long *)(0x40013814))

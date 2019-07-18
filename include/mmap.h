/* 
 * Example memory map for the Cortex-A3
 * Implementations vary among manufacturers. This one is
 * a STM32F013RC6. Addresses of peripherals vary amongst 
 * manufacturers of boards with similar chips
 * 
 * */

#define BSS_BASE ((volatile uint32_t *)(0x20000800)) //TODO: .data flexible siz
#define TOTAL_MEM_SIZE 64000;

/* SYSTEM INFO AND DEBUG */
#define MCU_ID ((volatile uint32_t*)( 0xE000ED00)) 
#define FLASH_MEM ((volatile uint32_t*)( 0x1FFFF000)) 

/* SYSTEM CONTROL BLOCK REGISTER */
#define SCB_VTOR ((volatile uint32_t *)( 0xE000ED08)) // VECTOR TABLE

/* NESTED VECTOR INTERRUPT CONTROL REGISTER */
#define NVIC_ISER0 ((volatile uint32_t*)( 0xE000E100)) // interrupt set enable register
#define NVIC_ISER1 ((volatile uint32_t*)( 0xE000E104)) // interrupt set enable register

/* SYSTICK REGISTER */
#define STK_CTRL ((volatile uint32_t *)(0xE000E010))
#define STK_RELOAD ((volatile uint32_t *)(0xE000E014))

/* CLOCK REGISTER */
#define RCC_CR ((volatile uint32_t *)(0x40021000))
#define RCC_CFGR ((volatile uint32_t *)(RCC_CR + 0x04))

/* SYSTEM CONTROL REGISTER */
#define SYSCTRL_RCC ((volatile unsigned long *)(0x40021000))
#define RCC_APB2ENR ((volatile uint32_t *)(0x40021018)) // register to enable USART1

#define SYSCTRL_RIS ((volatile unsigned long *)(0x400FE050))
#define SYSCTRL_RCGC1 ((volatile unsigned long *)(0x400FE104))
#define SYSCTRL_RCGC2 ((volatile unsigned long *)(0x400FE108))
#define GPIOPA_AFSEL ((volatile unsigned long *)(0x40004420))

#define GPIOA_CRH ((volatile unsigned long *)(0x40010804))

#define AFIO_EVCR ((volatile uint32_t *)(0x40010000))
//#define AFIO_EXTICR1 ((volatile uint32_t *)(AFIO_EVCR + 0x08))

/* EXTERNAL INTERRUPTS */
#define EXTI_IMR ((volatile uint32_t *)(0x40010400))
#define EXTI_RTSR ((volatile uint32_t *)  (EXTI_IMR + 0x08))
//#define EXTI_FTSR ((volatile uint32_t *)  (EXTI_IMR + 0x04))

/* UART1 REGISTERS */
#define USART1_BASE ((volatile uint32_t) (0x40013800))
#define USART1_SR ((volatile uint32_t *) (USART1_BASE))
#define USART1_DR ((volatile uint32_t *)  (USART1_BASE + 0x04))
#define USART1_BRR ((volatile uint32_t *) (USART1_BASE + 0x08))
#define USART1_CR1 ((volatile uint32_t *) (USART1_BASE + 0x0C))
#define USART1_CR2 ((volatile uint32_t *) (USART1_BASE + 0x10))
#define USART1_CR3 ((volatile uint32_t *) (USART1_BASE + 0x14))

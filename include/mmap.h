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
#define SYSCTRL_RCC ((volatile unsigned long *)(0x400FE060))
#define SYSCTRL_RIS ((volatile unsigned long *)(0x400FE050))
#define SYSCTRL_RCGC1 ((volatile unsigned long *)(0x400FE104))
#define SYSCTRL_RCGC2 ((volatile unsigned long *)(0x400FE108))
#define GPIOPA_AFSEL ((volatile unsigned long *)(0x40004420))

/* USART REGISTERS */
#define UART0_DATA ((volatile unsigned long *)(0x4000C000))
#define UART0_FLAG ((volatile unsigned long *)(0x4000C018))
#define UART0_IBRD ((volatile unsigned long *)(0x4000C024))
#define UART0_FBRD ((volatile unsigned long *)(0x4000C028))
#define UART0_LCRH ((volatile unsigned long *)(0x4000C02C))
#define UART0_CTRL ((volatile unsigned long *)(0x4000C030))
#define UART0_RIS ((volatile unsigned long *)(0x4000C03C))

#define UART1_DATA ((volatile unsigned long *)(0x4000D000))
#define UART1_FLAG ((volatile unsigned long *)(0x4000D018))
#define UART1_IBRD ((volatile unsigned long *)(0x4000D024))
#define UART1_FBRD ((volatile unsigned long *)(0x4000D028))
#define UART1_LCRH ((volatile unsigned long *)(0x4000D02C))
#define UART1_CTRL ((volatile unsigned long *)(0x4000D030))
#define UART1_RIS ((volatile unsigned long *)(0x4000D03C))

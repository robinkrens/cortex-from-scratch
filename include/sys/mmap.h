/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/20 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * Memory map for the Cortex-A3
 * Implementations vary among manufacturers. This one is
 * a STM32F013RC6. Addresses of peripherals vary amongst 
 * manufacturers of boards with similar chips
 *
 * $USAGE$
 * These are volatile memory addresses of 32 bit. The macro's
 * MEM_VALUE and MEM_ADDR should used.
 * In case you want to use a address on the lside of a assigment
 * use volatile uint32_t * p = MEM_ADDR(0x20000000); 
 *
 * */

#define OWRITE	0x01
#define SETBIT	0x02
#define CLRBIT	0x03

/* 64kB SRAM located at SRAM_OFFSET */
#define SRAM_SIZE	0x00010000 
#define SRAM_OFFSET	0x20000000

/* Safety macro's to get the address or value */
#define MEM_VALUE(addr) *((volatile uint32_t(*) (addr))
#define MEM_ADDR(addr) ((volatile uint32_t *) (addr))

/* SYSTEM INFO AND DEBUG */
#define MCU_ID MEM_ADDR(0xE000ED00) 
#define FLASH_MEM MEM_ADDR(0x1FFFF000) 

/* SYSTEM CONTROL BLOCK REGISTER */
#define SCB_VTOR MEM_ADDR(0xE000ED08) // VECTOR TABLE
#define SCB_VTOR_ST MEM_ADDR(0xE000ED04) // STATUS OF VECTOR
#define SCB_CCR MEM_ADDR(0xE000ED14) // SET SOFTWARE TRAPS

/* NESTED VECTOR INTERRUPT CONTROL REGISTER */
#define NVIC_ISER0 MEM_ADDR(0xE000E100) // interrupt set enable register
#define NVIC_ISER1 MEM_ADDR(0xE000E104) // interrupt set enable register

/* SYSTICK REGISTER */
#define STK_CTRL MEM_ADDR(0xE000E010)
#define STK_RELOAD MEM_ADDR(0xE000E014)

/* CLOCK REGISTER */
#define RCC_CR MEM_ADDR(0x40021000)
#define RCC_CFGR MEM_ADDR(0x40021004)

/* SYSTEM CONTROL REGISTER */
#define SYSCTRL_RCC MEM_ADDR(0x40021000)
#define RCC_APB1ENR MEM_ADDR(0x4002101C) // register to enable I2C
#define RCC_APB2ENR MEM_ADDR(0x40021018) // register to enable USART1

#define SYSCTRL_RIS MEM_ADDR(0x400FE050)
#define SYSCTRL_RCGC1 MEM_ADDR(0x400FE104)
#define SYSCTRL_RCGC2 MEM_ADDR(0x400FE108)
#define GPIOPA_AFSEL MEM_ADDR(0x40004420)

#define GPIOA_CRH MEM_ADDR(0x40010804) // for USART1
#define GPIOB_CRL MEM_ADDR(0x40010C00) // low register (!) for I2C1
#define GPIOC_CRL MEM_ADDR(0x40011000) // for led
#define GPIOC_ODR MEM_ADDR(0x4001100C) //

#define AFIO_EVCR MEM_ADDR(0x40010000)

/* I2C REGISTER */
#define I2C_CR1 MEM_ADDR(0x40005400)
#define I2C_CR2 MEM_ADDR(0x40005404)
#define I2C_DR MEM_ADDR(0x40005410)
#define I2C_SR1 MEM_ADDR(0x40005414)
#define I2C_SR2 MEM_ADDR(0x40005418)
#define I2C_CCR MEM_ADDR(0x4000541C)
#define I2C_TRISE MEM_ADDR(0x40005420)

/* EXTERNAL INTERRUPTS */
#define EXTI_IMR MEM_ADDR(0x40010400)
#define EXTI_RTSR MEM_ADDR(0x40010408)

/* UART1 REGISTERS */
#define USART1_BASE MEM_ADDR(0x40013800)
#define USART1_SR MEM_ADDR(0x40013800)
#define USART1_DR MEM_ADDR(0x40013804)
#define USART1_BRR MEM_ADDR(0x40013808)
#define USART1_CR1 MEM_ADDR(0x4001380C)
#define USART1_CR2 MEM_ADDR(0x40013810)
#define USART1_CR3 MEM_ADDR(0x40013814)

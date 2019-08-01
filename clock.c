/* (CC-BY-NC-SA) ROBIN KRENS - ROBIN @ ROBINKRENS.NL
 * 
 * $LOG$
 * 2019/7/30 - ROBIN KRENS	
 * Initial version 
 * 
 * $DESCRIPTION$
 * 1. Routines to setup the high speed external (HSE) clock. 
 * Initially a (less accurate) high speed internal (HSI) 
 * clock is used. PPL is enabled; HSE is input for PPL.
 * PPL is multiplied to get the desired clock speed. 
 *
 * Some buses might not support the maximum speed and
 * should be prescaled (i.e. low speed APB)
 * 
 * $USAGE
 * Check external crystals on board and maximum speed
 * for buses. In this example, a 8 Mhz external crystal
 * is used. CPU speed is 36 Mhz. No prescaling is done.
 * 
 * */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/robsys.h>
#include <sys/mmap.h>

#include <lib/regfunc.h>

static void setup_hse() {

	rsetbit(RCC_CR, 16); /* HSE enable */
	rsetbit(RCC_CFGR, 17); /* HSE divider for PLL entry */
	
	while(!rchkbit(RCC_CR, 17)); /* Wait for HSE to come up */
	
	rsetbitsfrom(RCC_CFGR, 18, 0x7); /* PLL Multiplayer (x9) */
	rsetbit(RCC_CFGR, 16); /* HSE as PPL clock source */
	rsetbit(RCC_CR, 24); /* PLL enable */
	//rsetbitsfrom(RCC_CFGR, 8, 0x4); /* APB low speed prescraler */
	rsetbitsfrom(RCC_CFGR, 0, 0x2); /* use PPL as system clock */

	while(!rchkbit(RCC_CFGR, 3)); /* Wait for the clock switch to complete */
}

void clock_init() {

#ifdef ENABLE_HSE
setup_hse();
#endif

}


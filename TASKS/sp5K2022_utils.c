/*
 * sp5K2022_utils.c
 *
 *  Created on: 28 jul. 2022
 *      Author: pablo
 */


#include "sp5K2022.h"

//------------------------------------------------------------------------------
void reset(void)
{
	/* Reseteo al micro activando el watchdog */
	cli();
	wdt_reset();
	/* Start timed equence */
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	/* Set new prescaler(time-out) value = 64K cycles (~0.5 s) */
	WDTCSR = (1<<WDE) | (1<<WDP2) | (1<<WDP0);
	sei();

	wdt_enable(WDTO_30MS);
	while(1) {}
}
//------------------------------------------------------------------------------


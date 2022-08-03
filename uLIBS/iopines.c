/*
 * iopines.c
 *
 *  Created on: 1 ago. 2022
 *      Author: pablo
 */


#include "iopines.h"

// ---------------------------------------------------------------
void TICK_init(void)
{
	// Configura el pin del led como output ( 1 en DDR )
	//LED_DDR |= LED_bm;
	TICK_DDR |= TICK_bm;
	CLEAR_TICK();
}
// ---------------------------------------------------------------
void TICK_flash(void)
{
	// Prende el led 50ms y lo apaga
	SET_TICK();
	vTaskDelay( (TickType_t)( 100 / portTICK_PERIOD_MS ) );
	CLEAR_TICK();
}
// ---------------------------------------------------------------


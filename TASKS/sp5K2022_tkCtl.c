/*
 * sp5K2022_tkCtl.c
 *
 *  Created on: 28 jul. 2022
 *      Author: pablo
 */


#include "sp5K2022.h"
#include "led.h"

//------------------------------------------------------------------------------
void tkCtl(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );

	LED_init();

	starting_flag = true;

	for( ;; )
	{
		wdt_reset();
		vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
		TOGGLE_LED();

	}
}
//------------------------------------------------------------------------------

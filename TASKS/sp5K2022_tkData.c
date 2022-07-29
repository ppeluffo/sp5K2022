/*
 * sp5K2022_tkData.c
 *
 *  Created on: 28 jul. 2022
 *      Author: pablo
 */


#include "sp5K2022.h"

//------------------------------------------------------------------------------
void tkData(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;

	while (! starting_flag )
		vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );


	for( ;; )
	{
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
		xprintf("Prueba sp5K2022\r\n");
		xfprintf(fdCOMMS, "Prueba sp5K2022\r\n");

	}
}
//------------------------------------------------------------------------------

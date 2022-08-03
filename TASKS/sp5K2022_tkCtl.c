/*
 * sp5K2022_tkCtl.c
 *
 *  Created on: 28 jul. 2022
 *      Author: pablo
 */


#include "sp5K2022.h"

void pv_flash_led(void);

//------------------------------------------------------------------------------
void tkCtl(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );

	TICK_init();
	CONFIG_UARTCTL_ASOUTPUT();
	ENABLE_UARTCTL();

	MCP0_init();
	MCP1_init();

	LED_D1_init();
	LED_D2_init();
	LED_D3_init();


	xprintf("Starting tkCtl...\r\n");

	starting_flag = true;

	for( ;; )
	{
		wdt_reset();
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
		//TOGGLE_LED();
		pv_flash_led();

	}
}
//------------------------------------------------------------------------------
void pv_flash_led(void)
{
	/*
	 * Cada 5s flashea el led de la placa logica
	 */

static uint8_t led_timer = 5;

	if (led_timer-- > 0 )
		return;

	led_timer = 5;
//	MCP_setLed();
	vTaskDelay( ( TickType_t)( 50 / portTICK_PERIOD_MS ) );
//	MCP_clearLed();
}
//------------------------------------------------------------------------------

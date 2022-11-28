/*
 * sp5K2022_tkCtl.c
 *
 *  Created on: 28 jul. 2022
 *      Author: pablo
 */


#include "sp5K2022.h"

void pv_flash_led_D1(void);
void pv_flash_led_D2(void);
void pv_flash_led_D3(void);

//------------------------------------------------------------------------------
void tkCtl(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;

	vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );

	TICK_init();
	CONFIG_UARTCTL_ASOUTPUT();
	ENABLE_UARTCTL();

	MCP0_init();
	MCP1_init();

	LED_D1_init();
	LED_D2_init();
	LED_D3_init();

	systemVars.clock = 0;

	if ( ! load_config()) {
		load_defaults();
	}

	xprintf("Starting tkCtl...\r\n");

	starting_flag = true;

	for( ;; )
	{
		wdt_reset();
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
		TOGGLE_TICK();
		//pv_flash_led_D1();	// Genera 'ruido' en el bus I2C para el debug
		pv_flash_led_D2();
		pv_flash_led_D3();

		while ( xSemaphoreTake( sem_SYSVars, ( TickType_t ) 5 ) != pdTRUE )
			;
			systemVars.clock++;
		xSemaphoreGive( sem_SYSVars );
	}
}
//------------------------------------------------------------------------------
void pv_flash_led_D1(void)
{
	/*
	 * Cada 5s flashea el led de la placa logica
	 */

static uint8_t led_timer = 5;

	if (led_timer-- > 0 )
		return;

	led_timer = 5;
	if ( MCP0_status)
		LED_D1_flash();
}
//------------------------------------------------------------------------------
void pv_flash_led_D2(void)
{
	/*
	 * Cada 5s flashea el led D2 de la placa SPX_3CH
	 */

static uint8_t led_timer = 5;

	if (led_timer-- > 0 )
		return;

	led_timer = 5;
	LED_D2_flash();
}
//------------------------------------------------------------------------------
void pv_flash_led_D3(void)
{
	/*
	 * Cada 5s flashea el led D3 de la placa SPX_3CH
	 */

static uint8_t led_timer = 5;

	if (led_timer-- > 0 )
		return;

	led_timer = 5;
	LED_D3_flash();
}
//------------------------------------------------------------------------------



#include "led.h"

//------------------------------------------------------------------------------
void LED_D1_init(void)
{

}
//------------------------------------------------------------------------------
void SET_LED_D1(void)
{

uint8_t regValue;

	if (  MCP_read( MCP0, MCP0_GPIO, (char *)&regValue) ) {
		regValue |= (1<<MCP0_GPIO_OLED);		// Led_D1 = 1
		if ( MCP_write( MCP0, MCP0_GPIO, regValue) ) {
			return;
		}
	}

	return;
}
//------------------------------------------------------------------------------
void CLEAR_LED_D1(void)
{

uint8_t regValue;

	if (  MCP_read( MCP0, MCP0_GPIO, (char *)&regValue) ) {
		regValue &= ~(1<<MCP0_GPIO_OLED);		// Led_D1 = 0
		if ( MCP_write( MCP0, MCP0_GPIO,regValue) ) {
			return;
		}
	}

		return;
}
// ---------------------------------------------------------------
void TOGGLE_LED_D1(void)
{
uint8_t regValue;

	MCP_read( MCP0, MCP0_GPIO, (char *)&regValue );
	regValue ^= 0x20;		// Invierto solo el bit 6
	MCP_write( MCP0, MCP0_GPIO, regValue);
}
// ---------------------------------------------------------------
void LED_D1_flash(void)
{
	// Prende el led 50ms y lo apaga
	SET_LED_D1();
	vTaskDelay( (TickType_t)( 100 / portTICK_PERIOD_MS ) );
	CLEAR_LED_D1();
}
// ---------------------------------------------------------------
void LED_D2_init(void)
{
	// Configura el pin del led como output ( 1 en DDR )
	LED_D2_DDR |= LED_D2_bm;
	CLEAR_LED_D2();
}
// ---------------------------------------------------------------
void LED_D2_flash(void)
{
	// Prende el led 50ms y lo apaga
	SET_LED_D2();
	vTaskDelay( (TickType_t)( 100 / portTICK_PERIOD_MS ) );
	CLEAR_LED_D2();
}
// ---------------------------------------------------------------
void LED_D3_init(void)
{
	// Configura el pin del led como output ( 1 en DDR )
	LED_D3_DDR |= LED_D3_bm;
	CLEAR_LED_D3();
}
// ---------------------------------------------------------------
void LED_D3_flash(void)
{
	// Prende el led 50ms y lo apaga
	SET_LED_D3();
	vTaskDelay( (TickType_t)( 100 / portTICK_PERIOD_MS ) );
	CLEAR_LED_D3();
}
// ---------------------------------------------------------------

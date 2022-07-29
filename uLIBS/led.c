

#include "led.h"

// ---------------------------------------------------------------
void LED_init(void)
{
	// Configura el pin del led como output ( 1 en DDR )
	//LED_DDR |= LED_bm;
	sbi(LED_DDR, LED_BIT);
	APAGAR_LED();
}
// ---------------------------------------------------------------
void led_flash(void)
{
	// Prende el led 50ms y lo apaga
	PRENDER_LED();
	vTaskDelay( (TickType_t)( 100 / portTICK_PERIOD_MS ) );
	APAGAR_LED();
}
// ---------------------------------------------------------------


/*
 * sp5K2022_tkRxComms.c
 *
 *  Created on: 8 ago. 2022
 *      Author: pablo
 */

#include "sp5K2022.h"

#define RXBUFFER_SIZE	64
char buffer[RXBUFFER_SIZE];

lBuffer_s xCOMMSrxb;

//------------------------------------------------------------------------------
void tkRXComms(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;
uint8_t c = 0;
uint8_t ptr = 0;
uint16_t timeout;


	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );
    while ( ! starting_flag )
        vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );


	xprintf("Starting tkRXComms...\r\n");

	timeout = 0;
	ptr = 0;
	memset(buffer, '\0', RXBUFFER_SIZE );

	// loop
	for( ;; )
	{
        //kick_wdt(CMD_WDG_bp);

		c = '\0';	// Lo borro para que luego del un CR no resetee siempre el timer.
		// el read se bloquea 50ms. lo que genera la espera.

		// Mientras este recibiendo datos, los almaceno
		while ( frtos_read( fdCOMMS, (char *)&c, 1 ) == 1 ) {
			timeout = 0;
			buffer[ptr++] = c;
			if (ptr >= RXBUFFER_SIZE )
				ptr = 0;
		}

		// No recibi datos: espero.
		vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );

		if ( timeout++ >= 5 ) {
			// Di timeout:
			timeout = 0;
			// Hay datos en el buffer: imprimo y vacio el buffer.
			//if ( strlen(buffer) > 0 ) {
			if (ptr > 0 ) {
				xprintf_P(PSTR("%s\r\n"), &buffer );
				ptr = 0;
				memset(buffer, '\0', RXBUFFER_SIZE );
			}
		}

	}
}
//------------------------------------------------------------------------------
void tkRXComms_initoutofrtos(void)
{
	//lBchar_CreateStatic ( &xCOMMSrxb, &buffer[0], RXBUFFER_SIZE  );
	//lBchar_Flush(&xCOMMSrxb);

}
//------------------------------------------------------------------------------

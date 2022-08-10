/*
 * sp5K2022_tkDinputs.c
 *
 *  Created on: 5 ago. 2022
 *      Author: pablo
 *
 *  Esta tarea se encarga de contar los pulsos que llegan y el tiempo entre estos
 *  El ciclo son 100ms de modo que podemos contar hasta 10 pulsos por segundo.
 *  El filtrado (debounce) lo hace el propio latch por lo que no nos preocupamos.
 *  En cada ciclo:
 *  - leemos el MCP1:GPIOB y vemos si llegaron pulsos.
 *    En caso afirmativo incrementamos el contador correspondiente.
 *
 *  Medida de caudal:
 *  Para los casos que los pulsos son de caudalimetros, mas exacto es medir el tiempo entre pulsos.
 *  Como el tick es de 100ms, esta es la resolucion.
 *  Cuando llega un nuevo pulso, guardo los cticks en pulse_width ( ancho del pulso medido ) y
 *  reinicio el contador.
 *
 */


#include "sp5K2022.h"

#define NRO_DINPUTS	2


dinputs_t dinputs[2];

bool debug_din;

void pv_dinputs_init(void);
void pv_dinputs_count_pulses(void);

//------------------------------------------------------------------------------
void tkDinputs(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;
TickType_t xLastWakeTime = 0;

	while (! starting_flag )
		vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );

	xprintf("Starting tkDinputs...\r\n");
	pv_dinputs_init();
	LATCH0_CLR_init();
	LATCH1_CLR_init();
	debug_din = false;

	// Initialise the xLastWakeTime variable with the current time.
 	xLastWakeTime = xTaskGetTickCount();

	for( ;; )
	{
  		// Espero 100 ms.
  		vTaskDelayUntil( &xLastWakeTime, ( TickType_t)( 100 / portTICK_PERIOD_MS ) );

  		pv_dinputs_count_pulses();
	}
}
//------------------------------------------------------------------------------
void read_dinputs( dinputs_t *dvals, bool clear )
{

uint8_t i;

	//while ( xSemaphoreTake( sem_SYSVars, ( TickType_t ) 5 ) != pdTRUE )
	//	vTaskDelay( ( TickType_t)( 1 ) );
	memcpy( dvals, dinputs, sizeof(dinputs));
	//xSemaphoreGive( sem_SYSVars );

	if ( clear ) {
		for (i=0; i<NRO_DINPUTS; i++) {
			dinputs[i].pulse_counter = 0;
			dinputs[i].cticks = 0;
		}
	}

}
//------------------------------------------------------------------------------
void config_debug_din(bool val)
{
	debug_din = val;
}
//------------------------------------------------------------------------------
void clear_latches(void)
{
	// Borro los latches
	CLEAR_LATCH0_CLR();
	CLEAR_LATCH1_CLR();
	vTaskDelay( ( TickType_t)( 5 / portTICK_PERIOD_MS ) );
	SET_LATCH0_CLR();
	SET_LATCH1_CLR();

}
//------------------------------------------------------------------------------
void pv_dinputs_count_pulses(void)
{
	/*
	 * Cuenta los pulsos y resetea los latches.
	 */

uint8_t din0, din1;

	// Leo las entradas digitales
	if ( ! READ_DIGITAL_INPUTS( &din0, &din1 ) ) {
		return;
	}

	dinputs[0].cticks++;
	dinputs[1].cticks++;

	// Acumulo pulsos.
	if ( din0 == 0 ) {
		dinputs[0].pulse_counter++;
		dinputs[0].pulse_width = dinputs[0].cticks;
		dinputs[0].cticks = 0;
		if (debug_din) {
			xprintf_P(PSTR("DINPUTS: din0(*): count=%d,dT=%d, din1: count=%d,dT=%d\r\n"), dinputs[0].pulse_counter, dinputs[0].pulse_width, dinputs[1].pulse_counter, dinputs[0].pulse_width );
		}
	}

	if ( din1 == 0 ) {
		dinputs[1].pulse_counter++;
		dinputs[1].pulse_width = dinputs[1].cticks;
		dinputs[1].cticks = 0;
		if (debug_din) {
			xprintf_P(PSTR("DINPUTS: din0: count=%d,dT=%d, din1(*): count=%d,dT=%d\r\n"), dinputs[0].pulse_counter, dinputs[0].pulse_width, dinputs[1].pulse_counter, dinputs[0].pulse_width );
		}
	}

	clear_latches();

}
//------------------------------------------------------------------------------
void pv_dinputs_init(void)
{
uint8_t i;

	for (i=0; i<NRO_DINPUTS; i++) {
		dinputs[i].pulse_counter = 0;
		dinputs[i].cticks = 0;
		dinputs[i].pulse_width = 0;
	}
}
//------------------------------------------------------------------------------


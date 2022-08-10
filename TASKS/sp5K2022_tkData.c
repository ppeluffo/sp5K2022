/*
 * sp5K2022_tkData.c
 *
 *  Created on: 28 jul. 2022
 *      Author: pablo
 */


#include "sp5K2022.h"

typedef struct {
	dinputs_t dinputs[2];
	uint16_t ainputs[3];
	uint16_t bt;
} sysData_t;

sysData_t sysData;

//------------------------------------------------------------------------------
void tkData(void * pvParameters)
{

	/*
	 *  Cada intervalo timerpoll lee los canales analogicos, los digitales y
	 *  transmite por el cmdPort y por el modbusPort el frame
	 *
	 */


( void ) pvParameters;
TickType_t xLastWakeTime = 0;


	while (! starting_flag )
		vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );

	xprintf("Starting tkData...\r\n");

	xLastWakeTime = xTaskGetTickCount();

	// Prendo sensores y analog
	SET_OANALOG();
	SET_OPWRSENSORS();
	SET_OPWRMODBUS();
	vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );

	if ( systemVars.timerpoll < 15 ) {
		xprintf("WARN: Ajuste timerpoll x min.\r\n");
		systemVars.timerpoll = 15;
	}

	if ( systemVars.timerpoll > 600 ) {
		xprintf("WARN: Ajuste timerpoll x max.\r\n");
		systemVars.timerpoll = 600;
	}

	for( ;; )
	{
		// Espero timerpoll ms.
		vTaskDelayUntil( &xLastWakeTime, ( TickType_t)( (1000 * systemVars.timerpoll ) / portTICK_PERIOD_MS ) );

		// Leo entradas digitales
		read_dinputs( sysData.dinputs, true );

		// Leo entradas analogicas
		ADS7828_read(1, &sysData.bt, false);
		ADS7828_read(3, &sysData.ainputs[0], false);
		ADS7828_read(5, &sysData.ainputs[1], false);
		ADS7828_read(7, &sysData.ainputs[2], false);

		// Muestro en consola
		printData(fdTERM);

		// Transmito por RS485 puerto COMMS
		SET_ORTS();
		printData(fdCOMMS);
		vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
		CLEAR_ORTS();

	}
}
//------------------------------------------------------------------------------
void printData(int8_t fd)
{
	xfprintf_P(fd, PSTR(";TYPE:SP5K;VER:%s;"), FW_REV );
	xfprintf_P(fd, PSTR("A0:%d;A1:%d;A2:%d;"), sysData.ainputs[0], sysData.ainputs[1], sysData.ainputs[1] );
	xfprintf_P(fd, PSTR("C0:%d;C1:%d;T0:%d;T1:%d;"), sysData.dinputs[0].pulse_counter, sysData.dinputs[1].pulse_counter, sysData.dinputs[0].pulse_width, sysData.dinputs[1].pulse_width );
	xfprintf_P(fd, PSTR("bt:%d\r\n"), sysData.bt);
}
//------------------------------------------------------------------------------

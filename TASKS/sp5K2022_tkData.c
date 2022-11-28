/*
 * sp5K2022_tkData.c
 *
 *  Created on: 28 jul. 2022
 *      Author: pablo
 */


#include "sp5K2022.h"


typedef struct {
	counters_t counters_raw[COUNTER_CHANNELS];

	uint16_t ainputs_raw[ANALOG_CHANNELS];
	uint16_t bt_raw;

	int16_t counters_pulses[COUNTER_CHANNELS];
	int16_t counters_dtime[COUNTER_CHANNELS];
	float counters_mags[COUNTER_CHANNELS];

	float ainputs[ANALOG_CHANNELS];

	float bt;

} sysData_t;

sysData_t sysData;

void convert_ainputs_raw(void);
void convert_counters_raw(void);
void convert_bt_raw(void);

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
		vTaskDelayUntil( &xLastWakeTime,  (1000 * systemVars.timerpoll ) / portTICK_PERIOD_MS );
		//vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );

		// Leo entradas digitales
		read_counters( sysData.counters_raw, true );

		// Leo entradas analogicas
		ADS7828_read(1, &sysData.bt_raw, false);
		ADS7828_read(3, &sysData.ainputs_raw[0], false);
		ADS7828_read(5, &sysData.ainputs_raw[1], false);
		ADS7828_read(7, &sysData.ainputs_raw[2], false);

		convert_ainputs_raw();
		convert_counters_raw();
		convert_bt_raw();

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
void convert_ainputs_raw(void)
{

uint8_t channel;

float N = 1.0;
float D = 1.0;
float I = 1.0;

	for ( channel = 0; channel < ANALOG_CHANNELS; channel++) {

		I = 1.0 * sysData.ainputs_raw[channel] * 20 / 4096;
		D = 1.0 * ( systemVars.ainputs[channel].imax - systemVars.ainputs[channel].imin );
		if ( D == 0 ) {
			// Error: El denominador no puede ser 0
			sysData.ainputs[channel] = -99;
			continue;
		}
		N = 1.0 * ( systemVars.ainputs[channel].mmax - systemVars.ainputs[channel].mmin );
		sysData.ainputs[channel] = 1.0 * systemVars.ainputs[channel].mmin + ( N / D ) * ( I - systemVars.ainputs[channel].imin );
	}

}
//------------------------------------------------------------------------------
void convert_counters_raw(void)
{

uint8_t channel;

	for ( channel = 0; channel < COUNTER_CHANNELS; channel++) {
		sysData.counters_pulses[channel] = sysData.counters_raw[channel].pulse_counter;
		sysData.counters_mags[channel] =  systemVars.counters[channel].magpp * sysData.counters_raw[channel].pulse_counter;
	}
}
//------------------------------------------------------------------------------
void convert_bt_raw(void)
{
	sysData.bt = 1.0 * (15 * sysData.bt_raw) / 4096;
}
//------------------------------------------------------------------------------
void printData(int8_t fd)
{

uint8_t channel;

	if ( fd == fdTERM ) {
		while ( xSemaphoreTake( sem_SYSVars, ( TickType_t ) 5 ) != pdTRUE )
			;
			xfprintf_P(fd, PSTR("(%d)"), systemVars.clock );
			systemVars.clock = 0;
			xSemaphoreGive( sem_SYSVars );
	}

	xfprintf_P(fd, PSTR("ID:%s;TYPE:SP5K;VER:%s;"), systemVars.dlgid, FW_REV );

	for ( channel = 0; channel < ANALOG_CHANNELS; channel++) {
		if ( strcmp ( systemVars.ainputs[channel].name, "X" ) != 0 ) {
			xfprintf_P(fd, PSTR("%s:%0.3f;"), systemVars.ainputs[channel].name, sysData.ainputs[channel]);
		}
	}

	for ( channel = 0; channel < COUNTER_CHANNELS; channel++) {
		if ( strcmp ( systemVars.counters[channel].name, "X" ) != 0 ) {
			xfprintf_P(fd, PSTR("%s_P:%d;%s_M:%0.3f;"),
				systemVars.counters[channel].name,
				sysData.counters_pulses[channel],
				systemVars.counters[channel].name,
				sysData.counters_mags[channel]);
		}
	}
	xfprintf_P(fd, PSTR("bt:%0.3f\r\n"), sysData.bt);
}
//------------------------------------------------------------------------------

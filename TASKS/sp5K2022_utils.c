/*
 * sp5K2022_utils.c
 *
 *  Created on: 28 jul. 2022
 *      Author: pablo
 */


#include "sp5K2022.h"

//------------------------------------------------------------------------------
void reset(void)
{
	/* Reseteo al micro activando el watchdog */
	cli();
	wdt_reset();
	/* Start timed equence */
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	/* Set new prescaler(time-out) value = 64K cycles (~0.5 s) */
	WDTCSR = (1<<WDE) | (1<<WDP2) | (1<<WDP0);
	sei();

	wdt_enable(WDTO_30MS);
	while(1) {}
}
//------------------------------------------------------------------------------
uint8_t checksum( uint8_t *s, uint16_t size )
{
	/*
	 * Recibe un puntero a una estructura y un tamaño.
	 * Recorre la estructura en forma lineal y calcula el checksum
	 */

uint8_t *p = NULL;
uint8_t checksum = 0;
uint16_t i = 0;

	checksum = 0;
	p = s;
	for ( i = 0; i < size ; i++) {
		checksum += p[i];
	}
	checksum = ~checksum;
	return(checksum);
}
//------------------------------------------------------------------------------------
void load_defaults(void)
{
	// Almacena un string de bytes en la eeprom interna del micro
	systemVars.timerpoll = 60;
	snprintf_P( systemVars.dlgid, PARAMNAME_LENGTH, PSTR("DEFAULT"));
	load_analog_defaults();
	load_counters_defaults();

	xprintf("Loading defaults...\r\n");
	return;
}
//------------------------------------------------------------------------------------
void save_systemVars(void)
{
	// Almacena un string de bytes en la eeprom interna del micro

uint8_t wr_cks = 0;
uint8_t ee_wr_addr=0;

	// store parameters
	IEE_write( ee_wr_addr, (char *)&systemVars, sizeof(systemVars_t) );
	//
	wr_cks = checksum( (uint8_t *)&systemVars, sizeof(systemVars) );
	ee_wr_addr += sizeof(systemVars_t);
	IEE_write( ee_wr_addr, (char *)&wr_cks, sizeof(uint8_t) );
	//
	//ee_wr_addr = 0x00;
	//eeprom_write_block( &systemVars, 0x00, sizeof(systemVars_t) );
	// store checksum
	//cks = checksum( (uint8_t *)&systemVars, sizeof(systemVars) );
	//ee_wr_addr += sizeof(systemVars_t);
	//eeprom_write_byte( &ee_wr_addr, cks );
	//
	//xprintf_P(PSTR("save_systemVars wr_cks: 0x%02x, addr=0x%02x\r\n"), wr_cks, ee_wr_addr);
	return;
}
//------------------------------------------------------------------------------------
uint8_t load_systemVars(void)
{
	// Lee un string de bytes en la eeprom interna del micro

uint8_t rd_cks = 0;
uint8_t ee_rd_addr = 0;

	// load parameters
	IEE_read( ee_rd_addr, (char *)&systemVars, sizeof(systemVars_t) );
	//
	ee_rd_addr += sizeof(systemVars_t);
	IEE_read( ee_rd_addr, (char *)&rd_cks, sizeof(uint8_t) );
	//
	//ee_rd_addr = 0x00;
	//eeprom_read_block( &systemVars, 0x00, sizeof(systemVars_t) );
	// load checksum
	//ee_rd_addr += sizeof(systemVars_t);
	//cks = eeprom_read_byte( &ee_rd_addr );

	//xprintf_P(PSTR("TP=%d\r\n"), loadSvars.timerpoll);
	//xprintf_P(PSTR("load_systemVars rd_cks: 0x%02x, addr=0x%02x\r\n"), rd_cks, ee_rd_addr);
	return(rd_cks);
}
//------------------------------------------------------------------------------------
bool load_config(void)
{
uint8_t lcks, cks;

	lcks = load_systemVars();
	xprintf_P(PSTR("Loaded cks: 0x%02x\r\n"), lcks );
	cks = checksum( (uint8_t *)&systemVars, sizeof(systemVars) );
	xprintf_P(PSTR("Calculated cks: 0x%02x\r\n"), cks );

	if ( lcks == cks )
		return (true);

	return(false);

}
//------------------------------------------------------------------------------------
void load_analog_defaults(void)
{

	// Realiza la configuracion por defecto de los canales analogicos.

uint8_t channel;

	for ( channel = 0; channel < ANALOG_CHANNELS; channel++) {
		systemVars.ainputs[channel].imin = 0;
		systemVars.ainputs[channel].imax = 20;
		systemVars.ainputs[channel].mmin = 0;
		systemVars.ainputs[channel].mmax = 10.0;
		systemVars.ainputs[channel].offset = 0.0;
		snprintf_P( systemVars.ainputs[channel].name, PARAMNAME_LENGTH, PSTR("P%d"),channel );

	}
}
//------------------------------------------------------------------------------------
void load_counters_defaults(void)
{

uint8_t channel;

	for ( channel = 0; channel < COUNTER_CHANNELS; channel++) {
		systemVars.counters[channel].magpp = 0.1;
		snprintf_P( systemVars.counters[channel].name, PARAMNAME_LENGTH, PSTR("X") );
	}

}
//------------------------------------------------------------------------------------
bool config_analog_channel( uint8_t channel,char *s_name,char *s_imin,char *s_imax,char *s_mmin,char *s_mmax )
{

	// Configura los canales analogicos. Es usada tanto desde el modo comando como desde el modo online por gprs.

bool retS = false;

	while ( xSemaphoreTake( sem_SYSVars, ( TickType_t ) 5 ) != pdTRUE )
		taskYIELD();

	if ( ( channel >=  0) && ( channel < ANALOG_CHANNELS) ) {
		if ( s_imin != NULL ) { systemVars.ainputs[channel].imin = atoi(s_imin); }
		if ( s_imax != NULL ) { systemVars.ainputs[channel].imax = atoi(s_imax); }
		if ( s_mmin != NULL ) { systemVars.ainputs[channel].mmin = atoi(s_mmin); }
		if ( s_mmax != NULL ) { systemVars.ainputs[channel].mmax = atof(s_mmax); }
		snprintf_P( systemVars.ainputs[channel].name, PARAMNAME_LENGTH, PSTR("%s\0"), s_name );
		retS = true;
	}

	xSemaphoreGive( sem_SYSVars );
	return(retS);
}
//------------------------------------------------------------------------------------
bool config_counters_channel( uint8_t channel,char *s_name, char *s_magpp )
{

	// {0..1} dname magPP

bool retS = false;

	while ( xSemaphoreTake( sem_SYSVars, ( TickType_t ) 5 ) != pdTRUE )
		taskYIELD();

	if ( ( channel >=  0) && ( channel < COUNTER_CHANNELS ) ) {
		// NOMBRE
		snprintf_P( systemVars.counters[channel].name, PARAMNAME_LENGTH, PSTR("%s"), s_name );

		// MAGPP
		if ( s_magpp != NULL )
			systemVars.counters[channel].magpp = atof(s_magpp);

		retS = true;
	}

	xSemaphoreGive( sem_SYSVars );
	return(retS);

}
//------------------------------------------------------------------------------------

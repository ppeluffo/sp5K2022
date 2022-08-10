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
	//xprintf_P(PSTR("Loaded cks: 0x%02x\r\n"), lcks );
	cks = checksum( (uint8_t *)&systemVars, sizeof(systemVars) );
	//xprintf_P(PSTR("Calculated cks: 0x%02x\r\n"), cks );

	if ( lcks == cks )
		return (true);

	return(false);

}
//------------------------------------------------------------------------------------


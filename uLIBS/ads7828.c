/*
 * ads7828.c
 *
 *  Created on: 2 ago. 2022
 *      Author: pablo
 */


#include "ads7828.h"

//------------------------------------------------------------------------------------
int16_t ADS7828_rawRead( uint8_t cmdByte , char *data )
{
	/*
	 * EL ADS7828 solo se lee.
	 * Si vemos en las datasheet, el unico ciclo es de lectura ( no hay stop luego de la lectura )
	 * La caracteristica es que el dataAddress es de 1 byte y es el CommandByte.
	 */

int16_t rcode = -1;
uint16_t data_address = cmdByte;

	I2C_get_semaphore();

	rcode =  I2C_read( BUSADDR_ADS7828, data_address, 1, data, 2 );
	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1s que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_RATE_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf_P(PSTR("ERROR: ADS7828_read recovering i2c bus\r\n\0") );
		I2C_reinit_devices();
		goto quit;
	}

quit:

	I2C_release_semaphore();

	return( rcode );



}
//------------------------------------------------------------------------------------
bool ADS7828_read (uint8_t channel, uint16_t *value, bool f_debug )
{

uint8_t cmdByte;
char data[2];
uint8_t ads7828Channel;
int16_t retVal;

	if ( channel > 7) {
		return(false);
	}

	// Convierto el canal 0-7 al C2/C1/C0 requerido por el conversor.
	ads7828Channel = (((channel>>1) | (channel&0x01)<<2)<<4) | ADS7828_CMD_SD;
	if ( f_debug) {
		xprintf_P(PSTR("ADS7828_read: adsChannel=0x%02x\r\n"),ads7828Channel );
	}
	// Armo el COMMAND BYTE
	cmdByte = ads7828Channel & 0xF0;	// SD=1 ( single end inputs )
	cmdByte |= ADS7828_CMD_PDMODE3;		// Internal reference ON, A/D converter ON
	if ( f_debug) {
		xprintf_P(PSTR("ADS7828_read: cmdByte=0x%02x\r\n"), cmdByte );
	}
	// start conversion on requested channel
	retVal = ADS7828_rawRead( cmdByte, data);
	if ( retVal == -1 ) {
		return(false);
	}
	if ( f_debug) {
		xprintf_P(PSTR("ADS7828_read: d[1]=0x%02x,  d[0]=0x%02x\r\n"), data[1],data[0] );
	}

	*value = ( data[0] << 8) | data[1];

	if ( f_debug) {
		xprintf_P(PSTR("ADS7828_read: value=%d\r\n"), *value );
	}
	return(true);

}
//------------------------------------------------------------------------------------
bool ADC7828_test_read( char *s_channel)
{
uint8_t channel;
uint16_t value;

	channel = atoi(s_channel);
	return ( ADS7828_read( channel, &value, true) );
}
//------------------------------------------------------------------------------------

/*
 * mcp23008.c
 *
 *  Created on: 2 ago. 2022
 *      Author: pablo
 */


#include "mcp23008.h"

//------------------------------------------------------------------------------
bool MCP_read ( uint8_t id, uint8_t regAddress, char *value )
{

int16_t rcode = -1;


	I2C_get_semaphore();
	if ( id == MCP0 ) {
		rcode =  I2C_write ( BUSADDR_MCP0, regAddress, 0x01, value, 1 );
	} else if ( id == MCP1 ) {
		rcode =  I2C_write ( BUSADDR_MCP1, regAddress, 0x01, value, 1 );
	}
	I2C_release_semaphore();

	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1s que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_RATE_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf_P(PSTR("ERROR: MCP_read recovering i2c bus\r\n\0") );
		I2C_reinit_devices();
		return(false);
	}

	return( true );
}
//------------------------------------------------------------------------------
bool MCP_write ( uint8_t id, uint8_t regAddress, uint8_t value )
{

int16_t rcode = -1;
char data = (char)value;

//	xprintf_P(PSTR("DEBUG EEwrite: address=%d, length=%d\r\n"), wrAddress, length);

	I2C_get_semaphore();
	if ( id == MCP0 ) {
		rcode =  I2C_write ( BUSADDR_MCP0, regAddress, 0x01, &data, 1 );
	} else if ( id == MCP1 ) {
		rcode =  I2C_write ( BUSADDR_MCP1, regAddress, 0x01, &data, 1 );
	}
	I2C_release_semaphore();

	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1s que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_RATE_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf_P(PSTR("ERROR: MCP_write recovering i2c bus\r\n\0") );
		I2C_reinit_devices();
		return(false);
	}

	return( true );

}
//------------------------------------------------------------------------------
void MCP0_init(void)
{

uint8_t data;

	// MCP0_IODIR: inputs(1)/outputs(0)
	data = 0;
	data |= ( 1<<MCP_GPIO_IGPRSDCD) | ( 1<<MCP_GPIO_IGPRSRI) | (1<<MCP_GPIO_ITERMPWRSW) ;
	MCP_write( MCP0, MCPADDR_IODIR, data);

	// MCP0_GPINTEN: inputs interrupt on change.
	data = 0;
	data |= (1<<MCP_GPIO_IGPRSDCD) | (1<<MCP_GPIO_IGPRSRI) | (1<<MCP_GPIO_ITERMPWRSW);
	MCP_write( MCP0, MCPADDR_GPINTEN, data);

	// MCP0_IOCON: INT active H
	data = 2;
	MCP_write( MCP0, MCPADDR_IOCON, data);
	//
	// TERMPWR ON
	data = 0;
	data |= (1<<MCP_GPIO_OTERMPWR);	// TERMPWR = 1
	MCP_write( MCP0, MCPADDR_OLAT, data);

}
//------------------------------------------------------------------------------
void  MCP1_init(void)
{

uint8_t data;

	// MCP1_IODIR: inputs(1)/outputs(0)
	// Configuro DIN0/DIN1 como entradas(1) y SENSORSPWR como output(0)
	data = 0;
	data |= (1<<MCP_GPIO_DIN0) | (1<<MCP_GPIO_DIN1);
	MCP_write( MCP1, MCPADDR_IODIR, data);

	// MCP1_GPPU: pull-ups
	// Habilito los pull-ups en DIN0/1
	data = 0;
	data |= (1<<MCP_GPIO_DIN0) | (1<<MCP_GPIO_DIN1);
	MCP_write( MCP1, MCPADDR_GPPU, data);

	// MCP1_GPINTEN: inputs interrupt on change.
	// Habilito que DIN0/1 generen una interrupcion on-change.
	data = 0;
	data |= (1<<MCP_GPIO_DIN0) | (1<<MCP_GPIO_DIN1);
	MCP_write( MCP1, MCPADDR_GPINTEN, data);

	// MCP1_IOCON: INT active H.
	data = 2;
	MCP_write( MCP1, MCPADDR_IOCON, data);
}
//------------------------------------------------------------------------------
bool MCP_setLed( void )
{

uint8_t regValue;

	if (  MCP_read( MCP0, MCPADDR_GPIO, (char *)&regValue) ) {
		regValue |= (1<<MCP_GPIO_OLED);		// Led = 1
		if ( MCP_write( MCPADDR_OLAT, MCP_ADDR0, regValue) ) {
			return(true);
		}
	}

	return(false);
}
//------------------------------------------------------------------------------
bool MCP_clearLed( void )
{

uint8_t regValue;

	if (  MCP_read( MCP0, MCPADDR_GPIO, (char *)&regValue) ) {
		regValue &= ~(1<<MCP_GPIO_OLED);		// Led = 0
		if ( MCP_write( MCPADDR_OLAT, MCP_ADDR0, regValue) ) {
			return(true);
		}
	}

		return(false);
}
//------------------------------------------------------------------------------

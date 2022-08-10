/*
 * mcp23008.c
 *
 *  Created on: 2 ago. 2022
 *      Author: pablo
 */


#include <mcp.h>

//------------------------------------------------------------------------------
bool MCP_read ( uint8_t id, uint8_t regAddress, char *value )
{

int16_t rcode = -1;

	if ( (id == MCP0) && ( MCP0_status == false)) {
		return(false);
	}
	if ( (id == MCP1) && ( MCP1_status == false)) {
		return(false);
	}

	I2C_get_semaphore();
	if ( id == MCP0 ) {
		rcode =  I2C_read ( BUSADDR_MCP23008A, regAddress, 0x01, value, 1 );
	} else if ( id == MCP1 ) {
		rcode =  I2C_read ( BUSADDR_MCP23018, regAddress, 0x01, value, 1 );
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

	if ( (id == MCP0) && ( MCP0_status == false)) {
		return(false);
	}
	if ( (id == MCP1) && ( MCP1_status == false)) {
		return(false);
	}

	I2C_get_semaphore();
	if ( id == MCP0 ) {
		rcode =  I2C_write ( BUSADDR_MCP23008A, regAddress, 0x01, &data, 1 );
	} else if ( id == MCP1 ) {
		rcode =  I2C_write ( BUSADDR_MCP23018, regAddress, 0x01, &data, 1 );
	}
	I2C_release_semaphore();

	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1s que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_RATE_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf_P(PSTR("ERROR: MCP_write recovering i2c bus\r\n") );
		I2C_reinit_devices();
		return(false);
	}

	return( true );

}
//------------------------------------------------------------------------------
bool MCP0_init(void)
{

uint8_t data;

	MCP0_status = true;

	// MCP0_IODIR: inputs(1)/outputs(0)
	data = 0x00;
	//data |= ( 1<<MCP0_GPIO_IGPRSDCD) | ( 1<<MCP0_GPIO_IGPRSRI);
	if ( ! MCP_write( MCP0, MCP0_IODIR, data) ) {
		goto quit;
	}

	// MCP0_IPOL: polaridad normal
	data = 0x00;
	if ( ! MCP_write( MCP0, MCP0_IPOL, data) ) {
		goto quit;
	}

	// MCP0_GPINTEN: inputs interrupt on change.
	data = 0x00;
	//data |= ( 1<<MCP0_GPIO_IGPRSDCD) | ( 1<<MCP0_GPIO_IGPRSDCD) | ( 1<<MCP0_GPIO_IGPRSRI);
	if ( ! MCP_write( MCP0, MCP0_IODIR, data) ) {
		goto quit;
	}

	// MCP0_INTCON: Compara contra su valor anterior
	data = 0x00;
	if ( ! MCP_write( MCP0, MCP0_INTCON, data) ) {
		goto quit;
	}

	// MCP0_IOCON: INT active H
	data = 0x02;
	if ( ! MCP_write( MCP0, MCP0_IOCON, data) ) {
		goto quit;
	}

	// MCP0_GPPU: pull-ups
	data = 0x00;
	if ( ! MCP_write( MCP0, MCP0_GPPU, data) ) {
		goto quit;
	}

	// TERMPWR ON
	data = 0;
	data |= (1<<MCP0_GPIO_OTERMPWR);	// TERMPWR = 1
	if ( ! MCP_write( MCP0, MCP0_OLAT, data) ) {
		goto quit;
	}

	MCP0_status = true;
	return(true);
quit:

	xprintf_P(PSTR("ERROR: MCP0_init fail.\r\n"));
	MCP0_status = false;
	return(false);
}
//------------------------------------------------------------------------------
bool  MCP1_init(void)
{
	// Inicializo el MCP23018 de la placa analogica
	// El modo 0 indica uso normal, el modo 1 uso por reconfiguracion por lo que el log es por D_DEBUG

uint8_t data;

	MCP1_status = true;

	// IOCON
	data = 0x63; // 0110 0011
	//                      1->INTCC:Read INTCAP clear interrupt
	//                     1-->INTPOL: INT out pin active high
	//                    0--->ORDR: Active driver output. INTPOL set the polarity
	//                   0---->X
	//                 0----->X
	//                1------>SEQOP: sequential disabled. Address ptr does not increment
	//               1------->MIRROR: INT pins are ored
	//              0-------->BANK: registers are in the same bank, address sequential
	if (! MCP_write( MCP1, MCP1_IOCON, data) ) {
		xprintf_P(PSTR("ERROR: MCP1 001.\r\n"));
		goto quit;
	}

	// DIRECCION
	// 0->output
	// 1->input
	data = 0x80; // 1000 0000 ( GPA0..GPA6: outputs, GPA7 input )
	if ( ! MCP_write( MCP1, MCP1_IODIRA, data) ) {
		xprintf_P(PSTR("ERROR: MCP1 002.\r\n"));
		goto quit;
	}

	data = 0x64; // 0110 0100
	if ( ! MCP_write( MCP1, MCP1_IODIRB, data) ) {
		xprintf_P(PSTR("ERROR: MCP1 003.\r\n"));
		goto quit;
	}

	// PULL-UPS
	// 0->disabled
	// 1->enabled
	// Los dejo en 0 para ahorrar ma.
	//data = 0x00; // 1111 1111
	data = 0x00; // 1111 1000
	if ( ! MCP_write( MCP1, MCP1_GPPUA, data) ) {
		xprintf_P(PSTR("ERROR: MCP1 004.\r\n"));
		goto quit;
	}

	data = 0xF0; // 1111 0000
	if ( ! MCP_write( MCP1, MCP1_GPPUB, data) ) {
		xprintf_P(PSTR("ERROR: MCP1 005.\r\n"));
		goto quit;
	}

	// Valores iniciales de las salidas en 0
	data = 0x00;
	if ( ! MCP_write( MCP1, MCP1_OLATA, data) ) {
		xprintf_P(PSTR("ERROR: MCP1 006.\r\n"));
		goto quit;
	}

	data = 0x00;
	if ( ! MCP_write( MCP1, MCP1_OLATB, data) ) {
		xprintf_P(PSTR("ERROR: MCP1 007.\r\n"));
		goto quit;
	}

	MCP1_status = true;
	return(true);
quit:

	xprintf_P(PSTR("ERROR: MCP1_init fail.\r\n"));
	MCP1_status = false;
	return(false);


}
//------------------------------------------------------------------------------
bool SET_OPWRSENSORS(void)
{
uint8_t regValue;

	if (  MCP_read( MCP1, MCP1_OLATB, (char *)&regValue) ) {
		//xprintf_P(PSTR("DEBUG: MCP regValue A: 0x%02x\r\n"), regValue);
		regValue |= (1<<MCP1_OPWRSENSORS);
		//xprintf_P(PSTR("DEBUG: MCP regValue B: 0x%02x\r\n"), regValue);

		if ( MCP_write( MCP1, MCP1_OLATB, regValue) ) {
			return(true);
		}
	}
	return(false);
}
//------------------------------------------------------------------------------
bool CLEAR_OPWRSENSORS(void)
{
uint8_t regValue;

	if (  MCP_read( MCP1, MCP1_OLATB, (char *)&regValue) ) {
		//xprintf_P(PSTR("DEBUG: MCP regValue A: 0x%02x\r\n"), regValue);
		regValue &= ~(1<<MCP1_OPWRSENSORS);
		//xprintf_P(PSTR("DEBUG: MCP regValue B: 0x%02x\r\n"), regValue);
		if ( MCP_write( MCP1, MCP1_OLATB, regValue) ) {
			return(true);
		}
	}
	return(false);
}
//------------------------------------------------------------------------------
bool SET_OANALOG(void)
{
uint8_t regValue;

	if (  MCP_read( MCP1, MCP1_OLATB, (char *)&regValue) ) {
		//xprintf_P(PSTR("DEBUG: MCP regValue A: 0x%02x\r\n"), regValue);
		regValue |= (1<<MCP1_OANALOG);
		//xprintf_P(PSTR("DEBUG: MCP regValue B: 0x%02x\r\n"), regValue);
		if ( MCP_write( MCP1, MCP1_OLATB, regValue) ) {
			return(true);
		}
	}
	return(false);
}
//------------------------------------------------------------------------------
bool CLEAR_OANALOG(void)
{
uint8_t regValue;

	if (  MCP_read( MCP1, MCP1_OLATB, (char *)&regValue) ) {
		//xprintf_P(PSTR("DEBUG: MCP regValue A: 0x%02x\r\n"), regValue);
		regValue &= ~(1<<MCP1_OANALOG);
		//xprintf_P(PSTR("DEBUG: MCP regValue B: 0x%02x\r\n"), regValue);
		if ( MCP_write( MCP1, MCP1_OLATB, regValue) ) {
			return(true);
		}
	}
	return(false);
}
//------------------------------------------------------------------------------
bool READ_DIGITAL_INPUTS( uint8_t *din0, uint8_t *din1 )
{
	/*
	 * Lee el OPORTB del MCP1 donde estan las entradas digitales.
	 *
	 */

uint8_t regValue;

	if ( MCP_read( MCP1, MCP1_GPIOB, (char *)&regValue ) ) {

		*din0 = ( regValue & DIN0_bm ) >> DIN0_bp;
		*din1 = ( regValue & DIN1_bm ) >> DIN1_bp;

		return(true);
	}
	return(false);


}
//------------------------------------------------------------------------------
bool SET_ORTS(void)
{
uint8_t regValue;

	if (  MCP_read( MCP0, MCP0_OLAT, (char *)&regValue) ) {
		//xprintf_P(PSTR("DEBUG: MCP0 regValue A: 0x%02x\r\n"), regValue);
		regValue |= (1<<MCP0_GPIO_ORTS);
		//xprintf_P(PSTR("DEBUG: MCP0 regValue B: 0x%02x\r\n"), regValue);

		if ( MCP_write( MCP0, MCP0_OLAT, regValue) ) {
			return(true);
		}
	}
	return(false);
}
//------------------------------------------------------------------------------
bool CLEAR_ORTS(void)
{
uint8_t regValue;

	if (  MCP_read( MCP0, MCP0_OLAT, (char *)&regValue) ) {
		//xprintf_P(PSTR("DEBUG: MCP0 regValue A: 0x%02x\r\n"), regValue);
		regValue &= ~(1<<MCP0_GPIO_ORTS);
		//xprintf_P(PSTR("DEBUG: MCP0 regValue B: 0x%02x\r\n"), regValue);

		if ( MCP_write( MCP0, MCP0_OLAT, regValue) ) {
			return(true);
		}
	}
	return(false);
}
//------------------------------------------------------------------------------
bool SET_OPWRMODBUS(void)
{
uint8_t regValue;

	if (  MCP_read( MCP0, MCP0_OLAT, (char *)&regValue) ) {
		//xprintf_P(PSTR("DEBUG: MCP0 regValue A: 0x%02x\r\n"), regValue);
		regValue |= (1<<MCP0_GPIO_OMODBUSPWR);
		//xprintf_P(PSTR("DEBUG: MCP0 regValue B: 0x%02x\r\n"), regValue);

		if ( MCP_write( MCP0, MCP0_OLAT, regValue) ) {
			return(true);
		}
	}
	return(false);
}
//------------------------------------------------------------------------------
bool CLEAR_OPWRMODBUS(void)
{
uint8_t regValue;

	if (  MCP_read( MCP0, MCP0_OLAT, (char *)&regValue) ) {
		//xprintf_P(PSTR("DEBUG: MCP0 regValue A: 0x%02x\r\n"), regValue);
		regValue &= ~(1<<MCP0_GPIO_OMODBUSPWR);
		//xprintf_P(PSTR("DEBUG: MCP0 regValue B: 0x%02x\r\n"), regValue);

		if ( MCP_write( MCP0, MCP0_OLAT, regValue) ) {
			return(true);
		}
	}
	return(false);
}
//------------------------------------------------------------------------------

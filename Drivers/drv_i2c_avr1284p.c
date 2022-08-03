/*
 * i2c.c





 *
 *  Created on: 18/10/2015
 *      Author: pablo
 *
 *  Modificadas 20/05/2017 para adecuarlas al SP6K basado en xmega256
 *  El bus se trabaja en modo poleado.
 *  La velocidad se fija en 100Khz.
 *  Se utiliza el bus implementado en el puerto E.
 *
 *  Cuando el sistema arranca, el estado del bus es UNKNOWN. Se puede forzar a IDLE escribiendo
 *  los bits correspondientes del STATUS.
 *  Para pasarlo de nuevo a UNKNOWN solo un reset o deshabilitando el TWI.
 *  Para sacarlo de BUSY hay que generar un STOP
 *
 *  Al escribir en ADDR,DATA o STATUS se borran las flags. Esto hay que hacerlo de ultimo
 *  ya que luego de c/operacion, mientras las flags esten prendidas el master mantiene el bus.
 *
 *  CUANDO EL BUS QUEDA MAL NO TENGO FORMA DE SALIR QUE NO SEA APAGANDO Y PRENDIENDO !!!
 */


#include <drv_i2c_avr1284p.h>

#include "xprintf.h"

//------------------------------------------------------------------------------------
// FUNCIONES PRIVADAS
//------------------------------------------------------------------------------------

static inline bool pv_I2C_awaitBusTransaction(void);
static inline bool pv_I2C_send_start( void );
static inline bool pv_I2C_send_SLA_WR( const uint8_t devAddress );
static inline bool pv_I2C_send_SLA_RD( const uint8_t devAddress );
static inline bool pv_I2C_writeByte(uint8_t wrByte);
static inline bool pv_I2C_send_dataAddress( uint16_t dataAddress, uint8_t dataAddressLength);
static inline bool pv_I2C_send_data( char *wrBuffer, uint8_t bytesToWrite );
static inline void pv_I2C_sendStop(void);

static inline bool pv_I2C_readByte( uint8_t *rxByte, uint8_t ackFlag);
static inline bool pv_I2C_rcvd_data( char *rdBuffer, uint8_t bytesToRead );


//------------------------------------------------------------------------------------
void drv_I2C_init(void)
{
uint16_t bitrate_div = 0;
uint16_t bitrateKHz = 100;

	// set pull-up resistors on I2C bus pins
	//sbi(PORTC, 0);  // i2c SCL
	//sbi(PORTC, 1);  // i2c SDA
	PORTC |= 1 << SCL;
	PORTC |= 1 << SDA;

	// calculate bitrate division
	((TWSR) &= ~(1 << (TWPS0)));
	((TWSR) &= ~(1 << (TWPS1)));

	bitrate_div = ((F_CPU/1000l)/bitrateKHz);
	if(bitrate_div >= 16) {
		bitrate_div = (bitrate_div-16)/2;
	}
	TWBR = bitrate_div;

	// TWCR.TWEN = 1 : Habilita la interface TWI
	// TWCR.TWINT = 1: Borra la flag e inicia la operacion del TWI
	TWCR = (1 << TWINT) | (1 << TWEN);
	//
}
//------------------------------------------------------------------------------------
void drv_I2C_configDebugFlag(bool f_debug)
{
	f_debugBusI2C = f_debug;
}
//------------------------------------------------------------------------------------
int16_t drv_I2C_master_write ( const uint8_t devAddress, const uint16_t dataAddress, const uint8_t dataAddressLength, char *pvBuffer, size_t xBytes )
{

	xprintf_PD( f_debugBusI2C , PSTR("I2C: drv_I2C_master_write: START\r\n"));

	// Paso 1: PONER EL BUS EN CONDICIONES
	if ( ! pv_I2C_send_start() ) {
		return(-1);
	}

	// Paso 2: DEVICE_ADDRESS + WR
	if ( ! pv_I2C_send_SLA_WR (devAddress) )
		return(-1);


	// Paso 3: DATA_ADDRESS
	if ( ! pv_I2C_send_dataAddress ( dataAddress, dataAddressLength ))
		return(-1);

	// Paso 4: DATA
	if ( !  pv_I2C_send_data( pvBuffer, xBytes))
		return(-1);

	// Paso 5: STOP
	pv_I2C_sendStop();


	xprintf_PD( f_debugBusI2C, PSTR("I2C: drv_I2C_master_write: EXIT (%d)\r\n"), xBytes);
	return(xBytes);

}
//------------------------------------------------------------------------------------
int16_t drv_I2C_master_read ( const uint8_t devAddress, const uint16_t dataAddress, const uint8_t dataAddressLength, char *pvBuffer, size_t xBytes )
{

	xprintf_PD( f_debugBusI2C, PSTR("I2C: drv_I2C_master_read: START\r\n"));

	// Paso 1: START
	if ( ! pv_I2C_send_start() ) {
		return(-1);
	}

	// Paso 2: DEVICE_ADDRESS + WR
	if ( ! pv_I2C_send_SLA_WR (devAddress) )
		return(-1);

	// Paso 3: DATA_ADDRESS
	if ( ! pv_I2C_send_dataAddress ( dataAddress, dataAddressLength ))
		return(-1);

	// Paso 4: REPEATED START
	if ( ! pv_I2C_send_start() ) {
		return(-1);
	}

	// Paso 5: DEVICE_ADDRESS + RD
	if ( ! pv_I2C_send_SLA_RD (devAddress) )
		return(-1);

	// Paso 6: RCVD DATA
	if ( !  pv_I2C_rcvd_data( pvBuffer, xBytes))
		return(-1);

	// Paso 7: STOP
	pv_I2C_sendStop();

	xprintf_PD( f_debugBusI2C, PSTR("I2C: drv_I2C_master_read: EXIT\r\n"));
	return(xBytes);
}
//------------------------------------------------------------------------------------
static inline bool pv_I2C_awaitBusTransaction(void)
{
	/*
	 * Espero que termine la operacion del bus I2C.
	 * Esto se marca porque se prende la flag TWINT de TWCR
	 */

uint8_t ticks_to_wait = 50;		// ( 50ms es el maximo tiempo que espero )


	while ( ticks_to_wait-- > 0 ) {

		if ( TWCR & ( 1<<TWINT ) ) {
			xprintf_PD( f_debugBusI2C, PSTR("I2C: Rsp: 0x%02x\r\n"), ( TWSR & 0xF8) );
			return(true);
		}

		vTaskDelay( ( TickType_t)( 1 ) );
	}

	// DEBUG
	xprintf_P( PSTR("pv_I2C_awaitBusTransaction: TIMEOUT status=0x%02x\r\n"), ( TWSR & 0xF8) );
	return(false);

}
//------------------------------------------------------------------------------------
static inline bool pv_I2C_send_start( void )
{
	// Genera las condiciones de un START en el bus.
	// Debe responder con TWSR = 0x08 (start) o 0x10 (repeated start)

	xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_send_start\r\n"));

	// Borro la flag TWINT, habilito el I2C (TWEN) y mando un start (TWSTA)
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	// Espero la respuesta
	if ( pv_I2C_awaitBusTransaction()) {
		if ( ( TWSR & 0xF8) == TW_START ) {	// 0x08
			return(true);
		}
		if ( ( TWSR & 0xF8) == TW_REP_START ) {	// 0x10
			return(true);
		}
	}

	xprintf_P(PSTR("I2C: pv_I2C_send_start: ERROR, status=0x%02x\r\n"), ( TWSR & 0xF8) );
	return(false);
}
//------------------------------------------------------------------------------------
static inline bool pv_I2C_send_SLA_WR( const uint8_t devAddress )
{
	/*
	 * Envia SLA+W.
	 * Las posibles respuestas son 0x18 (OK) ,0x20 o 0x38 (ERROR )
	 */

uint8_t wrByte = (devAddress & ~0x01);

	xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_send_SLA_WR 0x%02x\r\n"), wrByte);

	// Borro la flag TWINT, habilito el I2C (TWEN)
	TWDR = wrByte;
	TWCR = (1<<TWINT) | (1<<TWEN);
	//
	// Espero la respuesta
	if ( pv_I2C_awaitBusTransaction()) {
		if ( ( TWSR & 0xF8) == TW_MT_SLA_ACK ) {	// 0x18
			return(true);
		}
	}

	xprintf_P(PSTR("I2C: pv_I2C_send_SLA_WR: ERROR, status=0x%02x\r\n"), ( TWSR & 0xF8) );
	return(false);

}
//------------------------------------------------------------------------------------
static inline bool pv_I2C_send_SLA_RD( const uint8_t devAddress )
{
	/*
	 * Envia SLA+R.
	 * Las posibles respuestas son 0x40 (OK) ,0x48 (ERROR )
	 */

uint8_t wrByte = (devAddress | 0x01);

	xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_send_SLA_RD 0x%02x\r\n"), wrByte);

	// Borro la flag TWINT, habilito el I2C (TWEN)
	TWDR = wrByte;
	TWCR = (1<<TWINT) | (1<<TWEN);
	//
	// Espero la respuesta
	if ( pv_I2C_awaitBusTransaction()) {
		if ( ( TWSR & 0xF8) == TW_MR_SLA_ACK ) {	// 0x18
			return(true);
		}
	}

	xprintf_P(PSTR("I2C: pv_I2C_send_SLA_RD: ERROR, status=0x%02x\r\n"), ( TWSR & 0xF8) );
	return(false);

}
//------------------------------------------------------------------------------------
static inline bool pv_I2C_writeByte(uint8_t wrByte)
{

	xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_writeByte (0x%02x)\r\n"), wrByte );

	// Envia el data por el bus I2C.
	TWDR = wrByte;
	// Habilita la trasmision y el envio de un ACK.
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Espero la respuesta
	if ( pv_I2C_awaitBusTransaction()) {
		if ( ( TWSR & 0xF8) == TW_MT_DATA_ACK ) {	// 0x28
			return(true);
		}
	}

	xprintf_P(PSTR("I2C: pv_I2C_writeByte: ERROR,status=0x%02x\r\n"), ( TWSR & 0xF8) );

	return(false);
}
//------------------------------------------------------------------------------------
static inline void pv_I2C_sendStop(void)
{
	// Genera la condicion STOP en el bus I2C.
	// !!! El TWINT NO ES SETEADO LUEGO DE UN STOP
	// por lo tanto no debo esperar que termine.

	xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_send_stop\r\n"));

	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

}
//-----------------------------------------------------------------------------------
static inline bool pv_I2C_send_dataAddress( uint16_t dataAddress, uint8_t dataAddressLength)
{
	xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_send_dataAddress: ADD=%d, LENGTH=%d\r\n"), dataAddress,dataAddressLength );

uint8_t txData;
bool retS = false;

	// HIGH address
	if ( dataAddressLength == 2 ) {
		txData = (dataAddress) >> 8;
		xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_send_dataAddress: ADDR_H=0x%02x\r\n"), txData );
		if ( ! pv_I2C_writeByte(txData)) {
			goto quit;
		}
	}

	// LOW address
	if ( dataAddressLength >= 1 ) {
		txData = (dataAddress) & 0x00FF;
		xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_send_dataAddress: ADDR_L=0x%02x\r\n"), txData );
		if ( ! pv_I2C_writeByte(txData)) {
			goto quit;
		}
	}

	retS = true;

quit:

	if ( !retS )
		xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_send_dataAddress: ERROR (status=%d)\r\n"), TWSR );

	return(retS);


}
//------------------------------------------------------------------------------------
static inline bool pv_I2C_send_data( char *wrBuffer, uint8_t bytesToWrite )
{
	// Envio todo un buffer en modo write.

uint8_t bytesWritten;
uint8_t txData;
bool retS;

	// No hay datos para enviar: dummy write.
	if ( bytesToWrite == 0 )
		return(true);

	// Mando el buffer de datos. Debo recibir 0x28 (DATA_ACK) en c/u
	for ( bytesWritten=0; bytesWritten < bytesToWrite; bytesWritten++ ) {

		txData = *wrBuffer++;
		xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_send_data: 0x%02x\r\n"),txData );
		if ( ! pv_I2C_writeByte(txData)) {
			goto quit;
		}
	}
	retS = true;

quit:

	if (! retS )
		xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_send_data: ERROR (status=%d)\r\n"), TWSR );

	return(retS);
}
//------------------------------------------------------------------------------------
static inline bool pv_I2C_readByte(uint8_t *rxByte, uint8_t ackFlag)
{
	/* Leo un byte del I2C.
	 * Las respuestas pueden ser 0x50 ( OK con ACK ) o 0x58 ( OK con NACK)
	 *
	 */
	if( ackFlag == ACK ) {
		// ackFlag = TRUE: ACK the recevied data
		TWCR = (1 << TWEA) | (1 << TWINT) | (1 << TWEN);
	} else {
		// ackFlag = FALSE: NACK the recevied data
		TWCR = (1 << TWINT) | (1 << TWEN);
	}

	// Espero la respuesta
	if ( pv_I2C_awaitBusTransaction()) {
		*rxByte = TWDR;
		if( ackFlag == ACK ) {
			xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_readByte(ACK) (0x%02x)\r\n"), *rxByte );
		} else {
			xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_readByte(NACK) (0x%02x)\r\n"), *rxByte );
		}

		if ( ( TWSR & 0xF8) == TW_MR_DATA_ACK ) {	// 0x50
			return(true);
		}
		if ( ( TWSR & 0xF8) == TW_MR_DATA_NACK ) {	// 0x58
			return(true);
		}
	}

	xprintf_P(PSTR("I2C: pv_I2C_readByte: ERROR, status=0x%02x\r\n"), ( TWSR & 0xF8) );
	return(false);

}
//------------------------------------------------------------------------------------
static inline bool pv_I2C_rcvd_data( char *rdBuffer, uint8_t bytesToRead )
{
	// Envio todo un buffer en modo write.

uint8_t rxData;
bool retS = false;

	xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_rcvd_data: LENGTH=%d\r\n"), bytesToRead );

	// Leo el buffer de datos.
	while ( bytesToRead > 1 ) {
		if ( pv_I2C_readByte( &rxData, ACK ) ) {
			*rdBuffer++ = rxData;
			bytesToRead--;
		} else {
			goto quit;
		}
	}
	// Acepto el ultimo byte y respondo con NACK
	if ( pv_I2C_readByte( &rxData, NACK ) ) {
		*rdBuffer++ = rxData;
	} else {
		goto quit;
	}

	retS = true;

quit:

	if (! retS )
		xprintf_PD( f_debugBusI2C, PSTR("I2C: pv_I2C_send_data: ERROR (status=%d)\r\n"), TWSR );


	return(retS);
}
//------------------------------------------------------------------------------------


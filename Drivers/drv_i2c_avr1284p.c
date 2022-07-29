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

void pv_I2C_MasterArbitrationLostBusErrorHandler(void);
void pv_I2C_MasterTransactionFinished(uint8_t result);
bool pv_I2C_set_bus_idle(void);

bool pv_I2C_send_start( void );
bool pv_I2C_writeByte(uint8_t wrByte);
bool pv_I2C_readByte( uint8_t *rxByte, uint8_t ackFlag);
void pv_I2C_sendStop(void);

bool pv_I2C_send_devAddress( uint8_t devAddress);
bool pv_I2C_send_dataAddress( uint16_t dataAddress, uint8_t dataAddressLength);
bool pv_I2C_send_data( char *wrBuffer, uint8_t bytesToWrite );
bool pv_I2C_rcvd_data( char *rdBuffer, uint8_t bytesToRead );
void pv_I2C_reset(void);
uint8_t pv_I2C_busTransactionStatus(void);

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
int16_t drv_I2C_master_write ( const uint8_t devAddress, const uint16_t dataAddress, const uint8_t dataAddressLength, char *pvBuffer, size_t xBytes )
{

	xprintf_PD(DEBUG_I2C, PSTR("I2C: drv_I2C_master_write: START\r\n"));

	// Paso 1: PONER EL BUS EN CONDICIONES
	if ( ! pv_I2C_set_bus_idle() ) {
		pv_I2C_reset();
		return(-1);
	}

	// Paso 2: DEVICE_ADDRESS + WR
	if ( ! pv_I2C_send_devAddress( (devAddress & ~0x01) ) )
		return(-1);

	// Paso 3: DATA_ADDRESS
	if ( ! pv_I2C_send_dataAddress ( dataAddress, dataAddressLength ))
		return(-1);

	// Paso 4: DATA
	if ( !  pv_I2C_send_data( pvBuffer, xBytes))
		return(-1);


	xprintf_PD(DEBUG_I2C, PSTR("I2C: drv_I2C_master_write: EXIT\r\n"));
	return(xBytes);

}
//------------------------------------------------------------------------------------
int16_t drv_I2C_master_read ( const uint8_t devAddress, const uint16_t dataAddress, const uint8_t dataAddressLength, char *pvBuffer, size_t xBytes )
{

	xprintf_PD(DEBUG_I2C, PSTR("I2C: drv_I2C_master_read: START\r\n"));

	// Paso 1: PONER EL BUS EN CONDICIONES
	if ( ! pv_I2C_set_bus_idle() ) {
		pv_I2C_reset();
		return(-1);
	}

	// Paso 2: DEVICE_ADDRESS + WR
	if ( ! pv_I2C_send_devAddress( (devAddress & ~0x01) ) )
		return(-1);

	// Paso 3: DATA_ADDRESS
	if ( ! pv_I2C_send_dataAddress ( dataAddress, dataAddressLength ))
		return(-1);


	// Paso 4: REPEATED START + DEVICE_ADDRESS + RD
	if ( ! pv_I2C_send_devAddress( (devAddress | 0x01) ) )
		return(-1);


	// Paso 5: DATA
	if ( !  pv_I2C_rcvd_data( pvBuffer, xBytes))
		return(-1);


	xprintf_PD(DEBUG_I2C, PSTR("I2C: drv_I2C_master_read: EXIT\r\n"));
	return(xBytes);
}
//------------------------------------------------------------------------------------
bool pv_I2C_send_start( void )
{
	xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_send_start\r\n"));
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	if ( pv_I2C_busTransactionStatus() == TWIM_RESULT_OK)
		return(true);

	xprintf_P(PSTR("I2C: pv_I2C_send_start: ERROR\r\n"));
	return(false);
}
//------------------------------------------------------------------------------------
bool pv_I2C_writeByte(uint8_t wrByte)
{

	xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_writeByte (%d)\r\n"), wrByte );
	// Envia el data por el bus I2C.
	TWDR = wrByte;
	// Habilita la trasmision y el envio de un ACK.
	TWCR = (1 << TWINT) | (1 << TWEN);

	if ( pv_I2C_busTransactionStatus() == TWIM_RESULT_OK)
		return(true);

	xprintf_P(PSTR("I2C: pv_I2C_writeByte: ERROR\r\n"));
	return(false);
}
//------------------------------------------------------------------------------------
bool pv_I2C_readByte(uint8_t *rxByte, uint8_t ackFlag)
{
	// begin receive over i2c
	if( ackFlag ) {
		// ackFlag = TRUE: ACK the recevied data
		TWCR = (1 << TWEA) | (1 << TWINT) | (1 << TWEN);
	} else {
		// ackFlag = FALSE: NACK the recevied data
		TWCR = (1 << TWINT) | (1 << TWEN);
	}

	if ( pv_I2C_busTransactionStatus() == TWIM_RESULT_OK) {
		*rxByte = TWDR;
		return(true);
	}

	return(false);

}
//------------------------------------------------------------------------------------
void pv_I2C_sendStop(void)
{
	// Genera la condicion STOP en el bus I2C.
	// !!! El TWINT NO ES SETEADO LUEGO DE UN STOP
	// por lo tanto no debo esperar que termine.

	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

}
//-----------------------------------------------------------------------------------
bool pv_I2C_send_devAddress( uint8_t devAddress )
{
	xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_send_devAddress: START + SLA/RW (%d)\r\n"), devAddress);
	if ( pv_I2C_writeByte(devAddress)) {
		return(true);
	}

	xprintf_P(PSTR("pv_I2C_send_devAddress: ERROR\r\n"));
	return(false);
}
//------------------------------------------------------------------------------------
bool pv_I2C_send_dataAddress( uint16_t dataAddress, uint8_t dataAddressLength)
{
	xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_send_dataAddress: (%d)(%d)\r\n"), dataAddress,dataAddressLength );

uint8_t txData;
bool retS = false;

	// HIGH address
	if ( dataAddressLength == 2 ) {
		txData = (dataAddress) >> 8;
		xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_send_dataAddress: ADDR_H=0x%02x, status=0x%02x\r\n"), txData, TWSR );
		if ( ! pv_I2C_writeByte(txData)) {
			goto quit;
		}
	}

	// LOW address
	if ( dataAddressLength >= 1 ) {
		txData = (dataAddress) & 0x00FF;
		xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_send_dataAddress: ADDR_L=0x%02x, status=0x%02x\r\n"), txData, TWSR );
		if ( ! pv_I2C_writeByte(txData)) {
			goto quit;
		}
	}

	retS = true;

quit:

	if ( !retS )
		xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_send_dataAddress: ERROR (status=%d)\r\n"), TWSR );

	return(retS);


}
//------------------------------------------------------------------------------------
bool pv_I2C_send_data( char *wrBuffer, uint8_t bytesToWrite )
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
		xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_send_data: 0x%02x (%c),0x%02x\r\n"),txData,txData,TWSR );
		if ( ! pv_I2C_writeByte(txData)) {
			goto quit;
		}
	}
	retS = true;

quit:

	if (! retS )
		xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_send_data: ERROR (status=%d)\r\n"), TWSR );

	// Envie todo el buffer. Termino con un STOP.
	pv_I2C_sendStop();

	return(retS);
}
//------------------------------------------------------------------------------------
bool pv_I2C_rcvd_data( char *rdBuffer, uint8_t bytesToRead )
{
	// Envio todo un buffer en modo write.

uint8_t bytesRead;
uint8_t rxData;
bool retS = false;

	// Leo el buffer de datos.
	while ( bytesRead > 1 ) {
		if ( pv_I2C_readByte( &rxData, ACK ) ) {
			*rdBuffer++ = rxData;
			xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_rcvd_data: 0x%02x(%c),0x%02x\r\n"),rxData,rxData, TWSR );
		} else {
			goto quit;
		}
	}
	// Acepto el ultimo byte y respondo con NACK
	if ( pv_I2C_readByte( &rxData, NACK ) ) {
		*rdBuffer++ = rxData;
		xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_rcvd_data: 0x%02x(%c),0x%02x\r\n"),rxData,rxData, TWSR );
	} else {
		goto quit;
	}

	retS = true;

quit:

	if (! retS )
		xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_send_data: ERROR (status=%d)\r\n"), TWSR );

	// STOP + NACK
	pv_I2C_sendStop();

	return(retS);
}
//------------------------------------------------------------------------------------
uint8_t pv_I2C_busTransactionStatus(void)
{

	// Evalua el estatus y actua en consecuencia para dejar el bus listo para otra transaccion

uint8_t ticks_to_wait = 50;		// ( 50ms es el maximo tiempo que espero )
uint8_t currentStatus;

	// wait for i2c interface to complete write operation ( MASTER WRITE INTERRUPT )
	while ( ticks_to_wait-- > 0 ) {

		currentStatus = TWSR;
		xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_busTransactionStatus: status=0x%02x\r\n"), currentStatus );

		/* If arbitration lost or bus error. */
		if ( currentStatus & TW_MT_ARB_LOST ) {
			pv_I2C_MasterArbitrationLostBusErrorHandler();
			return(TWIM_RESULT_ARBITRATION_LOST);

		} else if ( currentStatus & TWI_MASTER_BUSERR_bm ) {
			pv_I2C_MasterArbitrationLostBusErrorHandler();
			return(TWIM_RESULT_BUS_ERROR);

		/* If master write interrupt. */
		} else if (currentStatus & TWI_MASTER_WIF_bm) {
			// NACK
			if ( currentStatus & TWI_MASTER_RXACK_bm) {
				TWIE.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
				return(TWIM_RESULT_NACK_RECEIVED);

			} else {
				// ACK
				return(TWIM_RESULT_OK);
			}

		/* If master read interrupt. */
		} else if (currentStatus & TWI_MASTER_RIF_bm) {
			return(TWIM_RESULT_OK);
		}


		vTaskDelay( ( TickType_t)( 1 ) );
	}

	// DEBUG
	xprintf_P( PSTR("pv_I2C_busTransactionStatus: TIMEOUT status=0x%02x\r\n"), currentStatus );
	TWIE.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
	return(TWIM_RESULT_TIMEOUT);

}
//------------------------------------------------------------------------------------
void pv_I2C_MasterArbitrationLostBusErrorHandler(void)
{

uint8_t currentStatus = TWIE.MASTER.STATUS;

	xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_MasterArbitrationLostBusErrorHandler\r\n"));

	/* If bus error. */
	if (currentStatus & TWI_MASTER_BUSERR_bm) {
	//	I2C_control.result = TWIM_RESULT_BUS_ERROR;

	} else {
	/* If arbitration lost. */
	//	I2C_control.result = TWIM_RESULT_ARBITRATION_LOST;
	}

	/* Clear interrupt flag. */
	TWIE.MASTER.STATUS = currentStatus | TWI_MASTER_ARBLOST_bm;

	//I2C_control.status = TWIM_STATUS_READY;
}
//------------------------------------------------------------------------------------
void pv_I2C_MasterTransactionFinished(uint8_t result)
{
	xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_MasterTransactionFinished\r\n"));
	//I2C_control.result = result;
	//I2C_control.status = TWIM_STATUS_READY;
}
//------------------------------------------------------------------------------------
bool pv_I2C_set_bus_idle(void)
{

	// Para comenzar una operacion el bus debe estar en IDLE o OWENED.
	// Intento pasarlo a IDLE hasta 3 veces antes de abortar, esperando 100ms
	// entre c/intento.

uint8_t	reintentos = I2C_MAXTRIES;

	while ( reintentos-- > 0 ) {

		// Los bits CLKHOLD y RXACK son solo de read por eso la mascara !!!

		//xprintf_PD(DEBUG_I2C, PSTR("I2C: drv_i2c:SetBusIdle tryes=(%d): status=0x%02x\r\n\0"),reintentos,TWIE.MASTER.STATUS );

		if (  ( ( TWIE.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm ) == TWI_MASTER_BUSSTATE_IDLE_gc ) ||
				( ( TWIE.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm ) == TWI_MASTER_BUSSTATE_OWNER_gc ) ) {
			return(true);

		} else {
			// El status esta indicando errores. Debo limpiarlos antes de usar la interface.
			if ( (TWIE.MASTER.STATUS & TWI_MASTER_ARBLOST_bm) != 0 ) {
				TWIE.MASTER.STATUS  |= TWI_MASTER_ARBLOST_bm;
			}
			if ( (TWIE.MASTER.STATUS & TWI_MASTER_BUSERR_bm) != 0 ) {
				TWIE.MASTER.STATUS |= TWI_MASTER_BUSERR_bm;
			}
			if ( (TWIE.MASTER.STATUS & TWI_MASTER_WIF_bm) != 0 ) {
				TWIE.MASTER.STATUS |= TWI_MASTER_WIF_bm;
			}
			if ( (TWIE.MASTER.STATUS & TWI_MASTER_RIF_bm) != 0 ) {
				TWIE.MASTER.STATUS |= TWI_MASTER_RIF_bm;
			}

			TWIE.MASTER.STATUS |= TWI_MASTER_BUSSTATE_IDLE_gc;	// Pongo el status en 01 ( idle )
			vTaskDelay( ( TickType_t)( 10 / portTICK_RATE_MS ) );
		}
	}

	// No pude pasarlo a IDLE: Error !!!
	xprintf_P( PSTR("drv_i2c:SetBusIdle ERROR!!: status=0x%02x\r\n\0"),TWIE.MASTER.STATUS );

	return(false);
}
//------------------------------------------------------------------------------------
void pv_I2C_reset(void)
{

uint8_t i = 0;

	// https://stackoverflow.com/questions/5497488/failed-twi-transaction-after-sleep-on-xmega
	// There is a common problem on I2C/TWI where the internal state machine gets stuck in an
	// intermediate state if a transaction is not completed fully. The slave then does not respond
	// correctly when addressed on the next transaction. This commonly happens when the master
	// is reset or stops outputting the SCK signal part way through the read or write.
	// A solution is to toggle the SCK line manually 8 or 9 times before starting any data
	// transactions so the that the internal state machines in the slaves are all reset to the
	///start of transfer point and they are all then looking for their address byte

	// La forma de resetear el bus es deshabilitando el TWI
	TWIE.MASTER.CTRLA &= ~( 1<<TWI_MASTER_ENABLE_bp);	// Disable TWI
	vTaskDelay( 5 );

	// Clockeo el SCK varias veces para destrabar a los slaves
	IO_config_SCL();
	for (i=0; i<10;i++) {
		IO_set_SCL();
		vTaskDelay( 1 );
		IO_clr_SCL();
		vTaskDelay( 1 );
	}
	// Lo dejo en reposo alto
	IO_set_SCL();

	TWIE.MASTER.CTRLA |= ( 1<<TWI_MASTER_ENABLE_bp);	// Enable TWI

//	TWIE.MASTER.CTRLC =  TWI_MASTER_CMD_REPSTART_gc;	// Send START

	// El status esta indicando errores. Debo limpiarlos antes de usar la interface.
	if ( TWIE.MASTER.STATUS & TWI_MASTER_ARBLOST_bm ) {
		TWIE.MASTER.STATUS |= TWI_MASTER_ARBLOST_bm;
	}
	if ( TWIE.MASTER.STATUS & TWI_MASTER_BUSERR_bm ) {
		TWIE.MASTER.STATUS |= TWI_MASTER_BUSERR_bm;
	}
	if ( TWIE.MASTER.STATUS & TWI_MASTER_WIF_bm ) {
		TWIE.MASTER.STATUS |= TWI_MASTER_WIF_bm;
	}
	if ( TWIE.MASTER.STATUS & TWI_MASTER_RIF_bm ) {
		TWIE.MASTER.STATUS |= TWI_MASTER_RIF_bm;
	}

	TWIE.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;	// Pongo el status en 01 ( idle )
	vTaskDelay( ( TickType_t)( 100 / portTICK_RATE_MS ) );

	xprintf_PD(DEBUG_I2C, PSTR("I2C: pv_I2C_reset: 0x%02x\r\n"), TWIE.MASTER.STATUS );
}
//------------------------------------------------------------------------------------

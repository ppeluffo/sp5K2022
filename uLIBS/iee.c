/*
 * iee.c
 *
 *  Created on: 9 ago. 2022
 *      Author: pablo
 */


#include "iee.h"


//------------------------------------------------------------------------------------
int16_t IEE_test_write( char *addr, char *str )
{
	// Funcion de testing de la EEPROM.
	// Escribe en una direccion de memoria un string
	// parametros: *addr > puntero char a la posicion de inicio de escritura
	//             *str >  puntero char al texto a escribir
	// retorna: -1 error
	//			nro.de bytes escritos

	// Calculamos el largo del texto a escribir en la eeprom.

int16_t xBytes = 0;
uint8_t length = 0;
char *p = NULL;

	p = str;
	while (*p != 0) {
		p++;
		length++;
	}

	xBytes = IEE_write( (uint16_t)(atol(addr)), str, length );

	if ( xBytes == -1 )
		xprintf_P(PSTR("ERROR: I2C:EE_test_write\r\n\0"));

	return(xBytes);
}
//------------------------------------------------------------------------------------
int16_t IEE_test_read( char *addr, char *size )
{
	// Funcion de testing de la EEPROM.
	// Lee de una direccion de la memoria una cantiad de bytes y los imprime
	// parametros: *addr > puntero char a la posicion de inicio de lectura
	//             *size >  puntero char al largo de bytes a leer
	// retorna: -1 error
	//			nro.de bytes escritos

int16_t xBytes = 0;
char buffer[32] = { 0 };
uint8_t i;

	// read ee {pos} {lenght}
	xBytes = IEE_read( (uint16_t)(atol(addr)), buffer, (uint8_t)(atoi(size) ) );
	if ( xBytes == -1 )
		xprintf_P(PSTR("ERROR: I2C:EE_test_read\r\n"));

	if ( xBytes > 0 )
		xprintf_P( PSTR( "%s\r\n"),buffer);

	for (i=0; i<xBytes; i++) {
		xprintf_P( PSTR( "%c(0x%02x) "),buffer[i], buffer[i]);
	}
	xprintf_P(PSTR("\r\n"));

	return (xBytes );

}
//------------------------------------------------------------------------------------
void IEE_get_semaphore(void)
{
	frtos_ioctl( fdIEE ,ioctl_OBTAIN_BUS_SEMPH, NULL);
}
//------------------------------------------------------------------------------------
void IEE_release_semaphore(void)
{
	frtos_ioctl(fdIEE,ioctl_RELEASE_BUS_SEMPH, NULL);
}
//------------------------------------------------------------------------------------
int16_t IEE_read( uint16_t rdAddress, char *data, uint8_t length )
{

int16_t rcode = -1;


	IEE_get_semaphore();
	frtos_ioctl(fdIEE, ioctl_IEE_SET_EEVADDRESS, &rdAddress );
	rcode =  frtos_read(fdIEE, data, length);
	IEE_release_semaphore();

	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1s que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_RATE_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf_P(PSTR("ERROR: IEE_read\r\n") );
		goto quit;
	}

quit:

	return( rcode );
}
//------------------------------------------------------------------------------------
int16_t IEE_write( uint16_t wrAddress, char *data, uint8_t length )
{

int16_t rcode = -1;


	IEE_get_semaphore();
	frtos_ioctl(fdIEE, ioctl_IEE_SET_EEVADDRESS, &wrAddress );
	rcode =  frtos_write(fdIEE, data, length);
	IEE_release_semaphore();

	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1s que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_RATE_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf_P(PSTR("ERROR: IEE_read\r\n") );
		goto quit;
	}

quit:

	return( rcode );

}
//------------------------------------------------------------------------------------

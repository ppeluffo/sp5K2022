/*
 * frtos-io.c
 *
 *  Created on: 11 jul. 2018
 *      Author: pablo
 */

#include "frtos-io.h"
#include "iopines.h"
#include "xprintf.h"

//------------------------------------------------------------------------------------
int16_t frtos_open( file_descriptor_t fd, uint32_t flags)
{
	// Funcion general para abrir el puerto que invoca a una mas
	// especializada para c/periferico.
	// Es la que invoca la aplicacion.
	// Retorna -1 en error o un nro positivo ( fd )

int16_t xRet = -1;

	switch(fd) {
        
	case fdTERM:
		drv_uart_init1 ( flags );
        xRet=0;
		break;

	case fdCOMMS:
		drv_uart_init0 ( flags );
        xRet=0;
		break;

	case fdI2C:
		// En este caso usamos el periferico_i2c_port_t como paso intermedio antes de acceder al driver.
		xRet = frtos_open_i2c( &xBusI2C, fd, &I2C_xMutexBuffer, flags );
		break;

	case fdIEE:
		xRet = frtos_open_iee( &xBusIEE, fd, &IEE_xMutexBuffer, flags );
		break;

	default:
		break;
	}


	return(xRet);
}
//------------------------------------------------------------------------------------
int16_t frtos_ioctl( file_descriptor_t fd, uint32_t ulRequest, void *pvValue )
{

int16_t xRet = -1;

	switch(fd) {
	case fdTERM:
		xRet = frtos_ioctl_uart1( ulRequest, pvValue );
		break;

	case fdCOMMS:
		xRet = frtos_ioctl_uart0( ulRequest, pvValue );
		break;

	case fdI2C:
		xRet = frtos_ioctl_i2c( &xBusI2C, ulRequest, pvValue );
		break;

	case fdIEE:
		xRet = frtos_ioctl_iee( &xBusIEE, ulRequest, pvValue );
		break;

	default:
		break;
	}

	return(xRet);
}
//------------------------------------------------------------------------------------
int16_t frtos_write( file_descriptor_t fd ,const char *pvBuffer, const uint16_t xBytes )
{

int16_t xRet = -1;

	switch(fd) {
        
	case fdTERM:
        xRet = drv_uart_write1 ( pvBuffer, xBytes );
		break;

	case fdCOMMS:
        xRet = drv_uart_write0 (pvBuffer, xBytes );
		break;

		// En este caso usamos el periferico_i2c_port_t como paso intermedio antes de acceder al driver.
	case fdI2C:
		xRet = frtos_write_i2c( &xBusI2C, pvBuffer, xBytes );
		break;

	case fdIEE:
		xRet = frtos_write_iee( &xBusIEE, pvBuffer, xBytes );
		break;

	default:
		break;
	}

	return(xRet);
}
//------------------------------------------------------------------------------
int16_t frtos_read( file_descriptor_t fd , char *pvBuffer, uint16_t xBytes )
{

int16_t xRet = -1;

	switch(fd) {
	case fdTERM:
		xRet = drv_uart_read1 ( pvBuffer, xBytes );
		break;

	case fdCOMMS:
		xRet = drv_uart_read0 ( pvBuffer, xBytes );
		break;

	// En este caso usamos el periferico_i2c_port_t como paso intermedio antes de acceder al driver.
	case fdI2C:
		xRet = frtos_read_i2c( &xBusI2C, pvBuffer, xBytes );
		break;

	case fdIEE:
		xRet = frtos_read_iee( &xBusIEE, pvBuffer, xBytes );
		break;

	default:
		break;
	}

	return(xRet);
}
//------------------------------------------------------------------------------
// FUNCIONES ESPECIFICAS DE UART's
//------------------------------------------------------------------------------
int16_t frtos_ioctl_uart0( uint32_t ulRequest, void *pvValue )
{

int16_t xReturn = 0;

	switch( ulRequest )
	{
		case ioctl_UART_CLEAR_TX_BUFFER:
			rBchar_Flush( &uart0.TXringBuffer );
			break;

		default :
			xReturn = -1;
			break;
	}

	return xReturn;

}
//------------------------------------------------------------------------------
int16_t frtos_ioctl_uart1( uint32_t ulRequest, void *pvValue )
{

int16_t xReturn = 0;

	switch( ulRequest )
	{
		case ioctl_UART_CLEAR_TX_BUFFER:
			rBchar_Flush( &uart1.TXringBuffer );
			break;

		default :
			xReturn = -1;
			break;
	}

	return xReturn;

}
//------------------------------------------------------------------------------
// FUNCIONES ESPECIFICAS DEL BUS I2C/TWI
//------------------------------------------------------------------------------------
int16_t frtos_open_i2c( periferico_i2c_port_t *xI2c, file_descriptor_t fd, StaticSemaphore_t *i2c_semph, uint32_t flags)
{
	// Asigno las funciones particulares ed write,read,ioctl
	xI2c->fd = fd;
	xI2c->xBusSemaphore = xSemaphoreCreateMutexStatic( i2c_semph );
	xI2c->xBlockTime = (10 / portTICK_RATE_MS );
	xI2c->i2c_error_code = I2C_OK;
	//
	// Abro e inicializo el puerto I2C solo la primera vez que soy invocado
	drv_I2C_init();

	return(1);
}
//------------------------------------------------------------------------------------
int16_t frtos_write_i2c( periferico_i2c_port_t *xI2c, const char *pvBuffer, const uint16_t xBytes )
{

int16_t xReturn = -1;

	xReturn = drv_I2C_master_write ( xI2c->devAddress, xI2c->dataAddress, xI2c->dataAddressLength,  (char *)pvBuffer, xBytes);

	if ( xReturn >= 0 ) {
		xI2c->i2c_error_code = I2C_OK;
	} else {
		// Error de escritura indicado por el driver.
		xI2c->i2c_error_code = I2C_WR_ERROR;
	}

	//xprintf_P(PSTR("DEBUG frtos_write_i2c (%d)\r\n"),xReturn );
	return(xReturn);

}
//------------------------------------------------------------------------------------
int16_t frtos_ioctl_i2c( periferico_i2c_port_t *xI2c, uint32_t ulRequest, void *pvValue )
{

int16_t xReturn = 0;
uint32_t *p = NULL;

	p = pvValue;

	switch( ulRequest )
	{
		case ioctl_OBTAIN_BUS_SEMPH:
			// Espero el semaforo en forma persistente.
			while ( xSemaphoreTake(xI2c->xBusSemaphore, ( TickType_t ) 5 ) != pdTRUE ) {
				//taskYIELD();
				vTaskDelay( ( TickType_t)( 1 ) );
			}
			break;
			case ioctl_RELEASE_BUS_SEMPH:
				xSemaphoreGive( xI2c->xBusSemaphore );
				break;
			case ioctl_SET_TIMEOUT:
				xI2c->xBlockTime = *p;
				break;
			case ioctl_I2C_SET_DEVADDRESS:
				xI2c->devAddress = (int8_t)(*p);
				break;
			case ioctl_I2C_SET_DATAADDRESS:
				xI2c->dataAddress = (uint16_t)(*p);
				break;
			case ioctl_I2C_SET_DATAADDRESSLENGTH:
				xI2c->dataAddressLength = (int8_t)(*p);
				break;
			case ioctl_I2C_GET_LAST_ERROR:
				xReturn = xI2c->i2c_error_code;
				break;
			case ioctl_I2C_SCAN:
				//xReturn = drv_I2C_scan_device(xI2c->devAddress );
				break;
			case ioctl_I2C_SET_DEBUG:
				drv_I2C_configDebugFlag(true);
				break;
			case ioctl_I2C_CLEAR_DEBUG:
				drv_I2C_configDebugFlag(false);
				break;

			default :
				xReturn = -1;
				break;
		}

	return xReturn;

}
//------------------------------------------------------------------------------------
int16_t frtos_read_i2c( periferico_i2c_port_t *xI2c, char *pvBuffer, const uint16_t xBytes )
{

int16_t xReturn = 0U;

	if ( ( xReturn = drv_I2C_master_read (xI2c->devAddress, xI2c->dataAddress, xI2c->dataAddressLength,  (char *)pvBuffer, xBytes) ) >= 0 ) {
		xI2c->i2c_error_code = I2C_OK;
	} else {
		// Error de escritura indicado por el driver.
		xI2c->i2c_error_code = I2C_RD_ERROR;
	}

	return(xReturn);
}
//------------------------------------------------------------------------------------
// FUNCIONES ESPECIFICAS DE EE interna
//------------------------------------------------------------------------------------
int16_t frtos_open_iee( periferico_iee_port_t *xIEEc, file_descriptor_t fd, StaticSemaphore_t *i2c_semph, uint32_t flags)
{
	// Asigno las funciones particulares ed write,read,ioctl
	xIEEc->fd = fd;
	xIEEc->xBusSemaphore = xSemaphoreCreateMutexStatic( i2c_semph );
	xIEEc->xBlockTime = (10 / portTICK_RATE_MS );
	//
	return(1);
}
//------------------------------------------------------------------------------------
int16_t frtos_read_iee( periferico_iee_port_t *xIEEc, char *pvBuffer, const uint16_t xBytes )
{

	// Una vez que lee, avanza el puntero de dirección

uint16_t i;
uint16_t address;

address = xIEEc->ieeDataAddress;
	for(i=0; i<xBytes; i++ ) {
		*pvBuffer++ = drv_IEE_read(address++);
	}

	return(xBytes);
}
//------------------------------------------------------------------------------------
int16_t frtos_write_iee( periferico_iee_port_t *xIEEc, const char *pvBuffer, const uint16_t xBytes )
{

	// Una vez que lee, avanza el puntero de dirección

uint16_t i;
uint16_t address;

	address = xIEEc->ieeDataAddress;
	for(i=0; i<xBytes; i++ ) {
		drv_IEE_write( address++ , *pvBuffer++ );
	}

	return(xBytes);

}
//------------------------------------------------------------------------------------
int16_t frtos_ioctl_iee( periferico_iee_port_t *xIEEc, uint32_t ulRequest, void *pvValue )
{

int16_t xReturn = 0;
uint32_t *p = NULL;

	p = pvValue;

	switch( ulRequest )
	{
		case ioctl_OBTAIN_BUS_SEMPH:
			// Espero el semaforo en forma persistente.
			while ( xSemaphoreTake(xIEEc->xBusSemaphore, ( TickType_t ) 5 ) != pdTRUE ) {
				//taskYIELD();
				vTaskDelay( ( TickType_t)( 1 ) );
			}
			break;
			case ioctl_RELEASE_BUS_SEMPH:
				xSemaphoreGive( xIEEc->xBusSemaphore );
				break;
			case ioctl_SET_TIMEOUT:
				xIEEc->xBlockTime = *p;
				break;
			case ioctl_IEE_SET_EEVADDRESS:
				xIEEc->ieeDataAddress = (int8_t)(*p);
				break;
			default :
				xReturn = -1;
				break;
		}

	return xReturn;

}
//------------------------------------------------------------------------------------




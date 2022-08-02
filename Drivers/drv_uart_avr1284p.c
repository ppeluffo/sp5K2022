/*
 * drv_uart_spx.c
 *
 *  Created on: 11 jul. 2018
 *      Author: pablo
 * 
 * PLACA BASE: sensor_cloro.
 * 
 * El driver de las uart permite crear las uarts y 2 estructuras tipo 
 * ringbuffer (chars) para c/u.
 * Estos son las interfaces a la capa de FRTOS-IO.
 * Para transmitir se escribe en el ringBuffer de TX y para leer lo recibido
 * se lee del ringBuffer de RX.
 * La transmision / recepcion se hace por interrupcion. Estas ISR son 
 * provistas por el driver
 * Cada placa tiene diferente asignacion de puertos por lo tanto hay
 * que modificar el driver a c/placa.
 * 
 * 
 */

#include <drv_uart_avr1284p.h>

//------------------------------------------------------------------------------
// USART0
//------------------------------------------------------------------------------
void drv_uart_init0(uint32_t baudrate )
{

	USART0_init();

	// Inicializo la estructura de la UART
    // RingBuffer de transmision
	uart0.TXringBuffer.buff = uart0_txBuffer;
	uart0.TXringBuffer.head = 0;	// start
	uart0.TXringBuffer.tail = 0;	// end
	uart0.TXringBuffer.count = 0;
	uart0.TXringBuffer.length = UART0_TXSIZE;
    
    // RingBuffer de recepcion
	uart0.RXringBuffer.buff = uart0_rxBuffer;
	uart0.RXringBuffer.head = 0;	// start
	uart0.RXringBuffer.tail = 0;	// end
	uart0.RXringBuffer.count = 0;
	uart0.RXringBuffer.length = UART0_RXSIZE;
}
//------------------------------------------------------------------------------
int16_t drv_uart_write0( const char *pvBuffer, const uint16_t xBytes )
{

uint16_t i;

    // Transmision x poleo ( No hablito al INT x DRIE ) por lo tanto no uso el driver

    for( i = 0; i < xBytes; i++) {
    	while ( !( UCSR0A & (1<<UDRE0)) )
    		taskYIELD();
    	UDR0 = pvBuffer[i];
    }
    vTaskDelay( ( TickType_t)( 1 ) );
    return(xBytes);
}
//------------------------------------------------------------------------------
int16_t drv_uart_read0( char *pvBuffer, uint16_t xBytes )
{
	// Lee caracteres de la cola de recepcion y los deja en el buffer.
	// El timeout lo fijo con ioctl.

int16_t xBytesReceived = 0U;
TickType_t xTicksToWait = 10;
TimeOut_t xTimeOut;

     /* Initialize xTimeOut.  This records the time at which this function was
        entered.
      */
	vTaskSetTimeOutState( &xTimeOut );

	// Are there any more bytes to be received?
	while( xBytesReceived < xBytes )
	{

        if( rBchar_Pop( &uart0.RXringBuffer, &((char *)pvBuffer)[ xBytesReceived ] ) == true ) {
			xBytesReceived++;
            /*
             Recibi un byte. Re-inicio el timeout.
             */
            vTaskSetTimeOutState( &xTimeOut );
			//taskYIELD();
            //vTaskDelay( ( TickType_t)( 1 ) );
		} else {
			// Espero xTicksToWait antes de volver a chequear
			vTaskDelay( ( TickType_t)( 1 ) );

            // Time out has expired ?
            if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )
            {
                break;
            }
        }

    }

	return ( xBytesReceived );

}
//------------------------------------------------------------------------------
/*
ISR(USART0_DRE_vect)
{
    // ISR de transmisi�n de la UART0 ( TERM )
    
char cChar = ' ';
int8_t res = false;

	res = rBchar_PopFromISR( &TXRB_uart0, (char *)&cChar );

	if( res == true ) {
		// Send the next character queued for Tx
		USART0.TXDATAL = cChar;
	} else {
		// Queue empty, nothing to send.Apago la interrupcion
        USART0.CTRLB &= ~USART_TXEN_bm;
	}
}
 */
//-----------------------------------------------------------------------------
ISR(USART0_RX_vect)
{
    // Driver ISR: Cuando se genera la interrupcion por RXIE, lee el dato
    // y lo pone en la cola (ringBuffer.)
char cChar = ' ';

	cChar = UDR0;
 	rBchar_PokeFromISR( &uart0.RXringBuffer, &cChar );
}
//------------------------------------------------------------------------------
// USART1
//------------------------------------------------------------------------------
void drv_uart_init1(uint32_t baudrate )
{
    
	USART1_init();
    
    // Las transmisiones son por poleo no INT.
    
    // RingBuffer de transmision
	uart1.TXringBuffer.buff = uart1_txBuffer;
	uart1.TXringBuffer.head = 0;	// start
	uart1.TXringBuffer.tail = 0;	// end
	uart1.TXringBuffer.count = 0;
	uart1.TXringBuffer.length = UART1_TXSIZE;
    
    // RingBuffer de recepcion
	uart1.RXringBuffer.buff = uart1_rxBuffer;
	uart1.RXringBuffer.head = 0;	// start
	uart1.RXringBuffer.tail = 0;	// end
	uart1.RXringBuffer.count = 0;
	uart1.RXringBuffer.length = UART1_RXSIZE;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int16_t drv_uart_write1( const char *pvBuffer, const uint16_t xBytes )
{

uint16_t i;

    // Transmision x poleo ( No hablito al INT x DRIE ) por lo tanto no uso el driver

    for( i = 0; i < xBytes; i++) {
    	while ( !( UCSR1A & (1<<UDRE1)) )
    		taskYIELD();
    	UDR1 = pvBuffer[i];
    }
    vTaskDelay( ( TickType_t)( 1 ) );
    return(xBytes);
}
//------------------------------------------------------------------------------
int16_t drv_uart_read1( char *pvBuffer, uint16_t xBytes )
{
	// Lee caracteres de la cola de recepcion y los deja en el buffer.
	// El timeout lo fijo con ioctl.

int16_t xBytesReceived = 0U;
TickType_t xTicksToWait = 10;
TimeOut_t xTimeOut;

     /* Initialize xTimeOut.  This records the time at which this function was
        entered.
      */
	vTaskSetTimeOutState( &xTimeOut );

	// Are there any more bytes to be received?
	while( xBytesReceived < xBytes )
	{

        if( rBchar_Pop( &uart1.RXringBuffer, &((char *)pvBuffer)[ xBytesReceived ] ) == true ) {
			xBytesReceived++;
            /*
             Recibi un byte. Re-inicio el timeout.
             */
            vTaskSetTimeOutState( &xTimeOut );
			//taskYIELD();
            //vTaskDelay( ( TickType_t)( 1 ) );
		} else {
			// Espero xTicksToWait antes de volver a chequear
			vTaskDelay( ( TickType_t)( 1 ) );

            // Time out has expired ?
            if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )
            {
                break;
            }
        }

    }

	return ( xBytesReceived );

}
//------------------------------------------------------------------------------
/*
ISR(USART1_DRE_vect)
{
    // ISR de transmisi�n de la UART1 ( MODBUS )
    
char cChar = ' ';
int8_t res = false;

	res = rBchar_PopFromISR( &TXRB_uart1, (char *)&cChar );

	if( res == true ) {
		// Send the next character queued for Tx
		USART1.TXDATAL = cChar;
	} else {
		// Queue empty, nothing to send.Apago la interrupcion
        USART1.CTRLB &= ~USART_TXEN_bm;
	}
}
 */
//-----------------------------------------------------------------------------
ISR(USART1_RX_vect)
{
    // Driver ISR: Cuando se genera la interrupcion por RXIE, lee el dato
    // y lo pone en la cola (ringBuffer.)
char cChar = ' ';

	cChar = UDR1;
 	rBchar_PokeFromISR( &uart1.RXringBuffer, &cChar );
}
//------------------------------------------------------------------------------

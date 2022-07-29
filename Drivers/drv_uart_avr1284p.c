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
// USART0: TERM
//------------------------------------------------------------------------------
void drv_uart0_init(uint32_t baudrate )
{

	USART0_init();
    
    // RingBuffer de transmision
    TXRB_uart0.buff = uart0_txBuffer;
	TXRB_uart0.head = 0;	// start
	TXRB_uart0.tail = 0;	// end
	TXRB_uart0.count = 0;
	TXRB_uart0.length = UART0_TXSIZE;
    
    // RingBuffer de recepcion
    RXRB_uart0.buff = uart0_rxBuffer;
	RXRB_uart0.head = 0;	// start
	RXRB_uart0.tail = 0;	// end
	RXRB_uart0.count = 0;
	RXRB_uart0.length = UART0_RXSIZE;
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
 	rBchar_PokeFromISR( &RXRB_uart0, &cChar );
}
//------------------------------------------------------------------------------
// USART1: MODBUS
//------------------------------------------------------------------------------
void drv_uart1_init(uint32_t baudrate )
{
    
	USART1_init();
    
    // Las transmisiones son por poleo no INT.
    
    // RingBuffer de transmision
    TXRB_uart1.buff = uart1_txBuffer;
	TXRB_uart1.head = 0;	// start
	TXRB_uart1.tail = 0;	// end
	TXRB_uart1.count = 0;
	TXRB_uart1.length = UART1_TXSIZE;
    
    // RingBuffer de recepcion
    RXRB_uart1.buff = uart1_rxBuffer;
	RXRB_uart1.head = 0;	// start
	RXRB_uart1.tail = 0;	// end
	RXRB_uart1.count = 0;
	RXRB_uart1.length = UART1_RXSIZE;
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
 	rBchar_PokeFromISR( &RXRB_uart1, &cChar );
}
//------------------------------------------------------------------------------

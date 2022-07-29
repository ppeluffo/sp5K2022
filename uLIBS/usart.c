/*
 * usart.c
 *
 * Created: 21/10/2020 12:21:17
 *  Author: Pablo
 */ 

#include "usart.h"


// ---------------------------------------------------------------
void USART0_init(void)
{
	// Configuro para 9600

uint16_t bauddiv;

	/* Set the baud rate.9600 */
	bauddiv = ((F_CPU+(UART0_DEFAULT_BAUD_RATE*8L))/(UART0_DEFAULT_BAUD_RATE*16L)-1);
	UBRR0L = bauddiv;
	UBRR0H = bauddiv>>8;
	
	// enable RxD/TxD and interrupts
	UCSR0B = ( 1 << RXCIE0) | ( 1 << RXEN0 ) | ( 1 << TXEN0 );

	/* Set the data bits to 8N1. */
	UCSR0C = ( serUCSRC_SELECT | serEIGHT_DATA_BITS );
	
}
// ---------------------------------------------------------------
void USART0_sendChar(char c)
{
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	UDR0 = c;

}
// ---------------------------------------------------------------
void USART0_sendString(char *str)
{
	for(size_t i = 0; i < strlen(str); i++)
	{
		USART0_sendChar(str[i]);
	}
}
// ---------------------------------------------------------------
char USART0_readChar(bool echo)
{
char c;

	while ( !(UCSR0A & (1<<RXC0)) ) {
		//wdt_reset();
		;
	}

	c = UDR0;
	if ( echo)
		USART0_sendChar(c);

	return(c);

}
// ---------------------------------------------------------------
bool USART0_getChar( char *c )
{

	if ( UCSR0A & (1<<RXC0) ) {
		*c = UDR0;
		return(true);
	}
	return(false);
}
// ---------------------------------------------------------------
void USART1_init(void)
{
	// Configuro para 9600

uint16_t bauddiv;

	/* Set the baud rate.9600 */
	bauddiv = ((F_CPU+(UART1_DEFAULT_BAUD_RATE*8L))/(UART1_DEFAULT_BAUD_RATE*16L)-1);
	UBRR1L = bauddiv;
	UBRR1H = bauddiv>>8;
	
	// enable RxD/TxD and interrupts
	UCSR1B = ( 1 << RXCIE1) | ( 1 << RXEN1 ) | ( 1 << TXEN1 );

	/* Set the data bits to 8N1. */
	UCSR1C = ( serUCSRC_SELECT | serEIGHT_DATA_BITS );
	
}
// ---------------------------------------------------------------
void USART1_sendChar(char c)
{
	while ( !( UCSR1A & (1<<UDRE1)) )
	;
	UDR1 = c;

}
// ---------------------------------------------------------------
void USART1_sendString(char *str)
{
	for(size_t i = 0; i < strlen(str); i++)
	{
		USART1_sendChar(str[i]);
	}
}
// ---------------------------------------------------------------
char USART1_readChar(bool echo)
{
char c;

	while ( !(UCSR1A & (1<<RXC1)) ) {
		//wdt_reset();
		;
	}

	c = UDR1;
	if ( echo)
		USART1_sendChar(c);

	return(c);

}
// ---------------------------------------------------------------
bool USART1_getChar( char *c )
{

	if ( UCSR1A & (1<<RXC1) ) {
		*c = UDR1;
		return(true);
	}
	return(false);
}
// ---------------------------------------------------------------

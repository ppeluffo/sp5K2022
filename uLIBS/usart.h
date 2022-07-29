/* 
 * File:   usart.h
 * Author: pablo
 *
 * Created on 27 de octubre de 2021, 03:58 PM
 */

#ifndef USART_H
#define	USART_H

#ifdef	__cplusplus
extern "C" {
#endif


#ifdef	__cplusplus
}
#endif

#include "stdbool.h"
#include "string.h"
#include "stdint.h"
#include <avr/io.h>

/* Constants for writing to UCSRB. */
#define serRX_INT_ENABLE				0x80
#define serRX_ENABLE					0x10
#define serTX_INT_ENABLE				0x20
#define serTX_ENABLE					0x08

/* Constants for writing to UCSRC. */
#define serUCSRC_SELECT					0x00
#define serEIGHT_DATA_BITS				0x06

#define UART0_DEFAULT_BAUD_RATE		9600	///< default baud rate for UART0
#define UART1_DEFAULT_BAUD_RATE		9600

void USART0_init(void);
void USART0_sendChar(char c);
void USART0_sendString(char *str);
char USART0_readChar(bool echo);
bool USART0_getChar( char *c );

void USART1_init(void);
void USART1_sendChar(char c);
void USART1_sendString(char *str);
char USART1_readChar(bool echo);
bool USART1_getChar( char *c );

#endif	/* USART_H */


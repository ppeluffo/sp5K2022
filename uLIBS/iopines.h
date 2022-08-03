/*
 * iopines.h
 *
 *  Created on: 1 ago. 2022
 *      Author: pablo
 */

#ifndef ULIBS_IOPINES_H_
#define ULIBS_IOPINES_H_


#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"

//------------------------------------------------------------------------------
// TICK es el pin en el que probamos generar una senal del FRTOS (port.c)

#define TICK_PORT	PORTA
#define TICK_DDR	DDRA
#define TICK_PIN	PINA
#define TICK_BIT	3

#define TICK_bm  	(1<<TICK_BIT)
#define TICK_bp      TICK_BIT

#define SET_TICK() 		( TICK_PORT |= TICK_bm )
#define CLEAR_TICK()  	( TICK_PORT &= ~TICK_bm )
#define TOGGLE_TICK()	( TICK_PIN |= TICK_bm )

void TICK_init(void);
void TICK_flash(void);

//------------------------------------------------------------------------------
// UARTCTL es el pin que maneja el driver LM365 de las 2 uarts

#define UARTCTL_PORT	PORTD
#define UARTCTL_DDR		DDRD
#define UARTCTL_PIN		PIND
#define UARTCTL_BIT		   4

#define UARTCTL_bm		(1<<UARTCTL_BIT)
#define UARTCTL_bp	   	UARTCTL_BIT

#define CONFIG_UARTCTL_ASOUTPUT()	( UARTCTL_DDR |= UARTCTL_bm )
#define DISABLE_UARTCTL() 			( UARTCTL_PORT |= UARTCTL_bm )
#define ENABLE_UARTCTL() 			( UARTCTL_PORT &= UARTCTL_bm )

//------------------------------------------------------------------------------
// TWI bus

#define SCL_PORT	PORTC
#define SCL_DDR		DDRC
#define SCL_PIN		PINC
#define SCL_BIT		0

#define SCL_bm  (1<<0)
#define SCL_bp      1

#define SDA_PORT	PORTC
#define SDA_DDR		DDRC
#define SDA_PIN		PINC
#define SDA_BIT		1

#define SDA_bm  (1<<1)
#define SDA_bp      1

#define CONFIG_SCL_ASOUTPUT()	( sbi(SCL_DDR, SCL_BIT))
#define CONFIG_SDA_ASOUTPUT()	( sbi(SDA_DDR, SDA_BIT))

#define SET_SCL() 		sbi(SCL_PORT, SCL_BIT)
#define CLEAR_SCL()  	cbi(SCL_PORT, SCL_BIT)
#define TOGGLE_SCL()	sbi(SCL_PIN, SCL_BIT)

//------------------------------------------------------------------------------
// Pines del micro de interrupcion a los que se conectan los MCP

// INT MCP23008 de placa SP5K_LOGIC
#define INT_MCP0_PORT	PORTD
#define INT_MCP0_PIN	PIND
#define INT_MCP0_BIT	5
#define INT_MCP0_DDR	DDRD
#define INT_MCP0_MASK	0x20

// INT MCP23018 de placa SPR5_3CH
#define INT_MCP1_PORT	PORTB
#define INT_MCP1_PIN	PINB
#define INT_MCP1_BIT	2
#define INT_MCP1_DDR	DDRB
#define INT_MCP1_MASK	0x04


#endif /* ULIBS_IOPINES_H_ */

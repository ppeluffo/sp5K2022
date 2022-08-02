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
#include "avrlibdefs.h"

//------------------------------------------------------------------------------
#define LED_PORT	PORTA
#define LED_DDR		DDRA
#define LED_PIN		PINA
#define LED_BIT		3

#define LED_bm  	(1<<LED_BIT)
#define LED_bp      LED_BIT

//#define PRENDER_LED() 	( LED_PORT |= LED_bm )
//#define APAGAR_LED()  	( LED_PORT &= ~LED_bm )

#define PRENDER_LED() 	sbi(LED_PORT, LED_BIT)
#define APAGAR_LED()  	cbi(LED_PORT, LED_BIT)
#define TOGGLE_LED()	sbi(LED_PIN, LED_BIT)

//------------------------------------------------------------------------------
#define UARTCTL_PORT	PORTD
#define UARTCTL_DDR		DDRD
#define UARTCTL_PIN		PIND
#define UARTCTL_BIT		   4

#define UARTCTL_bm		(1<<UARTCTL_BIT)
#define UARTCTL_bp	   	UARTCTL_BIT

#define CONFIG_UARTCTL_ASOUTPUT()	( sbi(UARTCTL_DDR, UARTCTL_BIT))
#define DISABLE_UARTCTL() 			sbi(UARTCTL_PORT, UARTCTL_BIT)
#define ENABLE_UARTCTL() 			cbi(UARTCTL_PORT, UARTCTL_BIT)

//------------------------------------------------------------------------------

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


#endif /* ULIBS_IOPINES_H_ */

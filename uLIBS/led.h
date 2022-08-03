/* 
 * File:   led.h
 * Author: pablo
 *
 * Created on 8 de febrero de 2022, 11:33 AM
 */

#ifndef LED_H
#define	LED_H

#ifdef	__cplusplus
extern "C" {
#endif


#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "mcp.h"

//------------------------------------------------------------------------------
// Led D1: Esta en la SP5K_LOGIC, en el MCP0, GPIO6
//
void SET_LED_D1(void);
void CLEAR_LED_D1(void);
void TOGGLE_LED_D1(void);

void LED_D1_init(void);
void LED_D1_flash(void);
//------------------------------------------------------------------------------
// Led D2: Esta en la SP5K_3CH, driveado por PC3
#define LED_D2_PORT	PORTC
#define LED_D2_DDR	DDRC
#define LED_D2_PIN	PINC
#define LED_D2_BIT	3

#define LED_D2_bm  		(1<<LED_D2_BIT)
#define LED_D2_bp      	LED_D2_BIT

#define SET_LED_D2() 		( LED_D2_PORT |= LED_D2_bm )
#define CLEAR_LED_D2()  	( LED_D2_PORT &= ~LED_D2_bm )
#define TOGGLE_LED_D2()		( LED_D2_PIN |= LED_D2_bm )

void LED_D2_init(void);
void LED_D2_flash(void);

//------------------------------------------------------------------------------
// Led D3: Esta en la SP5K_3CH, driveado por PD6
#define LED_D3_PORT	PORTD
#define LED_D3_DDR	DDRD
#define LED_D3_PIN	PIND
#define LED_D3_BIT	6

#define LED_D3_bm  		(1<<LED_D3_BIT)
#define LED_D3_bp      	LED_D3_BIT

#define SET_LED_D3() 		( LED_D3_PORT |= LED_D3_bm )
#define CLEAR_LED_D3()  	( LED_D3_PORT &= ~LED_D3_bm )
#define TOGGLE_LED_D3()		( LED_D3_PIN |= LED_D3_bm )

void LED_D3_init(void);
void LED_D3_flash(void);

//------------------------------------------------------------------------------

#ifdef	__cplusplus
}
#endif

#endif	/* LED_H */


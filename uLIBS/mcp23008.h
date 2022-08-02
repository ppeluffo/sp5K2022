/*
 * mcp23008.h
 *
 *  Created on: 2 ago. 2022
 *      Author: pablo
 */

#ifndef ULIBS_MCP23008_H_
#define ULIBS_MCP23008_H_

#include "i2c.h"
#include "frtos-io.h"
#include "stdint.h"
#include "stdbool.h"
#include "xprintf.h"

#define MCP0_INTPORT	PORTD
#define MCP0_INTPIN		PIND
#define MCP0_INT		5
#define MCP0_INTDDR		DDRD
#define MCP0_INTMASK	0x20

#define MCP1_INTPORT	PORTB
#define MCP1_INTPIN		PINB
#define MCP1_INT		2
#define MCP1_INTDDR		DDRB
#define MCP1_INTMASK	0x04

#define MCP_ID				0x40
#define MCP_ADDR0			0x00
#define MCP_ADDR1			0x01

#define MCPADDR_IODIR		0x00
#define MCPADDR_GPINTEN		0x02
#define MCPADDR_IOCON		0x05
#define MCPADDR_GPPU		0x06
#define MCPADDR_GPIO 		0x09
#define MCPADDR_OLAT 		0x0A

#define MCP_GPIO_IGPRSDCD			1	// IN
#define MCP_GPIO_IGPRSRI			2	// IN
#define MCP_GPIO_OGPRSSW			3	// OUT

#define MCP_GPIO_OTERMPWR			4
#define MCP_GPIO_OGPRSPWR			5
#define MCP_GPIO_OLED				6
#define MCP_GPIO_ITERMPWRSW			7
/*
#define MCP_GPIO_OGPRSPWR			4	// OUT
#define MCP_GPIO_OTERMPWR			5	// OUT
#define MCP_GPIO_ITERMPWRSW			6	// IN
#define MCP_GPIO_OLED				7	// OUT
*/
#define MCP_GPIO_DIN0				2	// IN
#define MCP_GPIO_DIN1				3	// IN
#define MCP_GPIO_PWRSENSORS			6	// OUT


#define MCP0	0
#define MCP1	1

bool MCP_read ( uint8_t id, uint8_t regAddress, char *value );
bool MCP_write ( uint8_t id, uint8_t regAddress, uint8_t value );

void MCP0_init(void);
void MCP1_init(void);

bool MCP_setLed( void );
bool MCP_clearLed( void );
bool MCP_setSensorPwr( void );
bool MCP_clearSensorPwr( void );

#endif /* ULIBS_MCP23008_H_ */

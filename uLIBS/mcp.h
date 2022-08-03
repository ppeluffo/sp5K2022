/*
 * mcp23008.h
 *
 *  Created on: 2 ago. 2022
 *      Author: pablo
 */

#ifndef ULIBS_MCP_H_
#define ULIBS_MCP_H_

#include "i2c.h"
#include "frtos-io.h"
#include "stdint.h"
#include "stdbool.h"
#include "xprintf.h"


// Registros MCP0
#define MCP0_IODIR		0x00
#define MCP0_IPOL		0x01
#define MCP0_GPINTEN	0x02
#define MCP0_DEFVAL		0x03
#define MCP0_INTCON		0x04
#define MCP0_IOCON		0x05
#define MCP0_GPPU		0x06
#define MCP0_INTF		0x07
#define MCP0_INTCAP		0x08
#define MCP0_GPIO 		0x09
#define MCP0_OLAT 		0x0A

// Bits del MCP0
#define MCP0_GPIO_IGPRSDCD			1	// IN
#define MCP0_GPIO_IGPRSRI			2	// IN
#define MCP0_GPIO_OGPRSSW			3	// OUT
#define MCP0_GPIO_OTERMPWR			4
#define MCP0_GPIO_OGPRSPWR			5
#define MCP0_GPIO_OLED				6

//------------------------------------------------------------------------------------
// Registros MCP1
#define MCP1_IODIRA					0x00
#define MCP1_IODIRB					0x01
#define MCP1_GPINTENA				0x04
#define MCP1_GPINTENB				0x05
#define MCP1_DEFVALA				0x06
#define MCP1_DEFVALB				0x07
#define MCP1_INTCONA				0x08
#define MCP1_INTCONB				0x09
#define MCP1_IOCON					0x0A
#define MCP1_GPPUA					0x0C
#define MCP1_GPPUB					0x0D
#define MCP1_INTFA					0x0E
#define MCP1_INTFB					0x0F
#define MCP1_INTCAPA				0x10
#define MCP1_INTCAPB				0x11
#define MCP1_GPIOA					0x12
#define MCP1_GPIOB					0x13
#define MCP1_OLATA					0x14
#define MCP1_OLATB					0x15

// Bits del MCP1
#define MCP1_ENA2						0
#define MCP1_ENB2						1
#define MCP1_PHB2						2
#define MCP1_PHB1						3
#define MCP1_ENB1						4
#define MCP1_ENA1						5
#define MCP1_PHA1						6
#define MCP1_RESET						0
#define MCP1_SLEEP						1
#define MCP1_FAULT						2
#define MCP1_PHA2						3
#define MCP1_OPWRSENSORS				4
#define MCP1_DIN0						5
#define MCP1_DIN1						6
#define MCP1_OANALOG					7

#define MCP1_GPIO_DIN0					6	// IN
#define MCP1_GPIO_DIN1					5	// IN
#define MCP1_GPIO_PWRSENSORS			4	// OUT
#define MCP1_GPIO_ANALOGPWR				7	// OUT
//------------------------------------------------------------------------------------

#define MCP0	0		// id del MCP de la Logic Board
#define MCP1	1		// id del MCP de la data Board

bool MCP_read ( uint8_t id, uint8_t regAddress, char *value );
bool MCP_write ( uint8_t id, uint8_t regAddress, uint8_t value );

void MCP0_init(void);
void MCP1_init(void);

bool SET_OPWRSENSORS(void);
bool CLEAR_OPWRSENSORS(void);

bool SET_OANALOG(void);
bool CLEAR_OANALOG(void);


#endif /* ULIBS_MCP_H_ */

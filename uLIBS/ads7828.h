/*
 * ads7828.h
 *
 *  Created on: 2 ago. 2022
 *      Author: pablo
 */

#ifndef ULIBS_ADS7828_H_
#define ULIBS_ADS7828_H_


#include "i2c.h"
#include "frtos-io.h"
#include "stdint.h"
#include "stdbool.h"
#include "xprintf.h"


#define 	ADS7828_CMD_SD   		0x80		//ADS7828 Single-ended/Differential Select bit.

#define 	ADS7828_CMD_PDMODE0   	0x00	//ADS7828 Power-down Mode 0.
#define 	ADS7828_CMD_PDMODE1   	0x04	//ADS7828 Power-down Mode 1.
#define 	ADS7828_CMD_PDMODE2   	0x08	//ADS7828 Power-down Mode 2.
#define 	ADS7828_CMD_PDMODE3   	0x0C	//ADS7828 Power-down Mode 3.


int16_t ADS7828_rawRead( uint8_t cmdByte , char *data );
bool ADS7828_read (uint8_t channel, uint16_t *value, bool f_debug );
bool ADC7828_test_read( char *s_channel);


#endif /* ULIBS_ADS7828_H_ */

/*
 * l_i2c.h
 *
 *  Created on: 26 de mar. de 2018
 *      Author: pablo
 */

#ifndef SPX_LIBS_L_I2C_H_
#define SPX_LIBS_L_I2C_H_

#include "frtos-io.h"

#include "xprintf.h"
#include "avr/pgmspace.h"

#define BUSADDR_EEPROM	0xA0
#define BUSADDR_ADS7828	0x90
#define BUSADDR_MCP0	0x40
#define BUSADDR_MCP1	0x42

int16_t I2C_read ( uint8_t i2c_bus_address, uint16_t data_address, uint8_t data_address_length, char *data, uint8_t length );
int16_t I2C_write ( uint8_t i2c_bus_address, uint16_t data_address, uint8_t data_address_length, char *data, uint8_t length );
void I2C_release_semaphore(void);
void I2C_get_semaphore(void);
bool I2C_scan_device( uint8_t i2c_bus_address );
void I2C_reinit_devices(void);


#endif /* SPX_LIBS_L_I2C_H_ */

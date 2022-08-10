/*
 * iee.h
 *
 *  Created on: 9 ago. 2022
 *      Author: pablo
 */

#ifndef ULIBS_IEE_H_
#define ULIBS_IEE_H_

#include <avr/io.h>
#include "frtos-io.h"
#include "stdint.h"
#include "stdbool.h"
#include "xprintf.h"

void IEE_get_semaphore(void);
void IEE_release_semaphore(void);

int16_t IEE_test_write( char *addr, char *str );
int16_t IEE_test_read( char *addr, char *size );
int16_t IEE_read( uint16_t rdAddress, char *data, uint8_t length );
int16_t IEE_write( uint16_t wrAddress, char *data, uint8_t length );


#endif /* ULIBS_IEE_H_ */

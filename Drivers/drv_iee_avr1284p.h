/*
 * drv_ee_avr1284p.h
 *
 *  Created on: 9 ago. 2022
 *      Author: pablo
 */

#ifndef DRIVERS_DRV_IEE_AVR1284P_H_
#define DRIVERS_DRV_IEE_AVR1284P_H_


#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "FreeRTOS.h"


void drv_IEE_write(unsigned int uiAddress, unsigned char ucData);
unsigned char drv_IEE_read(unsigned int uiAddress);


#endif /* DRIVERS_DRV_IEE_AVR1284P_H_ */

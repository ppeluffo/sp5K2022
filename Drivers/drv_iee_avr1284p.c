/*
 * drv_ee_avr1284p.c
 *
 *  Created on: 9 ago. 2022
 *      Author: pablo
 */


#include <drv_iee_avr1284p.h>


//------------------------------------------------------------------------------
void drv_IEE_write(unsigned int uiAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE))
		;
	/* Set up address and Data Registers */
	EEAR = uiAddress;
	EEDR = ucData;
	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	/* Start eeprom write by setting EEPE */
	EECR |= (1<<EEPE);
}
//------------------------------------------------------------------------------
unsigned char drv_IEE_read(unsigned int uiAddress)
	{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE))
		;
	/* Set up address register */
	EEAR = uiAddress;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from Data Register */
	return EEDR;
}
//------------------------------------------------------------------------------

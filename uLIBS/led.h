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
#include "avrlibdefs.h"
#include "iopines.h"
    
void LED_init(void);
void led_flash(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LED_H */


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
    

#define LED_PORT	PORTA
#define LED_DDR		DDRA
#define LED_PIN		PINA
#define LED_BIT		3

#define LED_bm  (1<<0)
#define LED_bp      1
    
//#define PRENDER_LED() 	( LED_PORT |= LED_bm )
//#define APAGAR_LED()  	( LED_PORT &= ~LED_bm )

#define PRENDER_LED() 	sbi(LED_PORT, LED_BIT)
#define APAGAR_LED()  	cbi(LED_PORT, LED_BIT)
#define TOGGLE_LED()	sbi(LED_PIN, LED_BIT)

void LED_init(void);
void led_flash(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LED_H */


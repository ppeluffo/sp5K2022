/*
 * sp5K2022.h
 *
 *  Created on: 28 jul. 2022
 *      Author: pablo
 */

#ifndef TASKS_SP5K2022_H_
#define TASKS_SP5K2022_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "croutine.h"
#include "semphr.h"
#include "timers.h"
#include "limits.h"
#include "portable.h"

#include <avr/io.h>
#include <avr/builtins.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <mcp.h>
#include "stdbool.h"
#include "stdlib.h"

#include "frtos_cmd.h"
#include "frtos-io.h"
#include "xprintf.h"
#include "eeprom.h"
#include "ads7828.h"
#include "led.h"
#include "iopines.h"
#include "linearBuffer.h"
#include "iee.h"

#define FW_REV "1.0.1"
#define FW_DATE "@ 20221128"
#define HW_MODELO "SP5K2022 FRTOS R001 HW:AVRMEGA1284P"
#define FRTOS_VERSION "FW:FreeRTOS V202111.00"

#define SYSMAINCLK 24

#define tkCtl_TASK_PRIORITY	 	( tskIDLE_PRIORITY + 1 )
#define tkCmd_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )
#define tkData_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )
#define tkCounters_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 )
#define tkRXComms_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 )

#define tkCtl_STACK_SIZE		384
#define tkCmd_STACK_SIZE		384
#define tkData_STACK_SIZE		384
#define tkCounters_STACK_SIZE	384
#define tkRXComms_STACK_SIZE	384

StaticTask_t tkCtl_Buffer_Ptr;
StackType_t tkCtl_Buffer [tkCtl_STACK_SIZE];

StaticTask_t tkCmd_Buffer_Ptr;
StackType_t tkCmd_Buffer [tkCmd_STACK_SIZE];

StaticTask_t tkData_Buffer_Ptr;
StackType_t tkData_Buffer [tkData_STACK_SIZE];

StaticTask_t tkCounters_Buffer_Ptr;
StackType_t tkCounters_Buffer [tkCounters_STACK_SIZE];

StaticTask_t tkRXComms_Buffer_Ptr;
StackType_t tkRXComms_Buffer [tkRXComms_STACK_SIZE];

SemaphoreHandle_t sem_SYSVars;
StaticSemaphore_t SYSVARS_xMutexBuffer;
#define MSTOTAKESYSVARSSEMPH ((  TickType_t ) 10 )

TaskHandle_t xHandle_tkCtl, xHandle_tkCmd, xHandle_tkData, xHandle_tkCounters, xHandle_tkRXComms;

bool starting_flag;

void tkCtl(void * pvParameters);
void tkCmd(void * pvParameters);
void tkData(void * pvParameters);
void tkCounters(void * pvParameters);
void tkRXComms(void * pvParameters);

typedef struct {
	uint16_t pulse_counter;
	uint32_t pulse_width;
	uint32_t cticks;
} counters_t;

#define DLGID_LENGTH		10
#define PARAMNAME_LENGTH	12
#define ANALOG_CHANNELS		3
#define COUNTER_CHANNELS	2

// Configuracion de canales analogicos
typedef struct {
	uint8_t imin;
	uint8_t imax;
	float mmin;
	float mmax;
	char name[PARAMNAME_LENGTH];
	float offset;
} ainputs_conf_t;

// Configuracion de canales digitales
typedef struct {
	char name[PARAMNAME_LENGTH];
	float magpp;
} counters_conf_t;

typedef struct {
	char dlgid[DLGID_LENGTH];
	uint16_t timerpoll;
	uint32_t clock;
	ainputs_conf_t ainputs[ANALOG_CHANNELS];
	counters_conf_t counters[COUNTER_CHANNELS];
} systemVars_t;

systemVars_t systemVars;

void reset(void);
void config_debug_counters(bool val);
void clear_latches(void);
void read_counters( counters_t *cntvals, bool clear );
void save_systemVars(void);
uint8_t load_systemVars(void);
void load_defaults(void);
bool load_config(void);
void load_analog_defaults(void);
void load_counters_defaults(void);
bool config_analog_channel( uint8_t channel,char *s_name,char *s_imin,char *s_imax,char *s_mmin,char *s_mmax );
bool config_counters_channel( uint8_t channel,char *s_name, char *s_magpp );

void printData(int8_t fd);

void tkRXComms_initoutofrtos(void);



#endif /* TASKS_SP5K2022_H_ */

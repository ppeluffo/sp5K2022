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

#define FW_REV "1.0.0"
#define FW_DATE "@ 20220802"
#define HW_MODELO "SP5K2022 FRTOS R001 HW:AVRMEGA1284P"
#define FRTOS_VERSION "FW:FreeRTOS V202111.00"

#define SYSMAINCLK 24

#define tkCtl_TASK_PRIORITY	 	( tskIDLE_PRIORITY + 1 )
#define tkCmd_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )
#define tkData_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )

#define tkCtl_STACK_SIZE		384
#define tkCmd_STACK_SIZE		384
#define tkData_STACK_SIZE		384

StaticTask_t tkCtl_Buffer_Ptr;
StackType_t tkCtl_Buffer [tkCtl_STACK_SIZE];

StaticTask_t tkCmd_Buffer_Ptr;
StackType_t tkCmd_Buffer [tkCmd_STACK_SIZE];

StaticTask_t tkData_Buffer_Ptr;
StackType_t tkData_Buffer [tkData_STACK_SIZE];

SemaphoreHandle_t sem_SYSVars;
StaticSemaphore_t SYSVARS_xMutexBuffer;
#define MSTOTAKESYSVARSSEMPH ((  TickType_t ) 10 )

TaskHandle_t xHandle_tkCtl, xHandle_tkCmd, xHandle_tkData;

bool starting_flag;

void tkCtl(void * pvParameters);
void tkCmd(void * pvParameters);
void tkData(void * pvParameters);

void reset(void);

#endif /* TASKS_SP5K2022_H_ */

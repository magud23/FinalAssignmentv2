/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: key.h
*
* PROJECT....: EMP
*
* DESCRIPTION: Test.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150321  MoH   Module created.
*
*****************************************************************************/

#ifndef _KEY_H
#define _KEY_H


/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "glob_def.h"

/*****************************    Defines    *******************************/

#define LSB4    0x0F
#define MSB4    0xF0
#define R1      0x01
#define R2      0x02
#define R3      0x04
#define R4      0x08
#define COL3    0x04
#define COL2    0x08
#define COL1    0x10

#define IDLE_S      0
#define HOLDING_S   1

/*****************************   Variables   *******************************/
QueueHandle_t key_q;



extern void key_task(void *pvParameters);

#endif

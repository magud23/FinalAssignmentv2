/*****************************************************************************
 * University of Southern Denmark
 * Embedded C Programming (ECP)
 *
 * MODULENAME.: button.h
 *
 * PROJECT....: ECP
 *
 * DESCRIPTION: Test.
 *
 * Change Log:
 ******************************************************************************
 * Date    Id    Change
 * YYMMDD
 * --------------------
 * 090215  MoH   Module created.
 *
 *****************************************************************************/

#ifndef BUTTON_H_
#define BUTTON_H_

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"


/*****************************    Defines    *******************************/
#define GE_NO_EVENT       0
#define BE_DOUBLE_PUSH    1
#define BE_LONG_PUSH      2
#define BE_SINGLE_PUSH    3

#define BS_IDLE           0
#define BS_FIRST_PUSH     1
#define BS_FIRST_RELEASE  2
#define BS_SECOND_PUSH    3
#define BS_LONG_PUSH      4

#define PERIOD_MS         5
#define TIM_2_SEC         2000/PERIOD_MS
#define TIM_100_MSEC      100/PERIOD_MS

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
QueueHandle_t button_q;

/*****************************   Functions   *******************************/

void button_task(void *pvParameters);
/*****************************************************************************
 *   Input    : -
 *   Output   : Button Event
 *   Function : Test function
 ******************************************************************************/

INT8U get_button_event(void);

/****************************** End Of Module *******************************/
#endif /*BUTTON_H_*/

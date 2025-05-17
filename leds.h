/*****************************************************************************
 * University of Southern Denmark
 * Embedded C Programming (ECP)
 *
 * MODULENAME.: status led.h
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
 * 100408  KHA    Module created.
 *
 *****************************************************************************/

#ifndef _LEDS_H
#define _LEDS_H

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "glob_def.h"
#include "emp_type.h"
#include "adcRTOS.h"
#include "leds.h"

/*****************************    Defines    *******************************/
#define LED_IDLE 0
#define LED_OPEN 1
#define LED_ACCELERATE 2
#define LED_DECELERATE 3
#define LED_CLOSED 4
#define LED_BROKEN 5


#define LED_TASK_PERIOD_MS 10
#define MOVE_BLINK_PERIOD_MS 200
#define BROKEN_BLINK_PERIOD_MS 100

#define RED_MASK 0x02
#define YELLOW_MASK 0x04
#define GREEN_MASK 0x08

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
QueueHandle_t led_q;
/*****************************   Functions   *******************************/


void set_led_mode(INT8U led_mode);
/*****************************************************************************
 *   Input    :  INT8U representing led_mod
 *   Output   :  -
 *   Function :  Put a LED_mode into buffer for led_task
 *****************************************************************************/

void leds_init(void);
/*****************************************************************************
 *   Input    :  -
 *   Output   :  -
 *   Function :  Init LEDs
 *****************************************************************************/

void leds_task(void *pvParameters);
/*****************************************************************************
 *   Input    :  -
 *   Output   :  -
 *   Function :  The task responsible for turning LEDS on or OFF
 *****************************************************************************/



/****************************** End Of Module *******************************/
#endif


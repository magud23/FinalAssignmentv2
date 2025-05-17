/*****************************************************************************
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME.: adc.h
 *
 * PROJECT....: EMP
 *
 * DESCRIPTION: Analog input from EMP-board pot-meter.
 *
 * Change Log:
 ******************************************************************************
 * Date    Id    Change
 * YYMMDD
 * --------------------
 * 250512  TF   Module created.
 *
 *****************************************************************************/

#ifndef _ENCODER_H
#define _ENCODER_H

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "emp_type.h"
#include "gpio.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

BaseType_t get_encoder_pos(INT8U * p_val);
/*****************************************************************
* Input: pointer in which to put the value returned from shared memory
* Output: success/fail of operation
* Function: gets position of encoder from shared memory
******************************************************************/

BaseType_t get_encoder_push(INT8U * p_val);
/*****************************************************************
* Input: pointer in which to put the value returned from buffer
* Output: success/fail of operation
* Function: gets push from buffer
******************************************************************/

void encoder_init(void);
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Initial function
 ******************************************************************************/

BOOLEAN encoder_a(void);
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Get channel A value function
 ******************************************************************************/

BOOLEAN encoder_b(void);
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Get channel B value function
 ******************************************************************************/

BOOLEAN digi_p2(void);
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Get pin value function
 ******************************************************************************/

void encoder_task(void *pvParameters);
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Task that puts encoder value and push button into buffer
 ******************************************************************************/


/****************************** End Of Module *******************************/
#endif


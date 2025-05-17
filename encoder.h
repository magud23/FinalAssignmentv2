/*****************************************************************************
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME.: adc.h
 *
 * PROJECT....: EMP
 *
 * DESCRIPTION: Input from EMP-board encoder.
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
/*************************** Header ******************************/
/*************************** Include files ***********************/
#include <stdint.h>

//FreeRTOS
#include "FreeRTOS.h"
#include "systick_frt.h"
#include "queue.h"

//Course
#include "tm4c123gh6pm.h"
#include "emp_type.h"

//Modules
#include "glob_def.h"

/*****************************    Defines    *******************************/
#define PA5 0x20
#define PA6 0x40
#define PA7 0x80
#define DELAY   1 // 1 ms
#define CHANGE_ON_A 0x02
#define CHANGE_ON_B 0x01

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

QueueHandle_t encoder_pos_q;
QueueHandle_t encoder_push_q;

/*****************************   Functions   *******************************/

BaseType_t get_encoder_pos(INT16S * p_val);
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


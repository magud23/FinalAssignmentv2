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
 * 150412  MoH   Module created.
 *
 *****************************************************************************/

#ifndef _ADC_H
#define _ADC_H
/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "glob_def.h"
#include "Task.h"
#include "queue.h"

/*****************************    Defines    *******************************/
#define MAX_POT_VAL 4095

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
QueueHandle_t adc_q;

/*****************************   Functions   *******************************/



BaseType_t get_pot(INT16U *p_adc_val);
/*****************************************************************************
*   Input    : Pointer to value to change to value from ADC
*   Output   : succes of operation
*   Function : Gets reading of ADC via shared memory
******************************************************************************/


BaseType_t set_pot(INT16U *p_adc_val);
/*****************************************************************************
*   Input    : Pointer to value to put to shared memory
*   Output   : succes of operation
*   Function : Sets reading of ADC via shared memory
******************************************************************************/


void init_adc();
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Test function
 ******************************************************************************/

void adc_task(void *pvParameters);
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Task that puts adc value into buffer
 ******************************************************************************/


/****************************** End Of Module *******************************/
#endif


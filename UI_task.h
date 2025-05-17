#ifndef UI_TASK_H_
#define UI_TASK_H_
/*************************** Header ******************************/
/*************************** Include files ***********************/
#include <stdint.h>

//FreeRTOS
#include "FreeRTOS.h"
#include "systick_frt.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

//Course
#include "tm4c123gh6pm.h"
#include "emp_type.h"

//Modules
#include "glob_def.h"
#include "adcRTOS.h"
#include "leds.h"
#include "lcd.h"
#include "encoder.h"
#include "UI_task.h"
#include "elevator.h"
#include "keyRTOS.h"
#include "password.h"
#include "button.h"

#include "encoder.h"
#include "lcd.h"
#include "elevator.h"
#include "password.h"
#include "glob_def.h"

/***************************     Defines     *******************************/
//STATES
#define UI_IDLE            0
#define UI_CURRENT_FLOOR   1
#define UI_POT_GOAL        2
#define UI_ENC_ERROR       3
#define UI_FLOOR_SELECT    4
#define UI_PASSWORD        5
#define UI_ENC             6

/*****************************   Variables   *******************************/
QueueHandle_t ui_mode_q;

/*****************************   Functions   *******************************/

BaseType_t get_ui_mode(INT8U *pv_ui_mode);
/*****************************************************************************
 *   Input    :  Pointer to variable in which to put ui mode from buffer
 *   Output   :  Success of operation?
 *   Function :  Put a ui_mode into buffer for ui_task
 *****************************************************************************/

BaseType_t set_ui_mode(INT8U led_mode);
/*****************************************************************************
 *   Input    :  Pointer to variable that is put into buffer
 *   Output   :  Success of operation?
 *   Function :  Put a ui_mode into buffer for ui_task
 *****************************************************************************/

BaseType_t set_ui_mode(INT8U ui_mode);
/*****************************************************************************
 *   Input    :  Pointer to variable in which to put ui mode from buffer
 *   Output   :  Success of operation?
 *   Function :  Put a ui_mode into buffer for ui_task
 *****************************************************************************/

void UI_task(void *pvParameters);
/*****************************************************************
* Input: -
* Output: -
* Function: The brains of the UI (FSM)
******************************************************************/


#endif /* UI_TASK_H_ */

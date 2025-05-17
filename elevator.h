#ifndef ELEVATOR_H_
#define ELEVATOR_H_
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

/*************************** Defines *****************************/
//Task execution period (delay)
#define ELEVATOR_TASK_PERIOD_MS 50

//Timing
#define ACCEL_TIME_PER_FLOOR_MS 500
#define DOORS_STAY_OPEN_FOR_MS  3000

//STATES
#define FLOOR2_S 0
#define WAIT_FOR_PASS_S 1
#define SELECT_FLOOR_S 2
#define ACCELERATE_S 3
#define DECELERATE_S 4
#define DOORS_OPEN_S 5
#define DOORS_CLOSED_S 6
#define BROKEN1_S 7
#define BROKEN2_S 8

//Utility
#define FULL_REVOLUTION 15
#define INVALID_FLOOR 99
#define REMOVED_FLOOR 13
#define TOP_FLOOR 20
#define USES_PER_BREAK 4
/*************************** Variables ***************************/
QueueHandle_t destination_floor_q;
QueueHandle_t current_floor_q;

/*************************** Function ****************************/

void elevator_task(void *pvParameters);
/*****************************************************************
* Input: -
* Output: -
* Function: The brains of the elevator (FSM)
******************************************************************/

INT8U floor_name2loc(INT8U name);
/*****************************************************************
 * Input: name of floor
 * Output: location of floor
 * Function: if name is 0-12 location is name,
 * name larger 14-20 location -1 of name
 ******************************************************************/

INT8U floor_loc2name(INT8U name);
/*****************************************************************
 * Input: location of floor
 * Output: name of floor
 * Function: if name is 0-12 location is name,
 * name larger 14-20 location -1 of name
 ******************************************************************/

INT16U current_floor_to_randomlike_reference(INT8U current_floor);

BaseType_t get_current_floor(INT8U * p_current_floor);
/*****************************************************************
* Input: pointer to variable in which to put return from queue
* Output: success/fail of operation
* Function: gets current floor from buffer
******************************************************************/

BaseType_t set_current_floor(INT8U * current_floor);
/*****************************************************************
* Input:  variable which to put to queue
* Output: success/fail of operation
* Function: sets current floor in buffer
******************************************************************/

BaseType_t get_destination_floor(INT8U * p_destination_floor);
/*****************************************************************
* Input: pointer in which to put the value returned from buffer
* Output: success/fail of operation
* Function: gets dest floor from buffer
******************************************************************/

BaseType_t set_destination_floor(INT8U * destination_floor);
/*****************************************************************
* Input: pointer to variable
* Output: success/fail of operation
* Function: sets destination floor in buffer
******************************************************************/


/*************************** End of module ***********************/
#endif /* ELEVATOR_H_ */

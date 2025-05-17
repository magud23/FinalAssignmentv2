#ifndef PASSWORD_H_
#define PASSWORD_H_
/*************************** Header ******************************/
/*************************** Include files ***********************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "glob_def.h"
#include "keyRTOS.h"
#include "lcd.h"
#include "button.h"

/*************************** Defines *****************************/
#define PASS_ACCEPTED 1
#define PASS_DECLINED 0

#define PASS_LENGTH 4
#define DIVISOR 8

/*************************** Constants ***************************/
/*************************** Variables ***************************/
QueueHandle_t pass_accept_q;
QueueHandle_t password_length_q;

extern QueueHandle_t key_q;
extern QueueHandle_t xQueue_lcd;
/*************************** Function ****************************/

extern void password_task(void *pvParameters);
/*****************************************************************
* Input:
* Output:
* Function:
******************************************************************/


INT8U get_pass_status();
/*****************************************************************
* Input:
* Output: current status of password
* Function: Looks at buffer pass_accept_q and returns its contents if any or PASS_DECLINED by default
******************************************************************/

BaseType_t get_typed_pass_length(INT8U * ptr_len);
/*****************************************************************
* Input: ptr to variable
* Output: succes?
* Function: updates variable to length stored in buffer
******************************************************************/



/*************************** End of module ***********************/
#endif /* PASSWORD_H_ */

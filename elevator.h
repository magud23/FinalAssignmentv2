#ifndef ELEVATOR_H_
#define ELEVATOR_H_
/*************************** Header ******************************/
/*************************** Include files ***********************/
/*************************** Defines *****************************/
/*************************** Variables ***************************/
/*************************** Function ****************************/

void elevator_task(void *pvParameters);
/*****************************************************************
* Input: -
* Output: -
* Function: The brains of the elevator (FSM)
******************************************************************/

INT8U floor_name2loc(INT8U name);
INT8U floor_loc2name(INT8U name);

INT16U current_floor_to_randomlike_reference(INT8U current_floor);

INT8U get_current_floor(INT8U * p_current_floor);
/*****************************************************************
* Input: pointer to variable in which to put return from queue
* Output: success/fail of operation
* Function: gets current floor from shared memory
******************************************************************/

void set_current_floor(INT8U current_floor);
/*****************************************************************
* Input:  variable which to put to queue
* Output: success/fail of operation
* Function: sets current floor in shared memory
******************************************************************/

BaseType_t get_destination_floor(INT8U * p_destination_floor);
/*****************************************************************
* Input: pointer in which to put the value returned from shared memory
* Output: success/fail of operation
* Function: gets dest floor from shared memory
******************************************************************/

BaseType_t set_destination_floor(INT8U * destination_floor);
/*****************************************************************
* Input: pointer to variable
* Output: success/fail of operation
* Function: sets destination floor in shared memory
******************************************************************/


/*************************** End of module ***********************/
#endif /* ELEVATOR_H_ */

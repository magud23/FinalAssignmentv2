#ifndef ELEVATOR_H_
#define ELEVATOR_H_
/*************************** Header ******************************/
/*************************** Include files ***********************/
/*************************** Defines *****************************/
/*************************** Variables ***************************/
/*************************** Function ****************************/

extern void elevator_task(void *pvParameters);
/*****************************************************************
* Input:
* Output:
* Function:
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

void set_destination_floor(INT8U destination_floor);
/*****************************************************************
* Input: variable which to put to queue
* Output: success/fail of operation
* Function: sets destination floor in shared memory
******************************************************************/


/*************************** End of module ***********************/
#endif /* ELEVATOR_H_ */

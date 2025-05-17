#ifndef PASSWORD_H_
#define PASSWORD_H_
/*************************** Header ******************************/
/*************************** Include files ***********************/
/*************************** Defines *****************************/
#define PASS_ACCEPTED 1
#define PASS_DECLINED 0

#define PASS_LENGTH 4
/*************************** Variables ***************************/
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

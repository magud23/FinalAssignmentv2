/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: button.h
*
* PROJECT....: ECP
*
* DESCRIPTION: Test.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 090215  MoH   Module created.
*
*****************************************************************************/

#ifndef BUTTON_H_
#define BUTTON_H_

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
#define GE_NO_EVENT       0
#define BE_DOUBLE_PUSH    1
#define BE_LONG_PUSH      2
#define BE_SINGLE_PUSH    3
/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

void button_task(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : Button Event
*   Function : Test function
******************************************************************************/


/****************************** End Of Module *******************************/
#endif /*BUTTON_H_*/

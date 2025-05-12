/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: button.c
*
* PROJECT....: EMP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 090215  MoH   Module created.
* 250512  MKG   Modified for freeRTOS
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "button.h"

/*****************************    Defines    *******************************/
#define BS_IDLE           0
#define BS_FIRST_PUSH     1
#define BS_FIRST_RELEASE  2
#define BS_SECOND_PUSH    3
#define BS_LONG_PUSH      4

#define PERIOD_MS         5
#define TIM_2_SEC         2000/PERIOD_MS
#define TIM_100_MSEC      100/PERIOD_MS

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
QueueHandle_t button_q;

/*****************************   Functions   *******************************/
INT8U button_pushed()
{
  return( !(GPIO_PORTF_DATA_R & 0x10) );  // SW1 at PF4
}


INT8U get_button_event()
{
    INT8U ch = GE_NO_EVENT;
    xQueueReceive(button_q, &ch, 0);
    return ch;
}


void button_task(void *pvParameters)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  INT8U  button_state = BS_IDLE;
  INT16U button_timer;
  INT8U  button_event = GE_NO_EVENT;

  while(1)
  {
      vTaskDelay( PERIOD / portTICK_RATE_MS);

      switch( button_state )
      {
          case BS_IDLE:
              if( button_pushed() )                   // if button pushed
              {
                  button_state = BS_FIRST_PUSH;
                  button_timer = TIM_2_SEC;           // start timer = 2 sec;
              }
              break;
          case BS_FIRST_PUSH:
              if( ! --button_timer )                  // if timeout
              {
                  button_state = BS_LONG_PUSH;
                  button_event = BE_LONG_PUSH;
                  xQueueOverwrite(button_q, &button_event);
              }
              else
              {
              if( !button_pushed() )                  // if button released
              {
                  button_state = BS_FIRST_RELEASE;
                  button_timer = TIM_100_MSEC;        // start timer = 100 milli sec;
              }
              }
              break;
          case BS_FIRST_RELEASE:
              if( ! --button_timer )                  // if timeout
              {
                  button_state = BS_IDLE;
                  button_event = BE_SINGLE_PUSH;
                  xQueueOverwrite(button_q, &button_event);
              }
              else
              {
              if( button_pushed() )                   // if button pressed
              {
                   button_state = BS_SECOND_PUSH;
                   button_timer = TIM_2_SEC;          // start timer = 2 sec;
              }
              }
              break;
          case BS_SECOND_PUSH:
              if( ! --button_timer )                  // if timeout
              {
                  button_state = BS_LONG_PUSH;
                  button_event = BE_LONG_PUSH;
                  xQueueOverwrite(button_q, &button_event);
              }
              else
              {
              if( !button_pushed() )                  // if button released
              {
                    button_state = BS_IDLE;
                    button_event = BE_DOUBLE_PUSH;
                    xQueueOverwrite(button_q, &button_event);
              }
              }
              break;
          case BS_LONG_PUSH:
              if( !button_pushed() )                  // if button released
                  button_state = BS_IDLE;
              break;
          default:
              break;
      }
  }
}

/****************************** End Of Module *******************************/













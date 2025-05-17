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
#include "button.h"

/*****************************   Functions   *******************************/
INT8U button_pushed()
{
  return( !(GPIO_PORTF_DATA_R & 0x10) );  // SW1 at PF4
}


INT8U get_button_event()
{
    INT8U ch = BS_IDLE;
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
      vTaskDelay( PERIOD_MS / portTICK_RATE_MS);

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













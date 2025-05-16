/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: status led.h
*
* PROJECT....: EMP
*
* DESCRIPTION: Test.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 100408  KHA    Module created.
*
*****************************************************************************/

#ifndef _LEDS_H
  #define _LEDS_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
#define LED_IDLE 0
#define LED_OPEN 1
#define LED_ACCELERATE 2
#define LED_DECELERATE 3
#define LED_CLOSED 4
#define LED_BROKEN 5
/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/


void set_led_mode(INT8U led_mode);
/*****************************************************************************
*   Input    :  INT8U representing led_mod
*   Output   :  -
*   Function :  Put a LED_mode into buffer for led_task
*****************************************************************************/

void leds_init(void);
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :  Init LEDs
*****************************************************************************/

void leds_task(void *pvParameters);
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :  The task responsible for turning LEDS on or OFF
*****************************************************************************/



/****************************** End Of Module *******************************/
#endif


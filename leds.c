/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: leds.c
*
* PROJECT....: ECP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "emp_type.h"
#include "adcRTOS.h"
#include "leds.h"

/*****************************    Defines    *******************************/
#define LED_TASK_PERIOD_MS 10
#define MOVE_BLINK_PERIOD_MS 200
#define BROKEN_BLINK_PERIOD_MS 100

#define RED_MASK 0x02
#define YELLOW_MASK 0x04
#define GREEN_MASK 0x08


/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
QueueHandle_t led_q;
/*****************************   Functions   *******************************/

void set_led_mode(INT8U led_mode)
/*****************************************************************************
*   Input    :  INT8U representing led mode
*   Output   :  -
*   Function :  Put a LED_mode into buffer for led_task
*****************************************************************************/
{
xQueueOverwrite(led_q, &led_mode);
}

void leds_init(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function : Init LEDs
*****************************************************************************/
{
  INT8S dummy;
  // Enable the GPIO port that is used for the on-board LED.
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;

  // Do a dummy read to insert a few cycles after enabling the peripheral.
  dummy = SYSCTL_RCGC2_R;

  GPIO_PORTF_DIR_R |= RED_MASK | YELLOW_MASK | GREEN_MASK;
  GPIO_PORTF_DEN_R |= RED_MASK | YELLOW_MASK | GREEN_MASK;

  set_led_mode(LED_IDLE);
}


void leds_task(void *pvParameters)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :  The task responsible for turning LEDS on or OFF
*****************************************************************************/
{
  leds_init();
  INT8U led_mode = LED_IDLE;
  INT16U blink_timer = 0;

  while(1)
  {
    vTaskDelay( LED_TASK_PERIOD_MS / portTICK_RATE_MS); // wait 100-1000 ms. (200-1000)
    if(xQueueReceive(led_q, &led_mode, 0))
    {
        //if there is anything in the buffer, reset the blink_timer and shut off a
        blink_timer = 0;
        GPIO_PORTF_DATA_R |= (RED_MASK|YELLOW_MASK|GREEN_MASK); //shut off all
    }

    switch(led_mode)
    {
    case LED_IDLE:
        break;
    case LED_OPEN:
        GPIO_PORTF_DATA_R &= ~GREEN_MASK;
        break;
    case LED_ACCELERATE:
        if(blink_timer++ == MOVE_BLINK_PERIOD_MS/LED_TASK_PERIOD_MS)
        {
            blink_timer = 0;
            GPIO_PORTF_DATA_R ^= YELLOW_MASK;
        }
        break;
    case LED_DECELERATE:
        if(blink_timer++ == MOVE_BLINK_PERIOD_MS/LED_TASK_PERIOD_MS)
        {
            blink_timer = 0;
            GPIO_PORTF_DATA_R ^= RED_MASK;
        }
        break;
    case LED_BROKEN:
        if(blink_timer++ == BROKEN_BLINK_PERIOD_MS/LED_TASK_PERIOD_MS)
        {
            blink_timer = 0;
            GPIO_PORTF_DATA_R ^= (RED_MASK|YELLOW_MASK|GREEN_MASK);
        }
        break;
    }



  }
}




/****************************** End Of Module *******************************/





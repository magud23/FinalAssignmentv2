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


/*****************************    Defines    *******************************/
#define RED_MASK 0x02
#define YELLOW_MASK 0x04
#define GREEN_MASK 0x08

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
extern QueueHandle_t adc_q;

/*****************************   Functions   *******************************/

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

}


void leds_task(void *pvParameters)
{
  INT16U adc_value;
  portTickType delay;

  leds_init();

  while(1)
  {
    // Toggle red led

    GPIO_PORTF_DATA_R ^= RED_MASK;
    xQueueReceive(adc_q, &adc_value, 0);
    delay = 1000 - adc_value / 5;
    vTaskDelay( delay / portTICK_RATE_MS); // wait 100-1000 ms. (200-1000)
  }
}




/****************************** End Of Module *******************************/





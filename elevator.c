/*************************** Header ******************************/
/*************************** Include files ***********************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "lcd.h"
#include "glob_def.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/*************************** Defines *****************************/
/*************************** Constants ***************************/
/*************************** Variables ***************************/
extern QueueHandle_t xQueue_lcd;
/*************************** Function ****************************/

extern void elevator_task(void *pvParameters)
/*****************************************************************
* Input:
* Output:
* Function:
******************************************************************/
{
    INT8U ch;
    while(1)
    {
        vTaskDelay( 1000 / portTICK_RATE_MS); // wait 1000 ms
        ch = 't';
        xQueueSendToBack(xQueue_lcd, &ch, 0);
    }
}


/*************************** End of module ***********************/

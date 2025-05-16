/*************************** Header ******************************/
/*************************** Include files ***********************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "glob_def.h"
#include "keyRTOS.h"
#include "lcd.h"
#include "button.h"
#include "password.h"
/*************************** Defines *****************************/
#define LENGTH 4
#define DIVISOR 8
#define RESET 0

/*************************** Constants ***************************/
/*************************** Variables ***************************/
QueueHandle_t pass_accept_q;
QueueHandle_t password_q;

extern QueueHandle_t key_q;
extern QueueHandle_t xQueue_lcd;
/*************************** Function ****************************/

INT8U get_pass_status()
{
    INT8U ch = PASS_DECLINED;
    xQueueReceive(pass_accept_q, &ch, 0);
    return ch;
}


extern void password_task(void *pvParameters)
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function :
 ******************************************************************************/
{
    INT16U password = RESET;
    INT8U length = RESET;
    INT8U keypress;
    INT8U status;
    while(1)
    {
        if (xQueueReceive(key_q, &keypress, portMAX_DELAY) == pdTRUE)
        {
            if(keypress == '*')
            {
                password = RESET;
                length = RESET;
                xQueueOverwrite(password_q, &password);
            }
            else if(keypress == '#')
            {
                if(password % DIVISOR == 0 && password != 0 && length == LENGTH)
                {
                    // send confirmation to elevator
                    status = PASS_ACCEPTED;
                    xQueueOverwrite(pass_accept_q, &status);
                }
                password = RESET;
                length = RESET;
                xQueueOverwrite(password_q, &password);
            }
            else
            {
                password = password*(INT16U)10; // move decimal left
                password += (INT16U)keypress - '0';
                length++;
                xQueueOverwrite(password_q, &password);
            }
        }
    }
}


/*************************** End of module ***********************/

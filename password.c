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
/*************************** Defines *****************************/
#define LENGTH 4
#define DIVISOR 8

#define PASS_ACCEPTED 1
#define PASS_DECLINED 0
/*************************** Constants ***************************/
/*************************** Variables ***************************/
QueueHandle_t pass_accept_q;

extern QueueHandle_t key_q;
extern QueueHandle_t xQueue_lcd;
/*************************** Function ****************************/

INT8U get_pass_status()
{
    INT8U ch = PASS_DECLINED;
    if xQueueReceive(pass_accept_q, &ch, 0);
    return ch;
}


extern void password_task(void *pvParameters)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
    INT8U password = 0;
    INT8U length = 0;
    INT8U keypress;
    INT8U ch;
    INT8U status;
    while(1)
    {
        xQueueReceive(key_q, &keypress, portMAX_DELAY);
        ch = 'x';
        xQueueSendToBack(xQueue_lcd, &ch, 0);

        if(keypress == '*')
        {

            ch = 0xFF;
            xQueueSendToBack(xQueue_lcd, &ch, 0);
            password = 0;
            length = 0;
        }
        else if(keypress == '#')
        {
            if(password % DIVISOR == 0 && password != 0 && length == LENGTH)
            {
                // send confirmation to elevator
                status = PASS_ACCEPTED;
                xQueueOverwrite(pass_accept_q, &status);

                ch = 0xFF; //clear
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = 'Y';
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = 'E';
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = 'S';
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = '!';
                xQueueSendToBack(xQueue_lcd, &ch, 0);

            }
            else
            {
                // send confirmation to elevator
                status = PASS_DECLINED;
                xQueueOverwrite(pass_accept_q, &status);

                ch = 0xFF; //clear
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = 'N';
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = 'O';
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = '.';
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = '.';
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = '.';
                xQueueSendToBack(xQueue_lcd, &ch, 0);
            }

            password = 0;
        }
        else
        {
            password = password << 1;
            password += keypress - '0';
            length++;
        }

    }
}


/*************************** End of module ***********************/

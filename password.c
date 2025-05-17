/*************************** Header ******************************/
#include "password.h"
/*************************** Function ****************************/

INT8U get_pass_status()
{
    INT8U ch = PASS_DECLINED;
    xQueueReceive(pass_accept_q, &ch, 0);
    return ch;
}

BaseType_t get_typed_pass_length(INT8U * ptr_len)
{
    return xQueueReceive(password_length_q, ptr_len, portMAX_DELAY);
}



void password_task(void *pvParameters)
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
                xQueueOverwrite(password_length_q, &length);
            }
            else if(keypress == '#')
            {
                if(password % DIVISOR == 0 && password != 0 && length == PASS_LENGTH)
                {
                    // send confirmation to elevator
                    status = PASS_ACCEPTED;
                    xQueueOverwrite(pass_accept_q, &status);
                }
                password = RESET;
                length = RESET;
                xQueueOverwrite(password_length_q, &length);
            }
            else
            {
                password = password*(INT16U)10; // move decimal left
                password += (INT16U)keypress - '0';
                length++;
                xQueueOverwrite(password_length_q, &length);
            }
        }
    }
}


/*************************** End of module ***********************/

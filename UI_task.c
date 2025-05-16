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
#include "emp_type.h"

#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "UI_task.h"
#include "encoder.h"
#include "lcd.h"
#include "elevator.h"


/*****************************    Defines    *******************************/
#define PF0     0       // Bit 0
#define setup_state   0
#define scale_state   1
#define offset_state  2



/*****************************   Constants   *******************************/

/*INT16U min_in 0;
INT16U max_in 4095;
INT16U min_out 100;
INT16U max_out 1000; */

/*****************************   Variables   *******************************/
QueueHandle_t ui_mode_q;
extern QueueHandle_t encoder_pos_q, xQueue_lcd, destination_floor_q;
/*****************************   Functions   *******************************/


void set_ui_mode(INT8U led_mode)
/*****************************************************************************
*   Input    :  INT8U representing led mode
*   Output   :  -
*   Function :  Put a ui_mode into buffer for ui_task
*****************************************************************************/
{
    xQueueOverwrite(ui_mode_q, &led_mode);
}


void LCD_print_char(INT8U ch)
{
    xQueueSend( xQueue_lcd, &ch, portMAX_DELAY);
}


char change_int_to_char1(INT8U number)
{
    return '0' + number;
}



void encoder_mode()
{
    INT16S encoder_value;
    char input;


    xQueueReceive(encoder_pos_q, &encoder_value, portMAX_DELAY);
    int cv_mod0 = encoder_value % 10;
    int cv_mod10 = encoder_value/10 % 10;
    int cv_mod100 = encoder_value/100 % 10;
    int cv_mod1000 = encoder_value/1000 % 10;
    input = change_int_to_char1(cv_mod1000);
    xQueueSend( xQueue_lcd, &input, portMAX_DELAY);
    input = change_int_to_char1(cv_mod100);
    xQueueSend( xQueue_lcd, &input, portMAX_DELAY);
    input = change_int_to_char1(cv_mod10);
    xQueueSend( xQueue_lcd, &input, portMAX_DELAY);
    input = change_int_to_char1(cv_mod0);
    xQueueSend( xQueue_lcd, &input, portMAX_DELAY);
    home_LCD();
}



void current_floor_mode()
{
    static INT8U previous_floor = 0;
    INT8U current_floor = previous_floor;

    get_current_floor(&current_floor);

    if(previous_floor != current_floor)
    {
        previous_floor = current_floor;

        LCD_print_char(0xff); //clear and home display
        LCD_print_char(change_int_to_char1(current_floor/10 % 10));
        LCD_print_char(change_int_to_char1(current_floor % 10));
    }
}


void floor_select_mode()
{

    LCD_print_char(0xff); //clear and home display

    LCD_print_char('S');
    LCD_print_char('E');
    LCD_print_char('L');
    LCD_print_char(' ');
    LCD_print_char('F');
    LCD_print_char('L');
    LCD_print_char('O');
    LCD_print_char('O');
    LCD_print_char('R');
    LCD_print_char(':');

    INT8U dest_floor = 0;
    get_current_floor(&dest_floor);
    xQueuePeek(destination_floor_q, &dest_floor,0);
    LCD_print_char(change_int_to_char1(dest_floor/10 % 10));
    LCD_print_char(change_int_to_char1(dest_floor % 10));


}

void password_mode()
{

    LCD_print_char(0xff); //clear and home display
    LCD_print_char('P');
    LCD_print_char('I');
    LCD_print_char('N');
    LCD_print_char(':');

}


void UI_task(void *pvParameters)
{

    INT8U ui_mode = UI_IDLE;

    while(1)
    {
        vTaskDelay(200 / portTICK_RATE_MS); // wait 20 ms.
        if(xQueueReceive(ui_mode_q, &ui_mode, 0))
        {
        }

        switch(ui_mode)
        {
        case UI_IDLE:
            break;
        case UI_CURRENT_FLOOR:
            current_floor_mode();
            break;
        case UI_POT_GOAL:
            break;
        case UI_ENC_ERROR :
            break;
        case UI_FLOOR_SELECT:
            floor_select_mode();
            break;
        case UI_PASSWORD:
            password_mode();
            break;
        }
    }
}






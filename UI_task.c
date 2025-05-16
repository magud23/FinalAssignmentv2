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

extern QueueHandle_t encoder_pos_q, xQueue_lcd;
/*****************************   Functions   *******************************/

char change_int_to_char1(INT8U number){
    switch(number){
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
    }
    return '0';
}

void UI_task(void *pvParameters){
    INT16S encoder_value;

    char input0;
    char input1;
    char input2;
    char input3;
    while(1){
        xQueuePeek(encoder_pos_q, &encoder_value, portMAX_DELAY);

        int cv_mod0 = encoder_value % 10;
        int cv_mod10 = encoder_value/10 % 10;
        int cv_mod100 = encoder_value/100 % 10;
        int cv_mod1000 = encoder_value/1000 % 10;

        input0 = change_int_to_char1(cv_mod1000);
        xQueueSend( xQueue_lcd, &input0, portMAX_DELAY);
        input1 = change_int_to_char1(cv_mod100);
        xQueueSend( xQueue_lcd, &input1, portMAX_DELAY);
        input2 = change_int_to_char1(cv_mod10);
        xQueueSend( xQueue_lcd, &input2, portMAX_DELAY);
        input3 = change_int_to_char1(cv_mod0);
        xQueueSend( xQueue_lcd, &input3, portMAX_DELAY);
        home_LCD();
        }
    }


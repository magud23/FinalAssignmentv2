/*****************************************************************************
 * University of Southern Denmark
 * Embedded C Programming (ECP)
 *
 * MODULENAME.: encoder.c
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
 * 250512  TF    Module created.
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
#include "gpio.h"
#include "encoder.h"

/*****************************    Defines    *******************************/
#define PA5 0x20
#define PA6 0x40
#define PA7 0x80

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

QueueHandle_t encoder_pos_q;
QueueHandle_t encoder_push_q;

/*****************************   Functions   *******************************/

void encoder_init(void)
{   // todo write proper init function
    init_gpio();
}

BOOLEAN digi_p2(void)
{
    return (GPIO_PORTA_DATA_R && PA5); //logical operation to get boolean
}

BOOLEAN encoder_a(void) //encoder channel A
{
    return (GPIO_PORTA_DATA_R && PA6); //logical operation to get boolean
}

BOOLEAN encoder_b(void) //encoder channel B
{
    return (GPIO_PORTA_DATA_R && PA7); //logical operation to get boolean
}

void encoder_task(void *pvParameters)
{
    INT16S encoder_value = 0;
    BOOLEAN encoder_push;
    BOOLEAN rising_edge = 1;
    portTickType delay;

    encoder_init();

    while(1)
    { //do stuff
        //get push button value
        encoder_push = digi_p2();

        if(encoder_a())
        {
            if(rising_edge) // check if rising edge
            {
                if(encoder_b()) // check rotation direction
                    encoder_value++;
                else
                    encoder_value--;
            }
            rising_edge = 0; // no longer rising edge
        }else{
            rising_edge = 1; // reset rising edge
        }

        //send stuff
        xQueueOverwrite(encoder_pos_q, &encoder_value);
        xQueueOverwrite(encoder_push_q, &encoder_push);
        delay = 1;
        vTaskDelay( delay / portTICK_RATE_MS); // wait 1 ms
    }
}

/****************************** End Of Module *******************************/

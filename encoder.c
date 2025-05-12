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
#include "queue.h"
#include "emp_type.h"
#include "encoder.h"

/*****************************    Defines    *******************************/
#define PA5 0x20
#define PA6 0x40
#define PA7 0x80
#define RESET 0
#define CHANGE_ON_A 0x02
#define CHANGE_ON_B 0x01

/*****************************   Constants   *******************************/


/*****************************   Variables   *******************************/

QueueHandle_t encoder_pos_q;
QueueHandle_t encoder_push_q;

/*****************************   Functions   *******************************/

void encoder_init(void)
{
    //enable clock for GPIO Port A
    SYSCTL_RCGCGPIO_R |= 0x01;              //set bit 0 to enable Port A
    while ((SYSCTL_PRGPIO_R & 0x01) == 0);  //wait for Port A to be ready

    //set PA5, PA6, PA7 as inputs
    GPIO_PORTA_DIR_R &= ~(PA5 | PA6 | PA7); //clear bits 5, 6, 7

    //enable digital function on PA5, PA6, PA7
    GPIO_PORTA_DEN_R |= (PA5 | PA6 | PA7);  //set bits 5, 6, 7
}

BOOLEAN encoder_a(void) //encoder channel A
{
    return (GPIO_PORTA_DATA_R & PA5) != 0; //logical operation to get boolean of a push
}

BOOLEAN encoder_b(void) //encoder channel B
{
    return (GPIO_PORTA_DATA_R & PA6) != 0; //logical operation to get boolean of a push
}

BOOLEAN digi_p2(void) //encoder push button
{
    return (GPIO_PORTA_DATA_R & PA7) == 0; //logical operation to get boolean of a push
}

void encoder_task(void *pvParameters)
{
    INT16S encoder_value = RESET;
    BOOLEAN encoder_push;
    INT8U current_AB = RESET, prev_AB = RESET, YY = RESET;
    portTickType delay = 1; // 1 ms

    encoder_init();

    while(1)
    { //do stuff
        //get push button value
        encoder_push = digi_p2();

        //update AB
        current_AB = (encoder_a() << 1) | encoder_b();

        //algorithm from datasheet
        if (current_AB == prev_AB)
        {
            //no change
        }
        else
        {
            //check for what change
            YY = current_AB ^ prev_AB;

            if (encoder_a() == encoder_b()) //check for position step
            {
                if (YY == CHANGE_ON_B)
                { //cw
                    encoder_value++;
                }
                else if (YY == CHANGE_ON_A)
                { //ccw
                    encoder_value--;
                }
            }
        }

        //save AB
        prev_AB = current_AB;

        //send stuff
        xQueueOverwrite(encoder_pos_q, &encoder_value);
        xQueueOverwrite(encoder_push_q, &encoder_push);
        vTaskDelay( delay / portTICK_RATE_MS); // wait 1 ms
    }
}
/****************************** End Of Module *******************************/

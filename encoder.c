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

#include "encoder.h"

/*****************************   Functions   *******************************/

BaseType_t get_encoder_pos(INT16S * p_val)
/*****************************************************************
 * Input: pointer in which to put the value returned from shared memory
 * Output: success/fail of operation
 * Function: gets position of encoder from shared memory
 ******************************************************************/
{
    return xQueueReceive(encoder_pos_q, p_val,0);
}

BaseType_t get_encoder_push(INT8U * p_val)
/*****************************************************************
 * Input: pointer in which to put the value returned from buffer
 * Output: success/fail of operation
 * Function: gets push from buffer
 ******************************************************************/
{
    return xQueueReceive(encoder_push_q, p_val,0);
}


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
        vTaskDelay( DELAY / portTICK_RATE_MS); // wait 1 ms
    }
}
/****************************** End Of Module *******************************/

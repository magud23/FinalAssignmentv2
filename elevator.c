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
#include "button.h"

/*************************** Defines *****************************/
//STATES
#define FLOOR2_S 0
#define WAIT_FOR_PASS_S 1
#define SELECT_FLOOR_S 2
#define ACCELERATE_S 3
#define DECELERATE_S 4
#define DOORS_OPEN_S 5
#define DOORS_CLOSED_S 6
#define BROKEN1_S 7
#define BROKEN2_S 8

//util
#define INVALID_FLOOR 99
#define REMOVED_FLOOR 13
#define TOP_FLOOR 20


/*************************** Constants ***************************/
/*************************** Variables ***************************/
extern QueueHandle_t xQueue_lcd;
/*************************** Function ****************************/

INT8U floor_name2loc(INT8U name)
/*****************************************************************
* Input: name of floor
* Output: location of floor
* Function: if name is 0-12 location is name,
* name larger 14-20 location -1 of name
******************************************************************/
{
    INT8U location = INVALID_FLOOR;
    if(name > REMOVED_FLOOR)
        location = name;
    else if (name > REMOVED_FLOOR && name <= TOP_FLOOR)
        location = name-1;

    return location;
}

INT8U floor_loc2name(INT8U location)
/*****************************************************************
* Input: location of floor
* Output: name of floor
* Function: if name is 0-12 location is name,
* name larger 14-20 location -1 of name
******************************************************************/
{
    INT8U name = INVALID_FLOOR;
    if(location < REMOVED_FLOOR)
        name = location;
    else if(location >= REMOVED_FLOOR && name < TOP_FLOOR)
        name = location + 1;

    return name;
}



extern void elevator_task(void *pvParameters)
/*****************************************************************
* Input:
* Output:
* Function:
******************************************************************/
{
    INT8U state = FLOOR2_S;
    INT8U current_floor = floor_name2loc(2);
    INT8U destination_floor = floor_name2loc(0);
    INT8U travelling_dist;
    INT8U use_counter = 0;
    while(1)
    {
        vTaskDelay( 100 / portTICK_RATE_MS); // wait 100 ms

        /*------STATE MACHINE ------*/
        switch(state)
        {
        case FLOOR2_S:
            if(get_button_evet() == BE_LONG_PUSH)
            {
                travelling_dist = destination_floor - current_floor;
                //TODO: set LEDs to accelerate mode
                ch = 0xFF; //clear
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = 'O';
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = 'M';
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = 'W';
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                ch = '!';
                xQueueSendToBack(xQueue_lcd, &ch, 0);
                state = ACCELERATE_S;
            }
            break;
        case WAIT_FOR_PASS_S:
            break;
        case SELECT_FLOOR_S:
            break;
        case ACCELERATE_S:
            if(1) //TODO:correct time has passed based on travelling dist
            {
                //TODO: set LEDs to decelerate mode
                state = DECELERATE_S;
            }
            break;
        case DECELERATE_S:
            if(1) //TODO: correct time has passed based on travelling dist
            {
                //TODO: set LEDs to open mode
                state = DOORS_OPEN_S;
            }
            break;
        case DOORS_OPEN_S:
            if(1) //TODO: first time on doors open
            {
                state = WAIT_FOR_PASS_S;
            }else if(2) //TODO: Wait some time
            {
                state = DOORS_CLOSED_S;
            }
            break;
        case DOORS_CLOSED_S:
            if(1) //TODO: wait for sw1 long press
            {
                //TODO: SET LEDS TO OPEN DOORS

                use_counter = (use_counter + 1) % 4;
                if(use_counter == 4) //breaks on 4th use
                {
                    state = BROKEN1_S;
                }
                else
                {
                    state = WAIT_FOR_PASS_S;
                }
            }
            break;
        case BROKEN1_S:
            if(1) //TODO: POT is MATCH
            {
                //TODO: stop blinking
                //TODO: switch encoder direction
                state = BROKEN2_S;
            }

            break;
        case BROKEN2_S:
            if(1) //TODO: Encoder turn + press
            {
                state = WAIT_FOR_PASS_S;
            }
            break;
        }






    }
}


/*************************** End of module ***********************/

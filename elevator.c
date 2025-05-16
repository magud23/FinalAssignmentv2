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
#include "leds.h"
#include "password.h"
#include "UI_task.h"

/*************************** Defines *****************************/
//Task execution period (delay)
#define ELEVATOR_TASK_PERIOD_MS 50

//Timing
#define ACCEL_TIME_PER_FLOOR_MS 1000
#define DOORS_STAY_OPEN_FOR_MS  5000

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
QueueHandle_t current_floor_q;
/*************************** Function ****************************/

INT8U get_current_floor(INT8U * p_current_floor)
/*****************************************************************
* Input: pointer to variable in which to put return from queue
* Output: success/fail of operation
* Function: gets current floor from shared memory
******************************************************************/
{
    return xQueuePeek(current_floor_q, p_current_floor,0);
}

INT8U set_current_floor(INT8U * p_current_floor)
/*****************************************************************
* Input: pointer to variable in which to put to queue
* Output: success/fail of operation
* Function: sets current floor in shared memory
******************************************************************/
{
    return xQueueOverwrite(current_floor_q, p_current_floor);
}


INT8U floor_name2loc(INT8U name)
/*****************************************************************
* Input: name of floor
* Output: location of floor
* Function: if name is 0-12 location is name,
* name larger 14-20 location -1 of name
******************************************************************/
{
    INT8U location = INVALID_FLOOR;
    if(name < REMOVED_FLOOR)
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
    INT8S travelling_dist = 0;
    INT8U use_counter = 0;
    INT16U elevator_timer = 0;
    BOOLEAN first_journey = TRUE;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1)
    {

        set_current_floor(&current_floor); //update shared memory

        vTaskDelayUntil(&xLastWakeTime, ELEVATOR_TASK_PERIOD_MS / portTICK_RATE_MS);
        /*------STATE MACHINE ------*/
        switch(state)
        {
        case FLOOR2_S:
            if(get_button_event() == BE_LONG_PUSH)
            {
                travelling_dist = (INT8S)destination_floor - (INT8S)current_floor;
                set_led_mode(LED_ACCELERATE);
                set_ui_mode(UI_CURRENT_FLOOR);
                state = ACCELERATE_S;
            }
            break;
        case WAIT_FOR_PASS_S:

            if(get_pass_status() == PASS_ACCEPTED)
            {
                set_ui_mode(UI_FLOOR_SELECT);
                state = SELECT_FLOOR_S;
            }

            break;
        case SELECT_FLOOR_S:

            if(1) //TODO: wait for encoderpress
            {
                // TODO: Assign next destination_floor
                destination_floor = 20; /*TODO: remove*********************************************************************************************************/
                travelling_dist = (INT8S)destination_floor - (INT8S)current_floor;
                set_led_mode(LED_ACCELERATE);
                state = ACCELERATE_S;
            }

            break;
        case ACCELERATE_S:
        {
            if(elevator_timer++ == abs(travelling_dist) * ACCEL_TIME_PER_FLOOR_MS/ELEVATOR_TASK_PERIOD_MS)
            {
                set_led_mode(LED_DECELERATE);
                elevator_timer = 0;
                state = DECELERATE_S;
            }
        }
            break;
        case DECELERATE_S:
            if(elevator_timer++ == abs(travelling_dist) * ACCEL_TIME_PER_FLOOR_MS/ELEVATOR_TASK_PERIOD_MS ) // correct time has passed based on travelling dist
            {
                set_led_mode(LED_OPEN);
                elevator_timer = 0;
                state = DOORS_OPEN_S;
            }
            break;
        case DOORS_OPEN_S:

            current_floor = destination_floor;

            if(first_journey)   // first time on doors open
            {
                first_journey = FALSE;
                state = WAIT_FOR_PASS_S;
            }else if(elevator_timer++ ==  DOORS_STAY_OPEN_FOR_MS/ELEVATOR_TASK_PERIOD_MS )
            {
                set_led_mode(LED_IDLE);
                elevator_timer = 0;
                state = DOORS_CLOSED_S;
            }
            break;
        case DOORS_CLOSED_S:
            if(get_button_event() == BE_LONG_PUSH) //wait for sw1 long press
            {
                set_led_mode(LED_IDLE);
                use_counter = (use_counter + 1) % 4;
                if(use_counter == 4) //breaks on 4th use
                {
                    set_led_mode(LED_BROKEN);
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
                set_led_mode(LED_OPEN); //stop blinking
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

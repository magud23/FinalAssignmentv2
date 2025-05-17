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
#include "adcRTOS.h"

/*************************** Defines *****************************/
//Task execution period (delay)
#define ELEVATOR_TASK_PERIOD_MS 50

//Timing
#define ACCEL_TIME_PER_FLOOR_MS 500
#define DOORS_STAY_OPEN_FOR_MS  3000

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

//Utility
#define RESET 0
#define FULL_REVOLUTION 15
#define INVALID_FLOOR 99
#define REMOVED_FLOOR 13
#define TOP_FLOOR 20

/*************************** Constants ***************************/
/*************************** Variables ***************************/
extern QueueHandle_t xQueue_lcd;
extern QueueHandle_t encoder_pos_q;
extern QueueHandle_t encoder_push_q;

QueueHandle_t destination_floor_q;
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

void set_current_floor(INT8U current_floor)
/*****************************************************************
* Input:  variable which to put to queue
* Output: success/fail of operation
* Function: sets current floor in shared memory
******************************************************************/
{
    xQueueOverwrite(current_floor_q, &current_floor);
}

void set_destination_floor(INT8U destination_floor)
/*****************************************************************
* Input: variable which to put to queue
* Output: success/fail of operation
* Function: sets destination floor in shared memory
******************************************************************/
{
    xQueueOverwrite(destination_floor_q, &destination_floor);
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

INT8U floor_loc2name(INT8U location) //TODO: move to UI task
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
    else if(location >= REMOVED_FLOOR && location < TOP_FLOOR)
        name = location + 1;

    return name;
}


INT16U current_floor_to_randomlike_reference(INT8U current_floor)
{
    return ((((INT16U)current_floor+7)%TOP_FLOOR)* (MAX_POT_VAL*3)/(2*TOP_FLOOR+5) + 44) % MAX_POT_VAL; //these numbers are actually magic and have no inherent meaning
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
    INT8S travel_dist = 0;
    INT8U use_counter = RESET;
    INT16U elevator_timer = RESET;
    BOOLEAN encoder_push = RESET;
    INT16S encoder_val, prev_encoder_val, dest_encoder_val;
    BOOLEAN first_journey = TRUE;
    BOOLEAN encoder_dir_is_clockwise = TRUE;


    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1)
    {

        set_current_floor(floor_loc2name(current_floor)); //update shared memory

        vTaskDelayUntil(&xLastWakeTime, ELEVATOR_TASK_PERIOD_MS / portTICK_RATE_MS);
        /*------STATE MACHINE ------*/
        switch(state)
        {
        case FLOOR2_S:
            if(get_button_event() == BE_LONG_PUSH)
            {
                travel_dist = (INT8S)destination_floor - (INT8S)current_floor;
                use_counter = (use_counter + 1) % 4;
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
                xQueueReceive(encoder_pos_q, &encoder_val, portMAX_DELAY);
                prev_encoder_val = encoder_val;
            }

            break;

        case SELECT_FLOOR_S:

            xQueueReceive(encoder_pos_q, &encoder_val, portMAX_DELAY);
            xQueueReceive(encoder_push_q, &encoder_push, portMAX_DELAY);

            if(encoder_push) // select the floor
            {
                travel_dist = (INT8S)destination_floor - (INT8S)current_floor;
                set_led_mode(LED_ACCELERATE);
                set_ui_mode(UI_CURRENT_FLOOR);
                state = ACCELERATE_S;
            }
            else // update selection
            {
                // check direction of change
                if(encoder_val > prev_encoder_val) // up
                {
                    destination_floor = (destination_floor+1) % TOP_FLOOR; // increment within floor loc's
                }
                else if(encoder_val < prev_encoder_val) // down
                {
                    destination_floor = (destination_floor-1); //needed to be split in two lines, something up with overflow
                    destination_floor = destination_floor % TOP_FLOOR; // decrement within floor loc's
                }
                set_destination_floor(floor_loc2name(destination_floor));// send to LCD
            }

            // update previous value
            prev_encoder_val = encoder_val;
            break;

        case ACCELERATE_S:
        {
            if(elevator_timer++ ==  ACCEL_TIME_PER_FLOOR_MS/ELEVATOR_TASK_PERIOD_MS)
            {
                elevator_timer = RESET;
                if(current_floor < destination_floor - travel_dist/2)
                {
                    current_floor++;
                }
                else if(current_floor > destination_floor - travel_dist/2)
                {
                    current_floor--;
                }
                else
                {
                    set_led_mode(LED_DECELERATE);
                    state = DECELERATE_S;
                }
            }
        }
        break;
        case DECELERATE_S:
            if(elevator_timer++ ==  ACCEL_TIME_PER_FLOOR_MS/ELEVATOR_TASK_PERIOD_MS ) // correct time has passed based on travelling dist
            {
                elevator_timer = RESET;
                if(current_floor < destination_floor)
                {
                    current_floor++;
                }
                else if(current_floor > destination_floor)
                {
                    current_floor--;
                }
                else
                {
                    set_led_mode(LED_OPEN);
                    state = DOORS_OPEN_S;
                }
            }
            break;

        case DOORS_OPEN_S:

            if(first_journey)   // first time on doors open
            {
                first_journey = FALSE;
                set_ui_mode(UI_PASSWORD);
                state = WAIT_FOR_PASS_S;
            }
            else if(elevator_timer++ ==  DOORS_STAY_OPEN_FOR_MS/ELEVATOR_TASK_PERIOD_MS )
            {
                set_ui_mode(UI_CURRENT_FLOOR);
                set_led_mode(LED_IDLE);
                elevator_timer = 0;
                state = DOORS_CLOSED_S;
            }
            break;

        case DOORS_CLOSED_S:
            if(get_button_event() == BE_LONG_PUSH) //wait for sw1 long press
            {
                if(use_counter == 0) //breaks on 4th use
                {
                    use_counter = (use_counter + 1) % 4;
                    set_led_mode(LED_BROKEN);
                    set_ui_mode(UI_POT_GOAL);
                    state = BROKEN1_S;
                }
                else
                {
                    use_counter = (use_counter + 1) % 4;
                    set_led_mode(LED_OPEN);
                    set_ui_mode(UI_PASSWORD);
                    state = WAIT_FOR_PASS_S;
                }
            }
            break;

        case BROKEN1_S:
            if(get_adc() == current_floor_to_randomlike_reference(floor_loc2name(current_floor))) //POT is MATCH
            {
                set_led_mode(LED_OPEN); //stop blinking
                encoder_dir_is_clockwise = !encoder_dir_is_clockwise;

                // update destination encoder value
                xQueueReceive(encoder_pos_q, &encoder_val, portMAX_DELAY);

                if(encoder_dir_is_clockwise)
                {
                    dest_encoder_val = encoder_val + FULL_REVOLUTION;
                }
                else
                {
                    dest_encoder_val = encoder_val - FULL_REVOLUTION;
                }

                set_ui_mode(UI_IDLE);
                state = BROKEN2_S;
            }

            break;

        case BROKEN2_S:
            xQueueReceive(encoder_pos_q, &encoder_val, portMAX_DELAY);
            xQueueReceive(encoder_push_q, &encoder_push, portMAX_DELAY);

            if(encoder_push && ( ((encoder_val >= dest_encoder_val) && encoder_dir_is_clockwise) || ((encoder_val <= dest_encoder_val) && !encoder_dir_is_clockwise) )) // encoder has reached destination and is pushed
            {
                state = WAIT_FOR_PASS_S;
                set_ui_mode(UI_PASSWORD);
            }
            else // update selection
            {
                // check direction of change
                if(((encoder_val < prev_encoder_val) && encoder_dir_is_clockwise) || ((encoder_val > prev_encoder_val) && !encoder_dir_is_clockwise)) // up
                {
                    set_ui_mode(UI_ENC_ERROR);
                }
                else if(encoder_val != prev_encoder_val)
                {
                    set_ui_mode(UI_IDLE);
                }

            }

            // update previous value
            prev_encoder_val = encoder_val;

            break;
        }
    }
}


/*************************** End of module ***********************/

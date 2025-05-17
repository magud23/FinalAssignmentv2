/*************************** Header ******************************/
#include "elevator.h"

/*************************** Functions ***************************/

BaseType_t get_current_floor(INT8U * p_current_floor)
/*****************************************************************
 * Input: pointer in which to put the value returned from buffer
 * Output: success/fail of operation
 * Function: gets current floor from buffer
 ******************************************************************/
{
    return xQueuePeek(current_floor_q, p_current_floor,0);
}

BaseType_t set_current_floor(INT8U * current_floor)
/*****************************************************************
 * Input:  variable which to put to shared memory (OBS: not pointer)
 * Output: success/fail of operation
 * Function: sets current floor in buffer
 ******************************************************************/
{
    INT8U temp = floor_loc2name( *current_floor);
    return xQueueOverwrite(current_floor_q, &temp);
}


BaseType_t get_destination_floor(INT8U * p_destination_floor)
/*****************************************************************
 * Input: pointer in which to put the value returned from shared memory
 * Output: success/fail of operation
 * Function: gets dest floor from buffer
 ******************************************************************/
{
    return xQueuePeek(destination_floor_q, p_destination_floor,0);
}
BaseType_t set_destination_floor(INT8U *destination_floor)
/*****************************************************************
 * Input: pointer to variable
 * Output: success/fail of operation
 * Function: sets destination floor in buffer
 ******************************************************************/
{   INT8U temp = floor_loc2name(*destination_floor);
return xQueueOverwrite(destination_floor_q, &temp);
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
    else if(location >= REMOVED_FLOOR && location < TOP_FLOOR)
        name = location + 1;

    return name;
}


INT16U current_floor_to_randomlike_reference(INT8U current_floor)
{
    return ((((INT16U)current_floor+7)%TOP_FLOOR)* (MAX_POT_VAL*3)/(2*TOP_FLOOR+5) + 44) % MAX_POT_VAL; //these numbers are actually magic and have no inherent meaning
}


void elevator_task(void *pvParameters)
/*****************************************************************
 * Input: -
 * Output: -
 * Function: The brains of the elevator (FSM)
 ******************************************************************/
{
    INT8U state = FLOOR2_S;
    INT8U current_floor = floor_name2loc(2);
    INT8U destination_floor = floor_name2loc(0);
    INT8S travel_dist = RESET;
    INT8U use_counter = RESET;
    INT16U elevator_timer = RESET;
    BOOLEAN encoder_push = RESET;
    INT16S encoder_val, prev_encoder_val, dest_encoder_val;
    BOOLEAN first_journey = TRUE;
    BOOLEAN encoder_dir_is_up = TRUE;
    set_current_floor(&current_floor);       //update shared memory

    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1)
    {

        vTaskDelayUntil(&xLastWakeTime, ELEVATOR_TASK_PERIOD_MS / portTICK_RATE_MS);

        /*------STATE MACHINE ------*/
        switch(state)
        {
        case FLOOR2_S:
            if(get_button_event() == BE_LONG_PUSH)
            {
                travel_dist = (INT8S)destination_floor - (INT8S)current_floor;
                use_counter = (use_counter + 1) % USES_PER_BREAK;
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
                get_encoder_pos(&encoder_val);
                prev_encoder_val = encoder_val;
            }
            break;

        case SELECT_FLOOR_S:
            get_encoder_pos(&encoder_val);
            get_encoder_push(&encoder_push);

            if(encoder_push)                                                // select the floor
            {
                travel_dist = (INT8S)destination_floor - (INT8S)current_floor;
                set_led_mode(LED_ACCELERATE);
                set_ui_mode(UI_CURRENT_FLOOR);
                state = ACCELERATE_S;
            }
            else // update selection
            {
                if(encoder_val > prev_encoder_val)                          // check direction of change
                {                                                           // up
                    destination_floor = (destination_floor+1) % TOP_FLOOR;  // increment within floor loc's
                }
                else if(encoder_val < prev_encoder_val)
                {                                                           // down
                    destination_floor = (destination_floor-1);              //needed to be split in two lines, something up with overflow
                    destination_floor = destination_floor % TOP_FLOOR;      // decrement within floor loc's
                }
                set_destination_floor(&destination_floor);   // send to LCD
            }

            // update previous value
            prev_encoder_val = encoder_val;
            break;

        case ACCELERATE_S:
        {
            if(elevator_timer++ ==  ACCEL_TIME_PER_FLOOR_MS/ELEVATOR_TASK_PERIOD_MS) // Time has passed and a new floor is reached
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
                set_current_floor(&current_floor);       //update shared memory
            }
        }
        break;

        case DECELERATE_S:
            if(elevator_timer++ ==  ACCEL_TIME_PER_FLOOR_MS/ELEVATOR_TASK_PERIOD_MS ) // Time has passed and a new floor is reached
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
                set_current_floor(&current_floor);       //update shared memory
            }
            break;

        case DOORS_OPEN_S:
            if(first_journey)   // first time on doors open stay open and go to password
            {
                first_journey = FALSE;
                set_ui_mode(UI_PASSWORD);
                state = WAIT_FOR_PASS_S;
            }
            else if(elevator_timer++ ==  DOORS_STAY_OPEN_FOR_MS/ELEVATOR_TASK_PERIOD_MS )
            {
                set_ui_mode(UI_CURRENT_FLOOR);
                set_led_mode(LED_IDLE);
                elevator_timer = RESET;
                state = DOORS_CLOSED_S;
            }
            break;

        case DOORS_CLOSED_S:
            if(get_button_event() == BE_LONG_PUSH) //wait for sw1 long press
            {
                use_counter = (use_counter + 1) % USES_PER_BREAK;
                if(use_counter == 0) //breaks on 4th use
                {
                    set_led_mode(LED_BROKEN);
                    set_ui_mode(UI_POT_GOAL);
                    state = BROKEN1_S;
                }
                else
                {
                    set_led_mode(LED_OPEN);
                    set_ui_mode(UI_PASSWORD);
                    state = WAIT_FOR_PASS_S;
                }
            }
            break;

        case BROKEN1_S:
            if(get_adc() == current_floor_to_randomlike_reference(floor_loc2name(current_floor))) //POT is a MATCH
            {
                encoder_dir_is_up = !encoder_dir_is_up;                         //Flip direction each time
                get_encoder_pos(&encoder_val);
                if(encoder_dir_is_up)                                           // update destination encoder value (dependent on desired direction)
                {
                    dest_encoder_val = encoder_val + FULL_REVOLUTION;
                }
                else
                {
                    dest_encoder_val = encoder_val - FULL_REVOLUTION;
                }
                set_led_mode(LED_OPEN);                                         //Stop blinking
                set_ui_mode(UI_ENC);                                            //Update screen
                state = BROKEN2_S;
            }
            break;

        case BROKEN2_S:
            get_encoder_pos(&encoder_val);                                  //Get newest encoder pos
            get_encoder_push(&encoder_push);                                //Get newest encoder push
            if(encoder_push && ( ((encoder_val >= dest_encoder_val) && encoder_dir_is_up) || ((encoder_val <= dest_encoder_val) && !encoder_dir_is_up) )) // encoder has reached destination and is pushed
            {
                state = WAIT_FOR_PASS_S;
                set_ui_mode(UI_PASSWORD);
            }
            else // update selection
            {
                // check direction of change "if (change is downwards and supposed to be up) OR (change is up but should be NOTup)"
                if(((encoder_val < prev_encoder_val) && encoder_dir_is_up) || ((encoder_val > prev_encoder_val) && !encoder_dir_is_up)) // up
                {
                    set_ui_mode(UI_ENC_ERROR);
                }
                else if(encoder_val != prev_encoder_val) //If there is a change (and it is correct)
                {
                    set_ui_mode(UI_ENC);
                }
            }
            prev_encoder_val = encoder_val;          // update previous value
            break;
        }
    }
}


/*************************** End of module ***********************/

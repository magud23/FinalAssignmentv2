/****************************     Header     ******************************/
#include "UI_task.h"

/*****************************   Functions   *******************************/

BaseType_t get_ui_mode(INT8U *pv_ui_mode)
/*****************************************************************************
 *   Input    :  Pointer to variable in which to put ui mode from buffer
 *   Output   :  Success of operation?
 *   Function :  Put a ui_mode into buffer for ui_task
 *****************************************************************************/
{
    return xQueueReceive(ui_mode_q, pv_ui_mode, 0);
}

BaseType_t set_ui_mode(INT8U led_mode)
/*****************************************************************************
 *   Input    :  Pointer to variable that is put into buffer
 *   Output   :  Success of operation?
 *   Function :  Put a ui_mode into buffer for ui_task
 *****************************************************************************/
{
    return xQueueOverwrite(ui_mode_q, &led_mode);
}


char change_int_to_char1(INT8U number)
{
    return '0' + number;
}

pot_goal_mode()
{
    INT8U current_floor = 0 ;
    get_current_floor(&current_floor);
    INT16U target_val = current_floor_to_randomlike_reference(current_floor);

    wr_ch_LCD(0xff);
    wr_str_LCD("REACH THIS: ");
    move_LCD(12,0);
    wr_ch_LCD(change_int_to_char1((target_val/1000) % 10));
    wr_ch_LCD(change_int_to_char1((target_val/100) % 10));
    wr_ch_LCD(change_int_to_char1((target_val/10) % 10));
    wr_ch_LCD(change_int_to_char1(target_val % 10));

    wr_str_LCD("ADJUST: ");
}


void UI_task(void *pvParameters)
{

    INT8U ui_mode = UI_IDLE;
    BOOLEAN first_time = TRUE;
    INT8U password_len = RESET;
    INT8U previous_floor = RESET;
    INT8U dest_floor = RESET;
    INT8U current_floor = RESET;


    while(1)
    {
        vTaskDelay(100 / portTICK_RATE_MS); // wait 20 ms.

        //set first time for static text
        if (get_ui_mode(&ui_mode) == pdTRUE)
        {
            first_time = TRUE;
        }
        switch(ui_mode)
        {
        case UI_IDLE:
            wr_ch_LCD(0xff);
            break;

        case UI_CURRENT_FLOOR:
            get_current_floor(&current_floor);
            if(first_time)
            {
                wr_ch_LCD(0xff); //clear and home display
                wr_str_LCD("CURRENT FLOOR:");
                first_time = FALSE;

                move_LCD(0,1);
                wr_ch_LCD(change_int_to_char1(current_floor/10 % 10));
                wr_ch_LCD(change_int_to_char1(current_floor % 10));
            }
            else if(previous_floor != current_floor)
            {
                move_LCD(0,1);
                wr_ch_LCD(change_int_to_char1(current_floor/10 % 10));
                wr_ch_LCD(change_int_to_char1(current_floor % 10));
            }
            previous_floor = current_floor;
            break;

        case UI_POT_GOAL:
            if(first_time)
            {
                pot_goal_mode();
                first_time = FALSE;
            }
            // update number on second line
            INT16U adc_val = get_adc();
            move_LCD(12,1);
            wr_ch_LCD(change_int_to_char1((adc_val/1000) % 10));
            wr_ch_LCD(change_int_to_char1((adc_val/100) % 10));
            wr_ch_LCD(change_int_to_char1((adc_val/10) % 10));
            wr_ch_LCD(change_int_to_char1(adc_val % 10));
            break;

        case UI_ENC :
            if(first_time)
            {
                wr_ch_LCD(0xff); //clear and home display
                wr_str_LCD("TURN ENCODER");
                first_time = RESET;
            }
            move_LCD(0,1);
            wr_str_LCD("                ");

            break;

        case UI_ENC_ERROR :
            move_LCD(0,1);
            wr_str_LCD("WRONG DIRECTION!");
            break;

        case UI_FLOOR_SELECT:
            if(first_time)
            {
                wr_ch_LCD(0xff); //clear and home display
                wr_str_LCD("SELECT FLOOR:");
                first_time = FALSE;
            }

            // update number on second line
            move_LCD(0,1);
            get_destination_floor(&dest_floor);
            wr_ch_LCD(change_int_to_char1(dest_floor/10 % 10));
            wr_ch_LCD(change_int_to_char1(dest_floor % 10));
            break;

        case UI_PASSWORD:
            if(first_time)
            {
                wr_ch_LCD(0xff); //clear and home display
                wr_str_LCD("TYPE PIN THEN #");
                first_time = RESET;
            }

            // update number on second line
            move_LCD(0,1);

            get_typed_pass_length(&password_len);

			int i;
            for (i=0; i<PASS_LENGTH; i++)
            {
                if(i < password_len)
                {
                    wr_ch_LCD('*');
                }
                else
                {
                    wr_ch_LCD('-');
                }
            }
            break;
        }
    }
}






/*
 * UI_task.h
 *
 *  Created on: Apr 20, 2023
 *      Author: rasmu
 */

#ifndef UI_TASK_H_
#define UI_TASK_H_


//MODES
#define UI_IDLE            0
#define UI_CURRENT_FLOOR   1
#define UI_POT_GOAL        2
#define UI_ENC_ERROR       3
#define UI_FLOOR_SELECT    4
#define UI_PASSWORD        5

void UI_task(void *pvParameters);

void set_ui_mode(INT8U led_mode);
/*****************************************************************************
*   Input    :  INT8U representing led mode
*   Output   :  -
*   Function :  Put a ui_mode into buffer for ui_task
*****************************************************************************/


#endif /* UI_TASK_H_ */

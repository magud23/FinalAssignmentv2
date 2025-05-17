/*****************************************************************************
 * University of Southern Denmark
 * Embedded C Programming (ECP)
 *
 * MODULENAME.: leds.h
 *
 * PROJECT....: ECP
 *
 * DESCRIPTION: Test.
 *
 * Change Log:
 ******************************************************************************
 * Date    Id    Change
 * YYMMDD
 * --------------------
 * 050128  KA    Module created.
 * 250508  MKG   Handled magic numbers
 *
 *****************************************************************************/

#ifndef _LCD_H
#define _LCD_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
// Special ASCII characters
// ------------------------

#define LF		0x0A
#define FF		0x0C
#define CR		0x0D

#define ESC		0x1B
#define LCD_RS  0x04
#define LCD_E   0x08
#define LSB4    0x0F
#define MSB4    0xF0
#define SET_DRAM_ADDR 0x80
#define SECOND_LINE_ADDR 0x40


/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void wr_str_LCD( INT8U *pStr );
/*****************************************************************************
 *   Function : See module specification (.h-file).
 *****************************************************************************/


void move_LCD( INT8U x, INT8U y);
/*****************************************************************************
 *   Function : See module specification (.h-file).
 *****************************************************************************/

INT8U wr_ch_LCD( INT8U Ch );
/*****************************************************************************
 *   OBSERVE  : LCD_PROC NEEDS 20 mS TO PRINT OUT ONE CHARACTER
 *   Function : See module specification (.h-file).
 *****************************************************************************/

void home_LCD();
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Return cursor to the home position.
 ******************************************************************************/

void lcd_task( void *pvParameters );
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Handles the output to LCD, Access using QueueHandle_t xQueue_lcd
 *   >>ESC,@<< to home
 *   >>ESC (0x80 | cursor pos)<< to position cursor
 *   >>0xFF<< to clear
 ******************************************************************************/

/****************************** End Of Module *******************************/
#endif


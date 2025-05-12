/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: leds.c
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
* 050128  KA    Module created.
* 250508  MKG   Handled magic numbers
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "lcd.h"
#include "glob_def.h"
//#include "tmodel.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"




/*****************************    Defines    *******************************/

#define QUEUE_LEN   128

QueueHandle_t xQueue_lcd;
SemaphoreHandle_t xSemaphore_lcd;

enum LCD_states
{
  LCD_POWER_UP,
  LCD_INIT,
  LCD_READY,
  LCD_ESC_RECEIVED,
};

/*****************************   Constants   *******************************/
const INT8U LCD_init_sequense[]= 
{
  0x30,		// Reset
  0x30,		// Reset
  0x30,		// Reset
  0x20,		// Set 4bit interface
  0x28,		// 2 lines Display
  0x0C,		// Display ON, Cursor OFF, Blink OFF
  0x06,		// Cursor Increment
  0x01,		// Clear Display
  0x02,     // Home
  0xFF		// stop (not sent)
}; 


/*****************************   Variables   *******************************/


enum LCD_states LCD_state = LCD_POWER_UP;
INT8U LCD_init;



/*****************************   Functions   *******************************/
INT8U wr_ch_LCD( INT8U Ch )
/*****************************************************************************
*   OBSERVE  : LCD_PROC NEEDS 20 mS TO PRINT OUT ONE CHARACTER 
*   Function : See module specification (.h-file).
*****************************************************************************/
{
        return( xQueueSend( xQueue_lcd, &Ch, portMAX_DELAY));
}


void wr_str_LCD( INT8U *pStr )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  while( *pStr )
  {
    wr_ch_LCD( *pStr );
	pStr++;
  }
}

void move_LCD( INT8U x, INT8U y )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  INT8U Pos;

  Pos = y*SECOND_LINE_ADDR + x;
  Pos |= SET_DRAM_ADDR;
  wr_ch_LCD( ESC );
  wr_ch_LCD( Pos );
}
//----------------------------

void wr_ctrl_LCD_low( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write low part of control data to LCD.
******************************************************************************/
{
  INT8U temp;
  volatile int i;

  temp = GPIO_PORTC_DATA_R;
  temp  = ((Ch & LSB4) << 4); // set any LSB4 as MSB4 not already set
  GPIO_PORTC_DATA_R  = temp & MSB4;        //put data on LCD D7-4
  GPIO_PORTD_DATA_R &= ~LCD_RS;     // Select Control mode (write to instruction register)
  GPIO_PORTD_DATA_R |= LCD_E;		// Set E High LCD_E (Starts data read/write.)
  GPIO_PORTD_DATA_R &= ~LCD_E;		// Set E Low

}

void wr_ctrl_LCD_high( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write high part of control data to LCD.
******************************************************************************/
{
  wr_ctrl_LCD_low(( Ch & MSB4 ) >> 4 );
}

void out_LCD_low( INT8U Ch )
/*****************************************************************************
*   Input    : Mask
*   Output   : -
*   Function : Send low part of character to LCD. 
*              This function works only in 4 bit data mode.
******************************************************************************/
{
  INT8U temp;
	  
  temp = GPIO_PORTC_DATA_R & LSB4;
  GPIO_PORTC_DATA_R  = temp | ((Ch & LSB4) << 4);
  //GPIO_PORTD_DATA_R &= 0x7F;        // Select write
  GPIO_PORTD_DATA_R |= LCD_RS;        // Select data mode
  GPIO_PORTD_DATA_R |= LCD_E;		// Set E High
  GPIO_PORTD_DATA_R &= ~LCD_E;		// Set E Low
}

void out_LCD_high( INT8U Ch )
/*****************************************************************************
*   Input    : Mask
*   Output   : -
*   Function : Send high part of character to LCD. 
*              This function works only in 4 bit data mode.
******************************************************************************/
{
  out_LCD_low((Ch & MSB4) >> 4);
}

void wr_ctrl_LCD( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write control data to LCD.
******************************************************************************/
{
  static INT8U Mode4bit = FALSE;
  INT16U i;

  wr_ctrl_LCD_high( Ch );
  if( Mode4bit )
  {
	for(i=0; i<1000; i++);
	wr_ctrl_LCD_low( Ch );
  }
  else
  {
	if( (Ch & 0x30) == 0x20 )
	Mode4bit = TRUE;
  }
}

void clr_LCD()
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Clear LCD.
******************************************************************************/
{
  wr_ctrl_LCD( 0x01 );
}


void home_LCD()
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Return cursor to the home position.
******************************************************************************/
{
  wr_ctrl_LCD( 0x02 );
}

void Set_cursor( INT8U Ch )
/*****************************************************************************
*   Input    : New Cursor position
*   Output   : -
*   Function : Place cursor at given position.
******************************************************************************/
{
  wr_ctrl_LCD( Ch );
}


void out_LCD( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write control data to LCD.
******************************************************************************/
{
  INT16U i;

  out_LCD_high( Ch );
  for(i=0; i<1000; i++);
  out_LCD_low( Ch );
}


void lcd_task( void *pvParameters )
{
  INT8U ch;

  INT8U my_state = LCD_POWER_UP;
  while(1)
  {
      switch( my_state )
      {
        case LCD_POWER_UP:
          LCD_init = 0;
          my_state = LCD_INIT;
          vTaskDelay(100 / portTICK_RATE_MS); // wait 100 ms.
          break;

        case LCD_INIT:
          if( LCD_init_sequense[LCD_init] != 0xFF )
          {
            wr_ctrl_LCD( LCD_init_sequense[LCD_init++] );
          }
          else
          {
            my_state = LCD_READY;
          }
          vTaskDelay(10 / portTICK_RATE_MS); // wait 100 ms.
          break;
        case LCD_READY:
          if(uxQueueMessagesWaiting(xQueue_lcd))
          {
              if( xSemaphoreTake( xSemaphore_lcd, portMAX_DELAY))
              {
                  if( xQueueReceive( xQueue_lcd, &ch, portMAX_DELAY ))
                  {
                    switch( ch )
                    {
                      case 0xFF:
                        clr_LCD();
                        break;
                      case ESC:
                        my_state = LCD_ESC_RECEIVED;
                        break;
                      default:
                        out_LCD( ch );
                    }
                  }
                  xSemaphoreGive( xSemaphore_lcd );
              }
          }
          vTaskDelay( 10 / portTICK_RATE_MS); // wait 10 ms
          break;

        case LCD_ESC_RECEIVED:
          if(uxQueueMessagesWaiting(xQueue_lcd))
          {
              if( xSemaphoreTake( xSemaphore_lcd, portMAX_DELAY)) //( TickType_t ) 100 ) == pdTRUE )
              {
                  if( xQueueReceive( xQueue_lcd, &ch, portMAX_DELAY ))
                  {
                      if( ch & 0x80 )
                      {
                        Set_cursor( ch );
                      }
                      else //why is this not else if (ch = '@')
                      {
                          switch( ch )
                          {
                            case '@':
                                home_LCD();
                              break;
                          }
                      }
                      //set_state( LCD_READY );
                      my_state = LCD_READY;
                      vTaskDelay(100 / portTICK_RATE_MS); // wait 100 ms.
                  }
              xSemaphoreGive( xSemaphore_lcd );
              }
          }
          vTaskDelay( 100 / portTICK_RATE_MS); // wait 100 ms
          break;
      }
  }
}


/****************************** End Of Module *******************************/

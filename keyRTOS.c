/*****************************************************************************
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME.: keyRTOS.c
 *
 * PROJECT....: EMP
 *
 * DESCRIPTION: See module specification file (.h-file).
 *
 * Change Log:
 *****************************************************************************
 * Date    Id    Change
 * YYMMDD
 * --------------------
 * 150321  MoH   Module created.
 *
 *****************************************************************************/
#include "keyRTOS.h"
/*****************************   Functions   *******************************/

INT8U row( INT8U y )
{
    INT8U result = 0;

    switch( y )
    {
    case R1: result = 1; break;
    case R2: result = 2; break;
    case R3: result = 3; break;
    case R4: result = 4; break;
    }
    return( result );
}

INT8U key_catch( x, y )
INT8U x, y;
{
    const INT8U matrix[3][4] = {{'*','7','4','1'},
                                {'0','8','5','2'},
                                {'#','9','6','3'}};

    return( matrix[x-1][y-1] );
}


//BOOLEAN key_put_q( INT8U ch )
//{
//  xQueueSendToBack(key_q, &ch, 0 );
//  return( 1 );
//}
//
//BOOLEAN key_get_q( INT8U *pch )
//{
//  return( xQueueReceive(key_q, pch, 0 ) == pdPASS);
//}


BOOLEAN check_column(INT8U x)
{
    INT8U y = GPIO_PORTE_DATA_R & LSB4;             // Save the values of the 4 bits for the rows
    if( y )                                         // If one of them are set...
    {                                               // ...we first find the row number with the function row()
        INT8U ch = key_catch( x, row(y) );          // Now that we have the row and column we look up the corresponding character using the function key_catch
        xQueueSendToBack(key_q, &ch, 0 );                  //  Put the character in a queue so it can be used by another task
        return 1;
    }
    return 0;
}

extern void key_task(void *pvParameters)
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function :
 ******************************************************************************/
{
    INT8U my_state = IDLE_S;

    while(1)
    {
        switch(my_state)
        {
        case IDLE_S :
            GPIO_PORTA_DATA_R &= ~(COL1|COL2|COL3);     // Clear the 3 bits for the columns 0XE3
            GPIO_PORTA_DATA_R |= COL1;                  // Set the bit for column 1
            if (check_column(1))                        // Check all the rows for column 1, using the function check_column
            {                                           // If a button press is registered we go to next state so the press is only registered once
                my_state = HOLDING_S;
                break;
            }
            GPIO_PORTA_DATA_R &= ~(COL1|COL2|COL3);     // Repeat the above for the two other columns
            GPIO_PORTA_DATA_R |= COL2;
            if (check_column(2))
            {
                my_state = HOLDING_S;
                break;
            }
            GPIO_PORTA_DATA_R &= ~(COL1|COL2|COL3);
            GPIO_PORTA_DATA_R |= COL3;
            if (check_column(3))
            {
                my_state = HOLDING_S;
                break;
            }
            break;
        case HOLDING_S:
            if( !(GPIO_PORTE_DATA_R & LSB4) )   // We stay here until the button is released so a button press is not counted more than once
            {
                vTaskDelay( 10 / portTICK_RATE_MS); // wait 10 ms (5 ms is too little)
                my_state = IDLE_S;
            }
            break;
        }
    }

}

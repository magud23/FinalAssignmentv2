/*****************************************************************************
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME.: emp.c
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
 * 150228  MoH   Module created.
 *
 *****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "freeRTOS.h"
#include "queue.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"


/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
QueueHandle_t uart_rx_q;
QueueHandle_t uart_tx_q;

/*****************************   Functions   *******************************/

BOOLEAN uart0_put_q( INT8U ch )
{
    xQueueSendToBack(uart_tx_q, &ch, 0 );
    return( 1 );
}

BOOLEAN uart0_get_q( INT8U *pch )
{
    return( xQueueReceive(uart_rx_q, pch, 0 ) == pdPASS);
}

BOOLEAN uart0_rx_rdy()
/*****************************************************************************
 *   Function : See module specification (.h-file).
 *****************************************************************************/
{
    return( UART0_FR_R & UART_FR_RXFF );
}

INT8U uart0_getc()
/*****************************************************************************
 *   Function : See module specification (.h-file).
 *****************************************************************************/
{
    return ( UART0_DR_R );
}

BOOLEAN uart0_tx_rdy()
/*****************************************************************************
 *   Function : See module specification (.h-file).
 *****************************************************************************/
{
    return( UART0_FR_R & UART_FR_TXFE );
}

void uart0_putc( INT8U ch )
/*****************************************************************************
 *   Function : See module specification (.h-file).
 *****************************************************************************/
{
    UART0_DR_R = ch;
}

extern void uart_rx_task(void *pvParameters)
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function :
 ******************************************************************************/
{
    while(1)
    {
        if( uart0_rx_rdy() )
            xQueueSendToBack(uart_rx_q, uart0_getc(), portMAX_DELAY);
        else
            vTaskDelay( 1 );
    }

}

extern void uart_tx_task(void *pvParameters)
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function :
 ******************************************************************************/
{
    INT8U ch;
    while(1)
    {
        if(xQueueReceive(uart_tx_q, &ch, portMAX_DELAY))
            UART0_DR_R = ch;
    }
}

INT32U lcrh_databits( INT8U antal_databits )
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function : sets bit 5 and 6 according to the wanted number of data bits.
 *   		    5: bit5 = 0, bit6 = 0.
 *   		    6: bit5 = 1, bit6 = 0.
 *   		    7: bit5 = 0, bit6 = 1.
 *   		    8: bit5 = 1, bit6 = 1  (default).
 *   		   all other bits are returned = 0
 ******************************************************************************/
{
    if(( antal_databits < 5 ) || ( antal_databits > 8 ))
        antal_databits = 8;
    return(( (INT32U)antal_databits - 5 ) << 5 );  // Control bit 5-6, WLEN
}

INT32U lcrh_stopbits( INT8U antal_stopbits )
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function : sets bit 3 according to the wanted number of stop bits.
 *   		    1 stpobit:  bit3 = 0 (default).
 *   		    2 stopbits: bit3 = 1.
 *   		   all other bits are returned = 0
 ******************************************************************************/
{
    if( antal_stopbits == 2 )
        return( 0x00000008 );  		// return bit 3 = 1
    else
        return( 0x00000000 );		// return all zeros
}

INT32U lcrh_parity( INT8U parity )
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function : sets bit 1, 2 and 7 to the wanted parity.
 *   		    'e':  00000110b.
 *   		    'o':  00000010b.
 *   		    '0':  10000110b.
 *   		    '1':  10000010b.
 *   		    'n':  00000000b.
 *   		   all other bits are returned = 0
 ******************************************************************************/
{
    INT32U result;

    switch( parity )
    {
    case 'e':
        result = 0x00000006;
        break;
    case 'o':
        result = 0x00000002;
        break;
    case '0':
        result = 0x00000086;
        break;
    case '1':
        result = 0x00000082;
        break;
    case 'n':
    default:
        result = 0x00000000;
    }
    return( result );
}

void uart0_fifos_enable()
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function : Enable the tx and rx fifos
 ******************************************************************************/
{
    UART0_LCRH_R  |= 0x00000010;
}

void uart0_fifos_disable()
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function : Enable the tx and rx fifos
 ******************************************************************************/
{
    UART0_LCRH_R  &= 0xFFFFFFEF;
}




extern void uart0_init( INT32U baud_rate, INT8U databits, INT8U stopbits, INT8U parity )
/*****************************************************************************
 *   Function : See module specification (.h-file).
 *****************************************************************************/
{
    INT32U BRD;

#ifndef E_PORTA
#define E_PORTA
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;					// Enable clock for Port A
#endif

#ifndef E_UART0
#define E_UART0
    SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0;					// Enable clock for UART 0
#endif

    GPIO_PORTA_AFSEL_R |= 0x00000003;
    GPIO_PORTA_DIR_R   |= 0x00000002;
    GPIO_PORTA_DEN_R   |= 0x00000003;
    GPIO_PORTA_PUR_R   |= 0x00000002;

    BRD = 64000000 / baud_rate;
    UART0_IBRD_R = BRD / 64;
    UART0_FBRD_R = BRD & 0x0000003F;

    UART0_LCRH_R  = lcrh_databits( databits );
    UART0_LCRH_R += lcrh_stopbits( stopbits );
    UART0_LCRH_R += lcrh_parity( parity );

    uart0_fifos_disable();

    UART0_CTL_R  |= (UART_CTL_UARTEN | UART_CTL_TXE );  // Enable UART
}

/****************************** End Of Module *******************************/













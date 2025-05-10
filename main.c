
/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* PROJECT....: FINAL ASSIGNEMNT
*
* DESCRIPTION: Main file
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 080525  MKG    project created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
//FreeRTOS
#include "FreeRTOS.h"
#include "systick_frt.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

//Course
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"

//Modules
#include "glob_def.h"
#include "status_led.h"
#include "uart.h"
#include "adcRTOS.h"
#include "leds.h"
#include "lcd.h"
#include "elevator.h"


/*****************************    Defines    *******************************/
//task
#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3
#define QUEUE_LEN   128
#define BUFFER_LEN 1

//UART
#define BAUDRATE 9600
#define DATABITS 8
#define STOPBITS 1
#define PARITY 'n'


/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
extern QueueHandle_t uart_rx_q;
extern QueueHandle_t uart_tx_q;
extern QueueHandle_t adc_q;
extern QueueHandle_t xQueue_lcd;

extern SemaphoreHandle_t xSemaphore_lcd;


/*****************************   Functions   *******************************/


static void setupHardware(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :
*****************************************************************************/
{

  // Warning: If you do not initialize the hardware clock, the timings will be inaccurate
  init_systick();
  status_led_init();
  uart0_init(BAUDRATE,DATABITS,STOPBITS, PARITY);
  void init_gpio();

}

static INT16U setupInterTaskCommunication(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :
*****************************************************************************/
{
    INT16U tmp = TRUE;
    uart_rx_q = xQueueCreate(QUEUE_LEN, sizeof(INT8U));
    uart_tx_q = xQueueCreate(QUEUE_LEN, sizeof(INT8U));
    tmp = (uart_rx_q != NULL) && (uart_tx_q != NULL);

    adc_q = xQueueCreate(BUFFER_LEN, sizeof(INT16U));
    tmp = tmp && ( adc_q != NULL);

    xQueue_lcd = xQueueCreate( QUEUE_LEN , sizeof( INT8U ));
    tmp = tmp && ( xQueue_lcd != NULL);

    xSemaphore_lcd = xSemaphoreCreateMutex();
    tmp = tmp && ( xSemaphore_lcd != NULL);

     return tmp;
}



int main(void)
{
    setupHardware();

    INT16U tmp = TRUE;
    uart_rx_q = xQueueCreate(QUEUE_LEN, sizeof(INT8U));
    uart_tx_q = xQueueCreate(QUEUE_LEN, sizeof(INT8U));
    tmp = (uart_rx_q != NULL) && (uart_tx_q != NULL);

    adc_q = xQueueCreate(BUFFER_LEN, sizeof(INT16U));
    tmp = tmp && ( adc_q != NULL);

    xQueue_lcd = xQueueCreate( QUEUE_LEN , sizeof( INT8U ));
    tmp = tmp && ( xQueue_lcd != NULL);

    xSemaphore_lcd = xSemaphoreCreateMutex();
    tmp = tmp && ( xSemaphore_lcd != NULL);

    if(tmp){
           xTaskCreate( status_led_task, "Status_led", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( uart_rx_task, "Uart rx", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( uart_tx_task, "Uart tx", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( adc_task, "ADC (potmeter)", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( leds_task, "LEDS", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( lcd_task, "LCD driver", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
//           xTaskCreate( elevator_task, "Elevator", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );

           vTaskStartScheduler();
    }






    return 0;
}


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
#include "encoder.h"
#include "UI_task.h"
#include "elevator.h"
#include "keyRTOS.h"
#include "password.h"
#include "button.h"


/*****************************    Defines    *******************************/
//task
#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3
#define QUEUE_LEN   128
#define BUFFER_LEN 1
#define SHARED_MEM_LEN 1

//UART
#define BAUDRATE 9600
#define DATABITS 8
#define STOPBITS 1
#define PARITY 'n'


/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
//Queues
extern QueueHandle_t uart_rx_q;
extern QueueHandle_t uart_tx_q;

extern QueueHandle_t key_q;
extern QueueHandle_t xQueue_lcd;
extern SemaphoreHandle_t xSemaphore_lcd;

//Buffers
extern QueueHandle_t adc_q;
extern QueueHandle_t pass_accept_q;
extern QueueHandle_t button_q;
extern QueueHandle_t led_q;
extern QueueHandle_t ui_mode_q;

extern QueueHandle_t encoder_pos_q;
extern QueueHandle_t encoder_push_q;
extern QueueHandle_t destination_floor_q;

//Shared MEM
extern QueueHandle_t current_floor_q;




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
  init_gpio();
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

    led_q = xQueueCreate(BUFFER_LEN, sizeof(INT8U));
    tmp = tmp && ( led_q != NULL);

    ui_mode_q = xQueueCreate(BUFFER_LEN, sizeof(INT8U));
    tmp = tmp && ( ui_mode_q != NULL);

    current_floor_q = xQueueCreate(SHARED_MEM_LEN, sizeof(INT8U));
    tmp = tmp && ( current_floor_q != NULL);

    pass_accept_q = xQueueCreate(BUFFER_LEN, sizeof(INT16U));
    tmp = tmp && ( pass_accept_q != NULL);

    button_q = xQueueCreate(BUFFER_LEN, sizeof(INT16U));
    tmp = tmp && ( button_q != NULL);

    encoder_pos_q = xQueueCreate(BUFFER_LEN, sizeof(INT16S));
    tmp = tmp && ( encoder_pos_q != NULL);

    encoder_push_q = xQueueCreate(BUFFER_LEN, sizeof(BOOLEAN));
    tmp = tmp && ( encoder_push_q != NULL);
	
    destination_floor_q = xQueueCreate(BUFFER_LEN, sizeof(INT16U));
    tmp = tmp && ( destination_floor_q != NULL);

    key_q = xQueueCreate(QUEUE_LEN, sizeof(INT16U));
    tmp = tmp && ( key_q != NULL);

    xQueue_lcd = xQueueCreate( QUEUE_LEN , sizeof( INT8U ));
    tmp = tmp && ( xQueue_lcd != NULL);

    xSemaphore_lcd = xSemaphoreCreateMutex();
    tmp = tmp && ( xSemaphore_lcd != NULL);

     return tmp;
}



int main(void)
{
    setupHardware();

    if(setupInterTaskCommunication()){
           xTaskCreate( status_led_task, "Status_led", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( uart_rx_task, "Uart rx", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( uart_tx_task, "Uart tx", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( adc_task, "ADC (potmeter)", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( key_task, "Keypad", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
           xTaskCreate( button_task, "Button ", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
           xTaskCreate( password_task, "password", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
           xTaskCreate( leds_task, "LEDS", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( encoder_task, "Encoder driver", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( lcd_task, "LCD driver", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( UI_task, "UI task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( elevator_task, "Elevator", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );

           vTaskStartScheduler();
    }
    return 0;
}

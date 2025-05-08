

/**
 * main.c
 */

#include <stdint.h>
#include "FreeRTOS.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "systick_frt.h"
#include "task.h"
#include "status_led.h"
#include "queue.h"
#include "uart.h"
#include "adcRTOS.h"

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3


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
  uart0_init(9600,8,1,'n');

}




extern QueueHandle_t uart_rx_q;
extern QueueHandle_t uart_tx_q;
extern QueueHandle_t adc_q;


int main(void)
{
    setupHardware();

    uart_rx_q = xQueueCreate(128, sizeof(INT8U));
    uart_tx_q = xQueueCreate(128, sizeof(INT8U));
    adc_q = xQueueCreate(1, sizeof(INT16U));

    if( (uart_rx_q != NULL) && (uart_tx_q != NULL) ){
           xTaskCreate( status_led_task, "Status_led", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( uart_rx_task, "Uart rx", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( uart_tx_task, "Uart tx", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           xTaskCreate( adc_task, "ADC (potmeter)", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
           vTaskStartScheduler();
    }






    return 0;
}

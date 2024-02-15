#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/uart.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "main_functions.h"
#include "driver/gpio.h"


//For Uart communication
static const char *TAG = "uart_events";
#define EX_UART_NUM UART_NUM_0
#define PATTERN_CHR_NUM    (3)         /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/
#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
static QueueHandle_t uart0_queue;
QueueHandle_t dataUART = NULL;				// Handle for 64 queue

static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    //size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
    for(;;)
    {
        //Waiting for UART event.
        if(xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY))
        {
            bzero(dtmp, RD_BUF_SIZE);
            //ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);
            uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
            xQueueSend(dataUART, dtmp, (TickType_t) 10);
            //printf("Byte de entrad: %s\n",dtmp);
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}
//*******************************************************

void init_UART()
{
	esp_log_level_set(TAG, ESP_LOG_INFO);
	uart_config_t uart_config = {
	        .baud_rate = 115200,
	        .data_bits = UART_DATA_8_BITS,
	        .parity = UART_PARITY_DISABLE,
	        .stop_bits = UART_STOP_BITS_1,
	        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	        .source_clk = UART_SCLK_APB,
	    };
  //Install UART driver, and get the queue.
  uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);
  uart_param_config(EX_UART_NUM, &uart_config);

  //Set UART log level
  esp_log_level_set(TAG, ESP_LOG_INFO);
  //Set UART pins (using UART0 default pins ie no changes.)
  uart_set_pin(EX_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  //Set uart pattern detect function.
  uart_enable_pattern_det_baud_intr(EX_UART_NUM, '+', PATTERN_CHR_NUM, 9, 0, 0);
  //Reset the pattern queue length to record at most 20 pattern positions.
  uart_pattern_queue_reset(EX_UART_NUM, 20);

  dataUART = xQueueCreate( 16, sizeof(uint8_t) );
  if( dataUART == NULL )
	  {
	  	printf("UART queue creation failed!.\r\n");
	    for (;;);
	  }
	//Create a task to handler UART event from ISR
	xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
}

void tf_main(void) {
	char dato=0;
	printf("\n");
	setup();
	vTaskDelay(pdMS_TO_TICKS(1000));
	while (true) {
		if(xQueueReceive(dataUART,&dato,( TickType_t ) 100u)== pdTRUE)
		{
			//printf("dato: %i\n", dato);
			if(dato==73)loop();
			setup();
		}
	}
}

extern "C" void app_main() {
	init_UART();
	xTaskCreate((TaskFunction_t) &tf_main, "tf_main", 4 * 1024, NULL, 8, NULL);
	vTaskDelete(NULL);
}

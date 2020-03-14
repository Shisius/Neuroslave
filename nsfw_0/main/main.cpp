/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
//#include "wifiobj.hpp"
#include "tgam.hpp"
#include "freertos/ringbuf.h"

extern "C" {
	void app_main(void);
}

extern "C" {struct Ringbuffer_t;}

TGAM tgam = TGAM();

static void print_signal(void * data)
{
    TGAMSharedData * shared_data = (TGAMSharedData *)data;
    Ringbuffer_t * signal = (Ringbuffer_t *)(&shared_data->time_points);
    uint8_t * sample = (uint8_t *)signal;
    while(1){
        //xRingbufferPrintInfo(shared_data->raw_signal);
        for (int i = 0; i < 1024; i++){
            printf("%d ", *(sample + i));
        }
        printf("\n");
        vTaskDelay( pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}

void app_main()
{
    printf("Hello TGAM!\n");
    
    tgam.setup();
    tgam.run();

    xTaskCreate(print_signal, "print_task", 2048, tgam.shared_data, configMAX_PRIORITIES - 1, NULL);

}
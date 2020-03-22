/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
//#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
//#include "wifiobj.hpp"
#include "tgam.hpp"
#include "wifiobj.hpp"
#include "proto.hpp"
#include "freertos/ringbuf.h"

extern "C" {
	void app_main(void);
}

extern "C" {struct Ringbuffer_t;}

TGAM tgam = TGAM();
WiFiStation wifi_stattion = WiFiStation();
UDPClient udp_client = UDPClient();

static const int n_packed_samples = 10;

struct FullPack
{
    const char BEGIN = '$';
    const char Q = 'Q';
    uint8_t quality;
    const char A = 'A';
    uint8_t attention;
    const char M = 'M';
    uint8_t meditation;
    const char S = 'S';
    uint32_t signal[n_packed_samples];
    const char END[4] = "END";
};

static void print_signal(void * data)
{
    TGAMSharedData * shared_data = (TGAMSharedData *)data;
    size_t parcel_size;
    uint32_t * parcel;
    //Ringbuffer_t * signal = (Ringbuffer_t *)(&shared_data->time_points);
    //uint8_t * sample = (uint8_t *)signal;
    while(1) {
        //xRingbufferPrintInfo(shared_data->raw_signal);
        for (int i = 0; i < 10; i++) {
            parcel = (uint32_t *)xRingbufferReceive(shared_data->raw_signal, &parcel_size, pdMS_TO_TICKS(1));
            if (parcel != NULL) {
                printf("t: %d, s: %d ", UNPACK_TIME(*parcel), UNPACK_SIGNAL(*parcel));
                vRingbufferReturnItem(shared_data->raw_signal, (void *)parcel);
            }  
        }
        printf("\n");
        vTaskDelay( pdMS_TO_TICKS(20));
    }
    vTaskDelete(NULL);
}

static void tgam2udp(void * data)
{
    TGAMSharedData * shared_data = (TGAMSharedData *)data;
    size_t sample_size;
    uint32_t * sample;
    FullPack parcel;
    char mes[256];
    vTaskDelay( pdMS_TO_TICKS(1000));
    while(1) {
        parcel.quality = shared_data->signal_quality;
        parcel.attention = shared_data->attention;
        parcel.meditation = shared_data->meditation;
        for (int i = 0; i < n_packed_samples; i++) {
            sample = (uint32_t *)xRingbufferReceive(shared_data->raw_signal, &sample_size, pdMS_TO_TICKS(1));
            if (sample != NULL) {
                parcel.signal[i] = *sample;
                //printf("t%u,s%d", UNPACK_TIME(*sample), UNPACK_SIGNAL(*sample));
                vRingbufferReturnItem(shared_data->raw_signal, (void *)sample);
            }
        }
        memcpy(mes, &parcel, sizeof(FullPack));
        //printf("\n");
        udp_client.sendmesssage(mes, sizeof(FullPack));
        vTaskDelay( pdMS_TO_TICKS(2 * n_packed_samples));
    }
    vTaskDelete(NULL);
}

void app_main()
{

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    printf("Hello TGAM!\n"); 

    tgam.setup();
    tgam.run();

    wifi_stattion.setup("shisius", "fallen-chebosko");
    udp_client.setup("192.168.1.121"/*192.168.43.1*/, 9998);
    
    //xTaskCreate(print_signal, "print_task", 2048, tgam.shared_data, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(tgam2udp, "tgam2udp", 2048, tgam.shared_data, configMAX_PRIORITIES - 1, NULL);
}
// TGAM reader and parser
#ifndef __TGAM__
#define __TGAM__
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#define CONFIG_FREERTOS_HZ 100
#include "driver/uart.h"
#include "sys/time.h"

// TGAM protocol defines
#define HEADER_SYNC '\xAA'
#define HEADER_SYNC_NUM 2
#define PAYLOAD_SIZE_OFFSET 2
#define PAYLOAD_OFFSET 3
#define NONPAYLOAD_SIZE 4
#define CRC_OFFSET -1
#define MAX_TGAM_PACK_SIZE 169
#define SIGNAL_QUALITY_CODE 0x02
#define ATTENTION_CODE 0x04
#define MEDITATION_CODE 0x05
#define BLINK_STRENGTH_CODE 0x16
#define RAW_SIGNAL_CODE 0x80
#define ASIC_EEG_POWER_CODE 0x83
#define RAW_SIGNAL_PAYLOAD_OFFSET 2

struct TGAMSharedData
{
    // pack
    uint8_t * tgam_pack;
    int tgam_pack_size;
    uint8_t attention;
    uint8_t meditation;
    uint8_t signal_quality;
    uint8_t blink_strength;
    QueueHandle_t uart_queue;
    RingbufHandle_t raw_signal;
    int16_t signal_sample;
    RingbufHandle_t time_points;
    uint16_t time_sample;
    // uint64_t start_time;
    struct timeval current_time;
};

class TGAM
{
private:

    // uart settings
    uart_config_t * uart_config;
    static const uart_port_t uart_port = UART_NUM_2;
    const int uart_rx_pin = 16;
    // reading settings
    const int rx_buf_size = 1024;
    static const int read_wait_time_ms = 100;
    // task settings
    const uint32_t usStackDepth = 2048;
    // event queue settings
    const int queue_size = 128;  
    // raw signal buffer size
    const int raw_signal_buffer_size = 2048;
    const int time_points_buffer_size = 2048; 

public:

    // debug flag
    static bool debug;
    // pack
    TGAMSharedData * shared_data;

    TGAM();

    int setup();

    static void uart_task(void *pvParameters);

    static void parse_tgam_pack(TGAMSharedData * shared_data);

    static uint8_t tgam_crc_calculate(uint8_t * some_pack, int some_pack_size);

    static void print(const char *);

    int run();

    ~TGAM();

};

#endif // __TGAM__
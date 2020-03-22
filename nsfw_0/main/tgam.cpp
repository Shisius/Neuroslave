// TGAM reader and parser
#include "tgam.hpp"

TGAM::TGAM() 
{
    this->uart_config = new uart_config_t({
        .baud_rate = 57600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    });
    shared_data = new TGAMSharedData();
    shared_data->tgam_pack = (uint8_t *)calloc(1, MAX_TGAM_PACK_SIZE);
    shared_data->raw_signal = xRingbufferCreate(raw_signal_buffer_size, RINGBUF_TYPE_NOSPLIT);
    //shared_data->time_points = xRingbufferCreate(time_points_buffer_size, RINGBUF_TYPE_NOSPLIT);
    //debug = true;
}

int TGAM::setup() 
{
    // config
    uart_param_config(uart_port, uart_config);
    // set pin
    uart_set_pin(uart_port, UART_PIN_NO_CHANGE, uart_rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // setup queue
    uart_driver_install(uart_port, rx_buf_size, 0, queue_size, &shared_data->uart_queue, 0);
    // set uart pattern detect function.
    uart_enable_pattern_det_intr(uart_port, HEADER_SYNC, HEADER_SYNC_NUM, 10000, 10, 10);
    // reset the pattern queue length to record at most queue_size pattern positions.
    uart_pattern_queue_reset(uart_port, queue_size);
    return 0;
}

int TGAM::run()
{
    xTaskCreate(TGAM::uart_task, "tgam_task", usStackDepth, this->shared_data, configMAX_PRIORITIES, NULL);
    return 0;
}

void TGAM::uart_task(void * shared)
{
    // type conversion
    TGAMSharedData * shared_data = (TGAMSharedData *)shared;
    // event
    uart_event_t event;
    // header position
    int header_position = 0;
    // payload length
    uint8_t payload_length = 0;
    // crc
    uint8_t crc = 0;
    // endless cycle
    while (1) 
    {
        if(xQueueReceive(shared_data->uart_queue, (void * )&event, (portTickType)portMAX_DELAY))
        {
            switch (event.type) {
            case UART_DATA:
                break;
            case UART_FIFO_OVF:
                uart_flush(TGAM::uart_port);
                xQueueReset(shared_data->uart_queue);
                TGAM::print("fifo overflow\n");
                break;
            case UART_BUFFER_FULL:
                uart_flush(TGAM::uart_port);
                xQueueReset(shared_data->uart_queue);
                TGAM::print("buffer full\n");
                break;
            case UART_BREAK:
                break;
            case UART_DATA_BREAK:
                break;
            case UART_EVENT_MAX:
                break;
            case UART_PARITY_ERR:
                break;
            case UART_FRAME_ERR:
                break;
            case UART_PATTERN_DET:
                // get header position
                header_position = uart_pattern_pop_pos(TGAM::uart_port);
                if (header_position != -1) {
                    // read whole pack - bytes between two headers
                    shared_data->tgam_pack_size = 
                        uart_read_bytes(TGAM::uart_port, shared_data->tgam_pack, header_position, TGAM::read_wait_time_ms / portTICK_PERIOD_MS);
                    payload_length = shared_data->tgam_pack[PAYLOAD_SIZE_OFFSET];
                    // check sum
                    crc = tgam_crc_calculate(shared_data->tgam_pack + PAYLOAD_OFFSET, payload_length);
                    // check size
                    while (shared_data->tgam_pack_size != payload_length + NONPAYLOAD_SIZE)
                    {
                        // If crc == HEADER_SYNC, pattern detector can cosider it as header part
                        if (crc == HEADER_SYNC) {
                            uart_read_bytes(TGAM::uart_port, shared_data->tgam_pack + shared_data->tgam_pack_size, 1, 
                                TGAM::read_wait_time_ms / portTICK_PERIOD_MS);
                            shared_data->tgam_pack_size += 1;
                        } else {
                            TGAM::print("Wrong size\n");
                            break;
                        }
                    }
                    // Check size and crc
                    if (payload_length + 4 == shared_data->tgam_pack_size){
                        if (crc == shared_data->tgam_pack[shared_data->tgam_pack_size + CRC_OFFSET]) {
                            // parse pack
                            parse_tgam_pack(shared_data);
                        } else {
                            TGAM::print("Wrong crc\n");
                        }
                    }
                } else {
                    uart_flush_input(TGAM::uart_port);
                }
                break;
            //default:
                //break;
            }
        }
        
    }
    vTaskDelete(NULL);
}

void TGAM::parse_tgam_pack(TGAMSharedData * shared_data)
{
    uint8_t pack_position = PAYLOAD_OFFSET;
    while (pack_position < shared_data->tgam_pack_size - 1) {
        switch (shared_data->tgam_pack[pack_position]) {
            case RAW_SIGNAL_CODE:
                shared_data->signal_sample = (shared_data->tgam_pack[pack_position + RAW_SIGNAL_PAYLOAD_OFFSET] << 8) | 
                    shared_data->tgam_pack[pack_position + RAW_SIGNAL_PAYLOAD_OFFSET + 1];
                // send sample to ring buffer
                //xRingbufferSend(shared_data->raw_signal, &shared_data->signal_sample, sizeof(int16_t), pdMS_TO_TICKS(1));
                // get time
                gettimeofday(&shared_data->current_time, NULL);
                shared_data->time_sample = uint16_t(shared_data->current_time.tv_usec);
                // Send sample and time points into one buffer as one element
                shared_data->timed_sample = (uint32_t(shared_data->time_sample) << 16) | uint16_t(shared_data->signal_sample);
                xRingbufferSend(shared_data->raw_signal, &shared_data->timed_sample, sizeof(uint32_t), pdMS_TO_TICKS(1));
                pack_position += 4;
                //printf("%d %d %d %d\n", shared_data->time_sample, shared_data->signal_sample, 
                //        (shared_data->timed_sample) >> 16, int16_t(shared_data->timed_sample & 0xFFFF));
                break;
            case SIGNAL_QUALITY_CODE:
                shared_data->signal_quality = shared_data->tgam_pack[pack_position + 1];
                pack_position += 2;
                //printf("Signal Quality = %d\n", shared_data->signal_quality);
                break;
            case ATTENTION_CODE:
                shared_data->attention = shared_data->tgam_pack[pack_position + 1];
                pack_position += 2;
                //printf("Attention = %d\n", shared_data->attention);
                break;
            case MEDITATION_CODE:
                shared_data->meditation = shared_data->tgam_pack[pack_position + 1];
                pack_position += 2;
                //printf("Meditation = %d\n", shared_data->meditation);
                break;
            case BLINK_STRENGTH_CODE:
                shared_data->blink_strength = shared_data->tgam_pack[pack_position + 1];
                pack_position += 2;
                //printf("Blink Strength = %d\n", shared_data->blink_strength);
                break;
            case ASIC_EEG_POWER_CODE:
                pack_position += 26;
                //printf("ASIC\n");
                break;
            default:
                pack_position++;
                TGAM::print("WTF?\n");
                break;
        }
    }
}

uint8_t TGAM::tgam_crc_calculate(uint8_t * some_data, int some_pack_size)
{
    uint32_t sum = 0;
    for (int i = 0; i < some_pack_size; i++){
        sum += some_data[i];
    }
    return uint8_t(~(sum & 0xFF));
}

void TGAM::print(const char * stuff)
{
    if (true) printf(stuff);
}

TGAM::~TGAM() {}
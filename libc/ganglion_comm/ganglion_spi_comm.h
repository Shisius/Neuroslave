#ifndef _GANGLION_SPI_COMM_
#define _GANGLION_SPI_COMM_

#include <unistd.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "definitions/ganglion_comm_defines.h"
#include "neuroslave_data.h"

#define GANGLION_SPI_DELAY_US_DEFAULT 100
#define GANGLION_SPI_SPEED 10000000
#define GANGLION_SPI_CHANNEL 0

bool ganglion_spi_comm_setup(void);

bool ganglion_spi_start(void);

bool ganglion_spi_stop(void);

bool ganglion_spi_get_sample(McpSample * mcp_sample);

// bool ganglion_spi_get_sample_pack(EegSamplePack * eeg_sample_pack)
// {
// 	uint8_t sample_counter = 0;
// 	McpSample mcp_sample;
// 	while (sample_counter < eeg_sample_pack->header.n_samples) {
// 		if (ganglion_spi_get_sample(&mcp_sample)) {

// 		}
// 	}
// }




#endif //_GANGLION_SPI_COMM_
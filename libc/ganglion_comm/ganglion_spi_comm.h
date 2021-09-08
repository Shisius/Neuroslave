#ifndef _GANGLION_SPI_COMM_
#define _GANGLION_SPI_COMM_

#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "definitions/ganglion_comm_defines.h"

#define GANGLION_SPI_DELAY_US_DEFAULT 100
#define GANGLION_SPI_SPEED 10000000
#define GANGLION_SPI_CHANNEL 0

bool ganglion_spi_comm_setup(void);

void ganglion_spi_start(void);

void ganglion_spi_stop(void);

bool ganglion_spi_get_sample(McpSample * mcp_sample);

#endif //_GANGLION_SPI_COMM_

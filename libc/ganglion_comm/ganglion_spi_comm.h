#ifndef _GANGLION_SPI_COMM_
#define _GANGLION_SPI_COMM_

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "definitions/ganglion_comm_defines.h"

#define GANGLION_SPI_DELAY_US_DEFAULT 100

typedef struct GanglionComm GanglionComm;
struct GanglionComm {
	uint16_t spi_wait_us;
}

bool ganglion_spi_comm_setup();




#endif //_GANGLION_SPI_COMM_
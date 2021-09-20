#include "ganglion_spi_comm.h"

static const uint8_t     spiBPW   = 8 ;
static const uint16_t    spiDelay = 0 ;

static uint32_t    spiSpeed;
static int         spiFd;
static int 		   spiChannel;

int piSpiSetup(int channel, int speed, int mode)
{
	int fd ;
	char spiDev [32] ;
	mode &= 3;	// Mode is 0, 1, 2 or 3

	snprintf (spiDev, 31, "/dev/spidev0.%d", channel) ;

	if ((fd = open (spiDev, O_RDWR)) < 0)
    	return fd;

  	spiSpeed = speed ;
  	spiFd = fd ;
  	spiChannel = channel;

	// Set SPI parameters.

  	if (ioctl (fd, SPI_IOC_WR_MODE, &mode)            < 0)
    	return fd;
  
  	if (ioctl (fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0)
    	return fd;

  	if (ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed)   < 0)
    	return fd;

  	return fd ;
}

int piSpiRW(unsigned char * data, int len)
{
	struct spi_ioc_transfer spi ;
	memset (&spi, 0, sizeof (spi)) ;

	spi.tx_buf        = (unsigned long)data ;
	spi.rx_buf        = (unsigned long)data ;
	spi.len           = len ;
	spi.delay_usecs   = spiDelay ;
	spi.speed_hz      = spiSpeed ;
	spi.bits_per_word = spiBPW ;

	return ioctl (spiFd , SPI_IOC_MESSAGE(1), &spi) ;
}


bool ganglion_spi_comm_setup(void)
{
	if (piSpiSetup (GANGLION_SPI_CHANNEL, GANGLION_SPI_SPEED, 0) < 0)
		return false;
	return true;
}

void ganglion_spi_start(void)
{
	uint8_t command = GANGLION_CMD_START;
	piSpiRW(&command, sizeof(command));
}

void ganglion_spi_stop(void)
{
	uint8_t command = GANGLION_CMD_STOP;
	piSpiRW(&command, sizeof(command));
}

bool ganglion_spi_get_sample(McpSample * mcp_sample)
{
	uint8_t command = GANGLION_CMD_SAMPLE;
	piSpiRW(&command, sizeof(command));
	if (command == GANGLION_ANS_READY) {
		memset(mcp_sample, GANGLION_CMD_FETCH, sizeof(McpSample));
		piSpiRW((unsigned char *)mcp_sample, sizeof(McpSample));
		return true;
	}
	return false;
}

#include "ganglion_spi_comm.h"

bool ganglion_spi_comm_setup(void)
{
	if (wiringPiSPISetup (GANGLION_SPI_CHANNEL, GANGLION_SPI_SPEED) == -1)
		return false;
	return true;
}

void ganglion_spi_start(void)
{
	uint8_t command = GANGLION_CMD_START;
	wiringPiSPIDataRW(GANGLION_SPI_CHANNEL, &command, sizeof(command));
}

void ganglion_spi_stop(void)
{
	uint8_t command = GANGLION_CMD_STOP;
	wiringPiSPIDataRW(GANGLION_SPI_CHANNEL, &command, sizeof(command));
}

bool ganglion_spi_get_sample(McpSample * mcp_sample)
{
	uint8_t command = GANGLION_CMD_SAMPLE;
	wiringPiSPIDataRW(GANGLION_SPI_CHANNEL, &command, sizeof(command));
	if (command == GANGLION_ANS_READY) {
		memset(mcp_sample, GANGLION_CMD_FETCH, sizeof(McpSample));
		wiringPiSPIDataRW(GANGLION_SPI_CHANNEL, mcp_sample, sizeof(McpSample));
		return true;
	}
	return false;
}

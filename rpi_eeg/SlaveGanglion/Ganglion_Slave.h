#ifndef GANGLION_SLAVE_H
#define GANGLION_SLAVE_H

#include <SPI.h>
#include <SPIS.h>
#include <SimbleeBLE.h>
#include "Definitions_Ganglion.h"
#include "Ganglion_Slave_Proto.h"
#include "queue_mcu_rnr.h"

#define SPIS_BUF_LEN sizeof(McpSample)
#define SPIS_DATA_READY 2
#define MCP_SPI_KHZ 4000
#define MCP_QUEUE_SIZE_BL 8

int32_t conv24to32(int32_t value24);

class GanglionSlave
{

private:

	// SPIS
	char spis_rx[SPIS_BUF_LEN];
	char spis_tx[SPIS_BUF_LEN];
	QueueRNR<McpSample, MCP_QUEUE_SIZE_BL> mcp_sample_queue;

public:

	typedef enum SAMPLE_RATE {
	    SAMPLE_RATE_25600,
	    SAMPLE_RATE_12800,
	    SAMPLE_RATE_6400,
	    SAMPLE_RATE_3200,
	    SAMPLE_RATE_1600,
	    SAMPLE_RATE_800,
	    SAMPLE_RATE_400,
	    SAMPLE_RATE_200
	};

	// Construct
	GanglionSlave();
	~GanglionSlave() {}
	void init();
	void init_spi();
	// SPIS
	void init_spis();
	void spis_routine(int bytes_received, bool overflow);
	void start_stream();
	void stop_stream();
	// LED
	void init_led();
	void led_on();
	void led_off();
	// MCP3912
	volatile bool mcp_data_ready = false;
	uint32_t mcp_sample_counter = 0;
	void init_mcp();
	void mcp_send_cmd(uint8_t address, uint8_t rw);
	int32_t mcp_read_register();
	void mcp_write_register(uint32_t settings);
	void mcp_process_data();
	void mcp_config(uint32_t gain, SAMPLE_RATE sample_rate);
	void mcp_turn_on_channels();

};

#endif // GANGLION_SLAVE_H

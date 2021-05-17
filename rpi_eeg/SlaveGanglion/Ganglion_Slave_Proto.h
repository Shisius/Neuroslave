#ifndef _GANGLION_SLAVE_PROTO_H_
#define _GANGLION_SLAVE_PROTO_H_
#include "Definitions_Ganglion.h"

struct McpSample
{
	uint32_t sample_index;
	int32_t eeg_data[NUM_CHANNELS];
	uint32_t state = 239;
};

enum class GanglionSlaveEvent : uint32_t
{
	SPIS_OVERFLOW = 0x1
};

enum GanglionSpisCommands
{
	SPIS_ANSWER = 0x0,
	SPIS_MCP_SAMPLE = 0x1,
	SPIS_START = 0x2,
	SPIS_STOP = 0x3,
	SPIS_MCP_GAIN = 0x16
};

#endif// _GANGLION_SLAVE_PROTO_H_

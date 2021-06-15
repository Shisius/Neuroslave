#ifndef _GANGLION_SLAVE_PROTO_H_
#define _GANGLION_SLAVE_PROTO_H_
#include "Definitions_Ganglion.h"

#define NSV_N_CHANNELS 4

struct McpSample
{
	uint32_t sample_index;
	int32_t eeg_data[NSV_N_CHANNELS];
	uint32_t state = 239;
};

enum class GanglionSlaveEvent : uint32_t
{
	SPIS_OVERFLOW = 0x1
};

enum class McpSampleState : uint8_t
{
	GOOD = 239,
	OLD = 55,
	SKIPPED = 30
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

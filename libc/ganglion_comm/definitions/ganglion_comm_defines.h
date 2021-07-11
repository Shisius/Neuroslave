#ifndef _GANGLION_COMM_DEFINES_H_
#define _GANGLION_COMM_DEFINES_H_

/// Ganglion parameters
#define GANGLION_SAMPLES_PER_PACK 10

/// Commands
#define GANGLION_CMD_FETCH 0x0
#define GANGLION_CMD_SAMPLE 0x1
#define	GANGLION_CMD_START 0x2
#define GANGLION_CMD_STOP 0x3
/// Answers
#define GANGLION_ANS_READY 0x0
#define GANGLION_ANS_WAIT 0x1
/// Data defines
#define GANGLION_N_ELECTRODES 2
#define MCP_QUEUE_DEPTH 512

/// Data types
typedef struct {
	uint32_t sample_index;
	int32_t eeg_data[GANGLION_N_ELECTRODES];
} McpSample;

#endif //_GANGLION_COMM_DEFINES_H_

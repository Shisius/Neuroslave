#ifndef __GANGLION_COMM_H_
#define __GANGLION_COMM_H_

#ifndef NSV_DUMMY_SOURCE
	extern "C" {
		#include "ganglion_spi_comm.h"
	}
#endif
extern "C" {
	#include "ganglion_comm_defines.h"
	#include "neuroslave_state.h"
}
#include "neuroslave_struct.h"
#include "threadsafe_queue.h"
#include <atomic>
#include <thread>
#include <chrono>
#define USE_MATH_DEFINES
#include <cmath>
#include <unistd.h>

class GanglionComm {

protected:

	eeg_sample_t d_eeg_sample_bad = EEG_SAMPLE_BAD;

	/// Communication
	std::atomic<nsv_state_t> * d_state;
	NeuroslaveSession * d_session;
	threadsafe_queue<EegSamplePack> * d_eeg_sample_queue;

	/// Settings
	unsigned int d_spi_wait_us = 10;
	unsigned int d_idle_wait_us = 1000;
	unsigned int d_session_wait_us = 100;

	/// Internal
	std::thread spi_thread;
	uint32_t cur_sample_index;
	McpSample cur_mcp_sample;
	EegSamplePack cur_pack;

	void spi_process();
	void get_eeg_pack();
	void prepare();
	void finish();

public:

	GanglionComm(std::atomic<nsv_state_t> * state, NeuroslaveSession * session, threadsafe_queue<EegSamplePack> * eeg_sample_queue) : 
		d_state(state), d_session(session), d_eeg_sample_queue(eeg_sample_queue) {}

	~GanglionComm() {}

	bool start();
	void stop();

};

#endif //__GANGLION_COMM_H_

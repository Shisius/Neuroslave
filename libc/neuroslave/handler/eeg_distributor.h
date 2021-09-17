#ifndef _EEG_DISTRIBUTOR_H_
#define _EEG_DISTRIBUTOR_H_

extern "C" {
	#include "neuroslave_state.h"
	#include "neuroslave_data.h"
}
#include "neuroslave_struct.h"
#include "neuroslave_fs.h"
#include "threadsafe_queue.h"
#include "tcpserver.h"
#include "streamlogger.h"
#include <atomic>
#include <thread>
#include <errno.h>

class EegDistributor
{
protected:

	// Communication
	std::atomic<nsv_state_t> * d_state;
	NeuroslaveSession * d_session;
	NeuroslaveMusic * d_music;
	NeuroslaveGame * d_game;
	threadsafe_queue<EegSamplePack> * d_eeg_sample_queue;

	// Objects
	TCPServer * d_eeg_server = nullptr;
	StreamLogger * d_logger = nullptr;

	// Settings
	unsigned int d_accept_wait_us = 100000;
	long d_srv_wait_us = 10;
	unsigned int d_idle_wait_us = 1000;
	unsigned int d_session_wait_us = 100;
	unsigned int d_eeg_wait_us = 100;

	// Internal
	NeuroslaveRecord * d_record;
	std::thread d_data_thread;
	EegSamplePack d_eeg_pack;

	// Methods
	void eeg_process();
	bool send_eeg_pack();
	void prepare_session();
	void finish_session();
	void prepare_record();

public:

	EegDistributor(std::atomic<nsv_state_t> * state, NeuroslaveSession * session, NeuroslaveMusic * music, NeuroslaveGame * game, 
		threadsafe_queue<EegSamplePack> * eeg_sample_queue);
	~EegDistributor();

	bool start();
	void stop();

};

#endif //_EEG_DISTRIBUTOR_H_

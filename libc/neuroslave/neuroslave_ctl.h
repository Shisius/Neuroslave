#ifndef _NEUROSLAVE_CTL_H_
#define _NEUROSLAVE_CTL_H_

#include "neuroslave_msg.h"
#include "neuroslave_fs.h"
#include "neuroslave_state.h"
#include "threadsafe_queue.h"
//#include "ganglion_comm.h"

#include <thread>
#include <atomic>

class NeuroslaveController
{
protected:

	/// Workers control
	std::atomic<nsv_state_t> d_state;
	NeuroslaveSession d_session;
	threadsafe_queue<EegSamplePack> d_eeg_sample_queue;

	/// Message Server
	TCPServer * d_msg_server = nullptr;
	long d_srv_wait_us = 10;

	/// Internal
	std::thread d_control_thread;

	/// Workers

	/// Routines
	void control_process();

	/// Commands handling
	bool msg_handler(std::string & msg);
	bool turnon();
	bool turnoff();
	bool choose();
	bool record();
	bool stop();
	bool user();
	bool game();
	bool set();
	bool connect();
	bool update();

public:

	void launch();
	void terminate();

	NeuroslaveController();
	~NeuroslaveController();

};

#endif //_NEUROSLAVE_CTL_H_

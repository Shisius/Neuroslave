#ifndef _NEUROSLAVE_CTL_H_
#define _NEUROSLAVE_CTL_H_

#include "neuroslave_msg.h"
#include "neuroslave_fs.h"
#include "neuroslave_state.h"
#include "threadsafe_queue.h"
#include "tcpserver.h"
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
	unsigned int d_msg_buf_size = 1024;
	long d_srv_wait_us = 10;
	unsigned int d_accept_wait_us = 100000;

	/// Internal
	std::thread d_msg_thread;

	/// Workers

	/// Routines
	void msg_process();

	/// Commands handling
	bool msg_handler(std::vector<std::string> & msgvec);
	bool answer(neuroslave::UserAnswer ans);
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

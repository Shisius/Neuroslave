#ifndef _NEUROSLAVE_CTL_H_
#define _NEUROSLAVE_CTL_H_

#include <algorithm>
#include "neuroslave_msg.h"
#include "neuroslave_fs.h"
#include "threadsafe_queue.h"
#include "ganglion_comm.h"
#include "eeg_distributor.h"
#include "tcpserver.h"
extern "C" {
	#include "neuroslave_state.h"
}
//#include "ganglion_comm.h"

#include <thread>
#include <atomic>

class NeuroslaveController
{
protected:

	/// Workers control
	std::atomic<nsv_state_t> d_state;
	NeuroslaveSession d_session;
	NeuroslaveMusic d_music;
	NeuroslaveGame d_game;
	threadsafe_queue<EegSamplePack> d_eeg_sample_queue;

	/// Message Server
	TCPServer * d_msg_server = nullptr;
	unsigned int d_msg_buf_size = 1024;
	long d_srv_wait_us = 10;
	unsigned int d_accept_wait_us = 100000;
	unsigned int d_die_wait_ms = 10;

	/// Internal
	std::thread d_msg_thread;
	std::vector<std::string> d_answer;

	/// Workers
	GanglionComm * d_ganglion_comm = nullptr;
	EegDistributor * d_eeg_distributor = nullptr;

	/// Routines
	void msg_process();

	/// Commands handling
	bool msg_handler(std::vector<std::string> & msgvec);
	bool answer();
	bool turnon();
	bool turnoff();
	bool choose();
	bool record();
	bool stop();
	bool get_users();
	bool set_user(std::string & choosen_user);
	bool game();
	bool set();
	bool connect();
	bool update();

public:

	void launch();
	void terminate();
	void wait_die();

	NeuroslaveController();
	~NeuroslaveController();

};

#endif //_NEUROSLAVE_CTL_H_

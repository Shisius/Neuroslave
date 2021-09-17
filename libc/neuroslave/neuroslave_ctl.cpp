
#include "neuroslave_ctl.h"

NeuroslaveController::NeuroslaveController()
{
	// SIGINT
	attach_sigint(&NeuroslaveController::terminate, this);
	// Server
	d_msg_server = new TCPServer(neuroslave::MSGPORT, 0, d_srv_wait_us);
	// State
	d_state.store(nsv_set_state(0, NSV_STATE_ALIVE, true), std::memory_order_relaxed);
	// Workers
	d_ganglion_comm = new GanglionComm(&d_state, &d_session, &d_eeg_sample_queue);
	d_eeg_distributor = new EegDistributor(&d_state, &d_session, &d_music, &d_game, &d_eeg_sample_queue);
}

NeuroslaveController::~NeuroslaveController()
{
	terminate();
	// Delete workers
	if (d_ganglion_comm != nullptr) {
		delete d_ganglion_comm;
		d_ganglion_comm = nullptr;
	}
	if (d_eeg_distributor != nullptr) {
		delete d_eeg_distributor;
		d_eeg_distributor = nullptr;
	}

	// Delete Internals
	if (d_msg_server != nullptr) {
		delete d_msg_server;
		d_msg_server = nullptr;
	}

	printf("Nsv_ctl: deleted!\n");
}

void NeuroslaveController::launch()
{
	// Start server
	if (!d_msg_server->start()) {
		printf("Msg server start failed!\n");
		return;
	}
	
	// Start threads
	d_msg_thread = std::thread(&NeuroslaveController::msg_process, this);

	// Start workers
	d_eeg_distributor->start();
	d_ganglion_comm->start();

}

void NeuroslaveController::terminate()
{
	printf("Nsv_ctl: terminating... \n");
	d_state.store(nsv_set_state(d_state.load(std::memory_order_relaxed), NSV_STATE_ALIVE, false), std::memory_order_relaxed);

	// Stop server
	d_msg_server->stop();

	// Stop workers
	d_eeg_distributor->stop();
	d_ganglion_comm->stop();
	
	// Wait for msg thread
	if (d_msg_thread.joinable())
		d_msg_thread.join();
}

void NeuroslaveController::wait_die()
{
	while (nsv_get_state(d_state.load(std::memory_order_relaxed), NSV_STATE_ALIVE)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(d_die_wait_ms));
	}
}

void NeuroslaveController::msg_process()
{
	//char msg[d_msg_buf_size];
	std::string msg;
	std::vector<std::string> cmd_vec;
	int bytes_received = 0;
	while (nsv_get_state(d_state.load(std::memory_order_relaxed), NSV_STATE_ALIVE)) {
		// Accept
		if (d_msg_server->acceptSocket()) {
			while (true) {
				// Receive
				msg.resize(d_msg_buf_size);
				bytes_received = d_msg_server->receiveMessage(&msg[0], d_msg_buf_size);
				if (bytes_received > 0) {
					printf("Incoming: [%s]\n", msg.c_str());
					msg.resize(bytes_received);
					cmd_vec = neuroslave::msg2strvec(msg);
					if (!msg_handler(cmd_vec)) {
						printf("MsgSrv msg handler failed\n");
					} 
				// Receive Error
				} else if(errno == EINTR || errno == EAGAIN) {
                	continue;
				} else if (bytes_received == 0) {
					printf("MsgSrv connection closed\n");
					break;
				} else if (bytes_received == SOCKET_ERROR) {
					std::cout << "recv errno:" << errno << "\n";
					printf("MsgSrv socket error\n");
					break;
				}
				if (!nsv_get_state(d_state.load(std::memory_order_relaxed), NSV_STATE_ALIVE))
					break;
			}
		// Accept Error
		} else {
			printf("MsgSrv Accept failed\n");
			std::this_thread::sleep_for(std::chrono::microseconds(d_accept_wait_us));
		}
	}
	d_msg_server->stop();
}

bool NeuroslaveController::msg_handler(std::vector<std::string> & msgvec)
{
	bool result = false;
	if (msgvec.size() <= 0)
		return false;
	if (neuroslave::UserCommandStr.count(msgvec[0]) != 0) {
		switch (neuroslave::UserCommandStr.at(msgvec[0])) {
			case neuroslave::UserCommand::TURN_ON:
				result = turnon();
				break;
			case neuroslave::UserCommand::TURN_OFF:
				result = turnoff();
				break;
			case neuroslave::UserCommand::USER:
				if (msgvec.size() > 1) 
					result = set_user(msgvec[1]);
				else
					result = get_users();
				break;

		}
	}
	// Answer on command
	d_answer.push_back(msgvec[0]);
	if (result) {
		d_answer.push_back(neuroslave::get_str_by_enum(neuroslave::UserAnswerStr, neuroslave::UserAnswer::ACCEPTED));
	} else {
		d_answer.push_back(neuroslave::get_str_by_enum(neuroslave::UserAnswerStr, neuroslave::UserAnswer::DENIED));
	}
	answer();
	// 
	msgvec.clear();
	return result;
}

bool NeuroslaveController::answer()
{
	std::string msg = neuroslave::strvec2msg(d_answer);
	d_answer.clear();
	if (d_msg_server->sendMessage(msg.c_str(), msg.size()) != SOCKET_ERROR)
		return true;
	std::cout << "send errno:" << errno << "\n";
	return false;
}

bool NeuroslaveController::turnon()
{
	// Send session
	d_answer.push_back(neuroslave::get_str_by_enum(neuroslave::UserAnswerStr, neuroslave::UserAnswer::SESSION));
	d_answer.push_back(d_session.to_json());
	if (!answer())
		return false;
	// Turn on
	d_state.store(nsv_set_state(d_state.load(std::memory_order_relaxed), NSV_STATE_SESSION, true), std::memory_order_relaxed);
	// Wait for other workers
	// ???????????
	return true;
}

bool NeuroslaveController::turnoff()
{
	d_state.store(nsv_set_state(d_state.load(std::memory_order_relaxed), NSV_STATE_SESSION, false), std::memory_order_relaxed);
	return true;
}

bool NeuroslaveController::get_users()
{
	d_answer.push_back(neuroslave::get_str_by_enum(neuroslave::UserAnswerStr, neuroslave::UserAnswer::USERS));
	d_answer.push_back(neuroslave::to_json(neuroslave_get_users()));
	if (!answer())
		return false;
	return true;
}

bool NeuroslaveController::set_user(std::string & choosen_user)
{
	std::vector<std::string> users = neuroslave_get_users();
	if (std::count(users.begin(), users.end(), choosen_user) > 0) {
		d_session.user_name = choosen_user;
		return true;
	}
	printf("Unknown user: %s\n", choosen_user.c_str());
	return false;
}

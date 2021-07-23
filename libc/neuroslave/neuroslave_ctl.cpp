
#include "neuroslave_ctl.h"

NeuroslaveController::NeuroslaveController()
{
	d_msg_server = new TCPServer(neuroslave::MSGPORT, 0, d_srv_wait_us);
	// State
	d_state.store(nsv_set_state(0, NSV_STATE_ALIVE, true), std::memory_order_relaxed);
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

}

void NeuroslaveController::terminate()
{
	d_state.store(nsv_set_state(d_state.load(std::memory_order_relaxed), NSV_STATE_ALIVE, false), std::memory_order_relaxed);
	
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
					msg.resize(bytes_received);
					cmd_vec = neuroslave::msg2strvec(msg);
				// Receive Error
				} else if (bytes_received == 0) {
					printf("MsgSrv connection closed\n");
					break;
				} else if (bytes_received == SOCKET_ERROR) {
					printf("MsgSrv socket error\n");
					break;
				}
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

		}
	}
	if (result) {
		answer(msgvec[0], neuroslave::get_str_by_enum(neuroslave::UserAnswerStr, neuroslave::UserAnswer::ACCEPTED));
	}
	msgvec.clear();
	return result;
}

bool NeuroslaveController::answer()
{
	std::string msg = neuroslave::strvec2msg(d_answer);
	d_answer.clear();
	if (d_msg_server->sendMessage(msg.c_str(), msg.size()) != SOCKET_ERROR)
		return true;
	return false;
}

bool NeuroslaveController::turnon()
{
	if (!answer(neuroslave::UserAnswer::SESSION))
		return false;
	d_state.store(nsv_set_state(d_state.load(std::memory_order_relaxed), NSV_STATE_SESSION, true), std::memory_order_relaxed);
	return true;
}

bool NeuroslaveController::turnoff()
{
	d_state.store(nsv_set_state(d_state.load(std::memory_order_relaxed), NSV_STATE_SESSION, false), std::memory_order_relaxed);
	return true;
}

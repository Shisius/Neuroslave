
#include "eeg_distributor.h"

EegDistributor::EegDistributor(std::atomic<nsv_state_t> * state, NeuroslaveSession * session, NeuroslaveMusic * music, NeuroslaveGame * game, 
	threadsafe_queue<EegSamplePack> * eeg_sample_queue) : 
		d_state(state), d_session(session), d_music(music), d_game(game), d_eeg_sample_queue(eeg_sample_queue) 
{
	d_eeg_server = new TCPServer(EEG_DATA_TCP_PORT, 0, d_srv_wait_us);
	d_logger = new StreamLogger(NEUROSLAVE_EEG_PATH);
}

EegDistributor::~EegDistributor() 
{
	// Objects
	if (d_logger != nullptr) {
		delete d_logger;
		d_logger = nullptr;
	}
	// Internals
	if (d_eeg_server != nullptr) {
		delete d_eeg_server;
		d_eeg_server = nullptr;
	}
}

bool EegDistributor::start()
{
	// Start server
	if (!d_eeg_server->start()) {
		printf("EEG server start failed!\n");
		return false;
	}
	// Start thread
	d_data_thread = std::thread(&EegDistributor::eeg_process, this);
	return true;
}

void EegDistributor::stop()
{
	if (d_data_thread.joinable())
		d_data_thread.join();
}

void EegDistributor::prepare_session()
{
	d_state->store(nsv_set_state(d_state->load(std::memory_order_relaxed), NSV_STATE_HANDLER_READY_SESSION, true), std::memory_order_relaxed);
}

void EegDistributor::finish_session()
{
	d_state->store(nsv_set_state(d_state->load(std::memory_order_relaxed), NSV_STATE_HANDLER_READY_SESSION, false), std::memory_order_relaxed);
	// Turn off session
	d_state->store(nsv_set_state(d_state->load(std::memory_order_relaxed), NSV_STATE_SESSION, false), std::memory_order_relaxed);
}

void EegDistributor::prepare_record()
{
	
}

void EegDistributor::eeg_process()
{
	while (nsv_get_state(d_state->load(std::memory_order_relaxed), NSV_STATE_ALIVE)) {
		// Accept
		if (d_eeg_server->acceptSocket()) {
			// Cycle
			while (true) {
				// IDLE
				if (nsv_get_state(d_state->load(std::memory_order_relaxed), NSV_STATE_SESSION)) {
					// PREPARE
					prepare_session();
					/// WAIT FOR SESSION
					while (!nsv_state_session_enabled(d_state->load(std::memory_order_relaxed))) {
						std::this_thread::sleep_for(std::chrono::microseconds(d_session_wait_us));
						if (!nsv_get_state(d_state->load(std::memory_order_relaxed), NSV_STATE_ALIVE)) {
							break;
						}
					}
					/// SESSION
					while (nsv_state_session_enabled(d_state->load(std::memory_order_relaxed))) {
						if (d_eeg_sample_queue->try_pop(d_eeg_pack)) {
							// Send eeg pack
							send_eeg_pack();
							// Record
						}
						std::this_thread::sleep_for(std::chrono::microseconds(d_eeg_wait_us));
					}
					/// STOP SESSION
					finish_session();
				}
				if (!nsv_get_state(d_state->load(std::memory_order_relaxed), NSV_STATE_ALIVE))
					break;
				std::this_thread::sleep_for(std::chrono::microseconds(d_idle_wait_us));
			}
		} else {
			printf("EegSrv Accept failed\n");
			std::this_thread::sleep_for(std::chrono::microseconds(d_accept_wait_us));
		}
	}
	d_eeg_server->stop();
}

bool EegDistributor::send_eeg_pack()
{
	int bytes_sent = 0;
	int msg_size = sizeof(eeg_sample_t) * d_eeg_pack.header.n_samples * d_eeg_pack.header.n_channels + sizeof(d_eeg_pack.header);
	char tmp_msg[msg_size];
	memcpy(tmp_msg, &d_eeg_pack.header, sizeof(EegSampleHeader));
	memcpy(tmp_msg + sizeof(EegSampleHeader), d_eeg_pack.samples, sizeof(eeg_sample_t) * d_eeg_pack.header.n_samples * d_eeg_pack.header.n_channels);
	while (bytes_sent < msg_size) {
		bytes_sent += d_eeg_server->sendMessage(tmp_msg + bytes_sent, sizeof(tmp_msg) - bytes_sent);
		if (bytes_sent == msg_size)
			return true;
		else if (bytes_sent == SOCKET_ERROR)
			break;
		else if (bytes_sent == 0)
			break;
	}
	std::cout << "eeg send errno:" << errno << " bytes_sent = " << bytes_sent << "\n";
	return false;
}

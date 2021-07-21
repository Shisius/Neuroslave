#include "ganglion_comm.h"

bool GanglionComm::start()
{
	if (!ganglion_spi_comm_setup()) return false;
	spi_thread = std::thread(&GanglionComm::spi_process, this);
	return true;
}

void GanglionComm::stop()
{
	if (spi_thread.joinable())
		spi_thread.join();
}

void GanglionComm::spi_process()
{
	/// ALIVE
	while (nsv_get_state(d_state->load(std::memory_order_relaxed), NSV_STATE_ALIVE)) {
		/// IDLE
		if (nsv_get_state(d_state->load(std::memory_order_relaxed), NSV_STATE_SESSION)) {
			/// PREPARE
			prepare();
			/// WAIT FOR SESSION
			while (!nsv_state_session_enabled(d_state->load(std::memory_order_relaxed))) {
				std::this_thread::sleep_for(std::chrono::microseconds(d_session_wait_us));
			}
			/// SESSION
			while (nsv_state_session_enabled(d_state->load(std::memory_order_relaxed))) {
				get_eeg_pack();
				sample_queue.push(cur_pack);
			}
			/// STOP SESSION
			finish();
		}
		std::this_thread::sleep_for(std::chrono::microseconds(d_idle_wait_us));
	}
}

void GanglionComm::prepare()
{
	ganglion_spi_start();
	cur_sample_index = 0;
	cur_mcp_sample.sample_index = 0;
	cur_pack.header.label = EEG_SAMPLE_LABEL;
	cur_pack.header.n_channels = d_session->n_channels;
	cur_pack.header.n_samples = d_session->n_samples_per_pack;
	cur_pack.samples = (eeg_sample_t*) malloc(sizeof(eeg_sample_t) * cur_pack.header.n_channels * cur_pack.header.n_samples);
	d_state->store(nsv_set_state(d_state->load(std::memory_order_relaxed), NSV_STATE_SOURCE_READY_SESSION, true), std::memory_order_relaxed);
}

void GanglionComm::finish() 
{
	ganglion_spi_stop();
	free(cur_pack.samples);
	d_state->store(nsv_set_state(d_state->load(std::memory_order_relaxed), NSV_STATE_SOURCE_READY_SESSION, false), std::memory_order_relaxed);
	// Turn off session
	d_state->store(nsv_set_state(d_state->load(std::memory_order_relaxed), NSV_STATE_SESSION, false), std::memory_order_relaxed);
}

void GanglionComm::get_eeg_pack()
{
	unsigned int i_mcp_sample = 0;
	while (i_mcp_sample < cur_pack.header.n_samples) {
		// Set BAD
		if (cur_mcp_sample.sample_index > cur_sample_index) {
			for (unsigned int i = 0; i < cur_pack.header.n_channels; i++)
				memcpy(cur_pack.samples + cur_pack.header.n_channels * i_mcp_sample + i, &eeg_sample_bad, sizeof(eeg_sample_t));
			cur_sample_index++;
			i_mcp_sample++;
		} else if (cur_mcp_sample.sample_index == cur_sample_index) {
		// Set sample
			memcpy(cur_pack.samples + cur_pack.header.n_channels * i_mcp_sample, cur_mcp_sample.eeg_data, sizeof(eeg_sample_t) * cur_pack.header.n_channels);
			cur_sample_index++;
			i_mcp_sample++;
		} else {
		// Get sample
			if (!ganglion_spi_get_sample(&cur_mcp_sample)) {
				std::this_thread::sleep_for(std::chrono::microseconds(d_spi_wait_us));
			}
		}	
	}
}


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
	if (!d_msg_server.start()) {
		printf("Msg server start failed!\n");
		return;
	}
	


}

void NeuroslaveController::terminate()
{
	d_state.store(nsv_set_state(0, NSV_STATE_ALIVE, false), std::memory_order_relaxed);
	
}

void NeuroslaveController::control_process()
{
	while (nsv_get_state(d_state->load(std::memory_order_relaxed), NSV_STATE_ALIVE)) {

	}
}

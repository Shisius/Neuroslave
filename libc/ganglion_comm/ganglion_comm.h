#ifndef __GANGLION_COMM_H_
#define __GANGLION_COMM_H_

extern "C" {
	#include "ganglion_spi_comm.h"
	#include "neuroslave_state.h"
}
#include <atomic>

class GanglionComm {

protected:

	/// Communication
	std::atomic<nsv_state_t> * state;

	/// Settings
	uint16_t spi_wait_us;

public:

};

#endif //__GANGLION_COMM_H_
#ifndef _QUEUE_MCU_RNR_H_ 
#define _QUEUE_MCU_RNR_H_

#include <atomic>
#include "Ganglion_Slave_Proto.h"

#define MCP_QUEUE_SIZE 256
// size_bl - size bit length
class McpQueue
{

protected:

	McpSample storage[MCP_QUEUE_SIZE];

	volatile uint8_t pop_pos;
	volatile uint8_t push_pos;
	volatile std::atomic<bool> d_lock;
	uint32_t current_index;

public:

	McpQueue() {
		pop_pos = 0;
		push_pos = 0;
		d_lock.store(false, std::memory_order_relaxed);
		current_index = 0;
	}

	~McpQueue() {
		delete [] storage;
	}

	void push(McpSample & sample) {
    	d_lock.store(true, std::memory_order_seq_cst);
		memcpy(storage + push_pos, &sample, sizeof(McpSample));
		push_pos++;
		d_lock.store(false, std::memory_order_seq_cst);	
	}

	void pop_first(McpSample * sample) {
		d_lock.store(true, std::memory_order_seq_cst);
		storage[pop_pos].state = static_cast<uint32_t>(McpSampleState::GOOD);
		memcpy(sample, storage + pop_pos, sizeof(McpSample));
		current_index = storage[pop_pos].sample_index;
		pop_pos++;
		d_lock.store(false, std::memory_order_seq_cst);
	}

	void pop(McpSample * sample) {
		d_lock.store(true, std::memory_order_seq_cst);
		if (storage[pop_pos].sample_index == current_index + 1) {
			storage[pop_pos].state = static_cast<uint32_t>(McpSampleState::GOOD);
			memcpy(sample, storage + pop_pos, sizeof(McpSample));
			current_index++;
			pop_pos++;
		} else if (storage[pop_pos].sample_index < current_index + 1) {
			storage[pop_pos].state = static_cast<uint32_t>(McpSampleState::OLD);
			memcpy(sample, storage + pop_pos, sizeof(McpSample));
		} else {
			storage[pop_pos].state = static_cast<uint32_t>(McpSampleState::SKIPPED);
			memcpy(sample, storage + pop_pos, sizeof(McpSample));
			current_index = storage[pop_pos].sample_index;
			pop_pos++;
		}
		d_lock.store(false, std::memory_order_seq_cst);
	}

};

#endif //_QUEUE_MCU_RNR_H_

#ifndef _QUEUE_MCU_RNR_H_ 
#define _QUEUE_MCU_RNR_H_

#include <atomic>
#include "/home/shisius/Projects/BCI/Neuroslave/libc/ganglion_comm/definitions/ganglion_comm_defines.h"
#define _GLIBCXX_DEQUE_BUF_SIZE MCP_QUEUE_DEPTH * sizeof(McpSample)
#include <queue>


class McpQueue
{
protected:

	std::queue<McpSample> d_queue;
	volatile std::atomic<bool> d_lock;

public:

	McpQueue() {
		d_lock.store(false, std::memory_order_relaxed);
	}

	~McpQueue() {
		
	}

	void reset() {
    std::queue<McpSample> new_queue;
		d_lock.store(false, std::memory_order_relaxed);
		d_queue.swap(new_queue);
	}

  size_t size() {
    return d_queue.size();
  }

	bool overflow() {
		return d_queue.size() >= MCP_QUEUE_DEPTH;
	}

	bool empty() {
		return d_queue.empty();
	}

	void push(McpSample & sample) {
		//if (overflow()) return false;
    	d_lock.store(true, std::memory_order_seq_cst);
    	if (!overflow())
    		d_queue.push(sample);	
		d_lock.store(false, std::memory_order_seq_cst);	
		//return true;
	}

	void pop(McpSample * sample) {
		d_lock.store(true, std::memory_order_seq_cst);
		if (!d_queue.empty()) {
			memcpy(sample, &d_queue.front(), sizeof(McpSample));
			d_queue.pop();
		}
		d_lock.store(false, std::memory_order_seq_cst);
	}

};

#endif //_QUEUE_MCU_RNR_H_

#ifndef _QUEUE_MCU_RNR_H_ 
#define _QUEUE_MCU_RNR_H_

#include <atomic>

// size_bl - size bit length
template<typename T, uint8_t size_bl>
class QueueRNR
{

private:

	T storage[1 << (size_bl - 1)];

	uint32_t pop_pos = 0;
	uint32_t push_pos = 0;
	std::atomic_bool lock;
	uint32_t mask = (1 << (size_bl - 1)) - 1;
	uint32_t size = 1 << (size_bl - 1);

public:

	QueueRNR() {
		pop_pos = 0;
		push_pos = 0;
		lock.store(false, std::memory_order_relaxed);
	}

	~QueueRNR() {
		delete [] storage;
	}

	void push(T & sample) {
		while (lock.load(std::memory_order_relaxed)) {}
    lock.store(true, std::memory_order_relaxed);
		push_pos++;
		if (push_pos == size) push_pos = 0;
		memcpy(storage + push_pos, &sample, sizeof(T));
		lock.store(false, std::memory_order_relaxed);	
	}


	// Block pop when nothing pushed?
	T pop() {
		T sample;

		while (lock.load(std::memory_order_relaxed)) {}
		lock.store(true, std::memory_order_relaxed);
		pop_pos++;
		if (pop_pos == size) pop_pos = 0;
		sample = storage[pop_pos];
		lock.store(false, std::memory_order_relaxed);
		
		return sample;
	}

	void pop(T & sample) {
		while (lock.load(std::memory_order_relaxed)) {}
		lock.store(true, std::memory_order_relaxed);
		pop_pos++;
		if (pop_pos == size) pop_pos = 0;
		memcpy(&sample, storage + pop_pos, sizeof(T));
		lock.store(false, std::memory_order_relaxed);
	}

	void pop(T * sample) {
		while (lock.load(std::memory_order_relaxed)) {}
		lock.store(true, std::memory_order_relaxed);
		pop_pos++;
		if (pop_pos == size) pop_pos = 0;
		memcpy(sample, storage + pop_pos, sizeof(T));
		lock.store(false, std::memory_order_relaxed);
	}

	void pop_new(T * sample) {
		while (!lock.load(std::memory_order_relaxed)) {} // Wait push
		while (lock.load(std::memory_order_relaxed)) {}
		lock.store(true, std::memory_order_relaxed);
		pop_pos = push_pos;
		memcpy(sample, storage + pop_pos, sizeof(T));
		lock.store(false, std::memory_order_relaxed);
	}

	void pop_wait(T * sample) {
		while (true) {
        while (lock.load(std::memory_order_relaxed)) {}
        lock.store(true, std::memory_order_relaxed);
		    if (push_pos == ((pop_pos + 1) & mask)) { 
            lock.store(false, std::memory_order_relaxed);
            break;
		    }
        lock.store(false, std::memory_order_relaxed);
		}
		pop(sample);	
	}

};

#endif //_QUEUE_MCU_RNR_H_

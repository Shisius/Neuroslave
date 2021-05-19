#ifndef _QUEUE_MCU_RNR_H_ 
#define _QUEUE_MCU_RNR_H_

#include <atomic>

// size_bl - size bit length
template<typename T, uint8_t size_bl>
class QueueRNR
{

protected:

	T storage[1 << size_bl];

	volatile std::atomic<uint32_t> pop_pos;
	volatile std::atomic<uint32_t> push_pos;
	volatile std::atomic<bool> d_lock;
	uint32_t mask = (1 << size_bl) - 1;
	uint32_t qsize = 1 << size_bl;

public:

	QueueRNR() {
		pop_pos.store(0);
		push_pos.store(0);
		d_lock.store(false, std::memory_order_relaxed);
	}

	~QueueRNR() {
		delete [] storage;
	}

	void push(T & sample) {
		//while (d_lock.load(std::memory_order_relaxed)) {delay(1);}
    	d_lock.store(true, std::memory_order_seq_cst);
		push_pos.store(push_pos.load() + 1);
		if (push_pos.load() == qsize) push_pos.store(0);
		memcpy(storage + push_pos.load(), &sample, sizeof(T));
		d_lock.store(false, std::memory_order_seq_cst);	
	}


	// Block pop when nothing pushed?
//	T pop() {
//		T sample;
//
//		while (d_lock.load(std::memory_order_relaxed)) {}
//		d_lock.store(true, std::memory_order_relaxed);
//		pop_pos++;
//		if (pop_pos == qsize) pop_pos = 0;
//		sample = storage[pop_pos];
//		d_lock.store(false, std::memory_order_relaxed);
//		
//		return sample;
//	}

//	void pop(T & sample) {
//		while (d_lock.load(std::memory_order_relaxed)) {}
//		d_lock.store(true, std::memory_order_relaxed);
//		pop_pos++;
//		if (pop_pos == qsize) pop_pos = 0;
//		memcpy(&sample, storage + pop_pos, sizeof(T));
//		d_lock.store(false, std::memory_order_relaxed);
//	}

	void pop(T * sample) {
		//while (d_lock.load(std::memory_order_relaxed)) {delay(1);}
		d_lock.store(true, std::memory_order_seq_cst);
		pop_pos.store(pop_pos.load() + 1);
		if (pop_pos.load() == qsize) pop_pos.store(0);
		memcpy(sample, storage + pop_pos.load(), sizeof(T));
		d_lock.store(false, std::memory_order_seq_cst);
	}

	void pop_new(T * sample) {
		while (!(d_lock.load(std::memory_order_relaxed))) {delay(1);} // Wait push
		while (d_lock.load(std::memory_order_relaxed)) {delay(1);}
		d_lock.store(true, std::memory_order_relaxed);
		pop_pos.store(push_pos.load());
		memcpy(sample, storage + pop_pos.load(), sizeof(T));
		d_lock.store(false, std::memory_order_relaxed);
	}

	void pop_wait(T * sample) {
		while (true) {
	        //while (d_lock.load(std::memory_order_relaxed)) {delay(1);}
	        d_lock.store(true, std::memory_order_seq_cst);
			if (push_pos.load(std::memory_order_relaxed) == ((pop_pos.load(std::memory_order_relaxed) + 1) & mask)) { 
	            break;
			}
	        Serial.println(push_pos.load(std::memory_order_relaxed));
	        d_lock.store(false, std::memory_order_seq_cst);
	        
		}
		pop(sample);	
	}

};

#endif //_QUEUE_MCU_RNR_H_

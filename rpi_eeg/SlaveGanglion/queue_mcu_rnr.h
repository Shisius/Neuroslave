#ifndef _QUEUE_MCU_RNR_H_ 
#define _QUEUE_MCU_RNR_H_

// size_bl - size bit length
template<typename T, uint8_t size_bl>
class QueueRNR
{

private:

	T storage[1 << (size_bl - 1)];

	uint32_t pop_pos = 0;
	uint32_t push_pos = 0;
	volatile bool lock = false;
	uint32_t mask = (1 << (size_bl - 1)) - 1;
	uint32_t size = 1 << (size_bl - 1);

public:

	QueueRNR() {
		pop_pos = 0;
		push_pos = 0;
		lock = false;
	}

	~QueueRNR() {
		delete [] storage;
	}

	void push(T & sample) {
		while (lock) {}
		lock = true;
		push_pos++;
		if (push_pos == size) push_pos = 0;
		memcpy(storage + push_pos, &sample, sizeof(T));
		lock = false;	
	}


	// Block pop when nothing pushed?
	T pop() {
		T sample;

		while (lock) {}
		lock = true;
		pop_pos++;
		if (pop_pos == size) pop_pos = 0;
		sample = storage[pop_pos];
		lock = false;
		
		return sample;
	}

	void pop(T & sample) {
		while (lock) {}
		lock = true;
		pop_pos++;
		if (pop_pos == size) pop_pos = 0;
		memcpy(&sample, storage + pop_pos, sizeof(T));
		lock = false;
	}

	void pop(T * sample) {
		while (lock) {}
		lock = true;
		pop_pos++;
		if (pop_pos == size) pop_pos = 0;
		memcpy(sample, storage + pop_pos, sizeof(T));
		lock = false;
	}

	void pop_new(T * sample) {
		while (!lock) {} // Wait push
		while (lock) {}
		lock = true;
		pop_pos = push_pos;
		memcpy(sample, storage + pop_pos, sizeof(T));
		lock = false;
	}

	void pop_wait(T * sample) {
		while (push_pos != ((pop_pos + 1) & mask)) {}
		pop(sample);	
	}

};

#endif //_QUEUE_MCU_RNR_H_

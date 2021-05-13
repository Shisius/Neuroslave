#ifndef _QUEUE_MCU_RNR_H_ 
#define _QUEUE_MCU_RNR_H_

template<typename T, uint32_t size>
class QueueRNR
{

private:

	T storage[size];

	uint32_t pop_pos = 0;
	uint32_t push_pos = 0;
	bool lock = false;

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
		if (push_pos == pop_pos) {
			while (lock) {}
			lock = true;
			memcpy(storage + push_pos, &sample, sizeof(T));
			push_pos++;
			lock = false;
		} else {
			memcpy(storage + push_pos, &sample, sizeof(T));
			push_pos++;
		}
		if (push_pos == size) push_pos = 0;
	}


	// Block pop when nothing pushed?
	T pop() {
		T sample;
		if (push_pos == pop_pos) {
			while (lock) {}
			lock = true;
			sample = storage[pop_pos];
			pop_pos++;
			lock = false;
		} else {
			sample = storage[pop_pos];
			pop_pos++;
		}
		if (pop_pos == size) pop_pos = 0;
		return sample;
	}

	void pop(T & sample) {
		if (push_pos == pop_pos) {
			while (lock) {}
			lock = true;
			memcpy(&sample, storage + push_pos, sizeof(T));
			pop_pos++;
			lock = false;
		} else {
			memcpy(&sample, storage + push_pos, sizeof(T));
			pop_pos++;
		}
		if (pop_pos == size) pop_pos = 0;
	}

	void pop(T * sample) {
		if (push_pos == pop_pos) {
			while (lock) {}
			lock = true;
			memcpy(sample, storage + push_pos, sizeof(T));
			pop_pos++;
			lock = false;
		} else {
			memcpy(sample, storage + push_pos, sizeof(T));
			pop_pos++;
		}
		if (pop_pos == size) pop_pos = 0;
	}

};

#endif //_QUEUE_MCU_RNR_H_

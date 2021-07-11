#ifndef _NEUROSLAVE_DATA_H_
#define _NEUROSLAVE_DATA_H_

#define EEG_SAMPLE_LABEL 0xACDC
#define EEG_SAMPLE_BAD 0x7BADBAD7
/**
 * EEG sample pack
 */
typedef int eeg_sample_t; // 32 bit

typedef struct EegSampleHeader EegSampleHeader;
struct EegSampleHeader {
	unsigned short label;
	unsigned char n_channels;
	unsigned char n_samples;
};

typedef struct EegSamplePack EegSamplePack;
struct EegSamplePack {
	EegSampleHeader header;
	eeg_sample_t * samples;
};

#endif //_NEUROSLAVE_DATA_H_

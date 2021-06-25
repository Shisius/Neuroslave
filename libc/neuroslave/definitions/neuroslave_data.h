#ifndef _NEUROSLAVE_DATA_H_
#define _NEUROSLAVE_DATA_H_

#define EEG_SAMPLE_LABEL 0xACDC

/**
 * EEG sample pack
 */
typedef int eeg_sample_t; // 32 bit
typedef enum {
	EEG_SAMPLE_NO_INFO = 0,
	EEG_SAMPLE_SKIPPED
} EegSampleInfoType;

typedef struct EegSampleHeader EegSampleHeader;
struct EegSampleHeader {
	unsigned short label;
	unsigned short n_samples;
	unsigned char n_channels;
	unsigned char info_type;
	unsigned short info;
};

typedef struct EegSamplePack EegSamplePack;
struct EegSamplePack {
	EegSampleHeader header;
	eeg_sample_t * samples;
};

#endif //_NEUROSLAVE_DATA_H_

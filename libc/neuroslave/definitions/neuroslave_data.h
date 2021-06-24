#ifndef _NEUROSLAVE_DATA_H_
#define _NEUROSLAVE_DATA_H_

#define EEG_SAMPLE_LABEL 0xACDC

/**
 * EEG sample pack
 */
typedef enum {
	NO_INFO = 0x0

} EEG_SAMPLE_INFO_TYPE;

typedef struct EegSampleHeader EegSampleHeader
struct EegSampleHeader {
	uint16_t label;
	uint16_t n_samples;
	uint8_t n_channels;
	uint8_t info_type;
	uint16_t info;
};

#endif //_NEUROSLAVE_DATA_H_

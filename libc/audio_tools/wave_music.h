#ifndef _WAVE_MUSIC_H_
#define _WAVE_MUSIC_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "portaudio.h"

extern const char WAVE_FILE_HEADER_LE[4];
extern const char WAVE_FILE_TYPE_HEADER[4];
extern const char WAVE_FILE_FMT_LABEL[4];
extern const char WAVE_FILE_DATA_LABEL[4];
//#define WAVE_FILE_FIELDS_SIZE 4

#define PA_DEFAULT_FRAMES_PER_BUFFER (64)

typedef enum {
	WAVE_FORMAT_TYPE_PCM = 1
} WaveFormatType;

typedef struct {
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate; // bytes per sample * num_channels * sample_rate
	uint16_t block_align; // bytes per sample * num_channels
	uint16_t bits_per_sample;
} WaveFileFormat;

typedef struct {
	PaStreamParameters stream_parameters;
	PaStream * stream;
	unsigned long frames_per_buffer;
} PortAudioParameters;

typedef struct WaveMusic WaveMusic;
struct WaveMusic {
	char * music_file_name;
	FILE * music_file;
	uint32_t music_length; // In blocks
	WaveFileFormat music_format;
	char * music_data;
	uint32_t music_position; // In blocks
	PortAudioParameters pa_parameters;
	// methods
	bool (*play)(WaveMusic*);
	bool (*stop)(WaveMusic*);
	bool (*is_playing)(WaveMusic*);
};

/// Constructors
bool createWaveMusic(WaveMusic * wave_music, const char * file_name);
bool openWaveMusic(WaveMusic * wave_music, const char * file_name);
bool readFileWaveMusic(WaveMusic * wave_music, const char * file_name);
bool setupAudioWaveMusic(WaveMusic * wave_music);

/// Methods 
bool playWaveMusic(WaveMusic * wave_music);
bool stopWaveMusic(WaveMusic * wave_music);
bool is_playingWaveMusic(WaveMusic * wave_music);

/// Destructors
void closeWaveMusic(WaveMusic * wave_music);

/// Portaudio functions
static int paStreamCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, 
					  PaStreamCallbackFlags statusFlags, void * userData );
static void paFinishedCallback(void* userData);

#endif //_WAVE_MUSIC_H_

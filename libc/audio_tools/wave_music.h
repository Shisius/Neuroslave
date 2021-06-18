#ifndef _WAVE_MUSIC_H_
#define _WAVE_MUSIC_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "portaudio.h"

#define WAVE_FILE_HEADER_LE "RIFF"
#define WAVE_FILE_TYPE_HEADER "WAVE"
#define WAVE_FILE_FMT_LABEL "fmt "
#define WAVE_FILE_DATA_LABEL "data"
//#define WAVE_FILE_FIELDS_SIZE 4

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

} PortAudioParameters;

typedef struct WaveMusic WaveMusic;
struct WaveMusic {
	char * music_file_name;
	FILE * music_file;
	uint32_t music_length; // In blocks
	WaveFileFormat music_format;
	char * music_data;
	uint32_t music_position; // In blocks
	// methods
	void (*play)(WaveMusic*);
	void (*stop)(WaveMusic*);
	void (*is_playing)(WaveMusic*);
};

/// Constructors
bool createWaveMusic(WaveMusic * wave_music, const char * file_name);
bool openWaveMusic(WaveMusic * wave_music, const char * file_name);
bool readFileWaveMusic(WaveMusic * wave_music, const char * file_name);
bool setupAudioWaveMusic(WaveMusic * wave_music);

/// Methods 
void playWaveMusic(WaveMusic * wave_music);
void stopWaveMusic(WaveMusic * wave_music);
bool is_playingWaveMusic(WaveMusic * wave_music);

/// Destructors
void closeWaveMusic(WaveMusic * wave_music);

#endif //_WAVE_MUSIC_H_

#ifndef _WAVE_MUSIC_H_
#define _WAVE_MUSIC_H_

#include <stdio.h>
#include <string.h>

#define WAVE_FILE_HEADER_LE "RIFF"
//#define WAVE_FILE_FIELDS_SIZE 4

typedef struct WaveMusic WaveMusic;
struct WaveMusic {
	char * file_name;
	FILE * music_file;
	size_t music_length; // In samples
	// methods
	void (*play)(WaveMusic*);
	void (*stop)(WaveMusic*);
	void (*is_playing)(WaveMusic*);
};

/// Constructors
bool createWaveMusic(WaveMusic * wave_music, const char * file_name);
bool openWaveMusic(WaveMusic * wave_music, const char * file_name);

/// Methods 
void playWaveMusic(WaveMusic * wave_music);
void stopWaveMusic(WaveMusic * wave_music);
bool is_playingWaveMusic(WaveMusic * wave_music);

/// Destructors
void closeWaveMusic(WaveMusic * wave_music);

#endif //_WAVE_MUSIC_H_

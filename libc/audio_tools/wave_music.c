
#include "wave_music.h"

/// Constructors
bool openWaveMusic(WaveMusic * wave_music, const char * file_name)
{
	// Local
	size_t full_file_size = 0; // without header
	char field_buffer4[4];
	char field_buffer2[2];
	// File name
	wave_music->file_name = (char*) malloc(strlen(file_name));
	strcpy(wave_music->file_name, file_name);
	// Open file
	wave_music->music_file = fopen(wave_music->file_name, "rb");
	if (wave_music->music_file == NULL) {
		prinf("WaveMusic: error opening file %s\n", file_name);
		return false;
	}
	// Header & File size without header
	if (fread(field_buffer4, sizeof(field_buffer4), wave_music->music_file) != sizeof(field_buffer4))
		return false;
	if (strcmp(WAVE_FILE_HEADER_LE, field_buffer) != 0) {
		printf("WaveMusic: No RIFF label: %s\n", field_buffer);
	}
	if (fread(field_buffer4, sizeof(field_buffer4), wave_music->music_file) != sizeof(field_buffer4))
		return false;


}


/// Destructors
void closeWaveMusic(WaveMusic * wave_music)
{
	// Close file
	if (wave_music->music_file != NULL)
		fclose(wave_music->music_file);
	// Free
	free(wave_music->file_name);
}

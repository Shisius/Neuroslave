
#include "wave_music.h"

/// Constructors
bool readFileWaveMusic(WaveMusic * wave_music, const char * file_name)
{
	// Local
	uint32_t full_file_size = 0; // without header
	uint32_t section_size = 0;
	char field_buffer4[4];
	char field_buffer2[2];
	char section_label[4];
	// File name
	wave_music->music_file_name = (char*) malloc(strlen(file_name));
	strcpy(wave_music->music_file_name, file_name);
	// Open file
	wave_music->music_file = fopen(wave_music->music_file_name, "rb");
	if (wave_music->music_file == NULL) {
		prinf("WaveMusic: error opening file %s\n", file_name);
		return false;
	}
	// Header & File size without header
	if (fread(field_buffer4, sizeof(field_buffer4), wave_music->music_file) != sizeof(field_buffer4))
		fclose(wave_music->music_file); return false;
	if (strcmp(WAVE_FILE_HEADER_LE, field_buffer4) != 0) {
		printf("WaveMusic: No RIFF label: %s\n", field_buffer4);
	}
	if (fread(field_buffer4, sizeof(field_buffer4), wave_music->music_file) != sizeof(field_buffer4))
		fclose(wave_music->music_file); return false;
	memcpy(&full_file_size, field_buffer4, sizeof(field_buffer4));
	// Type Header
	if (fread(field_buffer4, sizeof(field_buffer4), wave_music->music_file) != sizeof(field_buffer4))
		fclose(wave_music->music_file); return false;
	if (strcmp(WAVE_FILE_TYPE_HEADER, field_buffer4) != 0) {
		printf("WaveMusic: No WAVE label: %s\n", field_buffer4);
	}
	// Parse other sections. Find "fmt " and "data".
	while (true) {
		// Label
		if (fread(field_buffer4, sizeof(field_buffer4), wave_music->music_file) != sizeof(field_buffer4)) {
			printf("WaveMusic: No data found\n");
			fclose(wave_music->music_file); return false;
		}
		memcpy(section_label, field_buffer4, sizeof(field_buffer4));
		// Size
		if (fread(field_buffer4, sizeof(field_buffer4), wave_music->music_file) != sizeof(field_buffer4)) {
			printf("WaveMusic: No data size found\n");
			fclose(wave_music->music_file); return false;
		}
		memcpy(&section_size, field_buffer4, sizeof(field_buffer4));
		// Process section
		if (strcmp(WAVE_FILE_FMT_LABEL, section_label) == 0) {
			if (section_size != sizeof(WaveFileFormat)) {
				printf("WaveMusic: wrong WaveFileFormat size\n");
				fclose(wave_music->music_file); return false;
			}
			if (fread(&(wave_music->music_format), sizeof(WaveFileFormat), wave_music->music_file) != sizeof(WaveFileFormat)) {
				printf("WaveMusic: cannot read format section\n");
				fclose(wave_music->music_file); return false;
			}
			if (wave_music->music_format.audio_format != WAVE_FORMAT_TYPE_PCM) {
				printf("WaveMusic: audio format != PCM\n");
				fclose(wave_music->music_file); return false;
			}
		} else if (strcmp(WAVE_FILE_DATA_LABEL, section_label) == 0) {
			if (section_size % wave_music->music_format.block_align != 0) {
				printf("WaveMusic: music data is not divided by block_align\n");
				fclose(wave_music->music_file); return false;
			}
			wave_music->music_data = (char*)malloc(section_size);
			if (fread(wave_music->music_data, section_size, wave_music->music_file) != section_size) {
				printf("WaveMusic: not enough music data\n");
				fclose(wave_music->music_file); return false;
			}
			wave_music->music_length = section_size / wave_music->music_format.block_align;
			wave_music->music_position = 0;
			break;
		} else {
			if (fseek(wave_music->music_file, section_size, SEEK_CUR) != 0) {
				printf("WaveMusic: Cannot skip wave section\n");
				fclose(wave_music->music_file); return false;
			}
		}
	}
	fclose(wave_music->music_file);
	return true;
}


/// Destructors
void closeWaveMusic(WaveMusic * wave_music)
{
	// Close file
	if (wave_music->music_file != NULL)
		fclose(wave_music->music_file);
	// Free
	free(wave_music->file_name);
	free(wave_music->music_data);
}

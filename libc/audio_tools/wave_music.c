
#include "wave_music.h"

const char WAVE_FILE_HEADER_LE[4] = {'R','I','F','F'};
const char WAVE_FILE_TYPE_HEADER[4] = {'W', 'A', 'V', 'E'};
const char WAVE_FILE_FMT_LABEL[4] = {'f', 'm', 't', ' '};
const char WAVE_FILE_DATA_LABEL[4] = {'d', 'a', 't', 'a'};

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
		printf("WaveMusic: error opening file %s\n", file_name);
		return false;
	}
	// Header & File size without header
	if (fread(field_buffer4, sizeof(char), sizeof(field_buffer4), wave_music->music_file) != sizeof(field_buffer4)) {
		fclose(wave_music->music_file); return false;
	}
	if (strncmp(WAVE_FILE_HEADER_LE, field_buffer4, sizeof(WAVE_FILE_HEADER_LE)) != 0) {
		printf("WaveMusic: No RIFF label: %s\n", field_buffer4);
		fclose(wave_music->music_file); return false;
	}
	if (fread(field_buffer4, sizeof(char), sizeof(field_buffer4), wave_music->music_file) != sizeof(field_buffer4)) {
		fclose(wave_music->music_file); return false;
	}
	memcpy(&full_file_size, field_buffer4, sizeof(field_buffer4));
	// Type Header
	if (fread(field_buffer4, sizeof(char), sizeof(field_buffer4), wave_music->music_file) != sizeof(field_buffer4)) {
		fclose(wave_music->music_file); return false;
	}
	if (strncmp(WAVE_FILE_TYPE_HEADER, field_buffer4, sizeof(WAVE_FILE_TYPE_HEADER)) != 0) {
		printf("WaveMusic: No WAVE label: %s\n", field_buffer4);
		fclose(wave_music->music_file); return false;
	}
	// Parse other sections. Find "fmt " and "data".
	while (true) {
		// Label
		if (fread(field_buffer4, sizeof(char), sizeof(field_buffer4), wave_music->music_file) != sizeof(field_buffer4)) {
			printf("WaveMusic: No data found\n");
			fclose(wave_music->music_file); return false;
		}
		memcpy(section_label, field_buffer4, sizeof(field_buffer4));
		// Size
		if (fread(field_buffer4, sizeof(char), sizeof(field_buffer4), wave_music->music_file) != sizeof(field_buffer4)) {
			printf("WaveMusic: No data size found\n");
			fclose(wave_music->music_file); return false;
		}
		memcpy(&section_size, field_buffer4, sizeof(field_buffer4));
		// Process section
		if (strncmp(WAVE_FILE_FMT_LABEL, section_label, sizeof(WAVE_FILE_FMT_LABEL)) == 0) {
			if (section_size != sizeof(WaveFileFormat)) {
				printf("WaveMusic: wrong WaveFileFormat size\n");
				fclose(wave_music->music_file); return false;
			}
			if (fread(&(wave_music->music_format), sizeof(char), sizeof(WaveFileFormat), wave_music->music_file) != sizeof(WaveFileFormat)) {
				printf("WaveMusic: cannot read format section\n");
				fclose(wave_music->music_file); return false;
			}
			if (wave_music->music_format.audio_format != WAVE_FORMAT_TYPE_PCM) {
				printf("WaveMusic: audio format != PCM\n");
				fclose(wave_music->music_file); return false;
			}
		} else if (strncmp(WAVE_FILE_DATA_LABEL, section_label, sizeof(WAVE_FILE_DATA_LABEL)) == 0) {
			if (section_size % wave_music->music_format.block_align != 0) {
				printf("WaveMusic: music data is not divided by block_align\n");
				fclose(wave_music->music_file); return false;
			}
			wave_music->music_data = (char*)malloc(section_size);
			if (fread(wave_music->music_data, sizeof(char), section_size, wave_music->music_file) != section_size) {
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

bool setupAudioWaveMusic(WaveMusic * wave_music)
{
	// Initialize
	if (Pa_Initialize() != paNoError) {
		printf("WaveMusic: Pa_Initialize failed\n");
		return false;
	}
	// Device
	wave_music->pa_parameters.stream_parameters.device = Pa_GetDefaultOutputDevice();
	if (wave_music->pa_parameters.stream_parameters.device == paNoDevice) {
		printf("WaveMusic: no output device found\n");
		return false;
	}
	// Set stream parameters
	wave_music->pa_parameters.stream_parameters.channelCount = wave_music->music_format.num_channels;
	wave_music->pa_parameters.stream_parameters.suggestedLatency = Pa_GetDeviceInfo( wave_music->pa_parameters.stream_parameters.device )->defaultLowOutputLatency;
	wave_music->pa_parameters.stream_parameters.hostApiSpecificStreamInfo = NULL;
	switch (wave_music->music_format.audio_format) {
		case WAVE_FORMAT_TYPE_PCM:
			switch (wave_music->music_format.bits_per_sample) {
				case 16:
					wave_music->pa_parameters.stream_parameters.sampleFormat = paInt16;
					break;
				case 24:
					wave_music->pa_parameters.stream_parameters.sampleFormat = paInt24;
					break;
				case 32:
					wave_music->pa_parameters.stream_parameters.sampleFormat = paInt32;
					break;
				default:
					printf("WaveMusic: unsupported bits_par_sample\n");
					return false;
			}
			break;
		default:
			printf("WaveMusic: unknown audio format\n");
			return false;
	}
	wave_music->pa_parameters.frames_per_buffer = PA_DEFAULT_FRAMES_PER_BUFFER;
	// Open stream
	if (Pa_OpenStream(&(wave_music->pa_parameters.stream), NULL, &(wave_music->pa_parameters.stream_parameters), 
					  wave_music->music_format.sample_rate, wave_music->pa_parameters.frames_per_buffer, paClipOff,
					  paStreamCallback, wave_music) != paNoError) {
		printf("WaveMusic: Pa_OpenStream failed\n");
		return false;
	}
	// Finished callback
	if (Pa_SetStreamFinishedCallback(wave_music->pa_parameters.stream, &paFinishedCallback) != paNoError) {
		printf("WaveMusic: set finished callback failed\n");
		return false;
	}
	return true;
}

bool openWaveMusic(WaveMusic * wave_music, const char * file_name)
{
	wave_music->music_data = NULL;
	wave_music->music_file_name = NULL;
	// Open file
	if (!readFileWaveMusic(wave_music, file_name)) return false;
	// Setup port audio
	if (!setupAudioWaveMusic(wave_music)) return false;
	// Prepare for working
	wave_music->play = playWaveMusic;
	wave_music->stop = stopWaveMusic;
	wave_music->is_playing = is_playingWaveMusic;
	return true;
}

/// Destructors
void closeWaveMusic(WaveMusic * wave_music)
{
	// Close file
	//if (wave_music->music_file != NULL)
		//fclose(wave_music->music_file);
	//printf("File closed\n");
	// Stop port audio
	Pa_CloseStream(wave_music->pa_parameters.stream);
	Pa_Terminate();
	// Free
	if (wave_music->music_file_name != NULL)
		free(wave_music->music_file_name);
	if (wave_music->music_data != NULL)
		free(wave_music->music_data);
}

/// Methods
bool playWaveMusic(WaveMusic * wave_music)
{
	if (Pa_StartStream(wave_music->pa_parameters.stream) != paNoError) {
		printf("WaveMusic: Pa_StartStream failed\n");
		return false;
	}
	return true;
}

bool stopWaveMusic(WaveMusic * wave_music)
{
	if (Pa_StopStream(wave_music->pa_parameters.stream) != paNoError) {
		printf("WaveMusic: Pa_StopStream failed\n");
		return false;
	}
	return true;
}

bool is_playingWaveMusic(WaveMusic * wave_music)
{
	if (Pa_IsStreamActive(wave_music->pa_parameters.stream) == 1)
		return true;
	return false;
}

/// Portaudio functions
static int paStreamCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, 
					  PaStreamCallbackFlags statusFlags, void * userData )
{
	WaveMusic * wave_music = (WaveMusic*)userData;
	(void) timeInfo;
	(void) statusFlags;
	(void) inputBuffer;

	if (wave_music->music_length >= wave_music->music_position + framesPerBuffer) {
		memcpy(outputBuffer, wave_music->music_data + wave_music->music_position * wave_music->music_format.block_align, 
		   	   framesPerBuffer * wave_music->music_format.block_align);
		wave_music->music_position += framesPerBuffer;
		return paContinue;
	} else {
		memcpy(outputBuffer, wave_music->music_data + wave_music->music_position * wave_music->music_format.block_align, 
		   	   (wave_music->music_length - wave_music->music_position) * wave_music->music_format.block_align);
		wave_music->music_position = wave_music->music_length;
		return paComplete;
	}
}

static void paFinishedCallback(void* userData)
{
	WaveMusic * wave_music = (WaveMusic*)userData;
	//wave_music->stop(wave_music);
}

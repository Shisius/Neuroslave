#ifndef _NEUROSLAVE_STRUCT_H_
#define _NEUROSLAVE_STRUCT_H_

#include <string>

struct NeuroslaveSession {
	std::string tag; // Session name
	std::string user_name;
	unsigned int sample_rate; // Sample rate in Hz
	unsigned int n_channels; // Number of data channels. This value should be equal to number of plotted curves.
	unsigned int n_samples_per_pack;
	unsigned int gain; // Gain value for amplifier
	unsigned int tcp_decimation; // Data decimation for tcp binary port

	NeuroslaveSession() : tag(""), user_name(""), sample_rate(1600), n_channels(2), n_samples_per_pack(10), gain(1), tcp_decimation(1) {}
};

struct NeuroslaveEegData {

};

struct NeuroslaveMusic {
	std::string name;
	unsigned char n_channels;
	unsigned char bits_per_sample;
	unsigned int sample_rate;
	unsigned int duration_us;
	unsigned int start_sample; // Number of start sample;
	unsigned int end_sample;
};

struct NeuroslaveGameSettings {
	std::string subfolder; // Subfolder in playlist folder
	float duration; // in seconds
	float volume; // from 0 to 100%
	uint8_t complexity; // from 2 to 6. Number of music files for choose
};

struct NeuroslaveGame {
	NeuroslaveGameSettings settings;
	std::string right_music;
	std::string guessed_music;
	bool user_wins;
};

struct EegRecord {
	NeuroslaveSession session;
	NeuroslaveMusic music;
	NeuroslaveGame game;
	unsigned long long start_time_ns;
	
};

#endif //_NEUROSLAVE_STRUCT_H_

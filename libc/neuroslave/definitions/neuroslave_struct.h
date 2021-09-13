#ifndef _NEUROSLAVE_STRUCT_H_
#define _NEUROSLAVE_STRUCT_H_

#include <string>
#include <vector>
#include "neuroslave_data.h"
#include "writer.h"
#include "document.h"

struct NeuroslaveSession {
	std::string tag; // Session name
	std::string user_name;
	unsigned int sample_rate; // Sample rate in Hz
	unsigned int n_channels; // Number of data channels. This value should be equal to number of plotted curves.
	unsigned int n_samples_per_pack;
	unsigned int gain; // Gain value for amplifier
	unsigned int tcp_decimation; // Data decimation for tcp binary port

	NeuroslaveSession() : tag(""), user_name(""), sample_rate(1600), n_channels(2), n_samples_per_pack(10), gain(1), tcp_decimation(1) {}

	std::string to_json();

	bool from_json(const std::string & json_str);

};

struct NeuroslaveEegData {
	std::vector<EegSamplePack> packs;

	std::string to_json();

};

struct NeuroslaveMusic {
	std::string name;
	unsigned int n_channels;
	unsigned int bits_per_sample;
	unsigned int sample_rate;
	unsigned int duration_us;
	unsigned int start_sample; // Number of start sample;
	unsigned int end_sample;

	std::string to_json();
};

struct NeuroslaveGameSettings {
	std::string subfolder; // Subfolder in playlist folder
	float duration; // in seconds
	float volume; // from 0 to 100%
	uint8_t complexity; // from 2 to 6. Number of music files for choose

	std::string to_json();

};

struct NeuroslaveGame {
	NeuroslaveGameSettings settings;
	std::string right_music;
	std::string guessed_music;
	bool user_wins;

	std::string to_json();

};

struct NeuroslaveRecord {
	NeuroslaveSession session;
	NeuroslaveMusic music;
	NeuroslaveGame game;
	NeuroslaveEegData eeg;
	unsigned long long start_time_ns;

	std::string to_json();
	
};

#endif //_NEUROSLAVE_STRUCT_H_

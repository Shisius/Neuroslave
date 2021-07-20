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

	std::string to_json()
	{
		rapidjson::StringBuffer buf;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
		writer.StartObject();
		writer.String("tag"); writer.String(tag.c_str(), tag.size());
		writer.String("user_name"); writer.String(user_name.c_str(), user_name.size());
		writer.String("sample_rate"); writer.Uint(sample_rate);
		writer.String("n_channels"); writer.Uint(n_channels);
		writer.String("n_samples_per_pack"); writer.Uint(n_samples_per_pack);
		writer.String("gain"); writer.Uint(gain);
		writer.String("tcp_decimation"); writer.Uint(tcp_decimation);
		writer.EndObject();
		return buf.GetString();
	}

	bool from_json(const std::string & json_str)
	{
		rapidjson::Document doc;		
		doc.Parse(json_str.c_str());
		if (!doc.IsObject()) return false;
		if (doc.HasMember("tag")) tag = doc["tag"].GetString();
		if (doc.HasMember("user_name")) user_name = doc["user_name"].GetString();
		if (doc.HasMember("sample_rate")) sample_rate = doc["sample_rate"].GetInt();
		if (doc.HasMember("n_channels")) n_channels = doc["n_channels"].GetInt();
		if (doc.HasMember("n_samples_per_pack")) n_samples_per_pack = doc["n_samples_per_pack"].GetInt();
		if (doc.HasMember("gain")) gain = doc["gain"].GetInt();
		if (doc.HasMember("tcp_decimation")) tcp_decimation = doc["tcp_decimation"].GetInt();
		return true;
	}
};

struct NeuroslaveEegData {
	std::vector<EegSamplePack> packs;
};

struct NeuroslaveMusic {
	std::string name;
	unsigned int n_channels;
	unsigned int bits_per_sample;
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
	NeuroslaveEegData eeg;
	unsigned long long start_time_ns;
	
};

#endif //_NEUROSLAVE_STRUCT_H_

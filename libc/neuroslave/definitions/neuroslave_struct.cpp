#include "neuroslave_struct.h"

std::string NeuroslaveSession::to_json()
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

bool NeuroslaveSession::from_json(const std::string & json_str)
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

std::string NeuroslaveEegData::to_json() 
{
	std::string data_str = "";
	if (packs.size() > 0) {
		for (auto && pack : packs) {
			data_str += (char*)(pack.samples);
		}
	}
	return data_str;
}

std::string NeuroslaveMusic::to_json()
{
	rapidjson::StringBuffer buf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
	writer.StartObject();
	writer.String("name"); writer.String(name.c_str(), name.size());
	writer.String("n_channels"); writer.Uint(n_channels);
	writer.String("bits_per_sample"); writer.Uint(bits_per_sample);
	writer.String("sample_rate"); writer.Uint(sample_rate);
	writer.String("duration_us"); writer.Uint(duration_us);
	writer.String("start_sample"); writer.Uint(start_sample);
	writer.String("end_sample"); writer.Uint(end_sample);
	writer.EndObject();
	return buf.GetString();
}

std::string NeuroslaveGameSettings::to_json()
{
	rapidjson::StringBuffer buf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
	writer.StartObject();
	writer.String("subfolder"); writer.String(subfolder.c_str(), subfolder.size());
	writer.String("duration"); writer.Double(duration);
	writer.String("volume"); writer.Double(volume);
	writer.String("complexity"); writer.Uint(complexity);
	writer.EndObject();
	return buf.GetString();
}

std::string NeuroslaveGame::to_json()
{
	rapidjson::StringBuffer buf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
	writer.StartObject();
	std::string settings_str = settings.to_json();
	writer.String("settings"); writer.String(settings_str.c_str(), settings_str.size());
	writer.String("right_music"); writer.String(right_music.c_str(), right_music.size());
	writer.String("guessed_music"); writer.String(guessed_music.c_str(), guessed_music.size());
	writer.String("user_wins"); writer.Bool(user_wins);
	writer.EndObject();
	return buf.GetString();
}

std::string NeuroslaveRecord::to_json()
{
	rapidjson::StringBuffer buf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
	writer.StartObject();
	std::string tmp_str = session.to_json();
	writer.String("session"); writer.String(tmp_str.c_str(), tmp_str.size());
	tmp_str = music.to_json();
	writer.String("music"); writer.String(tmp_str.c_str(), tmp_str.size());
	tmp_str = game.to_json();
	writer.String("game"); writer.String(tmp_str.c_str(), tmp_str.size());
	tmp_str = eeg.to_json();
	writer.String("eeg"); writer.String(tmp_str.c_str(), tmp_str.size());
	writer.String("start_time_ns"); writer.Uint(start_time_ns);
	writer.EndObject();
	return buf.GetString();
}

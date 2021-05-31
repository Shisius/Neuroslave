#ifndef STRUCT_TOOLS_H
#define STRUCT_TOOLS_H

#include "iostream"
#include <string>
#include <sstream>
#include <chrono>
#define XTOSTRUCT_SUPPORT_CHAR_ARRAY
#include "x2struct/x2struct.hpp"

namespace radar
{
	/// Special types conversions
	//XTOSTRUCT_OUT(std::ratio<1>)
	//XTOSTRUCT_OUT(std::chrono::duration, O(rep))
	//XTOSTRUCT_OUT(std::chrono::time_point<std::chrono::high_resolution_clock>, O(duration));

	/// Message constants
	static const char message_delimiter = ':';
	static const char message_ending = '\n';

	/// Struct conversion
	// Add struct name and convert to json
	template<class T>
	inline std::string get_type_name_ns(T &) {return std::string(__cxxabiv1::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr));}
	template<class T>
	inline std::string get_type_name(T & t) {return get_type_name_ns(t).substr(get_type_name_ns(t).find_last_of(':') + 1);}
	template<class T>
	inline std::string to_json(T & t, int json_indent = -1) {return get_type_name(t) + message_delimiter + x2struct::X::tojson(t, "", json_indent);}
	// Fill struct from json string without name
	template<class T>
	inline bool from_json(const std::string & str, T & t) {
		try {return x2struct::X::loadjson(str, t, false, false);}
		catch (std::exception & e) {return false;}
	}
	// Fill struct from json stringstream with "StructType:" in the beginning
	template<class T>
	inline bool from_sstream_json(std::stringstream & input_stream, T & t)
	{
		std::string tmp_str;
		std::getline(input_stream, tmp_str, message_delimiter);
		if (tmp_str.compare(get_type_name(t)) != 0) {
			return false;
		}
		std::getline(input_stream, tmp_str, '\x00');
		return from_json(tmp_str, t);
	}
	// Fill struct from json fstream with "StructType:" in the beginning
	// template<class T>
	// inline bool from_fstream_json(std::fstream & input_stream, T & t)
	// {
	// 	std::string type_str;
	// 	std::string json_str;
	// 	std::getline(input_stream, type_str, message_delimiter);
	// 	if (type_str.compare(get_type_name(t)) != 0) {
	// 		return false;
	// 	}
	// 	std::getline(input_stream, json_str, '\x00');
	// 	return from_json(json_str, t);
	// }
	// Write struct to file
	// template<class T>
	// bool json_to_file(const std::string & file_path, T & t)
	// {
	// 	return 0;
	// }

} // namespace radar
#endif //STRUCT_TOOLS_H
#include "neuroslave_msg.h"

namespace neuroslave
{

	std::string to_json(const std::vector<std::string> & strvec) 
	{
		rapidjson::StringBuffer buf;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
		writer.StartArray();
		for (auto & str : strvec) {
			writer.String(str.c_str(), str.size());
		}
		writer.EndArray();
		return buf.GetString();
	}

	std::vector<std::string> msg2strvec(std::string & msg)
	{
		size_t pos = 0;
		std::vector<std::string> strvec;
		std::string sub;
		while ((pos = msg.find(MSGDELIM)) != std::string::npos) {
			sub = msg.substr(0, pos);
			msg.erase(0, pos + sizeof(MSGDELIM));
			if (sub.compare(MSGEND) == 0)
				break;
			strvec.push_back(sub);	
		}
		return strvec;
	}

	std::string strvec2msg(std::vector<std::string> & strvec)
	{
		std::string msg;
		for (auto & str : strvec) {
			msg += str;
			msg.push_back(MSGDELIM);
		}
		msg.pop_back();
		msg += MSGEND;
		strvec.clear();
		return msg;
	}

}
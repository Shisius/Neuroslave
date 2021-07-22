#ifndef _NEUROSLAVE_MSG_H_
#define _NEUROSLAVE_MSG_H_

#include "neuroslave_struct.h"
#include <map>

namespace neuroslave
{
	const std::string MSGEND = "\r\n";
	const char MSGDELIM = ':';
	const ushort MSGPORT = 7239;
	/// GUI to Neuroslave
	enum class UserCommand {
		TURN_ON = 0,
		TURN_OFF,
		CHOOSE,
		RECORD,
		STOP,
		USER,
		GAME,
		SET,
		CONNECT,
		UPDATE
	};

	const std::map<std::string, UserCommand> UserCommandStr = {
		{"TurnOn", UserCommand::TURN_ON},
		{"TurnOff", UserCommand::TURN_OFF},
		{"Choose", UserCommand::CHOOSE},
		{"Record", UserCommand::RECORD},
		{"Stop", UserCommand::STOP},
		{"User", UserCommand::USER},
		{"Game", UserCommand::GAME},
		{"Set", UserCommand::SET},
		{"Connect", UserCommand::CONNECT},
		{"Update", UserCommand::UPDATE}
	};

	enum class UserSetOption {
		SESSION,
		GAME
	};

	const std::map<std::string, UserSetOption> UserSetOptionStr = {
		{"Session", UserSetOption::SESSION},
		{"Game", UserSetOption::GAME}
	};

	/// Neuroslave to GUI
	enum class UserAnswer {
		ACCEPTED = 0,
		DENIED,
		FINISHED,
		SESSION,
		PLAYLIST,
		USERS
	};

	const std::map<std::string, UserAnswer> UserAnswerStr = {
		{"Accepted", UserAnswer::ACCEPTED},
		{"Denied", UserAnswer::DENIED},
		{"Finished", UserAnswer::FINISHED},
		{"Session", UserAnswer::SESSION},
		{"Playlist", UserAnswer::PLAYLIST},
		{"Users", UserAnswer::USERS}
	};

	/// Finctions
	template <class EnumType>
	std::string get_str_by_enum(const std::map<std::string, EnumType> & strmap, EnumType value)
	{
		for (auto & p : strmap) {
			if (p.second == value) {
				return p.first;
			}
		}
		return "";
	}

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


} // neuroslave
#endif //_NEUROSLAVE_MSG_H_

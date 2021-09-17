#ifndef _NEUROSLAVE_MSG_H_
#define _NEUROSLAVE_MSG_H_

#include "neuroslave_struct.h"
#include <map>

namespace neuroslave
{
	const std::string MSGEND = "\n\r";
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

	std::string to_json(const std::vector<std::string> & strvec);

	std::vector<std::string> msg2strvec(std::string & msg);

	std::string strvec2msg(std::vector<std::string> & strvec);


} // neuroslave
#endif //_NEUROSLAVE_MSG_H_

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "common_tools.h"
#include "tcpserverstream.h"
#include "consolestream.h"

struct CommunicatorSettings
{
	enum CommunicatorType
	{
		DISABLED = -1,
		CONSOLE = 0,
		TCPSERVER
	};

	CommunicatorType type;
	u_short port;
	uint32_t timeout_us;

	CommunicatorSettings() : type(CommunicatorType::CONSOLE), port(0), timeout_us(10) {}

	XTOSTRUCT(O(type, port, timeout_us))
};

#endif // COMMUNICATION_H
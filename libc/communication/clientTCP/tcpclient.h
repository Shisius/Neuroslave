//client.h
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <stdint.h>
#include <vector>

#ifdef _WIN32 
	//windows
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
    //#include <windows.h>
	#include <winsock2.h>
    //#include <ws2tcpip.h>
    //#include <stdlib.h>
    //#include <stdio.h>
	#pragma comment(lib,"ws2_32.lib") //Winsock Library
    //#pragma comment(lib,"Mswsock.lib")
    //#pragma comment(lib,"advApi32.lib")
	#include "string"
#else
	//linux
    #include <arpa/inet.h>
	#include <sys/socket.h>
#endif


class Client{
public:

    explicit Client();
	~Client();

	bool createSocket();
	bool connectToServer();
	bool setServerIp(const char * ip);
    void setServerPort(const u_short port);
	int sendMessage(const char * message, const int messageSize);
	int receiveMessage(char * message, const int messageSize);
	void printServerInfo(const char * message);
	// void setNonBlocking();
	void closeSocket();

private:	

#ifdef _WIN32
	SOCKET clientSocket;
#else
    int clientSocket;
#endif
    sockaddr_in serverAddress;
	char * server_ip;
    u_short port;//default value
};

class ConnectionError{
public:
	explicit ConnectionError();
	virtual void printErrorInfo() = 0;
	
protected:
	int errorCode;
};

class SendError : public ConnectionError{
public:
	void printErrorInfo();
};

class ReceiveError : public ConnectionError{
public:
	void printErrorInfo();
};

#endif // TCPCLIENT_H

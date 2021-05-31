//server.h

//**********************************
//**** TCP server                *** 
//**** Platforms: Windows, Linux ***
//**** Date: 12.08.2020          ***
//**********************************

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <iostream>
#include <map>

#ifdef _WIN32 
	//windows
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#include <winsock2.h>
	#include "string"

//	#pragma comment(lib,"ws2_32.lib") //Winsock Library
#else
	//linux
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	//#include <netdb.h>
	#include <netinet/in.h> 
	#include "string.h"
	#include <unistd.h>
    #include <errno.h>
    #include <netdb.h>
    #include <stdio.h>

	const int SOCKET_ERROR = -1;
	const int INVALID_SOCKET = -1;
	const int INET_ATON_FAILED = 0;
    const int SD_SEND = 1;
#endif

    enum {
        DONTROUTE = MSG_DONTROUTE,
        OOB = MSG_OOB
    };

class TCPServer{

public:
    /* Constructor prepares the server address
     * port - server port for tcp connection
     * tv_sec - timeout for accept & recv (seconds)
     * tv_usec - timeout for accept & recv (microseconds)
     */
    TCPServer(const u_short port, long tv_sec = 0, long tv_usec = 0);

    /*Moveable but not Copyable*/
    TCPServer() = delete;
    TCPServer(const TCPServer&) = delete;
    TCPServer& operator=(const TCPServer&) = delete;
    

    /* Destructor closes the connection
     */
    ~TCPServer();

    /* Function create socket, bind it, listen to connection, and accept socket
     * returns bool: true, if no error occurs, false otherwise
     */
    bool start();

    /* Function permits an incoming connection attempt on a socket
     * returns bool: true, if no error occurs, false otherwise
     */
    bool acceptSocket();

    /* Function sends data on a connected socket
     * message - a pointer to a buffer containing the data to be transmitted
     * messageSize - the length, in bytes, of the data in buffer pointed to by the "message" parameter
     * flags - a set of flags that specify the way in which the call is made. This parameter is constructed by using the bitwise OR operator with any of the following values.
     *      0 - no flags
     *      DONTROUTE - Specifies that the data should not be subject to routing. A Windows Sockets service provider can choose to ignore this flag.
     *      OOB - Sends OOB data (Urgent data should be sent).
     * returns int: if no error occurs, returns the total number ob bytes sent, which can be less than the number requested to be sent in the "messageSize" parameter.
     *              Otherwise, a value of SOCKET_ERROR is returned.
     */
    int sendMessage(const char * message, int messageSize, int flags = 0);

    /* Function receives data from a connected socket or a bound connectionless socket.
     * message - a pointer to the buffer to receive the incoming data.
     * messageSize - the length, in bytes, of the buffer pointed to by the "message" parameter
     *  flags - a set of flags that specify the way in which the call is made. This parameter is constructed by using the bitwise OR operator with any of the following values.
     *      0 - no flags
     *      PEEK - Specifies that the data should not be subject to routing. A Windows Sockets service provider can choose to ignore this flag.
     *      OOB - Sends OOB data (Urgent data should be sent).
     *      WAITALL - The receive request will complete only when one of the following events occurs:
     *                  - The buffer supplied by the caller is completely full.
     *                  - The connection has been closed.
     *                  - The request has been canceled or an error occurred. If WAITALL is specified along with OOB, PEEK, then this call will fail with error.
     * returns int: if no error occurs, returns the total number ob bytes received. If the connection has been gracefully closed, the return value is zero.
     *              Otherwise, a value of SOCKET_ERROR is returned.
     */
    int receiveMessage(char * message, int messageSize, int flags = 0);

    /* Function causes connection on the socket associated with socket descriptor to be shut down and closed
     */
    void stop();

    /* returns an information about the accepted client, if no error occurs. Otherwise, an information about error returned.
     */
    std::string statusInfo() {return d_info;}

private:
    //void setServerPort(const u_short port);
    //bool setServerIp(const char * ip);
    void set_address(char *host, char *port, struct sockaddr_in *sap, char *protocol);

    bool createSocket();
    bool bindSocket();    
    bool listenToConnection();

    void closeServerSocket();
    u_short port(){return PORT;}
    std::string clientInfo(/*const char * message*/);
    void printError(const std::string&);

#ifdef _WIN32
    SOCKET d_serverSocket;
    SOCKET d_clientSocket;
#else
    int d_serverSocket;
    int d_clientSocket;
#endif
    struct sockaddr_in d_serverAddress;
    struct sockaddr_in d_clientAddress;

	u_short PORT;

    std::string d_info;

    const int on = 1;

    struct timeval d_tv;
};

#endif //TCPSERVER_H

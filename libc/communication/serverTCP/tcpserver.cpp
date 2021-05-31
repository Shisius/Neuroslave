//server.cpp
#include "tcpserver.h"

TCPServer::TCPServer(const u_short port, long tv_sec, long tv_usec): PORT(port)
{
    //setup address structure for client
    memset((char *)&d_clientAddress, 0, sizeof(d_clientAddress)); // d_clientAddress = 0
    //setup address structure for server
    memset((char *)&d_serverAddress, 0, sizeof(d_serverAddress)); // d_serverAddress = 0

    d_serverAddress.sin_family = AF_INET;
    d_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    d_serverAddress.sin_port = htons(PORT);
    d_clientSocket = INVALID_SOCKET;
    d_tv.tv_sec = tv_sec;
    d_tv.tv_usec = tv_usec;
    //setServerPort(PORT);

}

TCPServer::~TCPServer(){
    std::cout << "start ~TCPServer()" << std::endl;
    stop();
    std::cout << "end ~TCPServer()" << std::endl;
}

//bool TCPServer::setServerIp(const char * ip) {
//	//set ip
//#ifdef _WIN32
//	if (inet_addr(ip) != INADDR_NONE)
//        d_serverAddress.sin_addr.s_addr = inet_addr(ip);
//	else{
//		std::cout << "Invalid IP address." << std::endl;
//		return false;
//	}
//#else
//    int inet_aton_res = inet_aton(ip, &d_serverAddress.sin_addr);
//    if (inet_aton_res == 0) {
//		std::cout << "inet_aton() failed." << std::endl;
//		return false;
//	}
//#endif
//	return true;
//}

//void TCPServer::setServerPort(const u_short newport)
//{
//    d_serverAddress.sin_port = htons(newport);
//	//PORT = newport;
//}

void TCPServer::set_address(char *host, char *port, struct sockaddr_in *sap, char *protocol)
{
    struct servent *sp;
    struct hostent *hp;
    char *endptr;
    short portNum;
    memset((char *)sap, 0, sizeof(*sap));
    sap->sin_family = AF_INET;
    if(host != nullptr)
    {
        if (!inet_aton(host, &sap->sin_addr))
        {
            hp = gethostbyname(host);
            if (hp == nullptr)
                printError("unknown host: " + std::string(host));
            sap->sin_addr = *(struct in_addr *)hp->h_addr;
        }
    }
    else
        sap->sin_addr.s_addr = htonl(INADDR_ANY);
    portNum = strtol(port, &endptr, 0);
    if(*endptr == '\0')
        sap->sin_port = htons(portNum);
    else
    {
        sp = getservbyname(port, protocol);
        if (sp == nullptr)
            printError("unknown service: " + std::string(port));
        sap->sin_port = sp->s_port;
    }
}

bool TCPServer::createSocket(){

#ifdef _WIN32
	//Initialise winsock
	std::cout << "Initialising Winsock..." << std::endl;
	WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		std::cout << "Failed. Error Code : " << WSAGetLastError() << std::endl;
		return false;
	}
	std::cout << "Initialised." << std::endl;
#endif // _WIN32

	//Create a socket
    if ((d_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET){        
        printError("socket() failed with error");
#ifdef _WIN32        
        WSACleanup(); //
#endif // _WIN32

		return false;
	}

    std::cout << "Socket created." << std::endl;
    if(setsockopt(d_serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on)) == SOCKET_ERROR){ //SO_REUSEADDR socket option allows a socket to forcibly bind to a port in use by another socket.
        printError("setsockopt() SO_REUSEADDR failed with error");
//#endif // _WIN32
        closeServerSocket();
    }
    if(setsockopt(d_serverSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&d_tv, sizeof (d_tv)) == SOCKET_ERROR){ //SO_REUSEADDR socket option allows a socket to forcibly bind to a port in use by another socket.
        printError("setsockopt() SO_RCVTIMEO failed with error");
        closeServerSocket();
        return false;
    }

	return true;
}

bool TCPServer::bindSocket(){
	//Bind	
    if (bind(d_serverSocket, (sockaddr *)&d_serverAddress, sizeof(d_serverAddress)) == SOCKET_ERROR){
        printError("Bind failed with error");
        closeServerSocket();
		return false;
	}
    std::cout << "Bind done" << std::endl;
	return true;
}

bool TCPServer::listenToConnection(){
	//Listen to incoming connections (query length = 1)	
    if (listen(d_serverSocket, 1) == SOCKET_ERROR) {
        printError("Listen failed with error");
        closeServerSocket();
		return false;
	}
    //std::cout << "Server listens." << std::endl;
	return true;
}

bool TCPServer::acceptSocket(){
#ifdef _WIN32
    int clientAddressLen = sizeof(d_clientAddress);
#else
    socklen_t clientAddressLen = sizeof(d_clientAddress);
#endif //_WIN32
    //while (true) {
        d_clientSocket = accept(d_serverSocket, /*NULL, NULL*/(sockaddr *)&d_clientAddress, &clientAddressLen);
        //std::cout << "d_clientSocket=" << d_clientSocket;
        if (d_clientSocket == INVALID_SOCKET) {
#ifdef _WIN32
            if(errno == WSAWOULDBLOCK || errno == WSAEINTR)

#else
            if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)

#endif //_WIN32
                return false;
            //std::cout << "recvErr: d_clientSocket=" << d_clientSocket << std::endl;

            printError("Accept failed with error");
            d_info = "Accept error occured on TCP port " + std::to_string(PORT);
            closeServerSocket();
            return false;
        }


//    if(setsockopt(d_clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&d_tv, sizeof (d_tv)) == SOCKET_ERROR){ //SO_REUSEADDR socket option allows a socket to forcibly bind to a port in use by another socket.
//        printError("setsockopt() SO_RCVTIMEO failed with error");
//#ifdef _WIN32
//        if (closesocket(d_clientSocket) == SOCKET_ERROR)
//#else
//        if (close(d_clientSocket) == SOCKET_ERROR)
//#endif
//            printError("client close error ");
//        closeServerSocket();
//        return false;
//    }
    d_info = clientInfo();
    std::cout << "Server accepted with client." << clientInfo() << std::endl;
	return true;
}

int TCPServer::receiveMessage(char * message, int messageSize, int flags){
	//try to receive some data, this is a blocking call	
    //int flag = 0; //MSG_OOB - Urgent data should be received
    int bytes_recv; 

    bytes_recv = recv(d_clientSocket, message, messageSize, flags);
    if (bytes_recv == SOCKET_ERROR){
        d_info = "Recieve failed with error.";

#ifdef _WIN32
        if(errno != WSAEINTR && errno != WSAWOULDBLOCK) {//
            printError("recv() failed with error");
            closesocket(d_clientSocket);
#else
        if(errno != EINTR && errno != EAGAIN) {
            printError("recv() failed with error");
            close(d_clientSocket);
#endif //_WIN32

            //std::cout << "recvErr: d_clientSocket=" << d_clientSocket << std::endl;
        }
    }
    else{
        if(bytes_recv == 0){
            d_info = "Client disconected";
            std::cout << "Client disconected" << std::endl;
#ifdef _WIN32
    if (closesocket(d_clientSocket) == SOCKET_ERROR)
#else
    if (close(d_clientSocket) == SOCKET_ERROR)
#endif
        printError("client close error ");
            //std::cout << "client discon: d_clientSocket=" << d_clientSocket << std::endl;
        }
    }

	return bytes_recv;
}

int TCPServer::sendMessage(const char * message, int messageSize, int flags){
	//now reply the client with the same data	
    int bytes_sent = send(d_clientSocket, message, messageSize, flags);
    if (bytes_sent == SOCKET_ERROR){
        d_info = "Send failed with error.";
        printError("send() failed with error");
#ifdef _WIN32
        if (closesocket(d_clientSocket) == SOCKET_ERROR)
#else
        if (close(d_clientSocket) == SOCKET_ERROR)
#endif
            printError("client close error ");
        //std::cout << "sendErr: d_clientSocket=" << d_clientSocket << std::endl;
	}
    return bytes_sent;
}

std::string TCPServer::clientInfo(/*const char * message*/){
	//print details of the client/peer and the data received

    std::string str = "Accepted TCP client ";
    str.append(inet_ntoa(d_clientAddress.sin_addr));
    str.append(":");
    std::string port = std::to_string(static_cast<unsigned>(ntohs(d_clientAddress.sin_port)));
    str.append(port);
    str.append("\n");

    return str;
}

void TCPServer::printError(const std::string& error){
#ifdef _WIN32
        std::cout << error << ":" << WSAGetLastError() << std::endl;
#else
        perror(error.c_str());
#endif //_WIN32
}

void TCPServer::closeServerSocket(){
    std::cout << "closeServerSocket()" << std::endl;
#ifdef _WIN32
    if (closesocket(d_serverSocket) == SOCKET_ERROR)
    {
        printError("closesocket error ");
        return;
    }
    if (WSACleanup() == SOCKET_ERROR)
    {
        printError("WSACleanup error ");
        return;
    }
#else
    //std::cout << "bcl: d_serverSocket=" << d_serverSocket << std::endl;
//    if(d_serverSocket != INVALID_SOCKET)
//    {
        if (close(d_serverSocket) == SOCKET_ERROR)
        {
            printError("close error ");
        }
//        std::cout << "acl: d_serverSocket=" << d_serverSocket << std::endl;
//    }

#endif
}

bool TCPServer::start()
{
    //std::string str;
    if(!createSocket())
    {
        d_info = "Socket error occured on TCP port " + std::to_string(PORT);
        return false;
    }

    if(!bindSocket())
    {
        d_info = "Bind error occured on TCP port " + std::to_string(PORT);
        return false;
    }
    if(!listenToConnection())
    {
        d_info = "Listen error occured on TCP port " + std::to_string(PORT);
        return false;
    }

    return true;
}

void TCPServer::stop()
{
    std::cout << "stop" << std::endl;
    //std::cout << "stop bcl: d_clientSocket=" << d_clientSocket << std::endl;
    //if(d_clientSocket != INVALID_SOCKET)
    //{
    if (shutdown(d_clientSocket, SD_SEND) == SOCKET_ERROR) //subsequent calls to the send function are disallowed. A FIN will be sent after all data is sent and acknowledged by the receiver.
    {
        printError("shutdown error ");
        //return;
    }
#ifdef _WIN32
    if (closesocket(d_clientSocket) == SOCKET_ERROR){
#else
    if (close(d_clientSocket) == SOCKET_ERROR){
#endif
        printError("client close error ");
        //return;
        //std::cout << "stop acl: d_clientSocket=" << d_clientSocket << std::endl;
    }
    closeServerSocket();
    d_info = "Server stopped.";
}

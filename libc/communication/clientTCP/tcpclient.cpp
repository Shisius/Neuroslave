//client.cpp
#include <iostream>
#ifdef _WIN32
    #include "string"
#else
    #include "string.h"
    #include <unistd.h>
    const int SOCKET_ERROR = -1;
    const int INET_ATON_FAILED = 0;
#endif
#include "tcpclient.h"


Client::Client(){
	//setup address structure
	memset((char *)&serverAddress, 0, sizeof(serverAddress)); 
	serverAddress.sin_family = AF_INET;
	port = 51002;//default value
	serverAddress.sin_port = htons(port);
    server_ip = "10.30.100.72"; //default value
	serverAddress.sin_addr.s_addr = inet_addr(server_ip);
}

Client::~Client(){
	closeSocket();
}

bool Client::setServerIp(const char * ip) {
	//set ip
	#ifdef _WIN32
		if (inet_addr(ip) != INADDR_NONE)
			serverAddress.sin_addr.s_addr = inet_addr(ip);
		else{
			std::cout << "Invalid IP address." << std::endl;
			return false;
		}			
	#else
		int inet_aton_res = inet_aton(ip, &serverAddress.sin_addr);
		if (!inet_aton_res) {
			std::cout << "inet_aton() failed." << std::endl;
			return false;
		}
	#endif
	return true;
}

void Client::setServerPort(const u_short newport)
{
    serverAddress.sin_port = htons(newport);
    //port = newport;
}

bool Client::createSocket(){
	#ifdef _WIN32
		//Initialise winsock
		WSADATA wsa;
		std::cout << "\nInitialising Winsock..." << std::endl;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
			std::cout << "Failed.";
			std::cout << "(Error Code: " << WSAGetLastError() << " )."<< std::endl;
			return false;
		}
		std::cout << "Initialised." << std::endl;
	#endif //_WIN32
	
		//create socket
		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // return socket descriptor; AF_INET - address domain Internet; SOCK_STREAM - socket type (TCP); 
	#ifdef _WIN32
		if (clientSocket == INVALID_SOCKET){ 
			std::cout << "Could not create socket.";
	#else
		if (clientSocket == -1){
			std::cout << "Could not create socket.";
	#endif
	#ifdef _WIN32
			std::cout << "WSAGetLastError:" << WSAGetLastError() << std::endl;
			WSACleanup(); // Деиницилизация библиотеки Winsock
	#endif // WIN32
		return false;
	}
	std::cout << "Socket created" << std::endl;
	return true;
}

bool Client::connectToServer(){

	int iResult = connect(clientSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)); //clientSocket - socket descriptor, which have returned by socket(...) call, serverAddress - structure (sockaddr_in) with host address
	#ifdef _WIN32
		if (iResult){ // 0 - ошибка
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			printf("The server is down... did not connect");
			return false;
		}
	#else
		if (iResult < 0){ // -1 - ошибка
			close(clientSocket);
			clientSocket = -1;
			printf("The server is down... did not connect");
			return false;
		}
	#endif 
	std::cout << "Socket connected" << std::endl;
	//sockaddr other;
	//int slen = sizeof(sockaddr);
	//const int size = 10;
	//char buf[size];

	////blocking call
	////if (int recv_len = recvfrom(serverSocket, buf, size, 0, (sockaddr *)&other, &slen) == SOCKET_ERROR){
	////	if (int recv_len = recv(serverSocket, buf, size, 0/*, (sockaddr *)&other, &slen*/) == SOCKET_ERROR){
	////		std::cout << "recvfrom() failed.";
	////#ifdef WIN32
	////		std::cout << " (With error code:" << WSAGetLastError() << " )." << std::endl;
	////#endif //WIN32
	////	}
	//std::cout << "incomming connection ... " << std::endl;

	//if (connect(serverSocket, (sockaddr *)&other, slen)){
	//	std::cout << "connect() failed." << std::endl;
	//}
	//std::cout << "connected" << std::endl;

	return true;
}

//send the message
int Client::sendMessage(const char * message, const int messageSize){
	int bytes_sent;
	if ((bytes_sent = send(clientSocket, message, messageSize, 0)) == SOCKET_ERROR){
		throw new SendError();
	}
	return bytes_sent;
}

//receive a reply
int Client::receiveMessage(char * message, const int messageSize){	
	
		int bytes_recv;	
		if ((bytes_recv = recv(clientSocket, message, messageSize, 0)) == SOCKET_ERROR){
			throw new ReceiveError();
			std::cout << "recv() failed.";
	#ifdef WIN32
			std::cout << " (With error code:" << WSAGetLastError() << " )." << std::endl;
	#endif //WIN32
		}
		return bytes_recv;
}


//print details of the client/peer and the data received
void Client::printServerInfo(const char * message){
	std::cout << "Received packet from server " << inet_ntoa(serverAddress.sin_addr) <<
		":" << ntohs(serverAddress.sin_port) << std::endl;
	std::cout << "Data: " << message << std::endl;

}


//set nonblocking. Make the socket non-blocking, so calls to recv don't halt the program waiting for input.
// void Client::setNonBlocking(){	

// 	#ifdef WIN32
// 		DWORD nonBlocking = 1;
// 		if (ioctlsocket(clientSocket, FIONBIO, &nonBlocking) != 0){
// 			printf("failed to set non-blocking socket\n");
// 			return;
// 		}
// 	#else
// 		int nonBlocking = 1;
// 		if (fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1){
// 			printf("failed to set non-blocking socket\n");
// 			return;
// 		}
// 	#endif
// }

void Client::closeSocket(){
#ifdef WIN32
	closesocket(clientSocket);
	WSACleanup();
#else
	close(clientSocket);
#endif
}

ConnectionError::ConnectionError(){
#ifdef WIN32
	errorCode = WSAGetLastError();
#endif // WIN32
}

void SendError::printErrorInfo(){
	std::cout << "sendto() failed. ";
#ifdef WIN32
	std::cout << " ( With error code:" << WSAGetLastError() << " )." << std::endl;
#endif //WIN32
}


void ReceiveError::printErrorInfo(){
	std::cout << "recvfrom() failed.";
#ifdef WIN32
	std::cout << " (With error code:" << errorCode << " )." << std::endl;
#endif //WIN32
}


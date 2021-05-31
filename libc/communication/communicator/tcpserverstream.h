#ifndef TCPSERVERSTREAM_H
#define TCPSERVERSTREAM_H

#include "scoped_thread.h"
#include "tcpserver.h"
#include "communicator.h"

class TcpServerStream: public Communicator
{
    u_short d_port;
    TCPServer* d_tcpServer = nullptr;
    atomic<bool> d_isReading;
    atomic<bool> d_isSending;
    atomic<bool> d_isAccepting;
    std::mutex d_acceptedMutex;
    std::condition_variable d_acceptedCV;
    scoped_thread *tcpSession_thread = nullptr;
    scoped_thread *receive_thread = nullptr;
    scoped_thread *send_thread = nullptr;

public:
    /* Constructor prepares the server address
     * port - server port for tcp connection
     * tv_sec - timeout for accept & recv (seconds)
     * tv_usec - timeout for accept & recv (microseconds)
     */
    TcpServerStream(const u_short port, long tv_sec = 0, long tv_usec = 10);
    ~TcpServerStream();

    /* Function start() create socket, bind it, listen to connection, and accept socket
     * returns bool: true, if no error occurs, false otherwise
     */
    void start();
    bool sendMessage(const std::string&);
    bool sendMessage(const char*, int);
    bool receiveMessage(std::string&);
    void stop();
    std::string statusInfo();
    CommunicatorState state();
private:
    void recvThread();
    void sendThread();
    void tcpSession();
    bool init();

};

#endif // TCPSERVERSTREAM_H

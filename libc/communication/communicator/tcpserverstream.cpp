

#include "tcpserverstream.h"



TcpServerStream::TcpServerStream(const u_short port, long tv_sec, long tv_usec) : d_port(port)
{
    if (tv_sec == 0 && tv_usec == 0)
        tv_usec = 10;
    d_tcpServer = new TCPServer(port, tv_sec, tv_usec);
    d_isReading.store(false);
    d_isSending.store(false);
    d_isAccepting.store(false);
}

bool TcpServerStream::init()
{
    return d_tcpServer->start();
}

void TcpServerStream::tcpSession() // SHOULD BE CORRECTED!!!!!
{    
    if(!d_tcpServer->start())
        return;
    d_isAccepting.store(true);
    do {        
        std::unique_lock<std::mutex> lk(d_acceptedMutex);
        this->d_acceptedCV.wait(lk, [this]{return d_state != CommunicatorState::Alive;});
        lk.unlock();
        if (d_state == CommunicatorState::Stopped)
            break;
        while (true) {
            if(!d_tcpServer->acceptSocket())  {
                if (d_state == CommunicatorState::Stopped){
                    d_isAccepting.store(false);
                    return;
                }
    #ifdef _WIN32
                if(errno == WSAWOULDBLOCK || errno == WSAEINTR)
    #else
                if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
    #endif //_WIN32
                    continue;                
            }
            else{
                d_connectionOn.store(true);
                d_state = CommunicatorState::Alive;
                break;
            }
        }
    } while (d_connectionOn.load());
    d_isAccepting.store(false);
    //cout << "end of tcpSession" << '\n';
}

void TcpServerStream::recvThread()
{
    //cout << "recvThread" << '\n';
    while(d_state != CommunicatorState::Alive){
        if(d_state == CommunicatorState::Stopped)
            break;
        this_thread::yield();
    }
    string lastRecvMsg = "";
    //cout << "recvThread loop: " << '\n';
    while (d_connectionOn.load()) {
        d_isReading.store(true);

        string recvMsg;
        int MAXBUF = 65535;
        char buf[MAXBUF];
        int bytes_recv = d_tcpServer->receiveMessage(buf, MAXBUF);
        if(bytes_recv <= 0) // check all conditions (all returned values)
        {
#ifdef _WIN32
        if(errno == WSAEINTR || errno == WSAWOULDBLOCK)
            continue;
#else
        if(errno == EINTR || errno == EAGAIN)
            continue;
#endif //_WIN32
            {
                std::lock_guard<std::mutex> lk(d_acceptedMutex);
                if(bytes_recv < 0)
                    d_state = CommunicatorState::Error; //bytes_recv=0 => Client closed connection
                else
                    d_state = CommunicatorState::ClientDisconnected;
            }
            d_acceptedCV.notify_one();
            break;
        }

        recvMsg.clear();
        recvMsg.append(buf);
        recvMsg.resize(bytes_recv);
        if(!recvMsg.compare(lastRecvMsg))
            continue;
        lastRecvMsg = recvMsg;
        //cout << "recvThread: " << recvMsg << '\n';
        d_recvMsgQueue.push(recvMsg);
    }
    d_isReading.store(false);
    //cout << "end of recvThread" << '\n';
}

void TcpServerStream::sendThread()
{
    //cout << "sendThread" << '\n';
    while(d_state != CommunicatorState::Alive){
        if(d_state == CommunicatorState::Stopped)
            break;
        this_thread::yield();
    }
    //cout << "sendThread loop: " << '\n';
    while (d_connectionOn.load()) {
        d_isSending.store(true);

        //string sendMsg;
        communicator::Message sendMsg;
        //if (d_sendMsgQueue.try_pop(sendMsg))
        if (d_sendMsgQueue.try_pop(sendMsg))
        {

            //cout << "sendThread: from queue:  " << endl;
            //cout.width(sendMsg.size());
            //cout << sendMsg.m_data << endl;
            //if (d_tcpServer->sendMessage(sendMsg.data(), sendMsg.size()) < 0)
            if (d_tcpServer->sendMessage(sendMsg.m_data, sendMsg.m_size) < 0)
            {
                {
                    std::lock_guard<std::mutex> lk(d_acceptedMutex);
                    d_state = CommunicatorState::Error;
                }
                d_acceptedCV.notify_one();
                break;
            }
            //cout << "sendThread: " << sendMsg.c_str() << endl;
        }
        else{
            this_thread::sleep_for(100ms/*std::chrono::seconds(1)*/);
        }
    }
    d_isSending.store(false);
    //cout << "end of sendThread" << '\n';
}

void TcpServerStream::start()
{
    tcpSession_thread = new scoped_thread (thread(&TcpServerStream::tcpSession, this));
    receive_thread    = new scoped_thread (thread(&TcpServerStream::recvThread, this));
    send_thread       = new scoped_thread (thread(&TcpServerStream::sendThread, this));
}

bool TcpServerStream::sendMessage(const std::string& msg)
{
    //cout << "sendMessage()" << endl;
    //cout << "string: " << msg << endl;
    //communicator::Message message(msg);
    communicator::Message message;
    message.m_data = msg.data();
    message.m_size = msg.size();
    //cout << "communicator::Message: " << message.m_data << endl;
    d_sendMsgQueue.push(message);
    return d_connectionOn.load();
}

bool TcpServerStream::sendMessage(const char* msg, int length)
{
    communicator::Message message;
    message.m_data = msg;
    message.m_size = length;
    //d_sendMsgQueue.push(communicator::Message(msg, length));
    d_sendMsgQueue.push(message);
    return d_connectionOn.load();
}

bool TcpServerStream::receiveMessage(std::string& msg)
{
    return d_recvMsgQueue.try_pop(msg);
}

TcpServerStream::~TcpServerStream()
{
    //std::cout << "start ~TcpServerStream()" << std::endl;
    if(d_tcpServer != nullptr)
        delete d_tcpServer;
    if(tcpSession_thread != nullptr)
        delete tcpSession_thread;
    if(receive_thread != nullptr)
        delete receive_thread;
    if(send_thread != nullptr)
        delete send_thread;
    //std::cout << "end ~TcpServerStream()" << std::endl;
}

void TcpServerStream::stop()
{
    //std::cout << "TcpServerStream::stop()" << std::endl;
    d_connectionOn.store(false);
    {
        std::lock_guard<std::mutex> lk(d_acceptedMutex);
        d_state = CommunicatorState::Stopped;
    }
    d_acceptedCV.notify_one();
    while (d_isReading.load() || d_isSending.load() || d_isAccepting.load()) {
        this_thread::yield();
    }
    d_tcpServer->stop();

}

std::string TcpServerStream::statusInfo()
{
    return d_tcpServer->statusInfo();
}

CommunicatorState TcpServerStream::state()
{
    return d_state;
}

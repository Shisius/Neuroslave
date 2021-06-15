#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <atomic>
#include "string.h"
#include "threadsafe_queue.h"
//namespace communicator {
//    struct Message{
//    //private:
//        const char* m_data;
//        int m_size;
////    public:
////        Message() = default;
////        explicit Message(const char* msg, int size) : m_data(msg), m_size(size){}
////        explicit Message(const std::string& str) /*: m_data(str.data()), m_size(int(str.size()))*/{ m_data = str.data(); m_size = int(str.size());}
////        const char* data() const {return m_data;}
////        int size() const {return m_size;}
//    };
//}

enum class CommunicatorState{    
    Error = -3,
    Stopped,
    ClientDisconnected,
    UnConnected,
    Alive
};

class Communicator
{
public:
    Communicator():
        d_connectionOn(false),
        d_state(CommunicatorState::UnConnected)
    {}
    virtual ~Communicator(){}
    virtual void start() = 0;
    virtual bool sendMessage(const std::string&) = 0;
    //virtual bool sendMessage(const char*, int) = 0;
    virtual bool receiveMessage(std::string&) = 0;
    virtual void stop() = 0;
    virtual std::string statusInfo() = 0;
    virtual CommunicatorState state() = 0;
protected:
    atomic<bool> d_connectionOn;
    CommunicatorState d_state;
    threadsafe_queue<> d_recvMsgQueue;
    threadsafe_queue<> d_sendMsgQueue; //

};

#endif // COMMUNICATOR_H

#ifndef CONSOLESTREAM_H
#define CONSOLESTREAM_H

#include <iostream>
#include "communicator.h"

class ConsoleStream: public Communicator
{
public:
    ConsoleStream();
    ~ConsoleStream(){}
    void start(){ d_state = CommunicatorState::Alive;}
    bool sendMessage(const std::string&);
    //bool sendMessage(const char*, int);
    bool receiveMessage(std::string&);
    void stop(){d_state = CommunicatorState::Stopped;}
    std::string statusInfo(){return "Console is used.";}
    CommunicatorState state(){return d_state;}
};

#endif // CONSOLESTREAM_H

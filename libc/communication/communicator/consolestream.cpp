#include "consolestream.h"

ConsoleStream::ConsoleStream()
{

}

bool ConsoleStream::sendMessage(const std::string& msg)
{
    std::cout << msg;
    return true;
}

bool ConsoleStream::sendMessage(const char* msg, int /*length*/)
{
    //std::cout.width(length);
    std::cout << msg;
    return true;
}

bool ConsoleStream::receiveMessage(std::string& msg)
{
    std::cin >> msg;
    return true;
}



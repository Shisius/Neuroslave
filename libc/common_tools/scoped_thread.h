#ifndef SCOPED_THREAD_H
#define SCOPED_THREAD_H
#include <thread>

using namespace std;

class scoped_thread
{
    thread t;
public:
    explicit scoped_thread(thread t_) : t(move(t_))
    {
        if(!t.joinable())
            throw logic_error("No thread");
    }
    ~scoped_thread()
    {
        t.join();
    }

    scoped_thread(scoped_thread const&) = delete;
    scoped_thread& operator=(scoped_thread const&) = delete;
};
#endif // SCOPED_THREAD_H

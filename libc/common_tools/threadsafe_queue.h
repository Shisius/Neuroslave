/*************************************************
 * Threadsafe queue based on condition variables.
 *
 */
#ifndef THREADSAFE_QUEUE_H
#define THREADSAFE_QUEUE_H

#include <queue>
#include <memory>
#include <mutex>
#include <string>
#include <string.h>
#include <condition_variable>

//#include "declarationsForWorker.h"

using namespace std;
//typedef string T;
template<typename T = string>
class threadsafe_queue
{
    mutable mutex mut; //mutex must be changable, because other threads can store not const references to the object and call modifying member functions, which capture the mutex
    queue<T> data_queue; //queue<shared_ptr<T>> data_queue;
    condition_variable data_cond;

public:
    threadsafe_queue() {}
    threadsafe_queue(const threadsafe_queue & other)
    {
        lock_guard<mutex> lk(other.mut);
        data_queue = other.data_queue;
    }
    void operator=( const threadsafe_queue& other) = delete;
    void push (T new_data)
    {
        //shared_ptr<T> data(make_shared<T>(move(new_data)));
        lock_guard<mutex> lk(mut);
        data_queue.push(move(new_data)); //data_queue.push(data);
        data_cond.notify_one();
    }
    void wait_and_pop(T& data)
    {
        unique_lock<mutex> lk(mut);
        data_cond.wait(lk,[this]{return !data_queue.empty();});
        data = move(data_queue.front()); //data = move(*data_queue.front());
        data_queue.pop();
    }
    shared_ptr<T> wait_and_pop()
    {
        unique_lock<mutex> lk(mut);
        data_cond.wait(lk,[this]{return !data_queue.empty();});
        shared_ptr<T> res(make_shared<T>(move(data_queue.front()))); //shared_ptr<T> res = data_queue.front();
        data_queue.pop();
        return res;
    }
    bool try_pop(T& data)
    {
        lock_guard<mutex> lk(mut);
        if(data_queue.empty())
            return false;
        data = move(data_queue.front()); //data = move(*data_queue.front());
        data_queue.pop();
        return true;
    }
    shared_ptr<T> try_pop()
    {
        lock_guard<mutex> lk(mut);
        if(data_queue.empty())
            return shared_ptr<T>();
        shared_ptr<T> res(make_shared<T>(move(data_queue.front()))); // shared_ptr<T> res = data_queue.front();
        data_queue.pop();
        return res;
    }
    bool empty() const
    {
        lock_guard<mutex> lk(mut);
        return data_queue.empty();
    }
};

#endif // THREADSAFE_QUEUE_H


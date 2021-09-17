#ifndef _SIGNAL_TOOLS_H_
#define _SIGNAL_TOOLS_H_

#include <signal.h>
#include <sched.h>
#include <functional>
#include <unistd.h>

static std::function<void(void)> user_signal_handler;

static void signal_routine(int sig_value, siginfo_t *info, void *data)
{
    user_signal_handler();
}

// Only for class methods !!!!!!!!!!!!!!!!
template< class F, class T>
int attach_sigint(F&& user_func, T&& obj)
{
    // Sigaction
    struct sigaction _sigaction;
    _sigaction.sa_flags = SA_SIGINFO;
    user_signal_handler = std::bind(user_func, obj);
    _sigaction.sa_sigaction = signal_routine;
    return sigaction(SIGINT, &_sigaction, NULL);
}

#endif //_SIGNAL_TOOLS_H_


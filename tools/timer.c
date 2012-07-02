#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#else
#include <pthread.h>
#endif

#include "timer.h"

void* _event_param; // ugly! (unthread-safe) *shrug*
#ifdef WIN32
void CALLBACK timer_proc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    timer_callback_t tc = (timer_callback_t)dwUser;
    tc(_event_param);
}
#else
pthread_t _thread;
int _duration;
void* timer_proc(void* param)
{
    struct timespec time;
    struct timespec timeRemaining;
    
    time.tv_sec = 0;
    time.tv_nsec = _duration * 1000000;

    if (nanosleep(&time, &timeRemaining) == 0)
    {
        timer_callback_t tc = (timer_callback_t)param;
        tc(_event_param);
    }
    return NULL;
}
#endif

int timer_one_shot(int duration, timer_callback_t callback, void* param)
{
    _event_param = param;
#ifdef WIN32
    return timeSetEvent(duration, 0, timer_proc, (ULONG)callback, TIME_ONESHOT | TIME_CALLBACK_FUNCTION) != 0;
#else
    _duration = duration;
    return pthread_create(&_thread, NULL, timer_proc, callback) != 0;
#endif
}


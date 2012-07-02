#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#else
#include <pthread.h>
#endif

#include <stdlib.h>

#include "timer.h"

struct timer_t
{
    int duration;
    timer_callback_t callback;
    void* callback_param;
#ifdef WIN32
#else
    pthread_t thread;
#endif
};

#ifdef WIN32
void CALLBACK timer_proc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    struct timer_t* tmr = (struct timer_t*)dwUser;
    tmr->callback(tmr, tmr->callback_param);
}
#else
void* timer_proc(void* param)
{
    struct timer_t* tmr = param;
    struct timespec time;
    struct timespec timeRemaining;
    
    time.tv_sec = 0;
    time.tv_nsec = tmr->duration * 1000000;

    if (nanosleep(&time, &timeRemaining) == 0)
    {
        tmr->callback(tmr, tmr->callback_param);
    }
    return NULL;
}
#endif

struct timer_t* timer_create(int duration, timer_callback_t callback, void* callback_param)
{
    struct timer_t* tmr = (struct timer_t*)malloc(sizeof(struct timer_t));
    if (tmr == NULL)
        return NULL;
    tmr->duration = duration;
    tmr->callback = callback;
    tmr->callback_param = callback_param;
    return tmr;
}

void timer_free(struct timer_t* tmr)
{
    free(tmr);
}

int timer_one_shot(struct timer_t* tmr)
{
#ifdef WIN32
    return timeSetEvent(tmr->duration, 0, timer_proc, (ULONG)tmr, TIME_ONESHOT | TIME_CALLBACK_FUNCTION) != 0;
#else
    return pthread_create(&_thread, NULL, timer_proc, tmr) != 0;
#endif
}


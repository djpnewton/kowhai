#ifndef _TIMER_H_
#define _TIMER_H_

typedef void (*timer_callback_t)(void* param);

int timer_one_shot(int duration, timer_callback_t callback, void* param);

#endif

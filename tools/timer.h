#ifndef _TIMER_H_
#define _TIMER_H_

struct timer_t;
typedef void (*timer_callback_t)(struct timer_t* tmr, void* param);

struct timer_t* timer_create(int duration, timer_callback_t callback, void* callback_param);
void timer_free(struct timer_t* tmr);
int timer_one_shot(struct timer_t* tmr);

#endif

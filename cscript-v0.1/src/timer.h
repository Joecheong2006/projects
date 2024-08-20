#ifndef _STIMER_H_
#define _STIMER_H_
#include "core/defines.h"

typedef struct {
    f64 begin, end, dur;
} timer;

void start_timer(timer* t);
void end_timer(timer* t);

#endif

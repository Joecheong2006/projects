#ifndef _STIMER_
#define _STIMER_
#include "basic/util.h"

typedef struct {
    f64 begin, dur;
} stimer;

void start_stimer(stimer* t);
void end_stimer(stimer* t);
f32 get_current_time();

#endif

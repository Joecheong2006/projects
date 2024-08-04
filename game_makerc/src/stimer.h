#ifndef _STIMER_H_
#define _STIMER_H_
#include "core/defines.h"

typedef struct {
    f64 begin, end, dur;
} stimer;

void start_stimer(stimer* t);
void end_stimer(stimer* t);

#endif

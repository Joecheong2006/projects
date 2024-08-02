#ifndef _STIMER_
#define _STIMER_
#include <time.h>
#include "basic/util.h"

typedef struct {
    clock_t begin;
    f64 dur;
} stimer;

void start_stimer(stimer* t);
void end_stimer(stimer* t);

#endif
